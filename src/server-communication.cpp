#include "../include/motion-detection-tcp/server-communication.h"
#pragma warning(disable : 4996)

using namespace motion_detection;

void ServerCommunication::
    registerClient(int sockID, uint64_t sensitivity)
{
    mutex.lock();
    std::cout << "[MESSAGE]: Locked." << std::endl;
    sockIDSensitivity.insert({sockID, sensitivity});
    mutex.unlock();
    std::cout << "[MESSAGE]: Unlocked." << std::endl;
}

void ServerCommunication::
    disconnectClient(const int sockID, bool shouldCloseSocket = false)
{
    sockIDSensitivity.erase(sockID);
    if (shouldCloseSocket)
    {
        communication->close(sockID);
    }
    std::cout << "[MESSAGE]: A client has disconnected. Remaining clients: " << sockIDSensitivity.size() << std::endl;
}

void ServerCommunication::
    readSensitivity(int sockID)
{
    uint64_t sens;
    const int SENSITIVITY_LENGTH = sizeof(uint64_t);
    int sens_bytes = communication->localReceive(sockID, &sens, SENSITIVITY_LENGTH, 0);

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
    int retval = communication->getSockOptCheck(sockID, error);

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
    if (communication->localSend(sockID, &statusByte, 1, 0) == -1)
    {
        std::cerr << "[ERROR]: Sending status byte failed: " << std::strerror(errno) << std::endl;
    }

    if (!isSocketConnected(sockID))
    {
        disconnectClient(sockID);
    }

    struct ConfigPacket packet;
    packet.imageWidth = width;
    packet.imageHeight = height;

    sendConfigPacket(sockID, &packet);
    sendImage(sockID, fullImage, width * height);

    std::cout << "[MESSAGE]: Done." << std::endl;
}

void ServerCommunication::sendConfigPacket(int sockID, ConfigPacket *packet)
{
    std::cout << "[MESSAGE]: Sending ConfigPacket with parameters: width = "
              << packet->imageWidth << " height = " << packet->imageHeight << " ..." << std::endl;

    int packet_bytes = communication->localSend(sockID, packet, sizeof(ConfigPacket), 0);
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
}

void ServerCommunication::sendImage(int sockID, unsigned int *fullImage, int imageSize)
{
    unsigned char *bufferPtr = (unsigned char *)fullImage;

    int imageSizeInBytes = imageSize * sizeof(unsigned int);
    int currSentBytes = 0;
    int lastByteIdx = 0;
    while (true)
    {
        currSentBytes = communication->localSend(sockID, bufferPtr + lastByteIdx, imageSizeInBytes - lastByteIdx, 0);
        if (currSentBytes == -1)
        {
            std::cerr << "[ERROR]: Sending image failed: " << std::strerror(errno) << std::endl;
            // TODO: exception handling
        }
        else if (currSentBytes == 0)
        {
            std::cout << "[MESSAGE]: No bytes left to send." << std::endl;
            break;
        }

        lastByteIdx += currSentBytes;
        if (lastByteIdx > imageSizeInBytes)
        {
            std::cout << "[WARNING]: Byte index exceeded buffer size by "
                      << imageSizeInBytes - lastByteIdx << " bytes." << std::endl;
        }
        else
        {
            std::cout << "[MESSAGE]: Sent " << lastByteIdx << "/"
                      << imageSizeInBytes << " bytes..." << std::endl;
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
    int notifiedClientsCount = 0;
    for (auto &i : sockIDSensitivity)
    {
        if (i.second <= diff)
        {
            sendPicture(i.first, pixels);
            notifiedClientsCount++;
        }
    }
    mutex.unlock();
    std::cout << "[MESSAGE]: " << notifiedClientsCount << " clients notified." << std::endl;
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
            if (communication->localSend(i.first, &statusByte, 1, 0) == -1)
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
    return communication->initServer(port);
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

ServerCommunication::ServerCommunication(SendReceiveInterface &communication) : pixels(nullptr)
{
    this->button = &Button([this]()
                           { this->changePic(); });
    this->communication = &communication;
}

ServerCommunication::ServerCommunication(SendReceiveInterface &communication, Button &button) : pixels(nullptr)
{
    this->button = &button;
    this->communication = &communication;
}