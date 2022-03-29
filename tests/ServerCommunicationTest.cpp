#include <gtest/gtest.h>
#include "../src/server-communication.cpp"
#include "StubSendRecvLogic.hpp"
#include "MockSendRecv.h"
#include "MockButton.h"
#include "MockImageProcessor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

class ServerCommunicationTestClass
{

public:
    ServerCommunication *serverCommunication;

    void registerClient(int sockID, uint64_t sensitivity)
    {
        serverCommunication->registerClient(sockID, sensitivity);
    }
    void disconnectClient(const int sockID, bool shouldCloseSocket)
    {
        serverCommunication->disconnectClient(sockID, shouldCloseSocket);
    }
    void readSensitivity(int sockID)
    {
        serverCommunication->readSensitivity(sockID);
    }
    bool isSocketConnected(int sockID)
    {
        return serverCommunication->isSocketConnected(sockID);
    }
    void sendPicture(int sockID, unsigned int *fullImage)
    {
        serverCommunication->sendPicture(sockID, fullImage);
    }
    void sendConfigPacket(int sockID, ConfigPacket *packet)
    {
        serverCommunication->sendConfigPacket(sockID, packet);
    }
    void sendImage(int sockID, unsigned int *fullImage, int imageSize)
    {
        serverCommunication->sendImage(sockID, fullImage, imageSize);
    }
    void notifyClients(unsigned int *pixels, int pixelsCount)
    {
        serverCommunication->notifyClients(pixels, pixelsCount);
    }
    void checkConnectedClients()
    {
        serverCommunication->checkConnectedClients();
    }
    void changePic()
    {
        serverCommunication->changePic();
    }
    int getMapSize()
    {
        return serverCommunication->sockIDSensitivity.size();
    }
    void addNewClient(int sockID, uint64_t sensitivity)
    {
        serverCommunication->sockIDSensitivity.insert({sockID, sensitivity});
    }
};
class Utility : public testing::Test
{
protected:
    static void SetUp()
    {
        MockSendRecvLogic communication;
        MockButton mockedButton([]()
                                { return; });
        ServerCommunicationTestClass testClass;
        MockImageProcessor ip;
        ServerCommunication server(communication, mockedButton, ip);
        testClass.serverCommunication = &server;
    }
};
TEST(ServerCommunicationTest, registerClientTest)
{
    int sockID = 42;
    uint64_t sensitivity = 123;

    testClass.registerClient(sockID, sensitivity);
    EXPECT_EQ(1, testClass.getMapSize()) << "After regisering client the size should be increased!";
}

TEST(ServerCommunicationTest, disconnectClientTest)
{
    int sockID = 42;
    uint64_t sensitivity = 123;

    testClass.registerClient(sockID, sensitivity);
    testClass.disconnectClient(sockID, true);
    EXPECT_CALL(communication, close(sockID));
}
TEST(ServerCommunicationTest, readSensitivityTest)
{
    int sockID = 42;
    uint64_t sensitivity = 123;

    testClass.readSensitivity(sockID);
    EXPECT_CALL(communication, localReceive(sockID, (_), (_), 0)).Times(AtLeast(1)) << "Should call recv when run read sens";
}
TEST(ServerCommunicationTest, readSensitivityTest)
{
    int sockID = 42;
    uint64_t sensitivity = 123;

    testClass.readSensitivity(sockID);
    EXPECT_CALL(communication, localReceive(sockId, _, _, 0)).Times(AtLeast(1)) << "Should call recv when run read sens";
}
TEST(ServerCommunicationTest, notifyClientsTest)
{
    for (int i = 0; i < 10; i++)
    {
        testClass.addClient(i, 2 * i); // sensitivity varies in range[0;20]
    }
    ON_CALL(ip, calculatePictureDifference(_, _))
        .WillByDefault(Return(3));
    testClass.notifyClients(nullptr, 0);
    EXPECT_CALL(*testClass.serverCommunication, sendPicture(_)).Times(3);
}