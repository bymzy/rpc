

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
NetService::RecvMsg(Msg *msg)
{
    std::string data;
    msg->operator>>(data);
    std::cout<< "receive data from client: " << data << std::endl;
    delete msg;
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
            RecvMsg(ctx->GetMessage());
            ctx->SetMessage(NULL);
            break;
        case OperContext::OP_DROP:
            DropConnection(ctx);
            break;
    }

    return 0;
}


