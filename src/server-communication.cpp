#include "../include/motion-detection-tcp/server-communication.h"
#include "adafruit/bbio.h"

using namespace motion_detection;
using namespace adafruit::bbio;

void ServerCommunication::
    registerClient(int sockID, uint64_t sensitivity)
{
    mutex.lock();
    std::cout << "[MESSAGE]: Locked.";
    sockIDSensitivity.insert({sockID, sensitivity});
    mutex.unlock();
    std::cout << "[MESSAGE]: Unlocked.";
}

void ServerCommunication::
    disconnectClient(const int sockID, bool shouldCloseSocket = false)
{
    sockIDSensitivity.erase(sockID);
    if (shouldCloseSocket)
    {
        close(sockID);
    }
    std::cout << "[MESSAGE]: A client has disconnected. Remaining clients: " << sockIDSensitivity.size() << std::endl;
}

void ServerCommunication::
    read_sensitivity(int sockID)
{
    uint64_t sens;
    const int SENSITIVITY_LENGTH = sizeof(uint64_t);
    int sens_bytes = recv(sockID, &sens, SENSITIVITY_LENGTH, 0);

    if (sens_bytes == -1)
    {
        std::cerr << "[ERROR]: Sensitivity receival failed: " << std::strerror(errno) << std::endl;
        return;
    }
    else if (sens_bytes != sizeof(sens))
    {
        std::cout << "[WARNING]: Partially received sensitivity threshold." << std::endl;
    }

    std::cout << "[MESSAGE]: Before register: sens = " << sens << std::endl;
    registerClient(sockID, sens);
    mutex.lock();
    std::cout << "[MESSAGE]: Sensitivity: " << sens << " from map: " << sockIDSensitivity.at(sockID) << std::endl;
    mutex.unlock();
}

bool ServerCommunication::
    isSocketConnected(int sockID)
{
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(sockID, SOL_SOCKET, SO_ERROR, &error, &len);

    if (retval == 0 && error != 0)
    {
        std::cerr << "[ERROR]: Client socket failed: " << std::strerror(error) << std::endl;
        disconnectClient(sockID);
        return false;
    }
    return true;
}

void ServerCommunication::
    sendPicture(int sockID, unsigned int *fullImage)
{
    int height, width;
    getResolution(&width, &height);
    int sizeOfPicture = height * width;

    char statusByte = static_cast<char>(Status::MOTION_DETECTED);
    if (send(sockID, &statusByte, 1, 0) == -1)
    {
        std::cerr << "[ERROR]: Sending status byte failed: " << std::strerror(errno) << std::endl;
    }

    if (!isSocketConnected(sockID))
    {
        disconnectClient(sockID);
    }

    struct ConfigPacket packet;
    packet.image_width = width;
    packet.image_height = height;
    std::cout << "[MESSAGE]: Sending ConfigPacket with parameters: width = "
        << packet.image_width << " height = " << packet.image_height << " ..." << std::endl;

    int packet_bytes = send(sockID, &packet, sizeof(ConfigPacket), 0);
    if (packet_bytes == -1)
    {
        std::cerr << "[ERROR]: Sending ConfigPacket failed: " << std::strerror(errno) << std::endl;
    }
    else if (packet_bytes != sizeof(ConfigPacket))
    {
        std::cout << "[WARNING]: ConfigPacket not fully sent." << std::endl;
    }
    else
    {
        std::cout << "[MESSAGE] ConfigPacket successfully sent." << std::endl;
    }

	int image_size_in_bytes = sizeOfPicture * sizeof(unsigned int);
	unsigned char temp_buffer[image_size_in_bytes];
	memcpy(temp_buffer, fullImage, image_size_in_bytes);

	int curr_sent_bytes = 0;
	int last_byte_idx = 0;
	while (true)
	{
		curr_sent_bytes = send(sockID, temp_buffer + last_byte_idx, image_size_in_bytes - last_byte_idx, 0);
		if (curr_sent_bytes == -1)
		{
			std::cerr << "[ERROR]: Sending image failed: " << std::strerror(errno) << std::endl;
			// TODO: exception handling
		}
		else if (curr_sent_bytes == 0)
		{
			std::cout << "[MESSAGE]: No bytes left to send." << std::endl;
			break;
		}

		last_byte_idx += curr_sent_bytes;
		if (last_byte_idx > image_size_in_bytes)
		{
			std::cout << "[WARNING]: Byte index exceeded buffer size by "
					<< image_size_in_bytes - last_byte_idx << " bytes." << std::endl;
		}
		else
		{
			std::cout << "[MESSAGE]: Sent " << last_byte_idx << "/"
                << image_size_in_bytes << " bytes..." << std::endl;
		}
	}

    int image_size_in_bytes = sizeOfPicture * sizeof(unsigned int);
    unsigned char temp_buffer[image_size_in_bytes];
    memcpy(temp_buffer, fullImage, image_size_in_bytes);

    int curr_sent_bytes = 0;
    int last_byte_idx = 0;
    while (true)
    {
        curr_sent_bytes = send(sockID, temp_buffer + last_byte_idx, image_size_in_bytes - last_byte_idx, 0);
        if (curr_sent_bytes == -1)
        {
            std::cerr << "[ERROR]: Sending image failed: " << std::strerror(errno) << std::endl;
            // TODO: exception handling
        }
        else if (curr_sent_bytes == 0)
        {
            std::cout << "[MESSAGE]: No bytes left to send." << std::endl;
            break;
        }

        last_byte_idx += curr_sent_bytes;
        if (last_byte_idx > image_size_in_bytes)
        {
            std::cout << "[WARNING]: Byte index exceeded buffer size by "
                      << image_size_in_bytes - last_byte_idx << " bytes." << std::endl;
        }
        else
        {
            std::cout << "[MESSAGE]: Sent " << last_byte_idx << "/" << image_size_in_bytes
                      << " bytes..." << std::endl;
        }
    }
}

