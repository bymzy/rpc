

#include "NetService.hpp"
#include "Accepter.hpp"
#include "Log.hpp"

NetService::~NetService()
{
}

int 
NetService::StartListener()
{
    int err = 0;
    Accepter *accepter = NULL;
    assert(mDriver != NULL);

    std::set<Addr>::iterator iter = mLocalAddrs.begin();
    for (;iter != mLocalAddrs.end(); ++iter) {
        accepter = new Accepter(this, mDriver, iter->ip, iter->port);
        err = accepter->StartListen();
        if (0 != err) {
            error_log("accepter listen failed!");
            break;
        }
        mAccepters.insert(std::make_pair(iter->ToString(), accepter));
        debug_log("netservice listen on ip: port" << iter->ip << ":" << iter->port);
    }

    return err;
}

int 
NetService::StopListener()
{
    int err = 0;
    std::map<std::string, Accepter*>::iterator iter;
    iter = mAccepters.begin();
    for (;iter != mAccepters.end(); ++iter) {
        iter->second->StopListen();
        delete iter->second;
    }
}

int 
NetService::StartConnectRemote(std::string ip, short port, uint64_t& connId)
{
    int err = 0;
    TcpSocket * clientSocket = NULL;
    TcpConnection * conn = NULL;
    assert(mDriver != NULL);

    do {
        clientSocket = new TcpSocket(); 
        err = clientSocket->Create();
        if (err != 0) {
            break;
        }

        err = clientSocket->Connect(ip, port);
        if (err != 0) {
            break;
        }

        /* new tcp connection */
        connId = mMaxID++;
        conn = new TcpConnection(this, mDriver, clientSocket, connId);
        std::map<uint64_t, TcpConnection*>::iterator iter = mConns.find(connId);
        assert(iter == mConns.end());
        mConns.insert(std::make_pair(connId, conn));
        conn->RegistRWEvent(clientSocket->GetFd());
        debug_log("connect to remote ip:port: " << ip << ":" << port << ", conn id "
                << connId);

    } while(0);

    if (err != 0) {
        delete clientSocket;
    }

    return err;
}

int 
NetService::DropAllConnections()
{
    int err = 0;

    std::map<uint64_t, TcpConnection*>::iterator iter = mConns.begin();
    for (;iter != mConns.end(); ++iter) {
        iter->second->UnRegistRWEvent(iter->second->mSocket->GetFd());
        delete iter->second;
    }

    return 0;
}


int
NetService::Init()
{
    int err = 0;

    do {
        mDriver = new Driver("NetService driver");
        err = StartListener();
        if (err != 0) {
            break;
        }

        /* start driver */
        assert(0 == mDriver->Start());

        debug_log("netservice init success!");
    } while(0);
    
    return err;
}

int
NetService::Finit()
{
    StopListener();

    DropAllConnections();

    if (mDriver != NULL) {
        mDriver->Stop();
    }


    return 0;
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
        
        debug_log("enqueue send message to conn id " << ctx->GetConnID());
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

#if 0
int
NetService::AddRemote(std::string ip, short port)
{
    int err = 0;

    Addr remote;
    remote.ip = ip;
    remote.port = port;
    std::set<Addr>::iterator iter;

    do {
        iter = mRemoteAddrs.find(remote);
        if (iter != mRemoteAddrs.end()) {
            err = EEXIST;
            break;
        }
        mRemoteAddrs.insert(remote);
    } while(0);

    return err;
}
#endif

int 
NetService::AddListener(std::string ip, short port)
{
    int err = 0;

    Addr local;
    local.ip = ip;
    local.port = port;
    std::set<Addr>::iterator iter;

    do {
        iter = mLocalAddrs.find(local);
        if (iter != mLocalAddrs.end()) {
            err = EEXIST;
            break;
        }
        mLocalAddrs.insert(local);
    } while(0);

    return err;
}

void
NetService::ProcessStart(OperContext *ctx)
{
    /* stop logicservice thread*/
    SyncClosure *sync = ctx->GetClosure();
    mRunning = true;
    sync->Signal();
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
    debug_log("NetService::Process! ctx type: " << ctx->GetType());
    /* TODO handle connection dropped*/
    switch (ctx->GetType())
    {
        case OperContext::OP_STOP:
            ProcessStop(ctx);
            break;
        case OperContext::OP_START:
            ProcessStart(ctx);
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


