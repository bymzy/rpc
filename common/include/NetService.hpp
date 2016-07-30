

#ifndef __NET_SERVICE_HPP__
#define __NET_SERVICE_HPP__

#include <map>

#include "TcpConnection.hpp"
#include "Driver.hpp"
#include "TcpSocket.hpp"
#include "LogicService.hpp"


class Accepter;

/* NetService is used to maintain TcpConnection */
class NetService : public LogicService {
public:
    NetService(std::string name, std::string ip, short port): LogicService(name), mMaxID(0),
        mAccepter(NULL), mIP(ip), mPort(port) 
    {
    }

    virtual ~NetService();

public:
    /* start lisen and driver */
    virtual int Init();
    /* process message */
    virtual int Process(OperContext *ctx);

public:
    /* accept connection */
    void AcceptConnection(OperContext *ctx);
    /* client dropped */
    void DropConnection(OperContext *ctx);
    /* process stop */
    void ProcessStop(OperContext *ctx);
    /* recv msg */
    void RecvMsg(Msg *msg);

public:
    /* current connections */
    std::map<uint64_t, TcpConnection*> mConns;
    /* io driver */
    Driver *mDriver;
    /* current max conn id */
    uint64_t mMaxID;
    /* client accepter */
    Accepter *mAccepter;
    /* listen ip */
    std::string mIP;
    /* short port */
    short mPort;
};


#endif


