#ifndef SEND_RECEIVE_INTERFACE
#define SEND_RECEIVE_INTERFACE
class SendReceiveInterface
{
public:
    virtual int localSend(int sockId, void *buf, size_t len, int flags = 0) = 0;
    virtual int localReceive(int sockId, void *buf, size_t len, int flags = 0) = 0;
    virtual void close(int sockId) = 0;
    virtual int getSockOptCheck(int sockId, int& error) = 0;
    virtual int initServer(char* port) = 0; 

};
#endif /* SEND_RECEIVE_INTERFACE */
