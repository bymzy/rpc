

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
    while (mRunning)
    {
        pthread_mutex_lock(&mMutex);
        while (mQueue.size() <= 0
                && mRunning) {
            pthread_cond_wait(&mCond, &mMutex);
        }

        /* now there is something in queure try process it */
#if 0
        while (!mQueue.empty()) {
            ctx = mQueue.front();
            ctxVec.push_back(ctx);
            mQueue.pop_front();
            ctx = NULL;
        }
#endif

        trace_log("logicservice queue size: " << mQueue.size());
        ctxVec.assign(mQueue.begin(), mQueue.end());
        mQueue.clear();

        /* io driver will not be blocked when received msg */
        pthread_mutex_unlock(&mMutex);

        for (int i = 0;i < ctxVec.size();++i) {
            ctx = ctxVec[i];
            if (mRunning) {
                if (!LogicService::Process(ctx)) {
                    assert(Process(ctx));
                }
            } else {
                ctx->SetMessage(NULL);
            }

            OperContext::DecRef(ctx);
            ctx = NULL;
        }
        ctxVec.clear();
    }

    //Finit();
    std::cout << "logic service exit! vec size: " <<  ctxVec.size() <<",cap: "<< ctxVec.capacity() << std::endl;
}

/* Start only return */
void
LogicService::ProcessStart(OperContext *ctx)
{
    /* stop logicservice thread*/
    SyncClosure *sync = ctx->GetClosure();
    mRunning = true;
    sync->Signal();
    ctx->SetClosure(NULL);
}

void
LogicService::ProcessStop(OperContext *ctx)
{
    /* stop logicservice thread*/
    SyncClosure *sync = ctx->GetClosure();
    mRunning = false;
    mThread->Join();
    sync->Signal();
}

void
LogicService::RecvMsg(OperContext *ctx) {
    Msg *msg = ctx->GetMessage();
    std::string data;
    (*msg) >> (data);
    std::cout << "receive data from client, conn id:  " << ctx->GetConnID() << data << std::endl;
    delete msg;
    ctx->SetMessage(NULL);
}

bool
LogicService::Process(OperContext *ctx)
{
    bool processed = true;
    trace_log("LogicService process context: " << ctx->GetType());
    switch (ctx->GetType())
    {
        case OperContext::OP_STOP:
            ProcessStop(ctx);
            break;
        case OperContext::OP_START:
            ProcessStart(ctx);
            break;
#if 0
        case OperContext::OP_RECV:
            RecvMsg(ctx);
            break;
#endif
        default:
            processed = false;
            break;
    }

    return processed;
}

bool
LogicService::Enqueue(OperContext *ctx)
{
    bool pushed = false;
    pthread_mutex_lock(&mMutex);
    if (mRunning) {
        OperContext::IncRef(ctx);
        mQueue.push_back(ctx);
        pushed = true;
    }
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mMutex);

    return pushed;
}






