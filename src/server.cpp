#include "../libs/motion-detection/tcp_protocol.h"
#include "../libs/third-party/camera/camera.h"
#include "adafruit/bbio.h"	

#include <iostream>	

#include <thread>
#include <mutex>

#include <vector> 
#include <queue> 
#include <unordered_map> 

#include <netdb.h>
#include <netinet/in.h>

using namespace motion_detection;

using namespace adafruit::bbio;

const int MAX_CLIENTS = 10; 
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

void read_sensitivity(int sockID)
{
	uint64_t sens;
	const int SENSITIVITY_LENGTH = sizeof(uint64_t);
	int r_size = recv(sockID, &sens, SENSITIVITY_LENGTH, 0);
	if(r_size < SENSITIVITY_LENGTH) {
		return;
	}
	std::cout << "Before register: sens = " << sens << std::endl;
	registerClient(sockID, sens);
	mutex.lock();
	std::cout << "Sensitivity: " << sens << " from map: " << sockIDSensitivity.at(sockID) << std::endl;
	mutex.unlock();
	//TODO: Check if picture is changed
}

 void sendPicture(int sockID, unsigned int* pixels)
{
	    int height, width;
		getResolution(&height, &width);
		struct Packet packet(Status::MOTION_DETECTED, height, width);
		send(sockID, &packet, sizeof(packet), 0);
		int sizeOfPictureInBytes = height * width * sizeof(unsigned int);
		int numberOfPackages = sizeOfPictureInBytes/SIZE_OF_BYTES_PACKAGE;
        std::cout << "Image buffer size in bytes: " << sizeOfPictureInBytes << std::endl;
		for(int i = 0; i < numberOfPackages; i++)
		{
			std :: cout << "Sent pixels: " << i * send(sockID, pixels, SIZE_OF_BYTES_PACKAGE, 0) << std::endl;	
			pixels += SIZE_OF_BYTES_PACKAGE / sizeof(unsigned int);
		}
		int lastPackageSize = sizeOfPictureInBytes % SIZE_OF_BYTES_PACKAGE; 
		std :: cout << "Sent last pixels: " << send(sockID, pixels, lastPackageSize, 0) << std::endl;	

} 

unsigned char* convertPixelsToBytes(unsigned int* pixels)
{
	int height, width;
	getResolution(&height, &width);
	int pixelsCount = height * width;
	
	unsigned char* charPixels = new unsigned char[pixelsCount * 4];
	charPixels = (unsigned char *) pixels;	
	return charPixels;
}

uint64_t getAveragePixels(unsigned char* bytePixels, size_t arraySizeInBytes)
{
	int sum = 0;
	uint64_t av;
	for (size_t i = 0; i < arraySizeInBytes; i++)
	{
		if(i % 3 == 0) //alpha values
		{
			continue;
		}
		sum += bytePixels[i];
	}
	av = sum / (arraySizeInBytes / 4); 
	return av;
}

uint64_t calculatePictureDifference(unsigned char* bytePixels)
{
	//TODO
	//shoould store old pic average value
	return 120;		
}

void checkClients(unsigned int* pixels)
{
	uint64_t diff = calculatePictureDifference(convertPixelsToBytes(pixels));

	//check all clients if they should be notified
	mutex.lock();
	for (auto& i : sockIDSensitivity)
	{
		if(i.second >= diff)
		{
			sendPicture(i.first, pixels);
		}
	}
	mutex.unlock();
	std::cout << "Clients checked" << std::endl;
}

void disconnectClient(int sockID, int& clientsCount)
{
	//TODO
	clientsCount--; 
}

void changePic()
{
	init(lib_options(LOG_DEBUG, nullptr, LOG_PERROR));
	Gpio gpio("P8_43", Gpio::Direction::Input);
    Gpio::Value position;

	while (true)
	{
		position = gpio.get_position();
		if (position == Gpio::Value::Low)
		{
			std::cout << "Pressed button" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	generateMovement();
	std::cout << "Picture changed!" << std::endl;
	int height, width;
	getResolution(&height, &width);
	int pixelsCount = height * width;
	
	unsigned int* pixels = new unsigned int[pixelsCount];

	for (size_t i = 0; i < pixelsCount; i++)
	{
		pixels[i] = 0;
	}
	
	//getCurrentImage(pixels);	
	checkClients(pixels);
	delete[] pixels;
}

int main(int argc, char** args)
{
	int clientsCount = 0;
    int sockID = 0;

        socklen_t addr_size;

        int listener = socket(PF_INET, SOCK_STREAM, 0);

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons((u_short)strtoul(args[1], NULL, 0));
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        bind(listener, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

        if(listen(listener, 110) != 0)
        {
            return -1;
        }
		
		std::vector<std::thread> threads;
		std::thread t1(changePic);
		while(true)
		{
			if(clientsCount >= MAX_CLIENTS)
			{
				continue;
				//wait until disconnect
			}
			std::cout << "Listining" << std::endl;
			sockaddr_in cliAddr;
			socklen_t len = sizeof(cliAddr);
			sockID = accept(listener, (struct sockaddr *) &cliAddr, &len);
			std::cout << "Connection accepted. SockID: " << sockID << std::endl;
			threads.emplace_back(read_sensitivity, sockID);
			clientsCount++; 
			std::cout << "Number of connections accepted " << threads.size() << std::endl;
		}
		t1.join();
}
