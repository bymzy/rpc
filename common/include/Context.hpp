

#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include <pthread.h>
#include "Msg.hpp"

class OperContext {
public:
    enum _type
    {
        OP_RECV,
        OP_SEND,
    };

public:
    OperContext(int opType):mType(opType)
    {
    }
    ~OperContext()
    {
    }

public:
    int mRef;
    static pthread_mutex_t lock;

    static void Decref(OperContext *ctx)
    {
        pthread_mutex_lock(&OperContext::lock);
        assert(ctx->mRef > 0);
        --ctx->mRef;
        if (ctx->mRef <= 0) {
            delete ctx;
        }
        pthread_mutex_unlock(&OperContext::lock);
    }

    static void Incref(OperContext *ctx)
    {
        pthread_mutex_lock(&OperContext::lock);
        assert(ctx->mRef > 0);
        ++ctx->mRef;
        pthread_mutex_unlock(&OperContext::lock);
    }
public:
    Msg *mMsg;
    int mType;
};


#endif


