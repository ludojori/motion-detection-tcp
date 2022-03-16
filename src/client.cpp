#include <iostream>
#include <cstring>
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
		std::cerr << "[ERROR]: Failed to create time string." << std::endl;
		return false;
	}

	char destination[35]; // 21 + 10 + 4
	strcpy(destination, "../images/"); // 10 characters + null-character
	strcat(destination, name);
	strcat(destination, ".jpg"); // 4 characters + null-character

	if (stbi_write_jpg(destination, width, height, 4, pixels, 50) == 0)
	{
		std::cerr << "[ERROR]: Failed to save jpg file." << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char** args)
{
    if (argc != 4)
    {
        std::cerr << "[ERROR]: Invalid parameters. Parameters are: [server_addr] [port] [sens_threshold]. Exiting." << std::endl;
        return -1;
    }

    hostent* host = gethostbyname(args[1]);

    std::cout << "[MESSAGE]: Creating socket..." << std::endl;

    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID == -1)
    {
        std::cerr << "[ERROR]: Function socket() failed with error code: " << std::strerror(errno) << std::endl;
        return (int)errno;
    }

    std::cout << "[MESSAGE]: Assigning server address..." << std::endl;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((u_short)strtoul(args[2], NULL, 0));
    memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

	std::cout << "[MESSAGE]: Connecting to server..." << std::endl;

    if (connect(socketID, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "[ERROR]: Function connect() failed with error code: " << std::strerror(errno) << std::endl;
        close(socketID);
        return (int)errno;
    }

    std::cout << "[MESSAGE]: Connection accepted." << std::endl;
    
    uint64_t sensitivity = atoi(args[3]);
    int sens_bytes = send(socketID, &sensitivity, sizeof(sensitivity), 0);
    if (sens_bytes == -1)
    {
        std::cerr << "[ERROR]: Failed to send sensitivity threshold with error code: " << std::strerror(errno) << std::endl;
        close(socketID);
        return (int)errno;
    }
    else if (sens_bytes != sizeof(sensitivity))
    {
    	std::cout << "[WARNING]: Partially sent sensitivity threshold." << std::endl;
    }
    

	/* (IDEA) Receive status byte here:
		if Status::STATIC, then server only notifies that it's still alive,
		else if Status::MOTION_DETECTED, then proceed with the code below,
		else a timeout occurs after a specified amount of time.
	*/
	char statusByte = ' ';

	while (true)
	{
		// Receive status byte
		
		if (recv(socketID, &statusByte, 1, 0) == -1)
		{
			std::cerr << "[ERROR]: Receiving status byte failed: " << std::strerror(errno) << std::endl;
			close(socketID);
			return (int)errno;
		}

		if (statusByte == (char)Status::STILL_IMAGE)
		{
			std::cout << "[MESSAGE]: Received status byte STILL_IMAGE." << std::endl;
		}
		else if (statusByte == (char)Status::MOTION_DETECTED)
		{
			std::cout << "[MESSAGE]: Received status byte MOTION_DETECTED." << std::endl;
			std::cout << "[MESSAGE]: Attempting to receive config packet..." << std::endl;
			
			// Receive ConfigPacket

		    struct ConfigPacket packet;
		    int packet_bytes = recv(socketID, &packet, sizeof(packet), 0);

			if (packet_bytes == -1)
			{
				std::cerr << "[ERROR]: Receiving ConfigPacket failed: " << std::strerror(errno) << std::endl;
				close(socketID);
				return (int)errno;
			}
		    else if (packet_bytes != sizeof(ConfigPacket))
		    {
		        std::cout << "[WARNING]: ConfigPacket not fully received." << std::endl;
		    }

		    // Receive segments

		    std::cout << "[MESSAGE]: Config packet received." << std::endl;
		    std::cout << "[MESSAGE]: Parameters are: width = " << packet.fullImageWidth
		    		  << " height = " << packet.fullImageHeight
		    		  << " segment_count = " << packet.imageSegmentCount << std::endl;

			std::cout << "[MESSAGE]: Declaring variables..." << std::endl;
			int fullImageBufferSize = packet.fullImageWidth * packet.fullImageHeight;
			unsigned int* fullImageBuffer = new unsigned int[fullImageBufferSize] { 0 };
			int imageSegmentBufferSize = fullImageBufferSize / packet.imageSegmentCount;
			unsigned int* imageSegmentBuffer = new unsigned int[imageSegmentBufferSize] { 0 };

			std::cout << "[MESSAGE]: Receiving image segments..." << std::endl;

			int segmentBufferBytes = imageSegmentBufferSize * sizeof(unsigned int);

			int index = 0;
			int receiving = 0;
		    for (int i = 0; i < packet.imageSegmentCount; i++)
		    {
		    	receiving = recv(socketID, imageSegmentBuffer, segmentBufferBytes, 0);
		    	if (receiving == -1)
		    	{
		    		std::cerr << "[ERROR]: Function recv() failed with error code: " << std::strerror(errno) << std::endl;
		    		return (int)errno;
		    	}
		    	else if (receiving != segmentBufferBytes)
		    	{
		    		std::cout << "[WARNING]: Segment package No." << i << " not fully received." << std::endl;
		    	}
		    	memcpy(fullImageBuffer + index, imageSegmentBuffer, segmentBufferBytes);
		    	index += imageSegmentBufferSize;
		    }

			// Receive remaining bytes

			int remainingBytes = (fullImageBufferSize % imageSegmentBufferSize) * sizeof(unsigned int);
			std::cout << "[MESSAGE]: Receiving remaining " << remainingBytes << " bytes..." << std::endl;
			
			receiving = recv(socketID, imageSegmentBuffer, remainingBytes, 0);
			if (receiving == -1)
			{
			    std::cerr << "[ERROR]: Function recv() failed with error code: " << std::strerror(errno) << std::endl;
			    return (int)errno;
			}
			else if (receiving != remainingBytes)
			{
			    std::cout << "[WARNING]: Remaining segment not fully received." << std::endl;
			}
			memcpy(fullImageBuffer + index, imageSegmentBuffer, remainingBytes);

			std::cout << "[MESSAGE]: Fixing image endianess..." << std::endl;
		    little_to_big_endian(fullImageBuffer, fullImageBufferSize);

			std::cout << "[MESSAGE]: Saving image..." << std::endl;
		    save_to_jpg(packet.fullImageWidth, packet.fullImageHeight, fullImageBuffer);

			delete[] fullImageBuffer;
			delete[] imageSegmentBuffer;
		}
	}


	if (close(socketID) == -1)
	{
		std::cerr << "[ERROR]: Function close() failed with error code: " << std::strerror(errno) << std::endl;
		return (int)errno;
	}

	std::cout << "[MESSAGE]: Operation successful." << std::endl;
	
    return 0;
}
