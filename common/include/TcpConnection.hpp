

#ifndef __TCP_CONNECTION_HPP__
#define __TCP_CONNECTION_HPP__


#include "Msg.hpp"
#include "TcpSocket.hpp"
#include "RWConnection.hpp"

#define RECV_BUFF_LEN 4096
#define SEND_BUFF_LEN 4096

class TcpConnection : public RWConnection {
public:
    TcpConnection(Driver *driver, TcpSocket *sock): RWConnection(driver),
            mSocket(sock)
    {}
    virtual ~ TcpConnection()
    {
    }

public:
    virtual int WriteData(int fd);
    virtual int ReadData(int fd);

private:
    char recvBuf[RECV_BUFF_LEN];
    char sendBuf[SEND_BUFF_LEN];

public:
    /* current msg recv len */
    int mToRecv;
    int mRecved;
    Msg *mCurrentRecvMsg;

    /* current msg send len */
    int mToSend;
    int mSent;
    Msg *mCurrentSendMsg;

    TcpSocket *mSocket;
};


#endif


