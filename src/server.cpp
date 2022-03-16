#include "../libs/motion-detection/tcp_protocol.h"
#include "../libs/third-party/camera/camera.h"
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

#include <math.h>

using namespace motion_detection;
using namespace adafruit::bbio;

#define IMAGE_SEGMENT_SIZE 100

const int MAX_CLIENTS = 10;

int oldPictureAverage = 0;
std::unordered_map<int, uint64_t> sockIDSensitivity;

std::mutex mutex;

void registerClient(int sockID, uint64_t sensitivity)
{
	mutex.lock();
	std::cout << "Locked";
	sockIDSensitivity.insert({sockID, sensitivity});
	mutex.unlock();
	std::cout << "Unlocked";
}

void disconnectClient(const int sockID, bool shouldCloseSocket = false)
{
	sockIDSensitivity.erase(sockID);
	if (shouldCloseSocket)
	{
		close(sockID);
	}
	std::cout << "[MESSAGE]: A client has disconnected. Remaining clients: " << sockIDSensitivity.size() << std::endl;
}

void read_sensitivity(int sockID)
{
	uint64_t sens;
	const int SENSITIVITY_LENGTH = sizeof(uint64_t);
	int sens_bytes = recv(sockID, &sens, SENSITIVITY_LENGTH, 0);

	if (sens_bytes == -1)
	{
		std::cerr << "[ERROR]: Sensitivity receival failed: " << std::strerror(errno) << std::endl;
		return;
	}
	else if (sens_bytes != sizeof(sens))
	{
		std::cout << "[WARNING]: Partially received sensitivity threshold." << std::endl;
	}

	std::cout << "Before register: sens = " << sens << std::endl;
	registerClient(sockID, sens);
	mutex.lock();
	std::cout << "Sensitivity: " << sens << " from map: " << sockIDSensitivity.at(sockID) << std::endl;
	mutex.unlock();
	// TODO: Check if picture is changed
}

bool isSocketConnected(int sockID)
{
	int error = 0;
	socklen_t len = sizeof(error);
	int retval = getsockopt(sockID, SOL_SOCKET, SO_ERROR, &error, &len);

	if (retval == 0 && error != 0)
	{
		std::cerr << "[ERROR]: Client socket failed: " << std::strerror(error) << std::endl;
		return false;
		// TODO: Handle error
		// disconnectClient();
		//  close(sockID);
	}
	return true;
}

void sendPicture(int sockID, unsigned int *fullImage)
{
	int height, width;
	getResolution(&width, &height);
	int sizeOfPicture = height * width;

	unsigned int *imageSegment = new unsigned int[IMAGE_SEGMENT_SIZE]{0};
	int segmentCount = sizeOfPicture / IMAGE_SEGMENT_SIZE;
	std::cout << "Image segment count: " << segmentCount << std::endl;
	
	char statusByte = static_cast<char>(Status::MOTION_DETECTED);
	if (send(sockID, &statusByte, 1, 0) == -1)
	{
		std::cerr << "[ERROR]: Sending status byte failed: " << std::strerror(errno) << std::endl;
		// handle error
	}

	if (!isSocketConnected(sockID))
	{
		disconnectClient(sockID);
	}

	struct ConfigPacket packet;
	packet.fullImageWidth = width;
	packet.fullImageHeight = height;
	packet.imageSegmentCount = segmentCount;

	int packet_bytes = send(sockID, &packet, sizeof(ConfigPacket), 0);
	if (packet_bytes == -1)
	{
		std::cerr << "[ERROR]: Sending ConfigPacket failed: " << std::strerror(errno) << std::endl;
		// handle error
	}
	else if (packet_bytes != sizeof(ConfigPacket))
	{
		std::cout << "[WARNING]: ConfigPacket not fully sent." << std::endl;
	}
	// Send segments

	int segmentBytes = IMAGE_SEGMENT_SIZE * sizeof(unsigned int);
	int index = 0;
	int sending = 0;

	for (int i = 0; i < segmentCount; i++)
	{
		memcpy(imageSegment, fullImage + index, segmentBytes);
		sending = send(sockID, imageSegment, segmentBytes, 0);
		if (sending == -1)
		{
			std::cerr << "[ERROR]: Sending image segment failed: " << std::strerror(errno) << std::endl;
			// TODO: exception handling
		}
		if (sending != segmentBytes)
		{
			std::cout << "[WARNING]: Segment package No." << i + 1 << " not fully sent." << std::endl;
		}
		index += IMAGE_SEGMENT_SIZE;
	}

	// Send remaining bytes

	int remainingBytes = (sizeOfPicture % IMAGE_SEGMENT_SIZE) * sizeof(unsigned int);
	if (remainingBytes > 0)
	{
		std::cout << "[MESSAGE]: Sending remaining " << remainingBytes << " bytes..." << std::endl;
		memcpy(imageSegment, fullImage + index, remainingBytes);
		sending = send(sockID, imageSegment, remainingBytes, 0);
		if (sending == -1)
		{
			std::cerr << "[ERROR]: Sending remaining image segment failed: " << std::strerror(errno) << std::endl;
		}
		else if (sending != remainingBytes)
		{
			std::cout << "[WARNING]: Remaining segment not fully sent." << std::endl;
		}
	}
	delete[] imageSegment;
}

