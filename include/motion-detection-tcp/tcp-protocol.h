#ifndef TCP_PROTOCOL_H
#define TCP_PROTOCOL_H

namespace motion_detection
{
	enum class Status : char
	{
		STILL_IMAGE = 0,
		MOTION_DETECTED = 1
	};

	struct ConfigPacket
	{
		int imageWidth;
		int imageHeight;
	};
}

#endif /* TCP_PROTOCOL_H */
