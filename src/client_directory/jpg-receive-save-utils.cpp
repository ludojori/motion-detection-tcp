#include "jpg-receive-save-utils.h"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "../../libs/third-party/stb-image-write/stb_image_write.h"
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
	int JpgReceiveSaveUtils::receive_and_save(int socketfd, const char *dir)
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
				continue;
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
				int image_size = packet.fullImageWidth * packet.fullImageHeight;
				unsigned int *image_buffer = new unsigned int[image_size]{0};

				std::cout << "[MESSAGE]: Receiving image..." << std::endl;
				int image_error_status = receive_image(socketfd, image_buffer, image_size);
				if (image_error_status != 0)
				{
					return image_error_status;
				}

				std::cout << "[MESSAGE]: Fixing image endianess..." << std::endl;
				little_to_big_endian(image_buffer, image_size);

				std::cout << "[MESSAGE]: Saving image..." << std::endl;
				if (!try_save_to_jpg(dir, packet.fullImageWidth, packet.fullImageHeight, image_buffer))
				{
					return -1;
				}
<<<<<<< HEAD
				
				delete[] image_buffer;

=======
>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c
				std::cout << "[MESSAGE]: Done." << std::endl;
			}
		}
		return 0;
	}

	int JpgReceiveSaveUtils::receive_config_packet(int socketfd, ConfigPacket *packet)
	{
		int packet_bytes = recv(socketfd, packet, sizeof(ConfigPacket), 0);

		if (packet_bytes == -1)
		{
			std::cerr << "[ERROR]: Receiving ConfigPacket failed: " << std::strerror(errno) << std::endl;
			return (int)errno;
		}
		else if (packet_bytes != sizeof(ConfigPacket))
		{
			std::cout << "[WARNING]: ConfigPacket not fully received." << std::endl;
		}
		else
		{
			std::cout << "[MESSAGE]: Config packet received." << std::endl;
		}
		std::cout << "[MESSAGE]: Parameters are: width = " << packet->fullImageWidth
				  << " height = " << packet->fullImageHeight
				  << " segment_count = " << packet->imageSegmentCount << std::endl;

		return 0;
	}

	int JpgReceiveSaveUtils::receive_image(int socketfd, unsigned int *image_buffer, const int image_size)
	{
		int image_size_in_bytes = image_size * sizeof(unsigned int);
<<<<<<< HEAD
		unsigned char* temp_buffer = (unsigned char*)image_buffer;
=======
		unsigned char temp_buffer[image_size_in_bytes];
>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c

		int curr_recv_bytes = 0;
		int last_byte_idx = 0;
		while (true)
		{
			curr_recv_bytes = recv(socketfd, temp_buffer + last_byte_idx, image_size_in_bytes - last_byte_idx, 0);
			if (curr_recv_bytes == -1)
			{
				std::cerr << "[ERROR]: Receiving image failed: " << std::strerror(errno) << std::endl;
				return (int)errno;
			}
			else if (curr_recv_bytes == 0)
			{
				std::cout << "Nothing left to receive." << std::endl;
				break;
			}

			last_byte_idx += curr_recv_bytes;
			if (last_byte_idx > image_size_in_bytes)
			{
				std::cout << "[WARNING]: Byte index exceeded buffer size by "
						  << image_size_in_bytes - last_byte_idx << " bytes." << std::endl;
			}
			else
			{
				std::cout << "[MESSAGE]: Received " << last_byte_idx << "/" << image_size_in_bytes
						  << " bytes..." << std::endl;
			}
		}

		std::cout << "[MESSAGE]: Done." << std::endl;
<<<<<<< HEAD
=======
		memcpy(image_buffer, temp_buffer, image_size_in_bytes);
>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c

		return 0;
	}

	unsigned int *JpgReceiveSaveUtils::little_to_big_endian(unsigned int *data, const int length)
	{
		for (int i = 0; i < length; i++)
		{
			data[i] = __builtin_bswap32(data[i]);
		}
		return data;
	}

	bool JpgReceiveSaveUtils::try_save_to_jpg(const char *dir, const int width, const int height, unsigned int *pixels)
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

		if (stbi_write_jpg(destination, width, height, 4, pixels, 50) == 0)
		{
			std::cerr << "[ERROR]: Failed to save jpg file." << std::endl;
			return false;
		}

		return true;
	}
}
