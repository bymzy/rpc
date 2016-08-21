

#include "LogicService.hpp"
#include "TcpConnection.hpp"


void
TcpConnection::ResetRecv()
{
    mToRecv = 0;
    mRecved = 0;
    mCurrentRecvMsg = NULL;
    /* TODO this should be controlled in tcp connection */
    /* reset socket pre received */
    mSocket->SetPreRecv(false);
}

void
TcpConnection::ResetSend()
{
    mToSend = 0;
    mSent = 0;
    mCurrentSendMsg = NULL;
}

int 
TcpConnection::ReadData(int fd)
{
    int err = 0;
    int dataLen = 0;
    int recved = 0;

    do {
        if (mSocket->NoOK()) {
            trace_log("event for write but socket no okay");
            return EINVAL;
        }

        /* TODO change this to async */
        if (!mSocket->HasPreRecv()) {
            err = mSocket->PreRecv(dataLen);
            if (0 != err) {
                break;
            }

            assert(NULL == mCurrentRecvMsg);
            mCurrentRecvMsg = new Msg(dataLen);
            /* set msg head len */
            mCurrentRecvMsg->SetLen(dataLen);
            mRecved = 4;
            mToRecv = dataLen + HEAD_LENGTH;
        }

        /* try recv data from socket */
        err = mSocket->Recv(mCurrentRecvMsg->GetBuffer() + mRecved,
                mToRecv - mRecved, recved);
        if (0 != err) {
            error_log("TcpConnection read data failed, error: " << err);
            break;
        }

        mRecved += recved;
    } while(0);

    if (mRecved == mToRecv
            && mRecved >4) {
        /* recved full message , enque it to logic service */
        OperContext *ctx = new OperContext(OperContext::OP_RECV);
        ctx->SetMessage(mCurrentRecvMsg);
        ctx->SetConnID(mConnID);
        mLogicService->Enqueue(ctx);
        OperContext::DecRef(ctx);
        ResetRecv();
    }

    if (0 != err) {
        /* unregist read write event */
        delete mCurrentRecvMsg;
        ResetRecv();

        UnRegistRWEvent(mSocket->GetFd());
        OperContext *ctx = new OperContext(OperContext::OP_DROP);
        ctx->SetConnID(mConnID);
        mLogicService->Enqueue(ctx);
        OperContext::DecRef(ctx);
    }

    return err;
}

int 
TcpConnection::WriteData(int fd)
{
    int err = 0;
    int sent = 0;

    do {
        if (mSocket->NoOK()) {
            trace_log("event for write but socket no okay");
            return EINVAL;
        }

        if (NULL == mCurrentSendMsg
                && !InitNextSend()) {
            trace_log("current no message to send, conn id: " << mConnID);
            break;
        }

        assert(mCurrentSendMsg != NULL);

        err = mSocket->Send(mCurrentSendMsg->GetBuffer() + mSent, 
                mToSend, sent);
        if (0 != err) {
            error_log("TcpConnection::WriteData failed, error: " << err
                    << ", conn id: " << mConnID
                    << ", error: " << strerror(err));
            delete mCurrentSendMsg;
            ResetSend();

            UnRegistRWEvent(mSocket->GetFd());

            /* inform netservice clear this conn */
            OperContext *ctx = new OperContext(OperContext::OP_DROP);
            ctx->SetConnID(mConnID);
            mLogicService->Enqueue(ctx);
            OperContext::DecRef(ctx);
            break;
        }

        mSent += sent;
        mToSend -= sent;
        if (mToSend == 0) {
            /* you see all msg is delete on network driver */
            trace_log("message send complete, conn id: " << mConnID);
            delete mCurrentSendMsg;
            ResetSend();
        }
    } while(0);

    return err;
}

bool
TcpConnection::InitNextSend()
{
    bool hasMsg = false;
    pthread_mutex_lock(&mMutex);
    if (!mToSendMsg.empty()) {
        mCurrentSendMsg = mToSendMsg.front();
        mToSend = mCurrentSendMsg->GetTotalLen();
        mSent = 0;

        mToSendMsg.pop_front();
        hasMsg = true;
    }
    pthread_mutex_unlock(&mMutex);

    return hasMsg;
}

void 
TcpConnection::FreeNotSendMsg()
{
    Msg *msg = NULL;
    pthread_mutex_lock(&mMutex);
    while (!mToSendMsg.empty()) {
        msg = mToSendMsg.front();
        delete msg;
        mToSendMsg.pop_front();
    }
    pthread_mutex_unlock(&mMutex);
}

void
TcpConnection::Enqueue(Msg *msg)
{
    pthread_mutex_lock(&mMutex);
    mToSendMsg.push_back(msg);
    pthread_mutex_unlock(&mMutex);
}



