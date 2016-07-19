

#ifndef __RWCONNECTION_HPP__
#define __RWCONNECTION_HPP__

#include "Driver.hpp"
#include "TcpSocket.hpp"
#include "Msg.hpp"

class RWConnection {
public:
    RWConnection(Driver * driver, int fd):mDriver(driver), mFd(fd)
    {
    }
    virtual ~RWConnection()
    {

    }

public:
    int GetFd()
    {
        return mFd;
    }

    /* regist read event for this connection */ 
    void RegistReadEvent()
    {
        mDriver->RegistReadEvent(this);
    }

    /* regist write event for this connection */
    void RegistWriteEvent()
    {
        mDriver->RegistWriteEvent(this);
    }

    void RegistRWEvent()
    {
        RegistReadEvent();
        RegistWriteEvent();
    }

    /* try to send len bytes */
    virtual int WriteData() = 0;

    /* try read data from fd */
    virtual int ReadData() = 0;

public:
    /* fd to read write */
    int mFd;

    /* set driver */
    Driver *mDriver;
};

#endif


