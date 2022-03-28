#include <gtest/gtest.h>
#include "D:/Documents/Bosh C++ Camp/motion-detection-tcp/src/image-processor.cpp"

TEST(ServerCommunicationTest, TRY)
{
    ImageProcessor processor;
    int pixelsCount = 100;
    unsigned int *pixels = new unsigned int[pixelsCount];
    for (size_t i = 0; i < pixelsCount; i++)
    {
        pixels[i] = 1;
    }
<<<<<<< Updated upstream

    size_t sizeInBytes = pixelsCount * sizeof(int);
    // byte conversion
    unsigned char *bytePixels = new unsigned char[pixelsCount * 4];
    bytePixels = (unsigned char *)pixels;
    EXPECT_EQ(0, processor.getAveragePixels(bytePixels, sizeInBytes)) << "Average pixels were not calculated correctly";
    //delete[] bytePixels;
    delete[] pixels;
=======
    void changePic()
    {
        serverCommunication->changePic();
    }
    int getMapSize()
    {
        return serverCommunication->sockIDSensitivity.size();
    }
};

using ::testing::_;
using ::testing::AtLeast;

TEST(ServerCommunicationTest, registerClientTest)
{

    MockSendRecvLogic communication;
    MockButton mockedButton([]()
                            { return; });
    ServerCommunicationTestClass testClass;
    ServerCommunication server(communication, mockedButton);
    testClass.serverCommunication = &server;
    int sockID = 42;
    uint64_t sensitivity = 123;

    testClass.registerClient(sockID, sensitivity);
    EXPECT_EQ(1, testClass.getMapSize()) << "After regisering client the size should be increased!";
}

TEST(ServerCommunicationTest, disconnectClientTest)
{
    // Arrange
    MockSendRecvLogic communication;
    MockButton mockedButton([]()
                            { return; });
    ServerCommunicationTestClass testClass;
    ServerCommunication server(communication, mockedButton);
    testClass.serverCommunication = &server;
    int sockID = 42;
    uint64_t sensitivity = 123;

    // Act

    EXPECT_CALL(communication, close(sockID)); //  "Should close when disconnect is called with flag up";

    int mapSize = testClass.getMapSize();
    testClass.registerClient(sockID, sensitivity);
    testClass.disconnectClient(sockID, true);

    // Assert
    EXPECT_EQ(mapSize, testClass.getMapSize()) << "When user disconnected the map size should lower";
>>>>>>> Stashed changes
}

<<<<<<< Updated upstream
TEST(ServerCommunicationTest, calculatePictureDifferenceTest)
{
    ImageProcessor processor;
    int pixelsCount = 100;
    unsigned int *pixels = new unsigned int[pixelsCount];
    for (size_t i = 0; i < pixelsCount; i++)
    {
        pixels[i] = 1;//0001 0001 0001 ..... 0001 0001 //100 times
    }

    size_t sizeInBytes = pixelsCount * sizeof(int);
    // byte conversion
    unsigned char *bytePixels = new unsigned char[pixelsCount * 4];
    bytePixels = (unsigned char *)pixels;
    EXPECT_EQ(processor.getAveragePixels(bytePixels, sizeInBytes), processor.calculatePictureDifference(bytePixels, sizeInBytes)) <<
     "When the first picture is shown it's average is the difference";
    EXPECT_EQ(0, processor.calculatePictureDifference(bytePixels, sizeInBytes)) <<
     "The difference between same pictures should be 0";
    delete[] pixels; 
   // delete[] bytePixels;
=======
    testClass.readSensitivity(sockID);
    EXPECT_CALL(communication, localReceive(sockID, _, _, 0)).Times(AtLeast(1));  //<< "Should call recv when run read sens";
}
TEST(ServerCommunicationTest, readSensitivityTest)
{

    MockSendRecvLogic communication;
    MockButton mockedButton([]()
                            { return; });
    ServerCommunicationTestClass testClass;
    ServerCommunication server(communication, mockedButton);
    testClass.serverCommunication = &server;
    int sockID = 42;
    uint64_t sensitivity = 123;

    testClass.readSensitivity(sockID);
    EXPECT_CALL(communication, localReceive(sockID, _, _, 0)).Times(AtLeast(1));// << "Should call recv when run read sens";
>>>>>>> Stashed changes
}