#include "../include/motion-detection-tcp/jpg-receive-save-utils.h"
#include "../include/third-party/stb-image-write/stb-image-write.h"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#ifdef DEBUG
#define DEBUG_PRINT(type, msg)      \
	if (type == "m")                \
		std::cout << "[MESSAGE]: "; \
	else if (type == "w")           \
		std::cout << "[WARNING]: "; \
	else if (type == "e")           \
		std::cout << "[ERROR]: ";   \
	std::cout << msg << std::endl;
#endif

namespace motion_detection
{
	int JpgReceiveSaveUtils::receiveAndSave(int socketfd, const char *dir, int quality)
	{
		int errorStatus = 0;
		char statusByte = ' ';
		while (true)
		{
			// Receive status byte

			if (recv(socketfd, &statusByte, 1, 0) == -1)
			{
				std::cerr << "[ERROR]: Receiving status byte failed: " << std::strerror(errno) << std::endl;
				return (int)errno;
			}

			if (statusByte == (char)Status::STILL_IMAGE)
			{
				std::cout << "[MESSAGE]: Received status byte STILL_IMAGE." << std::endl;
				continue;
			}
			else if (statusByte == (char)Status::MOTION_DETECTED)
			{
				std::cout << "[MESSAGE]: Received status byte MOTION_DETECTED." << std::endl;

				// Receive ConfigPacket

				std::cout << "[MESSAGE]: Receiving ConfigPacket..." << std::endl;
				ConfigPacket packet;
				errorStatus = receiveConfigPacket(socketfd, &packet);
				if (errorStatus != 0)
				{
					return errorStatus;
				}

				int imageSize = packet.imageWidth * packet.imageHeight;
				unsigned int *imageBuffer = new unsigned int[imageSize] { 0 };

				std::cout << "[MESSAGE]: Receiving image..." << std::endl;
				errorStatus = receiveImage(socketfd, imageBuffer, imageSize);
				if (errorStatus != 0)
				{
					delete[] imageBuffer;
					return errorStatus;
				}

				std::cout << "[MESSAGE]: Fixing image endianess..." << std::endl;
				littleToBigEndian(imageBuffer, imageSize);

				std::cout << "[MESSAGE]: Saving image..." << std::endl;
				if (!trySaveToJpg(dir, packet.imageWidth, packet.imageHeight, imageBuffer, quality))
				{
					delete[] imageBuffer;
					return -1;
				}

				delete[] imageBuffer;
				std::cout << "[MESSAGE]: Done." << std::endl;
			}
		}
		return 0;
	}

	int JpgReceiveSaveUtils::receiveConfigPacket(int socketfd, ConfigPacket *packet)
	{
		int packetBytes = recv(socketfd, packet, sizeof(ConfigPacket), 0);

		if (packetBytes == -1)
		{
			std::cerr << "[ERROR]: Receiving ConfigPacket failed: " << std::strerror(errno) << std::endl;
			return (int)errno;
		}
		else if (packetBytes != sizeof(ConfigPacket))
		{
			std::cout << "[WARNING]: ConfigPacket not fully received." << std::endl;
		}
		else
		{
			std::cout << "[MESSAGE]: ConfigPacket received with parameters: width = "
				<< packet->imageWidth << " height = " << packet->imageHeight << std::endl;
		}

		return 0;
	}

	int JpgReceiveSaveUtils::receiveImage(int socketfd, unsigned int *imageBuffer, const int imageSize)
	{
		unsigned char* bufferPtr = (unsigned char*)imageBuffer;

		int imageSizeInBytes = imageSize * sizeof(unsigned int);
		int currRecvBytes = 0;
		int lastByteIdx = 0;
		while (true)
		{
			currRecvBytes = recv(socketfd, bufferPtr + lastByteIdx, imageSizeInBytes - lastByteIdx, 0);
			if (currRecvBytes == -1)
			{
				std::cerr << "[ERROR]: Receiving image failed: " << std::strerror(errno) << std::endl;
				return (int)errno;
			}
			else if (currRecvBytes == 0)
			{
				std::cout << "No bytes left to receive." << std::endl;
				break;
			}

			lastByteIdx += currRecvBytes;
			if (lastByteIdx > imageSizeInBytes)
			{
				std::cout << "[WARNING]: Byte index exceeded buffer size by "
						  << imageSizeInBytes - lastByteIdx << " bytes." << std::endl;
			}
			else
			{
				std::cout << "[MESSAGE]: Received " << lastByteIdx << "/" << imageSizeInBytes
						  << " bytes..." << std::endl;
			}
		}

		memcpy(imageBuffer, bufferPtr, imageSizeInBytes);
		std::cout << "[MESSAGE]: Done." << std::endl;

		return 0;
	}

	unsigned int *JpgReceiveSaveUtils::littleToBigEndian(unsigned int *data, const int length)
	{
		for (int i = 0; i < length; i++)
		{
			data[i] = __builtin_bswap32(data[i]);
		}
		return data;
	}

	bool JpgReceiveSaveUtils::trySaveToJpg(const char *dir, const int width, const int height, unsigned int *pixels, int quality)
	{
		time_t rawtime;
		struct tm *timeinfo;
		char name[21]; // YYYY-MM-DD--HH-MM-SS + null-character

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		if (strftime(name, 21, "%F--%H-%M-%S", timeinfo) == 0)
		{
			std::cerr << "[ERROR]: Failed to create time string." << std::endl;
			return false;
		}

		char destination[strlen(dir) + 21 + 4];
		strcpy(destination, dir);
		strcat(destination, name);
		strcat(destination, ".jpg");

		if (stbi_write_jpg(destination, width, height, 4, pixels, quality) == 0)
		{
			std::cerr << "[ERROR]: Failed to save jpg file." << std::endl;
			return false;
		}

		return true;
	}
}
