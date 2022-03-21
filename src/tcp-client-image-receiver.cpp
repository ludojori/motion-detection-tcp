#include "tcp-client-image-receiver.h"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "../libs/third-party/stb-image-write/stb_image_write.h"
#include "image-utils.h"
#ifdef DEBUG
#define DEBUG_PRINT(type, msg)							\
	if (type == "m")		std::cout << "[MESSAGE]: ";	\
	else if (type == "w")	std::cout << "[WARNING]: ";	\
	else if (type == "e")	std::cout << "[ERROR]: ";	\
	std::cout << msg << std::endl;						
#endif

namespace motion_detection
{
	int TcpClientImageReceiver::receive_and_save(int socketfd, const char* dir)
	{
		char status_byte = ' ';
		while (true)
		{
			// Receive status byte

			if (recv(socketfd, &status_byte, 1, 0) == -1)
			{
				std::cerr << "[ERROR]: Receiving status byte failed: " << std::strerror(errno) << std::endl;
				return (int)errno;
			}

			if (status_byte == (char)Status::STILL_IMAGE)
			{
				std::cout << "[MESSAGE]: Received status byte STILL_IMAGE." << std::endl;
			}
			else if (status_byte == (char)Status::MOTION_DETECTED)
			{
				std::cout << "[MESSAGE]: Received status byte MOTION_DETECTED." << std::endl;
				std::cout << "[MESSAGE]: Attempting to receive config packet..." << std::endl;

				// Receive ConfigPacket

				ConfigPacket packet;
				int config_packet_error_status = receive_config_packet(socketfd, &packet);
				if (config_packet_error_status != 0)
				{
					return config_packet_error_status;
				}

				std::cout << "[MESSAGE]: Declaring local variables..." << std::endl;
				int full_image_buffer_size = packet.fullImageWidth * packet.fullImageHeight;
				unsigned int* full_image_buffer = new unsigned int[full_image_buffer_size] { 0 };
				
				int segment_count = full_image_buffer_size / IMAGE_SEGMENT_SIZE;
				int remaining_buffer_size = full_image_buffer_size % IMAGE_SEGMENT_SIZE;
				if (remaining_buffer_size != 0)
				{
					segment_count += 1;
				}

				unsigned int** segments = new unsigned int*[segment_count];
				for (int i = 0; i < segment_count; i++)
				{
					segments[i] = new unsigned int[IMAGE_SEGMENT_SIZE] { 0 };
				}
				
				std::cout << "[MESSAGE]: Receiving image segments..." << std::endl;

				// Receive segments
				
				receive_segments(socketfd, segments, segment_count, remaining_buffer_size);

				// Construct image from segments
				
				ImageUtils::construct(segments, full_image_buffer_size, full_image_buffer);

				std::cout << "[MESSAGE]: Fixing image endianess..." << std::endl;
				little_to_big_endian(full_image_buffer, full_image_buffer_size);

				std::cout << "[MESSAGE]: Saving image..." << std::endl;
				if (!try_save_to_jpg(dir, packet.fullImageWidth, packet.fullImageHeight, full_image_buffer))
				{
					return -1;
				}

				delete[] full_image_buffer;
				for (int i = 0; i < segment_count; i++)
				{
					delete[] segments[i];
				}
				delete[] segments;
			}

			return 0;
		}
	}

	int TcpClientImageReceiver::receive_config_packet(int socketfd, ConfigPacket* packet)
	{
			int packet_bytes = recv(socketfd, packet, sizeof(packet), 0);

			if (packet_bytes == -1)
			{
				std::cerr << "[ERROR]: Receiving ConfigPacket failed: " << std::strerror(errno) << std::endl;
				return (int)errno;
			}
			else if (packet_bytes != sizeof(ConfigPacket))
			{
				std::cout << "[WARNING]: ConfigPacket not fully received." << std::endl;
			}
						
			std::cout << "[MESSAGE]: Config packet received." << std::endl;
			std::cout << "[MESSAGE]: Parameters are: width = " << packet->fullImageWidth
					<< " height = " << packet->fullImageHeight
					<< " segment_count = " << packet->imageSegmentCount << std::endl;

			return 0;
	}

	int TcpClientImageReceiver::receive_segments(int socketfd, unsigned int** segments, int segment_count, int remainder)
	{
		int end_idx = segment_count;
		
		if (remainder != 0)
		{
			end_idx -= 1;
		}

		int received_bytes = 0;
		for (int i = 0; i < end_idx; i++)
		{
			received_bytes = recv(socketfd, segments[i], IMAGE_SEGMENT_SIZE, 0);
			if (received_bytes == -1)
			{
				std::cerr << "[ERROR]: Receiving segment #" << i << "failed: " << std::strerror(errno) << std::endl;
				return (int)errno;
			}
			else if (received_bytes != IMAGE_SEGMENT_SIZE)
			{
				std::cout << "[WARNING]: Segment #" << i << " not fully received." << std::endl;
			}
		}

		if (remainder != 0)
		{
			received_bytes = recv(socketfd, segments[segment_count], remainder, 0);
			if (received_bytes == -1)
			{
				std::cerr << "[ERROR]: Receiving remaining segment failed:" << std::strerror(errno) << std::endl;
				return (int)errno;
			}
			else if (received_bytes != remainder)
			{
				std::cout << "[WARNING]: Remaining segment not fully received." << std::endl;
			}
		}
	
		return 0;
	}

	unsigned int* TcpClientImageReceiver::little_to_big_endian(unsigned int* data, const int& length)
	{
		for (int i = 0; i < length; i++)
		{
			data[i] = __builtin_bswap32(data[i]);
		}
		return data;
	}

	bool TcpClientImageReceiver::try_save_to_jpg(const char* dir, const int width, const int height, unsigned int* pixels)
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

		char destination[strlen(dir) + 21 + 4];
		strcpy(destination, dir);
		strcat(destination, name);
		strcat(destination, ".jpg");

		if (stbi_write_jpg(destination, width, height, 4, pixels, 50) == 0)
		{
			std::cerr << "[ERROR]: Failed to save jpg file." << std::endl;
			return false;
		}

		return true;
	}
}
