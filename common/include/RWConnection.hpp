

#ifndef __RWCONNECTION_HPP__
#define __RWCONNECTION_HPP__

#include "Driver.hpp"
#include "TcpSocket.hpp"
#include "Msg.hpp"

class RWConnection {
public:
    RWConnection(Driver * driver):mDriver(driver)
    {
    }
    virtual ~RWConnection()
    {
    }

public:
    /* regist read event for this connection */ 
    void RegistReadEvent(int fd)
    {
        mDriver->RegistReadEvent(fd, this);
    }

    /* regist write event for this connection */
    void RegistWriteEvent(int fd)
    {
        mDriver->RegistWriteEvent(fd, this);
    }

    void RegistRWEvent(int fd)
    {
        RegistReadEvent(fd);
        RegistWriteEvent(fd);
    }

    /* try to send len bytes */
    virtual int WriteData(int fd) = 0;

    /* try read data from fd */
    virtual int ReadData(int fd) = 0;

public:
    /* driver pointer */
    Driver *mDriver;
};

#endif


