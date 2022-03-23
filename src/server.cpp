#include "../libs/motion-detection/tcp_protocol.h"
#include "../libs/third-party/camera/camera.h"
#include "/home/debian/motion-detection-tcp/src/communication/server_communication.h"
#include "/home/debian/motion-detection-tcp/src/communication/image_processor.h"
#include "adafruit/bbio.h"

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

