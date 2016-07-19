

#ifndef __TCP_CONNECTION_HPP__
#define __TCP_CONNECTION_HPP__

#include "Msg.hpp"

class TcpConnection {
public:
    TcpConnection(int sock): mFd(sock)
    {
    }
    ~TcpConnection()
    {
        Close();
    }

public:
    void SendMessage(Msg *msg)
    {

    }

    void Close()
    {
        if (-1 != mFd) {
            close(mFd);
        }
    }

public:
    /* socket fd */
    int mFd;
};

#endif


