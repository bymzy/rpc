


#ifndef __COMMON_INCLUDE_THREAD_HPP__
#define __COMMON_INCLUDE_THREAD_HPP__

#include <pthread.h>
#include <assert.h>
#include <errno.h>

#include "Log.hpp"

class Thread {
public:
    typedef void *(*thread_func)(void *);
public:
    Thread(std::string name, bool detach): mName(name), mThread(-1), 
        mDetach(detach)
    {
    }
    ~Thread()
    {
    }

public:
    int Start(thread_func func, void *arg)
    {
        int err = 0;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        if (mDetach) {
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        }
        err = pthread_create(&mThread, &attr, func, arg);
        assert(err == 0);

        pthread_attr_destroy(&attr);

        return err;
    }

    int Stop()
    {
        int err = 0;
        err = pthread_cancel(mThread);
        if (0 != err) {
            error_log("pthread cancel failed, error: " << err);
        }
        assert(err == 0);
        return 0;
    }

    int Join()
    {
        int err = 0;
        if (mDetach) {
            return 0;
        }
        err = pthread_join(mThread, NULL);
        assert(err == 0);
        return err;
    }

public:
    std::string mName;
    pthread_t mThread;
    bool mDetach;
};

#endif


