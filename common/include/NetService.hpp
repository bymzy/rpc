

#ifndef __NET_SERVICE_HPP__
#define __NET_SERVICE_HPP__

#include <map>
#include <set>

#include "TcpConnection.hpp"
#include "Driver.hpp"
#include "TcpSocket.hpp"
#include "LogicService.hpp"
#include "common.h"


class Accepter;

class Addr {
public:
    bool operator<(const Addr& right) const
    {
        return ToString() < right.ToString();
    }
public:
    std::string ToString() const
    {
        return ip + i2s(port);
    }
public:
    std::string ip;
    short port;
};

/* NetService is used to maintain TcpConnection */
class NetService : public LogicService {
public:
    NetService(LogicService *service, std::string name): LogicService(name), mMaxID(0),
                mServer(service)
    {
    }

    virtual ~NetService();

public:
    /* start lisen and driver */
    virtual int Init();
    /* stop lisen and driver */
    virtual int Finit();
    /* process message */
    virtual bool Process(OperContext *ctx);

public:
    /* accept connection */
    void AcceptConnection(OperContext *ctx);
    /* client dropped */
    void DropConnection(OperContext *ctx);

#if 0
    /* process stop */
    void ProcessStop(OperContext *ctx);
    /* process start */
    void ProcessStart(OperContext *ctx);
#endif

    /* recv msg */
    void RecvMsg(OperContext *ctx);
    /* send msg */
    void SendMsg(OperContext *ctx);
    /* add local listener */
    int AddListener(std::string ip , short port);
    /* start connect remote */
    int StartConnectRemote(std::string ip, short port, uint64_t& connId);

private:
    /* start listener */
    int StartListener();
    /* stop listener */
    int StopListener();
    /* drop all conns */
    int DropAllConnections();
#if 0
    /* add remote server */
    int AddRemote(std::string ip, short port);
#endif

public:
    /* current connections */
    std::map<uint64_t, TcpConnection*> mConns;
    /* ip/port going to be connected */
    std::set<Addr> mRemoteAddrs;
    /* ip/port going to listen */
    std::set<Addr> mLocalAddrs;
    /* accepters key=addr.ToString(), value=pointer of Accepter */
    std::map<std::string, Accepter*> mAccepters;
    /* io driver */
    Driver *mDriver;
    /* current max conn id */
    uint64_t mMaxID;
    /* logic server */
    LogicService *mServer;
};


#endif


