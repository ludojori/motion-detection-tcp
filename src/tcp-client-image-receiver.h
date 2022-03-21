#ifndef TCP_CLIENT_IMAGE_RECEIVER
#define TCP_CLIENT_IMAGE_RECEIVER
#include "tcp-protocol.h"

namespace motion_detection
{
	class TcpClientImageReceiver
	{
	public:
		static int receive_and_save(int socketfd, const char* dir);

	private:
		static int receive_config_packet(int socketfd, ConfigPacket* packet);
		static int receive_segments(int socketfd, unsigned int** segments, int segment_count, int remainder = 0);
		static unsigned int* little_to_big_endian(unsigned int* data, const int& length);
		static bool try_save_to_jpg(const char* dir, const int width, const int height, unsigned int* pixels);
	};
}

#endif /* TCP_CLIENT_IMAGE_RECEIVER */
