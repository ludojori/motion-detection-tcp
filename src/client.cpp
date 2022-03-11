#include <iostream>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "../libs/third-party/stb-image-write/stb_image_write.h"
#include "../libs/motion-detection/tcp_protocol.h"

using namespace motion_detection;

unsigned int* little_to_big_endian(unsigned int* data, const int& size)
{
	for (int i = 0; i < size; i++)
	{
		data[i] = __builtin_bswap32(data[i]);
	}
	return data;
}

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
    unsigned int* fullImage;
    unsigned int* imageBuffer;
    
    while (true)
    {
        if (recv(socketID, &packet, sizeof(packet), 0) != sizeof(Packet))
        {
        	std::cerr << "Failed to receive packet." << std::endl;
        	close(socketID);
        	return (int)Error::RECEIVE;
        }

		int pixelsCount = packet.width * packet.height;
        fullImage = new unsigned int[pixelsCount];
        std::cout << "Image buffer size: " << pixelsCount << std::endl;

    	int numberOfPackages = pixelsCount * sizeof(unsigned int) /SIZE_OF_BYTES_PACKAGE;
        int recievedBytes; 
        int index = 0;

        imageBuffer = new unsigned int[SIZE_OF_BYTES_PACKAGE / sizeof(unsigned int)];
        for(int i = 0; i < numberOfPackages; i++)
		{
            if ((recievedBytes = recv(socketID, imageBuffer, SIZE_OF_BYTES_PACKAGE, 0)) != SIZE_OF_BYTES_PACKAGE)
            {
                std::cout << "Bytes recieved: " << i * recievedBytes << std::endl;
                std::cerr << "Failed to receive image." << std::endl;
                close(socketID);
                return (int)Error::RECEIVE;
            }
            std::cout << "Bytes recieved: " << i * recievedBytes << std::endl;
            memcpy(fullImage + index, imageBuffer, SIZE_OF_BYTES_PACKAGE);
            std::cout << "Bytes copied" << std::endl;
            index += SIZE_OF_BYTES_PACKAGE / sizeof(unsigned int); 
		}
        delete[] imageBuffer;

		int lastPackageSize = pixelsCount % SIZE_OF_BYTES_PACKAGE; 
        imageBuffer = new unsigned int[lastPackageSize];
		if ((recievedBytes = recv(socketID, imageBuffer, lastPackageSize, 0)) != lastPackageSize)
            {
                std::cout << "Bytes recieved: " << recievedBytes << std::endl;
                std::cerr << "Failed to receive image." << std::endl;
                close(socketID);
                return (int)Error::RECEIVE;
            }
        memcpy(fullImage + index, imageBuffer, lastPackageSize);

        imageBuffer = little_to_big_endian(imageBuffer, pixelsCount);
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
}