unsigned char *convertPixelsToBytes(unsigned int *pixels, int pixelsCount)
{
	// todo add charPixels argument + pixelsCount and make function void

	unsigned char *charPixels = new unsigned char[pixelsCount * 4];
	charPixels = (unsigned char *)pixels;
	return charPixels;
}

uint64_t getAveragePixels(unsigned char *bytePixels, size_t arraySizeInBytes)
{
	int sum = 0;
	uint64_t av;
	for (size_t i = 0; i < arraySizeInBytes; i++)
	{
		if (i % 3 == 0) // alpha values
		{
			continue;
		}
		sum += bytePixels[i];
	}
	std::cout << "sum: " << sum << std::endl;
	std::cout << "arraySizeInBytes: " << arraySizeInBytes << std::endl;
	av = sum / (arraySizeInBytes / 4);
	return av;
}

uint64_t calculatePictureDifference(unsigned char *bytePixels, size_t arraySizeInBytes)
{
	// TODO
	// shoould store old pic average value
	uint64_t newPictureAverage = getAveragePixels(bytePixels, arraySizeInBytes); // average needs bytes
	uint64_t diff = abs(static_cast<long long>(oldPictureAverage) - static_cast<long long>(newPictureAverage));
	oldPictureAverage = newPictureAverage;
	std::cout << "[MESSAGE]: Difference: " << diff << std::endl;
	return diff;
}

void notifyClients(unsigned int *pixels, int pixelsCount)
{
	size_t sizeInBytes = pixelsCount * sizeof(int);
	uint64_t diff = calculatePictureDifference(convertPixelsToBytes(pixels, pixelsCount), sizeInBytes);

	// check all clients if they should be notified
	mutex.lock();
	for (auto &i : sockIDSensitivity)
	{
		if (i.second <= diff)
		{
			sendPicture(i.first, pixels);
		}
	}
	mutex.unlock();
	std::cout << "Clients checked" << std::endl;
}

void checkConnectedClients()
{
	while (true)
	{
		mutex.lock();
		for (auto &i : sockIDSensitivity)
		{
			char statusByte = static_cast<char>(Status::STILL_IMAGE);
			if (send(i.first, &statusByte, 1, 0) == -1)
			{
				std::cerr << "[ERROR]: Sending status byte failed: " << std::strerror(errno) << std::endl;
				disconnectClient(i.first, true);
			}

			if (!isSocketConnected(i.first))
			{
				disconnectClient(i.first);
			}
		}
		mutex.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(3)); // check every 3 seconds if someone has disconnected
	}
}

void changePic()
{
	init(lib_options(LOG_DEBUG, nullptr, LOG_PERROR));
	Gpio gpio("P8_43", Gpio::Direction::Input);
	Gpio::Value position;

	while (true)
	{
		position = gpio.get_value();
		if (position == Gpio::Value::Low)
		{
			generateMovement();
			std::cout << "Picture changed!" << std::endl;
			int height, width;
			getResolution(&height, &width);
			int pixelsCount = height * width;

			unsigned int *pixels = new unsigned int[pixelsCount];

			getCurrentImage(pixels);
			notifyClients(pixels, pixelsCount);
			delete[] pixels;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
}

int main(int argc, char **args)
{
	if (argc != 2)
	{
		std::cerr << "[ERROR]: Missing parameter [port]. Exiting." << std::endl;
		return -1;
	}

	int sockID = 0;

	int listener = socket(PF_INET, SOCK_STREAM, 0);
	if (listener == -1)
	{
		std::cerr << "[ERROR]: Function socket() failed: " << std::strerror(errno) << std::endl;
	}

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)strtoul(args[1], NULL, 0));
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
	std::thread change_pic_thread(changePic);
	std::thread check_connected_clients_thread(checkConnectedClients);
	while (true)
	{
		if (sockIDSensitivity.size() >= MAX_CLIENTS)
		{
			continue;
			// wait until disconnect
		}
		std::cout << "Listining" << std::endl;
		sockaddr_in cliAddr;
		socklen_t len = sizeof(cliAddr);
		sockID = accept(listener, (struct sockaddr *)&cliAddr, &len);

		if (sockID == -1)
		{
			std::cerr << "[ERROR]: Function accept() failed: " << std::strerror(errno) << std::endl;
			return (int)errno;
		}
		std::cout << "Connection accepted. SockID: " << sockID << std::endl;

		threads.emplace_back(read_sensitivity, sockID);
		std::cout << "Number of connections accepted " << sockIDSensitivity.size() << std::endl;
	}
	change_pic_thread.join();
	check_connected_clients_thread.join();
}
