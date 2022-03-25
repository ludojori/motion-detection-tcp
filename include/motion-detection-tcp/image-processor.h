#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "../third-party/camera/camera.h"
#include "tcp-protocol.h"

#include <iostream>
#include <cstring>

#include <thread>
#include <mutex>

#include <vector>
#include <queue>
#include <unordered_map>


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