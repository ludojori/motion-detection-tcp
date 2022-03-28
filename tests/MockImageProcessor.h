#ifndef MOCK_IMAGE_PROCESSOR
#define MOCK_IMAGE_PROCESSOR
#include "../include/motion-detection-tcp/image-processor.h"
#include <gmock/gmock.h>

class MockImageProcessor : public ImageProcessor
{
public:
    MOCK_METHOD(uint64_t, getAveragePixels,
                (unsigned char *bytePixels, size_t arraySizeInBytes), (override));
    MOCK_METHOD(uint64_t, calculatePictureDifference,
                (unsigned char *bytePixels, size_t arraySizeInBytes), (override));
};

#endif /* MOCK_IMAGE_PROCESSOR */