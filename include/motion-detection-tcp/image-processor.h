#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "tcp-protocol.h"

#include <iostream>
#include <cstring>

class ImageProcessor
{
private:
    uint64_t oldPictureAverage;

public:
    uint64_t getAveragePixels(unsigned char *bytePixels, size_t arraySizeInBytes);
    uint64_t calculatePictureDifference(unsigned char *bytePixels, size_t arraySizeInBytes);
    ImageProcessor();
};

#endif /* IMAGE_PROCESSOR_H */