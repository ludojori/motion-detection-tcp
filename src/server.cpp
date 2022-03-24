#include "../include/motion-detection-tcp/server-communication.h"
//#include "../include/motion-detection-tcp/tcp-protocol.h"
//#include "../include/third-party/camera/camera.h"
//#include "../include/motion-detection-tcp/image-processor.h"
//#include "adafruit/bbio.h"

int main(int argc, char **args)
{
	if (argc != 2)
	{
		std::cerr << "[ERROR]: Missing parameter [port]. Exiting." << std::endl;
		return -1;
	}
	ServerCommunication server; 
	server.initServer(args[1]);
}

