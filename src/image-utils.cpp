#include "image-utils.h"
#include <cstring>

void ImageUtils::separate(unsigned int* image, int image_size, unsigned int** segments)
{
	int segment_count = image_size / IMAGE_SEGMENT_SIZE;
	int remainder = image_size % IMAGE_SEGMENT_SIZE;
	int end_idx = segment_count;
		
	if (remainder != 0)
	{
		end_idx -= 1;
	}
	
	for (int i = 0; i < end_idx; i++)
	{
		memcpy(segments[i], image + (i * IMAGE_SEGMENT_SIZE), IMAGE_SEGMENT_SIZE);
	}

	if (remainder != 0)
	{
		memcpy(segments[end_idx], image + (end_idx * IMAGE_SEGMENT_SIZE), remainder);
	}
}

void ImageUtils::construct(unsigned int** segments, int image_size, unsigned int* image)
{
	int segment_count = image_size / IMAGE_SEGMENT_SIZE;
	int remainder = image_size % IMAGE_SEGMENT_SIZE;
	int end_idx = segment_count;
	
	if (remainder != 0)
	{
		end_idx -= 1;
	}
	
	for (int i = 0; i < end_idx; i++)
	{
		memcpy(image + (i * IMAGE_SEGMENT_SIZE), segments[i], IMAGE_SEGMENT_SIZE);
	}

	if (remainder != 0)
	{
		memcpy(image + (end_idx * IMAGE_SEGMENT_SIZE), segments[end_idx], remainder);
	}
}
