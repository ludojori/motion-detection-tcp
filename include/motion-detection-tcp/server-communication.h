#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include "../third-party/camera/camera.h"
#include "tcp-protocol.h"
#include "image-processor.h"
#include "button.h"
#include "adafruit/bbio.h"

#include <iostream>
#include <cstring>

#include <thread>
#include <mutex>

#include <vector>
#include <queue>
#include <unordered_map>

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#define MAX_CLIENTS 10

class ServerCommunication
{
private:
    std::unordered_map<int, uint64_t> sockIDSensitivity;
    std::mutex mutex;
    ImageProcessor ip;
    Button button; 
    unsigned int* pixels;

    void registerClient(int sockID, uint64_t sensitivity);
    void disconnectClient(const int sockID, bool shouldCloseSocket);
    void readSensitivity(int sockID);
    bool isSocketConnected(int sockID);
    void sendPicture(int sockID, unsigned int *fullImage);
    void sendConfigPacket(int sockID, ConfigPacket* packet);
    void sendImage(int sockID, unsigned int *fullImage, int imageSize);
    void notifyClients(unsigned int *pixels, int pixelsCount);
    void checkConnectedClients();
    void changePic();

public:
    ServerCommunication();

    int initServer(char *port);

    ~ServerCommunication();
};

#endif /* SERVER_COMMUNICATION_H */