void ServerCommunication::
    notifyClients(unsigned int *pixels, int pixelsCount)
{
    size_t sizeInBytes = pixelsCount * sizeof(int);
    // byte conversion
    unsigned char *bytePixels = new unsigned char[pixelsCount * 4];
    bytePixels = (unsigned char *)pixels;

    uint64_t diff = ip.calculatePictureDifference(bytePixels, sizeInBytes);
    delete[] bytePixels;

    // check all clients if they should be notified
    mutex.lock();
    int notified_clients_count = 0;
    for (auto &i : sockIDSensitivity)
    {
        if (i.second <= diff)
        {
            sendPicture(i.first, pixels);
            notified_clients_count++;
        }
    }
    mutex.unlock();
    std::cout << "[MESSAGE]: " << notified_clients_count << " clients notified." << std::endl;
}

void ServerCommunication::
    checkConnectedClients()
{
    while (true)
    {
        mutex.lock();
        for (auto &i : sockIDSensitivity)
        {
            char statusByte = static_cast<char>(Status::STILL_IMAGE);
            if (send(i.first, &statusByte, 1, 0) == -1)
            {
                std::cerr << "[ERROR]: Sending status byte failed: " << std::strerror(errno) << std::endl;
                disconnectClient(i.first, true);
            }

            if (!isSocketConnected(i.first))
            {
                disconnectClient(i.first);
            }
        }
        mutex.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(3)); // check every 3 seconds if someone has disconnected
    }
}

void ServerCommunication::
    changePic()
{
    generateMovement();
    std::cout << "[MESSAGE]: Picture changed!" << std::endl;
    int height, width;
    getResolution(&height, &width);
    int pixelsCount = height * width;
    if (pixels != nullptr)
    {
        delete[] pixels;
    }
    pixels = new unsigned int[pixelsCount];
    getCurrentImage(pixels);
    notifyClients(pixels, pixelsCount);
}

int ServerCommunication::
    initServer(char *port)
{

    int sockID = 0;

    int listener = socket(PF_INET, SOCK_STREAM, 0);
    if (listener == -1)
    {
        std::cerr << "[ERROR]: Function socket() failed: " << std::strerror(errno) << std::endl;
    }

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short)strtoul(port, NULL, 0));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "[ERROR]: Function bind() failed: " << std::strerror(errno) << std::endl;
    }
    if (listen(listener, 110) != 0)
    {
        std::cerr << "[ERROR]: Function listen() failed: " << std::strerror(errno) << std::endl;
    }

    std::vector<std::thread> threads;
    // std::thread change_pic_thread(&button.changePic, &button, pixels, picChangeMutex, picChangeCv);
    // std::thread notify_click_thread(&ServerCommunication::changePic, this);
    std::thread check_connected_clients_thread(&ServerCommunication::checkConnectedClients, this);

    // may be in separate function
    while (true)
    {
        if (sockIDSensitivity.size() >= MAX_CLIENTS)
        {
            continue;
            // wait until disconnect
        }
        std::cout << "[MESSAGE]: Listening..." << std::endl;
        sockaddr_in cliAddr;
        socklen_t len = sizeof(cliAddr);
        sockID = accept(listener, (struct sockaddr *)&cliAddr, &len);

        if (sockID == -1)
        {
            std::cerr << "[ERROR]: Function accept() failed: " << std::strerror(errno) << std::endl;
            return (int)errno;
        }
        std::cout << "[MESSAGE]: Connection accepted. SockID: " << sockID << std::endl;
        // TODO: sync
        threads.emplace_back(&ServerCommunication::read_sensitivity, this, sockID);
        std::cout << "[MESSAGE]: Number of connections accepted: " << sockIDSensitivity.size() << std::endl;
    }
    // change_pic_thread.join();
    check_connected_clients_thread.join();
}

// ServerCommunication* ServerCommunication::
//     getInstance()
// {
//     if(instance == nullptr)
//     {
//         instance = new ServerCommunication();  //TODO: explore delete
//     }
//     return instance;
// }

ServerCommunication::~ServerCommunication()
{
    if (pixels != nullptr)
    {
        delete[] pixels;
    }
}

ServerCommunication::ServerCommunication() : button([this]()
                                                    { this->changePic(); }),
                                             pixels(nullptr)
{
}