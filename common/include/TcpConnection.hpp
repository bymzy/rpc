

#ifndef __TCP_CONNECTION_HPP__
#define __TCP_CONNECTION_HPP__

#include <deque>

#include "Msg.hpp"
#include "TcpSocket.hpp"
#include "RWConnection.hpp"
#include "Context.hpp"


class LogicService;

class TcpConnection : public RWConnection {
public:
    TcpConnection(LogicService *logic, Driver *driver, TcpSocket *sock, 
            uint64_t connID): RWConnection(driver),
            mCurrentRecvMsg(NULL), mCurrentSendMsg(NULL),mSocket(sock),
            mLogicService(logic), mConnID(connID)
    {
        pthread_mutex_init(&mMutex, NULL);
    }
    virtual ~ TcpConnection()
    {
        if (NULL != mSocket) {
            delete mSocket;
        }

        FreeNotSendMsg();
        pthread_mutex_destroy(&mMutex);
    }

public:
    virtual int WriteData(int fd);
    virtual int ReadData(int fd);

    /* clear recv flag */
    void ResetRecv();

    /* clear send flag */
    void ResetSend();

    /* free msg that is not send */
    void FreeNotSendMsg();

    /* get next message from deque front */
    bool InitNextSend();

    std::string GetClientIP()
    {
        return mSocket->mIP;
    }

    short GetClientPort()
    {
        return mSocket->mPort;
    }

public:
    /* current msg recv len */
    int mToRecv;
    int mRecved;
    Msg *mCurrentRecvMsg;

    /* current msg send len */
    int mToSend;
    int mSent;
    Msg *mCurrentSendMsg;

    /* current to send message queue */
    std::deque<Msg *> mToSendMsg;
    pthread_mutex_t mMutex;

    TcpSocket *mSocket;
    LogicService *mLogicService;

    /* connid */
    uint64_t mConnID;
};


#endif





