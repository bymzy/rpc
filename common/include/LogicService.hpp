

#ifndef __LOGIC_SERVICE_HPP__
#define __LOGIC_SERVICE_HPP__

#include "Thread.hpp"
#include "Context.hpp"
#include <deque>

class LogicService {
public:
    LogicService(std::string name): mName(name), mRunning(false)
    {
        mThread = new Thread(name, false);
        pthread_mutexattr_t mutexattr;
        pthread_mutexattr_init(&mutexattr);
        pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mMutex, &mutexattr);
        pthread_mutexattr_destroy(&mutexattr);
        pthread_cond_init(&mCond, NULL);
    }
    virtual ~LogicService()
    {
        pthread_mutex_destroy(&mMutex);
        pthread_cond_destroy(&mCond);
        if (NULL != mThread) {
            delete mThread;
        }
    }

public:
    static void* thread_func(void *arg)
    {
        LogicService* service = static_cast<LogicService*>(arg);
        service->Run();
        return NULL;
    }

public:
    int Start()
    {
        int err = 0;
        assert(NULL != mThread);

        do {
            err = mThread->Start(LogicService::thread_func, this);
            if (err != 0) {
                break;
            }

            /* wait server work! */
            OperContext *ctx = new OperContext(OperContext::OP_START);
            SyncClosure sync;
            ctx->SetClosure(&sync);
            Enqueue(ctx);
            sync.Wait();
            ctx->SetClosure(NULL);
            OperContext::DecRef(ctx);

        } while(0);

        return err;
    }

    void Stop()
    {
        /* finitialize logic service */
        Finit();

        pthread_mutex_lock(&mMutex);
        mRunning = false;
        pthread_cond_signal(&mCond);
        pthread_mutex_unlock(&mMutex);
        mThread->Join();
    }

    virtual int Init() = 0;
    virtual int Finit() = 0;
    /* run thread loop */
    void Run();
    /* process ctx */
    virtual int Process(OperContext *ctx) = 0;
    /* enqueue ctx to logic service */
    void Enqueue(OperContext *ctx);

public:
    /* name of this logicservice */
    std::string mName;
    /* cond variable */
    pthread_cond_t mCond;
    /* mutex lock */
    pthread_mutex_t mMutex;
    /* opercontext queue */
    std::deque<OperContext*> mQueue;
    /* logic service thread */
    Thread *mThread;
    /* logic service running */
    bool mRunning;
};


#endif






