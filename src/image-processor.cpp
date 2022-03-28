#include "../include/motion-detection-tcp/image-processor.h"

uint64_t
ImageProcessor::
    getAveragePixels(unsigned char *bytePixels, size_t arraySizeInBytes)
{   
    if(bytePixels == nullptr || arraySizeInBytes == 0)
    {
        return 0;
    }
    int sum = 0;
    uint64_t av;
    for (size_t i = 0; i < arraySizeInBytes; i++)
    {
        if (i % 3 == 0) // alpha values
        {
            continue;
        }
        sum += bytePixels[i];
    }
    std::cout << "sum: " << sum << std::endl;
    av = sum / (arraySizeInBytes / 4);
    return av;
}

uint64_t
ImageProcessor::
    calculatePictureDifference(unsigned char *bytePixels, size_t arraySizeInBytes)
{
    uint64_t newPictureAverage = getAveragePixels(bytePixels, arraySizeInBytes); // average needs bytes
    uint64_t diff = abs(static_cast<long long>(oldPictureAverage) - static_cast<long long>(newPictureAverage));
    oldPictureAverage = newPictureAverage;
    std::cout << "[MESSAGE]: Difference: " << diff << std::endl;
    return diff;
}

ImageProcessor::ImageProcessor() : oldPictureAverage(0) { }
