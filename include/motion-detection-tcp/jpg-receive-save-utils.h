#ifndef JPG_RECEIVE_SAVE_UTILS
#define JPG_RECEIVE_SAVE_UTILS
#include "tcp-protocol.h"

namespace motion_detection
{
	class JpgReceiveSaveUtils
	{
	private:
		virtual int receiveConfigPacket(int socketfd, ConfigPacket* packet) = 0;
		virtual int receiveImage(int socketfd, unsigned int* imageBuffer, const int imageSize) = 0;
		virtual unsigned int* littleToBigEndian(unsigned int* data, const int length) = 0;
		virtual bool trySaveToJpg(const char* dir, const int width, const int height, unsigned int* pixels, int quality) = 0;
		
	public:
		int receiveAndSave(int socketfd, const char* dir);
	};
}

#endif /* JPG_RECEIVE_SAVE_UTILS */