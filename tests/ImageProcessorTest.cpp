#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/image-processor.cpp"

TEST(ServerCommunicationTest, getAveragePixelsTest)
{
    ImageProcessor processor;
    int pixelsCount = 100;
    unsigned int *pixels = new unsigned int[pixelsCount];
    for (size_t i = 0; i < pixelsCount; i++)
    {
        pixels[i] = 1;
    }

    size_t sizeInBytes = pixelsCount * sizeof(int);
    // byte conversion
    unsigned char *bytePixels = (unsigned char *)pixels;
    EXPECT_EQ(0, processor.getAveragePixels(bytePixels, sizeInBytes))
        << "Average pixels were not calculated correctly";
    delete[] pixels;
}

TEST(ServerCommunicationTest, calculatePictureDifferenceTest)
{
    ImageProcessor processor;
    int pixelsCount = 100;
    unsigned int *pixels = new unsigned int[pixelsCount];
    for (size_t i = 0; i < pixelsCount; i++)
    {
        pixels[i] = 1; // 0001 0001 0001 ..... 0001 0001 //100 times
    }

    size_t sizeInBytes = pixelsCount * sizeof(int);
    // byte conversion
    unsigned char *bytePixels = (unsigned char *)pixels;
    EXPECT_EQ(processor.getAveragePixels(bytePixels, sizeInBytes),
              processor.calculatePictureDifference(bytePixels, sizeInBytes))
        << "When the first picture is shown it's average is the difference";
    EXPECT_EQ(0, processor.calculatePictureDifference(bytePixels, sizeInBytes))
        << "The difference between same pictures should be 0";
    delete[] pixels;
}