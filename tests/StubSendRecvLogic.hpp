#include "../include/motion-detection-tcp/send-receive-interface.h"

class StubSendRecvLogic
    : public SendReceiveInterface
{
public:
    virtual int localSend(int sockId, void *buf, size_t len, int flags) override
    {
        return 1;
    }
    virtual int localReceive(int sockId, void *buf, size_t len, int flags) override
    {
        return 0;
    };
};
