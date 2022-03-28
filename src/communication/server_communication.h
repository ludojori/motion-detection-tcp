#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include "../../libs/motion-detection/tcp_protocol.h"
#include "../../libs/third-party/camera/camera.h"
#include "image_processor.h"
#include "../button/button.h"
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
#define IMAGE_SEGMENT_SIZE 100

class ServerCommunication
{
private:
    std::unordered_map<int, uint64_t> sockIDSensitivity;
    std::mutex mutex;
    ImageProcessor ip;
    Button button; 
    //static ServerCommunication *instance;
    unsigned int* pixels;
    // std::mutex picChangeMutex; 
    // std::condition_variable picChangeCv;
<<<<<<< HEAD
    int pixelsCount;
=======
>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c
//  protected:

public:
    ServerCommunication();
    void registerClient(int sockID, uint64_t sensitivity);
    void disconnectClient(const int sockID, bool shouldCloseSocket);
    void read_sensitivity(int sockID);
    bool isSocketConnected(int sockID);
    void sendPicture(int sockID, unsigned int *fullImage);
    void notifyClients(unsigned int *pixels, int pixelsCount);
    void checkConnectedClients();
    void changePic();
    int initServer(char *port);
    ~ServerCommunication();
  //  void operator=(const ServerCommunication &rhs) = delete;
   // static ServerCommunication *getInstance();
};

#endif /*SERVER_COMMUNICATION_H*/