<<<<<<< HEAD
#include "common.h"
#include "protocol.h"
#include <thread>
/* 
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h> */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

//server accepts char messages
bool send_sensitivity(int socketID, uint64_t& sensitivity)
{
    std::cout << sensitivity << std::endl;
    int sending = send(socketID, &sensitivity, SENSITIVITY_LENGTH, 0);
    if (sending == -1)
    {
        return false;
    }
    
    return true;
}


void sendingTCP (int sockID, Request request)
{
	if(request.command == Command::MESSAGE)
	{
		std::cout << "Message: " << request.data << std::endl;
		send(sockID, &request.command, sizeof(request.command), 0);
		int len = std::strlen(request.data) + 1;
		send(sockID, &len, sizeof(len), 0);
		send(sockID, request.data, len, 0);
	}
=======
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "../libs/third-party/stb-image-write/stb_image_write.h"
#include "../libs/motion-detection/tcp_protocol.h"

using namespace motion_detection;

bool save_to_jpg(int width, int height, unsigned int* pixels)
{
	time_t rawtime;
	struct tm* timeinfo;
	char name[21]; // YYYY-MM-DD--HH-MM-SS + null-character

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	if (strftime(name, 21, "%F--%H-%M-%S", timeinfo) == 0)
	{
		std::cerr << "Failed to create time string." << std::endl;
		return false;
	}
	
	char destination[35]; // 21 + 10 + 4
	strcpy(destination, "../images/"); // 10 characters + null-character
	strcat(destination, name);
	strcat(destination, ".jpg"); // 4 characters + null-character

	if (stbi_write_jpg(destination, width, height, 4, pixels, 50) == 0)
	{
		std::cerr << "Failed to save jpg file." << std::endl;
		return false;
	}

	return true;
>>>>>>> 148df3ecd794528d5d81d6c933a851d49b847e67
}

void disconnect()
{
<<<<<<< HEAD
	//Send disconnect message and server checks if the message is not disconnect, than it removes the client
    //close(sockID)
}
=======
    if (argc != 4)
    {
        std::cerr << "Invalid number of parameters." << std::endl;
        return (int)Error::PARAMCOUNT;
    }
>>>>>>> 148df3ecd794528d5d81d6c933a851d49b847e67

int main(int argc, char **args)
{
	if(argc != 3) {
		return -1;
	}

<<<<<<< HEAD
	int sockID = 0;

    uint64_t sens; //optimise or get a variable
    sens = atoi(args[2]); 
    
	hostent *host = gethostbyname(args[1]);

	sockID = socket(AF_INET, SOCK_STREAM, 0);
	// server address
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(5588);
	std::memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

    std::cout <<"Sensitivity: " << sens << std::endl;

	connect(sockID, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    std::cout <<"Sensitivity: " << sens << std::endl;
	//std::cout << "before send\n";
	send_sensitivity(sockID, sens);
	//std::cout << "after send\n";

   /*  char buffer[1000000000];
    while (true)
    {
        int bytesReceived = recv(socketID, buffer, 10000, 0);
    } */ 
    close(sockID); 
=======
    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return (int)Error::SOCKET;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((u_short)strtoul(args[2], NULL, 0));
    memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

    if (connect(socketID, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to connect to server." << std::endl;
        close(socketID);
        return (int)Error::CONNECTION;
    }

    uint64_t sensitivity = atoi(args[3]);
    if (send(socketID, &sensitivity, sizeof(sensitivity), 0) == -1)
    {
        std::cerr << "Failed to send sensitivity threshold." << std::endl;
        close(socketID);
        return (int)Error::SEND;
    }

    struct Packet packet;
    unsigned int* imageBuffer;
    
    while (true)
    {
        if (recv(socketID, &packet, sizeof(packet), 0) != sizeof(Packet))
        {
        	std::cerr << "Failed to receive packet." << std::endl;
        	close(socketID);
        	return (int)Error::RECEIVE;
        }

		int imageBufferSize = packet.width * packet.height;
        imageBuffer = new unsigned int[imageBufferSize];

        if (recv(socketID, imageBuffer, imageBufferSize, 0) != imageBufferSize)
        {
        	std::cerr << "Failed to receive image." << std::endl;
        	close(socketID);
        	return (int)Error::RECEIVE;
        }
        
        save_to_jpg(packet.width, packet.height, imageBuffer);
        delete[] imageBuffer;
    }

	/** Currently unreachable (requires input):
    if (close(socketID) == -1)
    {
    	std::cerr << "Failed to close socket." << std::endl;
    	return (int)Error::CLOSE;
    }
    */

    return 0;
>>>>>>> 148df3ecd794528d5d81d6c933a851d49b847e67
}
