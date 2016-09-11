

#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include <pthread.h>
#include "TcpSocket.hpp"
#include "Msg.hpp"
#include "Closure.hpp"

class OperContext {
public:
    enum _type
    {
        /* message to send or received*/
        OP_RECV = 0,
        OP_SEND,
        /* start stop logic service */
        OP_START,
        OP_STOP,
        /* accept new client */
        OP_ACCEPT,
        /* client droped */
        OP_DROP,
    };

public:
    OperContext(int opType): mRef(1), mSocket(-1), mType(opType),
        mConnID(-1)
    {
        mUnion.mMsg = NULL;
    }
    virtual ~OperContext()
    {
        assert(mUnion.mMsg == NULL);
        assert(mUnion.mSyncClosure == NULL);
    }

public:
    int GetType()
    {
        return mType;
    }

    void SetMessage(Msg *msg)
    {
        mUnion.mMsg = msg;
    }

    Msg* GetMessage()
    {
        assert(NULL != mUnion.mMsg);
        return mUnion.mMsg;
    }

    void SetClosure(SyncClosure *closure)
    {
        mUnion.mSyncClosure = closure;
    }

    SyncClosure *GetClosure()
    {
        assert(NULL != mUnion.mSyncClosure);
        return mUnion.mSyncClosure;
    }

    uint64_t GetConnID()
    {
        return mConnID;
    }

    void SetConnID(uint64_t connID)
    {
        mConnID = connID; 
    }

public:
    int mRef;
    static pthread_mutex_t lock;

    static void DecRef(OperContext *ctx)
    {
        pthread_mutex_lock(&OperContext::lock);
        assert(ctx->mRef > 0);
        --ctx->mRef;
        if (ctx->mRef <= 0) {
            delete ctx;
        }
        pthread_mutex_unlock(&OperContext::lock);
    }

    static void IncRef(OperContext *ctx)
    {
        pthread_mutex_lock(&OperContext::lock);
        assert(ctx->mRef > 0);
        ++ctx->mRef;
        pthread_mutex_unlock(&OperContext::lock);
    }
public:
    /* which this context contain */
    union {
        Msg *mMsg;
        SyncClosure *mSyncClosure;
    } mUnion;
    /* client fd accepted */
    int mSocket;
    /* context type */
    int mType;
    /* conn id */
    uint64_t mConnID;
};

class AcceptContext : public OperContext {
public:
    AcceptContext(TcpSocket *clientSocket): OperContext(OP_ACCEPT), mClientSocket(clientSocket)
    {
    }
    virtual ~AcceptContext()
    {
    }

public:
    TcpSocket *mClientSocket;
};

#endif


