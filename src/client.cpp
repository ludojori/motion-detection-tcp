#include <iostream>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "stb_image_write.h"
#include "protocol.h"

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
	
	char destination[35];
	strcpy(destination, "../images/");
	strcat(destination, name);
	strcat(destination, ".jpg");

	if (stbi_write_jpg(destination, width, height, 4, pixels, 50) == 0)
	{
		std::cerr << "Failed to save jpg file." << std::endl;
		return false;
	}

	return true;
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
    if (socketID == -1)
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

    ServerNotification notification;
    
    while (true)
    {
        if (recv(socketID, &notification, sizeof(notification), 0) != sizeof(ServerNotification))
        {
        	std::cerr << "Failed to receive notification." << std::endl;
        	close(socketID);
        	return (int)Error::RECEIVE;
        }

        save_to_jpg(notification.width, notification.height, notification.image);
    }

	// Currently unreachable (requires input):
    if (close(socketID) == -1)
    {
    	std::cerr << "Failed to close socket." << std::endl;
    	return (int)Error::CLOSE;
    }

    return 0;
}
