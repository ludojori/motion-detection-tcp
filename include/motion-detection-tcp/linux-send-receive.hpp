#ifndef LINUX_SEND_RECEIVE
#define LINUX_SEND_RECEIVE

#include "send-receive-interface.h"

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

class LinuxSendReceive
    : public SendReceiveInterface
{
public:
    virtual int localSend(int sockId, void *buf, size_t len, int flags) override
    {
        return send(sockId, buf, len, flags);
    }
    
    virtual int localReceive(int sockId, void *buf, size_t len, int flags) override
    {
        return recv(sockId, buf, len, flags);
    }
    
    virtual void close(int sockId)
    {
        close(sockId);
    }
    
    virtual int getSockOptCheck(int sockId, int &error)
    {
        error = 0;
        socklen_t len = sizeof(error);
        return getsockopt(sockId, SOL_SOCKET, SO_ERROR, &error, &len);
    }

    virtual int initServer(char* port)
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
            threads.emplace_back(&ServerCommunication::readSensitivity, this, sockID);
            std::cout << "[MESSAGE]: Number of connections accepted: " << sockIDSensitivity.size() << std::endl;
        }
        // change_pic_thread.join();
        check_connected_clients_thread.join();
    }
};

#endif /* LINUX_SEND_RECEIVE */