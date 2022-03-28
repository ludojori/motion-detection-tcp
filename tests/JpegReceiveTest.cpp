#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/motion-detection-tcp/jpg-receive-save-utils.h"

using namespace motion_detection;
using ::testing::AtLeast;

class MockJpgReceiveSaveUtils : JpgReceiveSaveUtils
{
public:
    MOCK_METHOD(int, receiveConfigPacket, (int socketfd, ConfigPacket* packet), (override));
    MOCK_METHOD(int, receiveImage, (int socketfd, unsigned int* imageBuffer, int imageSize), (override));
    MOCK_METHOD(unsigned int*, littleToBigEndian, (unsigned int* data, const int length), (override));
    MOCK_METHOD(bool, trySaveToJpg, (const char* dir, const int width, const int height, unsigned int* pixels, int quality), (override));
};

TEST(JpgReceiveSave, ReceiveConfigPacket)
{
    MockJpgReceiveSaveUtils mockUtils;

    const char* dir = "images";
    EXPECT_CALL(mockUtils, trySaveToJpg).Times(AtLeast(1));
    EXPECT_EQ(0, mockUtils.trySaveToJpg("images", 600, 800, nullptr, 50));
}