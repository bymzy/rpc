

#ifndef __ACCEPTER_HPP__
#define __ACCEPTER_HPP__

#include "NetService.hpp"
#include "RWConnection.hpp"
#include "Context.hpp"
#include "TcpSocket.hpp"


class Accepter : public RWConnection {
public:
    Accepter(NetService *net, Driver *driver, std::string ip, short port):
        RWConnection(driver), mNetService(net), mIP(ip), mPort(port), mDriver(driver)
    {

    }

    ~Accepter()
    {
        if (NULL != mListenSocket) {
            delete mListenSocket;
            mListenSocket = NULL;
        }
    }

public:
    virtual int WriteData(int fd)
    {
        assert(0);
    }

    virtual int ReadData(int fd)
    {
        int err = 0;
        int clientFd = -1;
        std::string ip;
        int port = 0;

        do {
            err = mListenSocket->Accept(clientFd, ip, port);
            if (0 != err) {
                break;
            }

            TcpSocket *clientSocket = new TcpSocket();
            err = clientSocket->Create(clientFd);
            if (0 != err) {
                break;
            }
            
            debug_log("accpeter accept client " << ip << ":" << port);
            clientSocket->SetAddr(ip, port);

            AcceptContext *ctx = new AcceptContext(clientSocket);
            mNetService->Enqueue(ctx);
            OperContext::DecRef(ctx);

        } while(0);

        return 0;
    }

public:
    int StartListen()
    {
        int err = 0;

        do {
            mListenSocket = new TcpSocket();
            err = mListenSocket->Create();
            if (0 != err) {
                error_log("Accepter create failed!");
                break;
            }

            err = mListenSocket->Listen(mIP, mPort);
            if (0 != err) {
                error_log("Accepter listen failed!");
                break;
            }
            RegistReadEvent(mListenSocket->GetFd());
        } while(0);

        return err;
    }

    int StopListen()
    {
        UnRegistReadEvent(mListenSocket->GetFd());
        return mListenSocket->Close();
    }

public:
    /* net service pointer */
    NetService * mNetService;
    /* tcp listen socket */
    TcpSocket *mListenSocket;
    /* socket ip */
    std::string mIP;
    /* listen port */
    short mPort;
    /* Driver */
    Driver *mDriver;
};
#endif






