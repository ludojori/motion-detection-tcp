#include "common.h"
#include "protocol.h"
#include <thread>
/* 
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h> */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

//server accepts char messages
bool send_sensitivity(int socketID, uint64_t& sensitivity)
{
    std::cout << sensitivity << std::endl;
    int sending = send(socketID, &sensitivity, SENSITIVITY_LENGTH, 0);
    if (sending == -1)
    {
        return false;
    }
    
    return true;
}


void sendingTCP (int sockID, Request request)
{
	if(request.command == Command::MESSAGE)
	{
		std::cout << "Message: " << request.data << std::endl;
		send(sockID, &request.command, sizeof(request.command), 0);
		int len = std::strlen(request.data) + 1;
		send(sockID, &len, sizeof(len), 0);
		send(sockID, request.data, len, 0);
	}
}

void disconnect()
{
	//Send disconnect message and server checks if the message is not disconnect, than it removes the client
    //close(sockID)
}

int main(int argc, char **args)
{
	if(argc != 3) {
		return -1;
	}

	int sockID = 0;

    uint64_t sens; //optimise or get a variable
    sens = atoi(args[2]); 
    
	hostent *host = gethostbyname(args[1]);

	sockID = socket(AF_INET, SOCK_STREAM, 0);
	// server address
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(5588);
	std::memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

    std::cout <<"Sensitivity: " << sens << std::endl;

	connect(sockID, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    std::cout <<"Sensitivity: " << sens << std::endl;
	//std::cout << "before send\n";
	send_sensitivity(sockID, sens);
	//std::cout << "after send\n";

   /*  char buffer[1000000000];
    while (true)
    {
        int bytesReceived = recv(socketID, buffer, 10000, 0);
    } */ 
    close(sockID); 
}
