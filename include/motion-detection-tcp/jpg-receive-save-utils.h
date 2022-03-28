#ifndef JPG_RECEIVE_SAVE_UTILS
#define JPG_RECEIVE_SAVE_UTILS
#include "tcp-protocol.h"

namespace motion_detection
{
	class JpgReceiveSaveUtils
	{
	private:
<<<<<<< Updated upstream
		int receiveConfigPacket(int socketfd, ConfigPacket* packet);
		int receiveImage(int socketfd, unsigned int* imageBuffer, const int imageSize);
		unsigned int* littleToBigEndian(unsigned int* data, const int length);
		bool trySaveToJpg(const char* dir, const int width, const int height, unsigned int* pixels, int quality);

=======
		virtual int receiveConfigPacket(int socketfd, ConfigPacket* packet);
		virtual int receiveImage(int socketfd, unsigned int* imageBuffer, const int imageSize);
		virtual unsigned int* littleToBigEndian(unsigned int* data, const int length);
		virtual bool trySaveToJpg(const char* dir, const int width, const int height, unsigned int* pixels, int quality);
		
>>>>>>> Stashed changes
	public:
		int receiveAndSave(int socketfd, const char* dir, int quality);
	};
}

#endif /* JPG_RECEIVE_SAVE_UTILS */
