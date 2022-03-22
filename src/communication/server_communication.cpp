#include "server_communication.h"
#include "adafruit/bbio.h"

using namespace motion_detection;
using namespace adafruit::bbio;

void ServerCommunication::
    registerClient(int sockID, uint64_t sensitivity)
{
    mutex.lock();
    std::cout << "Locked";
    sockIDSensitivity.insert({sockID, sensitivity});
    mutex.unlock();
    std::cout << "Unlock";
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

    std::cout << "Before register: sens = " << sens << std::endl;
    registerClient(sockID, sens);
    mutex.lock();
    std::cout << "Sensitivity: " << sens << " from map: " << sockIDSensitivity.at(sockID) << std::endl;
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
        return false;
        disconnectClient(sockID);
    }
    return true;
}

void ServerCommunication::
    sendPicture(int sockID, unsigned int *fullImage)
{
    int height, width;
    getResolution(&width, &height);
    int sizeOfPicture = height * width;

    unsigned int *imageSegment = new unsigned int[IMAGE_SEGMENT_SIZE]{0};
    int segmentCount = sizeOfPicture / IMAGE_SEGMENT_SIZE;
    std::cout << "Image segment count: " << segmentCount << std::endl;

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
    packet.fullImageWidth = width;
    packet.fullImageHeight = height;
    packet.imageSegmentCount = segmentCount;

    int packet_bytes = send(sockID, &packet, sizeof(ConfigPacket), 0);
    if (packet_bytes == -1)
    {
        std::cerr << "[ERROR]: Sending ConfigPacket failed: " << std::strerror(errno) << std::endl;
    }
    else if (packet_bytes != sizeof(ConfigPacket))
    {
        std::cout << "[WARNING]: ConfigPacket not fully sent." << std::endl;
    }

    // Send segments

    int segmentBytes = IMAGE_SEGMENT_SIZE * sizeof(unsigned int);
    int index = 0;
    int sending = 0;

    for (int i = 0; i < segmentCount; i++)
    {
        memcpy(imageSegment, fullImage + index, segmentBytes);
        sending = send(sockID, imageSegment, segmentBytes, 0);
        if (sending == -1)
        {
            std::cerr << "[ERROR]: Sending image segment failed: " << std::strerror(errno) << std::endl;
            // TODO: exception handling
        }
        if (sending != segmentBytes)
        {
            std::cout << "[WARNING]: Segment package No." << i + 1 << " not fully sent." << std::endl;
        }
        index += IMAGE_SEGMENT_SIZE;
    }

    // Send remaining bytes

    int remainingBytes = (sizeOfPicture % IMAGE_SEGMENT_SIZE) * sizeof(unsigned int);
    if (remainingBytes > 0)
    {
        std::cout << "[MESSAGE]: Sending remaining " << remainingBytes << " bytes..." << std::endl;
        memcpy(imageSegment, fullImage + index, remainingBytes);
        sending = send(sockID, imageSegment, remainingBytes, 0);
        if (sending == -1)
        {
            std::cerr << "[ERROR]: Sending remaining image segment failed: " << std::strerror(errno) << std::endl;
        }
        else if (sending != remainingBytes)
        {
            std::cout << "[WARNING]: Remaining segment not fully sent." << std::endl;
        }
    }
    delete[] imageSegment;
}

void ServerCommunication::
    notifyClients(unsigned int *pixels, int pixelsCount)
{
    size_t sizeInBytes = pixelsCount * sizeof(int);
    // byte conversion
    unsigned char *bytePixels = new unsigned char[pixelsCount * 4];
    bytePixels = (unsigned char *)pixels;

    uint64_t diff = ip.calculatePictureDifference(bytePixels, sizeInBytes);

    // check all clients if they should be notified
    mutex.lock();
    for (auto &i : sockIDSensitivity)
    {
        if (i.second <= diff)
        {
            sendPicture(i.first, pixels);
        }
    }
    mutex.unlock();
    std::cout << "Clients checked" << std::endl;
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
        std::cout << "Picture changed!" << std::endl;
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
    //std::thread change_pic_thread(&button.changePic, &button, pixels, picChangeMutex, picChangeCv);
    //std::thread notify_click_thread(&ServerCommunication::changePic, this);
    std::thread check_connected_clients_thread(&ServerCommunication::checkConnectedClients, this);

    // may be in separate function
    while (true)
    {
        if (sockIDSensitivity.size() >= MAX_CLIENTS)
        {
            continue;
            // wait until disconnect
        }
        std::cout << "Listining" << std::endl;
        sockaddr_in cliAddr;
        socklen_t len = sizeof(cliAddr);
        sockID = accept(listener, (struct sockaddr *)&cliAddr, &len);

        if (sockID == -1)
        {
            std::cerr << "[ERROR]: Function accept() failed: " << std::strerror(errno) << std::endl;
            return (int)errno;
        }
        std::cout << "Connection accepted. SockID: " << sockID << std::endl;
        // TODO: sync
        threads.emplace_back(&ServerCommunication::read_sensitivity, this, sockID);
        std::cout << "Number of connections accepted " << sockIDSensitivity.size() << std::endl;
    }
    //change_pic_thread.join();
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

ServerCommunication::ServerCommunication() : button([this](){this->changePic();}), pixels(nullptr){
}
