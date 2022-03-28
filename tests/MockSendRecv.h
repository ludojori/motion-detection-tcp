#include "gmock/gmock.h"
#include "../include/motion-detection-tcp/send-receive-interface.h"


class MockSendRecvLogic : public SendReceiveInterface
{
public:
    MOCK_METHOD(int, localSend, (int sockId, void *buf, size_t len, int flags), (override));
    MOCK_METHOD(int, localReceive, (int sockId, void *buf, size_t len, int flags), (override));
    MOCK_METHOD(void, close, (int sockId), (override));
    MOCK_METHOD(int, getSockOptCheck,(int sockId, int& error), (override));
    MOCK_METHOD(int, initServer,(char* port), (override));
};
