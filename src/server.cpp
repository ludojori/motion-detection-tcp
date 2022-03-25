#include "../include/motion-detection-tcp/server-communication.h"
#include "../include/motion-detection-tcp/linux-send-receive.hpp"

int main(int argc, char **args)
{
	if (argc != 2)
	{
		std::cerr << "[ERROR]: Missing parameter [port]. Exiting." << std::endl;
		return -1;
	}
	LinuxSendReceive communication;
	ServerCommunication server(communication); //default button constr
	server.initServer(args[1]);

	return 0;
}
