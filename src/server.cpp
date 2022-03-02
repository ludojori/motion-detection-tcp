#include "protocol.h"
#include "common.h"
#include "../camera/camera.h"

#include <thread>
#include <vector> 
#include <queue> 
#include <unordered_map> 

#include <netdb.h>
#include <netinet/in.h>

const int MAX_CLIENTS = 10; 
std::unordered_map<int, int> sockIDSensitivity;



/* https://www.delftstack.com/howto/cpp/how-to-determine-if-a-string-is-number-cpp/#use-std-isdigit-method-to-determine-if-a-string-is-a-number */
bool isNumber(const char* str)
{
	int length = strlen(str);
    for (int i = 0; i < length; i++) 
	{
        if (std::isdigit(str[i]) == 0) return false;
    }
    return true;
}

void read_sensitivity(int sockID)
{
	uint64_t sens;
	int r_size = recv(sockID, &sens, SENSITIVITY_LENGTH, 0);
	if(r_size < SENSITIVITY_LENGTH) {
		return;
	}
	sockIDSensitivity.insert({sockID, sens});
}

/* void read_file(int sockID)
{
	int len = 0;
	int r_size = recv(sockID, &len, sizeof(len), 0);
	if(r_size < sizeof(len)) {
		return;
	}

	char *filename = new char[len];

	recv(sockID, filename, len, 0);

	std::ofstream file(filename, std::ios::binary);

	int file_length = 0;
	recv(sockID, &file_length, sizeof(file_length), 0);

	uint8_t *content = new uint8_t[file_length];
	recv(sockID, content, file_length, 0);

	file.write((const char*)content, file_length);

	std::cout << "File received: " << filename << std::endl;

	delete [] filename;
	delete [] content;
}
 */

 void sendPicture(int sockID, unsigned int* pixels)
{
		int height, width;
		getResolution(&height, &width);
		int pixelsCount = height * width;
		send(sockID, pixels, pixelsCount * sizeof(unsigned int), 0);
} 

unsigned char* convertPixelsToBytes(unsigned int* pixels)
{
	int height, width;
	getResolution(&height, &width);
	int pixelsCount = height * width;
	
	unsigned char* charPixels = new unsigned 	char[pixelsCount * 4];
	charPixels = (unsigned char *) pixels;	
	return charPixels;
}

uint64_t calculatePictureDifference(unsigned char* bytePixels)
{
	return 0;		
}

void checkClients(unsigned int* pixels)
{
	uint64_t diff = calculatePictureDifference(convertPixelsToBytes(pixels));
	//check all clients if they should be notified
	for (auto& i : sockIDSensitivity	)
	{
		if(i.second >= diff)
		{
			sendPicture(i.first, pixels);
		}
	}
}

void disconnectClient(int sockID, int& clientsCount)
{
	clientsCount--; 
}


int main(int argc, char** args)
{
	int clientsCount = 0;
    int sockID = 0;
	
        socklen_t addr_size;

        int listener = socket(PF_INET, SOCK_STREAM, 0);

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5588);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        bind(listener, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

        if(listen(listener, 110) != 0)
        {
            return -1;
        }
		
		std::vector<std::thread> threads;
		while(clientsCount < MAX_CLIENTS)
		{
			std::cout << "Listining" << std::endl;
			sockaddr_in cliAddr;
			socklen_t len = sizeof(cliAddr);
			sockID = accept(listener, (struct sockaddr *) &cliAddr, &len);
			std::cout << "Connection accepted. SockID: " << sockID << std::endl;
			// read_sensitivity(sockID);
			threads.emplace_back(read_sensitivity, sockID);
			clientsCount++; 
			std::cout << "Number of connections accepted " << threads.size() << std::endl;
		}
}
