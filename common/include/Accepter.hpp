

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
        short port;

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
            
            clientSocket->SetAddr(ip, port);

            AcceptContext *ctx = new AcceptContext(clientSocket);
            debug_log("accepter enque new context!");
            mNetService->Enqueue(ctx);
            OperContext::DecRef(ctx);

        } while(0);

        return 0;
    }

public:
    int StartListen()
    {
        int err = 0;
        mListenSocket = new TcpSocket();
        mListenSocket->Create();

        do {
            err = mListenSocket->Listen(mIP, mPort);
            if (0 != err) {
                error_log("Accepter listen failed!");
            }
            RegistReadEvent(mListenSocket->GetFd());
        } while(0);

        return err;
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






