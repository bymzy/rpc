

#include <vector>

#include "LogicService.hpp"
#include "Log.hpp"

void
LogicService::Run()
{
    int err = 0;
    OperContext *ctx= NULL;
    std::vector<OperContext *> ctxVec;

    err = Init();
    if (0 != err) {
        error_log("LogicService init failed, name: " << mName
                << ", error: " << err);
        return;
    }

    debug_log("logic service running!");
    mRunning = true;
    while(mRunning) 
    {
        pthread_mutex_lock(&mMutex);
        while (mQueue.size() <= 0) {
            pthread_cond_wait(&mCond, &mMutex);
        }

        /* now there is something in queure try process it */
        while (!mQueue.empty()) {
            ctx = mQueue.front();
            ctxVec.push_back(ctx);
            mQueue.pop_front();
            ctx = NULL;
        }

        /* io driver will not be blocked when received msg */
        pthread_mutex_unlock(&mMutex);

        for (int i = 0;i < ctxVec.size();++i) {
            Process(ctxVec[i]);
            OperContext::DecRef(ctxVec[i]);
        }
        ctxVec.clear();
    }

    pthread_exit(0);
}

void
LogicService::Enqueue(OperContext *ctx)
{
    pthread_mutex_lock(&mMutex);
    OperContext::IncRef(ctx);
    mQueue.push_back(ctx);
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mMutex);
}






