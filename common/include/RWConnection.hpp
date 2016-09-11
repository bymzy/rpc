

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
        mDriver = NULL;
    }

public:
    /* regist read event for this connection */ 
    int RegistReadEvent(int fd)
    {
        assert(0 == mDriver->RegistReadEvent(fd, this));
        return 0;
    }

    /* regist write event for this connection */
    int RegistWriteEvent(int fd)
    {
        assert(0 == mDriver->RegistWriteEvent(fd, this));
        return 0;
    }

    int RegistRWEvent(int fd)
    {
        RegistReadEvent(fd);
        RegistWriteEvent(fd);
        return 0;
    }

    /* unregist read event */
    int UnRegistReadEvent(int fd)
    {
        assert(0 == mDriver->UnRegistReadEvent(fd, this));
        return 0;
    }

    /* unregist read event */
    int UnRegistWriteEvent(int fd)
    {
        assert(0 == mDriver->UnRegistWriteEvent(fd, this));
        return 0;
    }

    /* unregist read write event */
    int UnRegistRWEvent(int fd)
    {
        UnRegistReadEvent(fd);
        UnRegistWriteEvent(fd);
        return 0;
    }

    /* unregist write event */

    /* try to send len bytes */
    virtual int WriteData(int fd) = 0;
    /* try read data from fd */
    virtual int ReadData(int fd) = 0;

public:
    /* driver pointer */
    Driver *mDriver;
};

#endif


