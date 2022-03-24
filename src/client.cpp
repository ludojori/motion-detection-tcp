#include <iostream>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "../include/motion-detection-tcp/jpg-receive-save-utils.h"

using namespace motion_detection;

int main(int argc, char** args)
{
    if (argc != 4)
    {
        std::cerr << "[ERROR]: Invalid parameters. Parameters are: [server_addr] [port] [sens_threshold]. Exiting." << std::endl;
        return -1;
    }
    errno = 0;

    hostent* host = gethostbyname(args[1]);

    std::cout << "[MESSAGE]: Creating socket..." << std::endl;

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "[ERROR]: Function socket() failed with error code: " << std::strerror(errno) << std::endl;
        return (int)errno;
    }
    errno = 0;

    std::cout << "[MESSAGE]: Assigning server address..." << std::endl;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((u_short)strtoul(args[2], NULL, 0));
    memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

	std::cout << "[MESSAGE]: Connecting to server..." << std::endl;

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "[ERROR]: Function connect() failed with error code: " << std::strerror(errno) << std::endl;
        close(clientSocket);
        return (int)errno;
    }
    errno = 0;

    std::cout << "[MESSAGE]: Connection accepted." << std::endl;
    
    uint64_t sensitivity = atoi(args[3]);
    int sensBytes = send(clientSocket, &sensitivity, sizeof(sensitivity), 0);
    if (sensBytes == -1)
    {
        std::cerr << "[ERROR]: Failed to send sensitivity threshold with error code: " << std::strerror(errno) << std::endl;
        close(clientSocket);
        return (int)errno;
    }
    else if (sensBytes != sizeof(sensitivity))
    {
    	std::cout << "[WARNING]: Partially sent sensitivity threshold." << std::endl;
    }
    errno = 0;
    
    JpgReceiveSaveUtils utils;  
    int errorStatus = utils.receiveAndSave(clientSocket, "../../images/", 50);
    if (errorStatus != 0)
    {
    	return errorStatus;
    }
    errno = 0;

	if (close(clientSocket) == -1)
	{
		std::cerr << "[ERROR]: Function close() failed with error code: " << std::strerror(errno) << std::endl;
		return (int)errno;
	}

	std::cout << "[MESSAGE]: Client shutting down." << std::endl;
	
    return 0;
}
