

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
            _read_conn = NULL;
            _write_conn = NULL;
        }
    public:
        void FreeReadEvent()
        {
            FreeEvent(_read);
        }
        void FreeWriteEvent()
        {
            FreeEvent(_write);
        }
        bool IsInvalid()
        {
            return ((_read == NULL) && (_write == NULL));
        }
    private:
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
    /* start driver */
    int Start();
    /* driver loop */
    int Run();
    /* stop driver */
    int Stop();
    /* init driver */
    virtual int Init();
    /* finit driver */
    virtual int Finit();
    /* regist read event */
    int RegistReadEvent(int fd, RWConnection *conn);
    /* regist write event */
    int RegistWriteEvent(int fd, RWConnection *conn);
    /* unregist read event */
    int UnRegistReadEvent(int fd, RWConnection *conn);
    /* unregist write event */
    int UnRegistWriteEvent(int fd, RWConnection *conn);
    /* free all events */
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
    /* timer event */ 
    struct event *mTimerEvent;
};


#endif


