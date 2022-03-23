#ifndef JPG_RECEIVE_SAVE_UTILS
#define JPG_RECEIVE_SAVE_UTILS
#include "tcp-protocol.h"

namespace motion_detection
{
	class JpgReceiveSaveUtils
	{
	public:
		static int receive_and_save(int socketfd, const char* dir);

	private:
		static int receive_config_packet(int socketfd, ConfigPacket* packet);
		static int receive_image(int socketfd, unsigned int* image_buffer, const int image_size);
		static unsigned int* little_to_big_endian(unsigned int* data, const int length);
		static bool try_save_to_jpg(const char* dir, const int width, const int height, unsigned int* pixels);
	};
}

#endif /* JPG_RECEIVE_SAVE_UTILS */
