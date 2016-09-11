

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
        assert(mQueue.size() == 0);
        pthread_mutex_destroy(&mMutex);
        pthread_cond_destroy(&mCond);
        if (NULL != mThread) {
            delete mThread;
            mThread = NULL;
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
    /* you may overwrite Start function if you don't want to new one thread */
    virtual int Start()
    {
        int err = 0;

        if (mRunning) {
            return 0;
        }
        mRunning = true;
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
        pthread_mutex_lock(&mMutex);
        mRunning = false;
        pthread_cond_signal(&mCond);
        pthread_mutex_unlock(&mMutex);

        mThread->Join();
    }

    /* init is something you want to do befor thread running */
    virtual int Init()
    {
        return 0;
    }
    /* init is something you want to do befor thread stopped */
    virtual int Finit()
    {
        return 0;
    }
    /* run thread loop */
    void Run();
    /* process ctx */
    virtual bool Process(OperContext *ctx);
    /* enqueue ctx to logic service */
    bool Enqueue(OperContext *ctx);

    /* start and stop of basic LogicService */
    void ProcessStart(OperContext *ctx);
    void ProcessStop(OperContext *ctx);

#if 1
    void RecvMsg(OperContext *ctx);
#endif

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






