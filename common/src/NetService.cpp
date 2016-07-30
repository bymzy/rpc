

#include "NetService.hpp"
#include "Accepter.hpp"
#include "Log.hpp"

NetService::~NetService()
{
    if (NULL != mAccepter) {
        delete mAccepter;
        mAccepter = NULL;
    }
}

int
NetService::Init()
{
    int err = 0;
    mDriver = new Driver("NetService driver");
    mAccepter = new Accepter(this, mDriver, mIP, mPort);

    debug_log("netservice listen on ip: port" << mIP << ":" << mPort);

    do {
        /* start listen */
        err = mAccepter->StartListen();
        if (0 != err) {
            break;
        }

        /* start driver */
        assert(0 == mDriver->Start());

        debug_log("netservice init success!");
    } while(0);
    
    return err;
}

void 
NetService::AcceptConnection(OperContext *ctx)
{
    uint64_t connID = mMaxID++;
    //AcceptContext *accCtx = dynamic_cast<AcceptContext*>(ctx);
    AcceptContext *accCtx = (AcceptContext*)ctx;
    TcpSocket * socket = accCtx->mClientSocket;

    debug_log("NetService::AcceptConnection,fd: " << socket->GetFd()
            <<", ip: " << socket->mIP
            <<", port: " << socket->mPort);

    TcpConnection *conn = new TcpConnection(this, mDriver, socket, connID);
    std::map<uint64_t, TcpConnection*>::iterator iter = mConns.find(connID);
    assert(iter == mConns.end());
    mConns.insert(std::make_pair(connID, conn));
    conn->RegistRWEvent(socket->GetFd());
}

void 
NetService::RecvMsg(OperContext *ctx)
{
    Msg *msg = ctx->GetMessage();
    std::string data;
    (*msg)>>(data);
    std::cout<< "receive data from client, conn id:  " << ctx->GetConnID() << data << std::endl;
    delete msg;
    ctx->SetMessage(NULL);


    OperContext *replyctx = new OperContext(OperContext::OP_SEND);
    Msg *repmsg = new Msg();
    (*repmsg) << "this is reply from netservice!";
    repmsg->SetLen();
    replyctx->SetMessage(repmsg);
    replyctx->SetConnID(ctx->GetConnID());
    Enqueue(replyctx);
    OperContext::DecRef(replyctx);
}

void
NetService::SendMsg(OperContext *ctx)
{
    std::map<uint64_t, TcpConnection*>::iterator iter;
    TcpConnection *conn = NULL;

    do {
        iter = mConns.find(ctx->GetConnID());
        if (iter == mConns.end()) {
            delete ctx->GetMessage();
            error_log("send msg, but connection not found!");
            break;
        }
        
        conn = iter->second;
        conn->Enqueue(ctx->GetMessage());
        conn->WriteData(conn->mSocket->GetFd());
    } while(0);

    ctx->SetMessage(NULL);
}

void 
NetService::DropConnection(OperContext *ctx)
{
    uint64_t connId = ctx->mConnID;
    TcpConnection * conn = NULL;
    std::map<uint64_t, TcpConnection*>::iterator iter;

    debug_log("drop connection conn id: " << ctx->mConnID);
    iter = mConns.find(connId);
    if (iter != mConns.end()) {
        conn = iter->second;
        debug_log("NetService drop connecton, conn id: " << conn->mConnID
                <<", ip: " << conn->GetClientIP()
                <<", port: " << conn->GetClientPort());
        delete conn;
        mConns.erase(iter);
    }
}

void
NetService::ProcessStop(OperContext *ctx)
{
    /* stop driver */
    mDriver->Stop();

    /* stop logicservice thread*/
    SyncClosure *sync = ctx->GetClosure();
    mRunning = false;
    sync->Signal();
}

int 
NetService::Process(OperContext *ctx)
{
    /* TODO handle connection dropped*/
    switch (ctx->GetType())
    {
        case OperContext::OP_STOP:
            ProcessStop(ctx);
            break;
        case OperContext::OP_ACCEPT:
            AcceptConnection(ctx);
            break;
        case OperContext::OP_RECV:
            RecvMsg(ctx);
            break;
        case OperContext::OP_SEND:
            SendMsg(ctx);
            break;
        case OperContext::OP_DROP:
            DropConnection(ctx);
            break;
    }

    return 0;
}


