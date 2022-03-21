#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#define IMAGE_SEGMENT_SIZE 100 // 100 RGBA pixels = 400 bytes

class ImageUtils
{
	public:
	static void separate(unsigned int* image, int image_size, unsigned int** segments);
	static void construct(unsigned int** segments, int image_size, unsigned int* image);

	private:
};

#endif /* IMAGE_UTILS_H */
