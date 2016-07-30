

#include <sys/time.h>
#include "RWConnection.hpp"
#include "Driver.hpp"
#include "Log.hpp"

static void ReadHandler(int fd, short event, void *arg)
{
    RWConnection *conn = static_cast<RWConnection *>(arg);
    conn->ReadData(fd);
}

static void WriteHandler(int fd, short event, void *arg)
{
    RWConnection *conn = static_cast<RWConnection *>(arg);
    conn->WriteData(fd);
}

static void TimeOut(int fd, short event, void *arg)
{
 //   std::cout << "timeout!" << std::endl;
}

static void * func(void *arg)
{
    Driver *driver = static_cast<Driver*>(arg);
    driver->Run();
    return NULL;
}

int
Driver::RegistReadEvent(int fd, RWConnection *conn)
{
    int err = 0;
    std::map<int, EventInfo*>::iterator iter;
    EventInfo *info = NULL;

    do {
        iter = mEventsInfo.find(fd);
        if (iter != mEventsInfo.end()) {
            if (NULL != iter->second->_read) {
                err = EEXIST;
                break;
            }
            info = iter->second;
        }

        struct event *readEvent = event_new(mEventBase, fd,
                EV_READ | EV_PERSIST, ReadHandler, conn);
        event_add(readEvent, NULL);

        if (NULL == info) {
            info = new EventInfo();
            mEventsInfo.insert(std::make_pair(fd, info));
        }

        info->_read = readEvent;
        info->_read_conn = conn;
    } while(0);

    return err;
}

int
Driver::UnRegistReadEvent(int fd, RWConnection *conn)
{
    int err = 0;
    std::map<int, EventInfo*>::iterator iter;
    EventInfo *info = NULL;

    trace_log("add read event for socket: " << fd);

    do {
        iter = mEventsInfo.find(fd);
        if (iter == mEventsInfo.end()) {
            err = ENOENT;
            break;
        }

        info = iter->second;
        info->FreeReadEvent();
        if (info->IsInvalid()) {
            delete info;
            mEventsInfo.erase(iter);
        }
    } while(0);

    return err;
}

int
Driver::RegistWriteEvent(int fd, RWConnection *conn)
{
    int err = 0;
    std::map<int, EventInfo*>::iterator iter;
    EventInfo *info = NULL;

    trace_log("add write event for socket: " << fd);

    do {
        iter = mEventsInfo.find(fd);
        if (iter != mEventsInfo.end()) {
            if (NULL != iter->second->_write) {
                err = EEXIST;
                break;
            }
            info = iter->second;
        }

        struct event *writeEvent = event_new(mEventBase, fd,
                EV_READ | EV_PERSIST, WriteHandler, conn);
        event_add(writeEvent, NULL);


        if (NULL == info) {
            info = new EventInfo();
            mEventsInfo.insert(std::make_pair(fd, info));
        }

        info->_write= writeEvent;
        info->_write_conn = conn;
    } while(0);

    return err;
}

int
Driver::UnRegistWriteEvent(int fd, RWConnection *conn)
{
    int err = 0;
    std::map<int, EventInfo*>::iterator iter;
    EventInfo *info = NULL;

    do {
        iter = mEventsInfo.find(fd);
        if (iter == mEventsInfo.end()) {
            err = ENOENT;
            break;
        }

        info = iter->second;
        info->FreeWriteEvent();
        if (info->IsInvalid()) {
            delete info;
            mEventsInfo.erase(iter);
        }
    } while(0);

    return err;
}

int Driver::Start()
{
    assert(NULL != mThread);
    mThread->Start(func, this);
    return 0;
}

int 
Driver::Run()
{
    int err = 0;

    do {
        err = Init();
        if (0 != err) {
            error_log("Driver " << mName
                    << "start failed, error: " << err);
            break;
        }

        err = event_base_loop(mEventBase, 0);
        if (0 != err) {
            error_log("driver event_base_loop failed, error: " << err);
            break;
        }

    } while(0);

    return err;
}

int 
Driver::Stop()
{
    int err = 0;

    err = event_base_loopbreak(mEventBase);
    assert(0 == err);
    if (NULL != mEventBase) {
        event_base_free(mEventBase);
        mEventBase = NULL;
    }

    if (NULL != mThread) {
        mThread->Join();
        delete mThread;
        mThread = NULL;
    }

    Finit();
    return err;
}

void 
Driver::FreeEvents()
{

}

int
Driver::Init()
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000000;
    mTimerEvent = event_new(mEventBase, -1, EV_TIMEOUT | EV_PERSIST, TimeOut, this);
    event_add(mTimerEvent, &timeout);
    return 0;
}

int
Driver::Finit()
{
    event_del(mTimerEvent);
    event_free(mTimerEvent);
    return 0;
}



