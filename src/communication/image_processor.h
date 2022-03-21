#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H


#include "../../libs/motion-detection/tcp_protocol.h"
#include "../../libs/third-party/camera/camera.h"
#include "adafruit/bbio.h"

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
    int oldPictureAverage;

public:
    uint64_t getAveragePixels(unsigned char *bytePixels, size_t arraySizeInBytes);
    uint64_t calculatePictureDifference(unsigned char *bytePixels, size_t arraySizeInBytes);
    ImageProcessor() = default;
};

#endif /* IMAGE_PROCESSOR_H */