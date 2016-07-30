

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
        pthread_mutex_init(&mMutex, NULL);
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
        assert(NULL != mThread);
        return mThread->Start(LogicService::thread_func, this);
    }
    void Stop()
    {
        OperContext *ctx = new OperContext(OperContext::OP_STOP);
        SyncClosure sync;
        ctx->SetClosure(&sync);
        Enqueue(ctx);
        ctx->GetClosure()->Wait();
        ctx->SetClosure(NULL);
        OperContext::DecRef(ctx);
        mThread->Join();
    }

    virtual int Init() = 0;
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






