

#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__

#include <event.h>
#include <map>
#include <string>

#include "Thread.hpp"

class RWConnection;

class Driver {
public:
    Driver(std::string name): mName(name)
    {
        mThread = new Thread(name, false);
        mEventBase = event_base_new();
        assert(NULL != mEventBase);
    }
    virtual ~Driver()
    {
    }

public:
    class EventInfo {
    public:
        EventInfo():_read(NULL), _write(NULL), 
        _read_conn(NULL), _write_conn(NULL)
        {
        }
        ~EventInfo()
        {
            FreeEvent(_read);
            FreeEvent(_write);
        }
    public:
        void FreeEvent(struct event *& ev)
        {
            if ( NULL != ev) {
                event_del(ev);
                event_free(ev);
            }

            ev = NULL;
        }
    public:
        struct event * _read;
        struct event * _write;
        RWConnection * _read_conn;
        RWConnection * _write_conn;
    };
public:
    int Start();
    int Run();
    int Stop();
    virtual int Init();
    virtual int Finit();
    int RegistReadEvent(int fd, RWConnection *conn);
    int RegistWriteEvent(int fd, RWConnection *conn);
    void FreeEvents();

private:
    /* libevent event base */
    struct event_base *mEventBase;
    /* store fd to EventInfo map */
    std::map<int, EventInfo*> mEventsInfo;
    /* store fd to RWConnection* map */
    std::map<int, RWConnection*> mConns;
    /* driver name */
    std::string mName;
    /* driver thread */
    Thread *mThread;
    struct event *mTimerEvent;
};


#endif


