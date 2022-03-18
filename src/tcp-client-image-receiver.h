#ifndef TCP_CLIENT_IMAGE_RECEIVER
#define TCP_CLIENT_IMAGE_RECEIVER

namespace motion_detection
{
	class TcpClientImageReceiver
	{
	public:

		TcpClientImageReceiver();
		TcpClientImageReceiver(const TcpClientImageReceiver& other) = delete;
		TcpClientImageReceiver& operator=(const TcpClientImageReceiver& other) = delete;
		~TcpClientImageReceiver();

		int receive_and_save(int socketfd, const char* save_directory);

	private:

		unsigned int* little_to_big_endian(unsigned int* data, const int& length);
		bool save_to_jpg(const int width, const int height, unsigned int* pixels);

		int socketID;
		char* save_directory;
	};
}

#endif /* TCP_CLIENT_IMAGE_RECEIVER */
