#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <cstdint>

enum class Status : char
{
	STILL_IMAGE, MOTION_DETECTED
};

struct ServerNotification
{
	Status status;
	uint32_t width;
	uint32_t height;
	char image[1000000];
};

#endif