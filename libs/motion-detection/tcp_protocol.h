#ifndef TCP_PROTOCOL_H
#define TCP_PROTOCOL_H

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
		Status status;
		int width;
		int height;
	};
}

#endif /* TCP_PROTOCOL_H */
