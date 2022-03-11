#ifndef TCP_PROTOCOL_H
#define TCP_PROTOCOL_H

#define SIZE_OF_BYTES_PACKAGE 10000 //should be divisible by 4

namespace motion_detection
{
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

	struct Packet
	{
		Packet(Status status, int width, int height)
			:status(status), width(width), height(height) {}
		Packet() = default;
		Status status;
		int width;
		int height;
	};
}

#endif /* TCP_PROTOCOL_H */
