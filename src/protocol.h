#ifndef PROTOCOL_H
#define PROTOCOL_H

enum class Error : int
{
	NONE,
	PARAMCOUNT,
	PARAMFORMAT,
	SOCKET,
	BIND,
	CONNECTION,
	SEND,
	RECEIVE,
	CLOSE
};

enum class Status : char
{
	STILL_IMAGE = 0,
	MOTION_DETECTED = 1
};

struct ServerNotification
{
	Status status;
	int width;
	int height;
	unsigned int* image;
};

#endif
