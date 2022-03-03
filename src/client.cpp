#include <iostream>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "protocol.h"

#define SENSITIVITY_LENGTH 8

bool try_send_sensitivity(int socketID, uint64_t sensitivity)
{
    char buffer[SENSITIVITY_LENGTH];
    for (int i = SENSITIVITY_LENGTH - 1; i >= 0; i--)
    {
        buffer[i] = (sensitivity >> i * SENSITIVITY_LENGTH) & 0xFF;
    }

    int sending = send(socketID, buffer, SENSITIVITY_LENGTH, 0);
    if (sending < 0)
    {
        return false;
    }

    return true;
}

bool try_receive_notification(int socketID)
{
	int bytesReceived = 0;
	ServerNotification notification;

	int statusSize = sizeof(notification.status);
	char statusByte;
	bytesReceived = recv(socketID, &statusByte, statusSize, 0);
	
	if (bytesReceived < statusSize)
	{
		std::cerr << "Failed to receive status." << std::endl;
		return false;
	}
	
	notification.status = (Status)statusByte;
	
	if (notification.status == Status::STILL_IMAGE)
	{
		std::cout << "Warning: status received is STILL_IMAGE." << std::endl;
	}

	int dimensionSize = sizeof(notification.width);

	if (dimensionSize != sizeof(notification.height))
	{
		std::cout << "Warning: size of width and height do not match." << std::endl;
	}
	
	char dimensionBuffer[dimensionSize];
	bytesReceived = recv(socketID, &dimensionBuffer, dimensionSize, 0);

	if (bytesReceived < dimensionSize)
	{
		std::cerr << "Failed to receive width." << std::endl;
		return false;
	}

	int width;
	// TODO: assign width

	bytesReceived = recv(socketID, &dimensionBuffer, dimensionSize, 0);
	if (bytesReceived < dimensionSize)
	{
		std::cerr << "Failed to receive height." << std::endl;
		return false;
	}

	int height;
	// TODO: assign height

	int imageSize = width * height * sizeof(uint32_t);
	char* imageBuffer = new char[imageSize];
	bytesReceived = recv(socketID, imageBuffer, imageSize, 0);

	if (bytesReceived < imageSize)
	{
		std::cerr << "Failed to receive image." << std::endl;
		return false;
	}

	notification.image = imageBuffer;

	return true;
}

void save_to_png(uint32_t width, uint32_t height, char* bytes)
{

}

int main(int argc, char** args)
{
    if (argc != 4)
    {
        std::cerr << "Invalid number of parameters." << std::endl;
        return (int)Error::PARAMCOUNT;
    }

    hostent* host = gethostbyname(args[1]);

    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID < 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return (int)Error::SOCKET;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((u_short)strtoul(args[2], NULL, 0));
    memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

    int connection = connect(socketID, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connection < 0)
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return (int)Error::CONNECTION;
    }

    uint64_t sensitivity = atoi(args[3]);
    if (!try_send_sensitivity(socketID, sensitivity))
    {
        std::cerr << "Failed to send sensitivity threshold." << std::endl;
        return (int)Error::SEND;
    }
    
    while (true)
    {
        if (!try_receive_notification(socketID))
        {
        	std::cerr << "An error occured while receiving data." << std::endl;
        	return (int)Error::RECEIVE;
        }
    }

    close(socketID);

    return 0;
}
