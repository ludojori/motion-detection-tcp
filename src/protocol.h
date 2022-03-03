#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdint.h>

#define MAX_IMAGE_RESOLUTION 1000000;

enum class Error : int
{
	NONE,
	PARAMCOUNT,
	PARAMFORMAT,
	SOCKET,
	BIND,
	CONNECTION,
	SEND,
	RECEIVE
};

enum class Status : char
{
	STILL_IMAGE = 0,
	MOTION_DETECTED = 1
};

struct ServerNotification
{
	Status status;
	uint32_t width;
	uint32_t height;
	char* image;
};

#endif
