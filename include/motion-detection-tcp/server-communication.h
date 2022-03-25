#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include "../third-party/camera/camera.h"
#include "send-receive-interface.h"
#include "tcp-protocol.h"
#include "image-processor.h"
#include "button.h"

#include <iostream>
#include <cstring>

#include <thread>
#include <mutex>

#include <vector>
#include <queue>
#include <unordered_map>

#define MAX_CLIENTS 10
using namespace motion_detection;
class ServerCommunicationTestClass;

class ServerCommunication
{
private:
    std::unordered_map<int, uint64_t> sockIDSensitivity;
    std::mutex mutex;
    ImageProcessor ip;
    Button *button;
    SendReceiveInterface *communication;
    unsigned int *pixels;

    void registerClient(int sockID, uint64_t sensitivity);
    void disconnectClient(const int sockID, bool shouldCloseSocket);
    void readSensitivity(int sockID);
    bool isSocketConnected(int sockID);
    void sendPicture(int sockID, unsigned int *fullImage);
    void sendConfigPacket(int sockID, ConfigPacket *packet);
    void sendImage(int sockID, unsigned int *fullImage, int imageSize);
    void notifyClients(unsigned int *pixels, int pixelsCount);
    void checkConnectedClients();
    void changePic();

public:
    ServerCommunication(SendReceiveInterface &communication, Button &button);
    ServerCommunication(SendReceiveInterface &communication);

    int initServer(char *port);

    ~ServerCommunication();

private:
    friend class ServerCommunicationTestClass;
};

#endif /* SERVER_COMMUNICATION_H */