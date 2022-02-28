//#include <netdb.h>
//#include <netinet/in.h>
#include <iostream>
#include <socketapi.h>
#include <winsock.h>

#define SENS_LENGTH 8

bool send_sensitivity(int socketID, uint64_t sensitivity)
{
    char buffer[SENS_LENGTH];
    for (int i = SENS_LENGTH - 1; i >= 0; i--)
    {
        buffer[i] = (sensitivity >> i * SENS_LENGTH) & 0xFF;
    }

    int sending = send(socketID, buffer, SENS_LENGTH, 0);
    if (sending == ERROR)
    {
        return false;
    }

    return true;
}

int main(int argc, char** args)
{
    if (argc < 4)
    {
        std::cerr << "Invalid number of parameters." << std::endl;
        return 1;
    }

    hostent* host = gethostbyname(args[1]);

    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID == SOCKET_ERROR)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((u_short)strtoul(args[2], NULL, 0));
    std::memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

    int connection = connect(socketID, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connection == ERROR_NOT_CONNECTED)
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }

    uint64_t sensitivity = atoi(args[3]);
    if (!send_sensitivity(socketID, sensitivity))
    {
        std::cerr << "Failed to send sensitivity threshold." << std::endl;
        return 1;
    }

    char input = ' ';
    while (input != 'q')
    {
        std::cin >> input;
    }

    closesocket(socketID);

    return 0;
}