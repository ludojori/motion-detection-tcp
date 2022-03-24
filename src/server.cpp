#include "../include/motion-detection-tcp/server-communication.h"

int main(int argc, char **args)
{
	if (argc != 2)
	{
		std::cerr << "[ERROR]: Missing parameter [port]. Exiting." << std::endl;
		return -1;
	}
	ServerCommunication server; 
	server.initServer(args[1]);

	return 0;
}

