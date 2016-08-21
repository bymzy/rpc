

#ifndef __TCP_SOCKET_HPP__
#define __TCP_SOCKET_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "Log.hpp"
#include "common.h"

#define MAX_BACK_LOG 1000


class TcpSocket
{
public:
    TcpSocket():mPreRecv(false), mSocket(-1), mClosed(true), mError(0)
    {
    }
    ~TcpSocket()
    {
        Close();
    }
    
public:
    int Create(int sock = -1)
    {
        int err = 0;
        do {
            if (-1 == sock) {
                /* default create one tcp socket*/
                mSocket = socket(AF_INET, SOCK_STREAM, 0);
                if (mSocket == -1) {
                    err = errno;
                    error_log(strerror(err));
                }
            } else {
                int opt;
                socklen_t len = sizeof(int);
                /* getsockopt to test whether the fd is a socket */
                err = getsockopt(sock, SOL_SOCKET, SO_TYPE, &opt, &len);
                if (err == 0) {
                    mSocket = sock;
                    break;
                }
                err = errno;
                error_log("getsockopt failed, errno: " << errno
                        << ", errstr: " << strerror(errno));
            }

        } while(0);

        if (0 == err) {
            mClosed = false;
        }
            
        return err;
    }

    int Listen(std::string ip, short port)
    {
        int err = 0;

        do {
            err = SetReuseAddr();
            if (0 != err) {
                break;
            }

            struct sockaddr_in listenAddr;
            memset(&listenAddr, 0, sizeof(sockaddr_in) );
            listenAddr.sin_family = AF_INET;
            listenAddr.sin_port = htons(port);
            listenAddr.sin_addr.s_addr = inet_addr(ip.c_str());

            err = bind(mSocket, (struct sockaddr*)&listenAddr, sizeof(sockaddr));
            if (0 != err) {
                err = errno;
                error_log(strerror(errno));
                break;
            }

            err = listen(mSocket, MAX_BACK_LOG);
            if (0 != err) {
                err = errno;
                error_log(strerror(errno));
                break;
            }

            mIP = ip;
            mPort = port;
        } while(0);

        return err;
    }

    int Connect(std::string ip, short port)
    {
        int err = 0;

        do {
            struct sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(sockaddr_in));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

            err = connect(mSocket, (struct sockaddr*)&serverAddr, sizeof(sockaddr));
            if ( 0 != err ) {
                if ( EINPROGRESS != errno ) {
                    err = errno;
                    error_log("socket connect failed,errno : " << errno
                            << ", errstr: " << strerror(errno));
                    break;
                }
                err = CheckConnect();
                if ( 0 != err ) {
                    break;
                }
            }

            mClosed = false;
            mIP = ip;
            mPort = port;
        } while(0);

        return err;
    }

    int Accept(int& fd, std::string& ip, int& port)
    {
        int tempFd = -1;
        int err = 0;
        struct sockaddr_in clientAddr;
        socklen_t len = sizeof(sockaddr_in);

        do {
            tempFd = accept(mSocket, (struct sockaddr*)&clientAddr, &len);
            if (tempFd < 0) {
                err = errno;
                error_log("accept failed! error: " << errno
                        << ", errstr: " << strerror(errno));
                break;
            }

            ip = GetPeerIP(&clientAddr);
            port = GetPeerPort(&clientAddr);
            fd = tempFd;
        } while(0);

        return err;
    }

    int Close()
    {
        int err = 0;

        if (-1 != mSocket) {
            err = close(mSocket);
        }

        return err;
    }

    void SetAddr(std::string ip , int port)
    {
        mIP = ip;
        mPort = port;
    }

    int SetNonblocking()
    {
        int err = 0;

        if (fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFD, 0) | O_NONBLOCK) < 0) {
            err = errno;
            error_log("fcntl set nonblock failed, error:"<<strerror(err));
        }
        return err;
    }

    int SetReuseAddr()
    {
        int err = 0;
        int opt = 1;

        err = setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
        if (0 != err) {
            err = errno;
            error_log("set reuse addr failed!");
        }
        
        return err;
    }

    int CheckConnect()
    {
        fd_set rset,wset;
        int err = 0;
        int ret = 0;
        struct timeval tv;

        while (1) {
            FD_ZERO(&rset);
            FD_ZERO(&wset);
            FD_SET(mSocket,&rset);
            FD_SET(mSocket,&wset);

            tv.tv_sec = 3;
            tv.tv_usec = 0;

            ret = select(mSocket+1,&rset,&wset,NULL,&tv);
            if ( ret < 0) {
                err = errno;
                error_log("select failed,error:" << strerror(errno));	
                break;
            } else if ( ret ==0 ) {
                continue;
            } else {
                if (FD_ISSET(mSocket,&rset)&&FD_ISSET(mSocket,&wset)) {
                    socklen_t len = sizeof(err);
                    if (getsockopt(mSocket,SOL_SOCKET,SO_ERROR,&err,&len) < 0) {
                        error_log("getsockopt for soekct failed,error:" << strerror(errno));
                        err = errno;
                        break;
                    }

                    if (0 != err) {
                        error_log("connect failed,error:" << strerror(errno));
                        err = errno;
                        break;
                    }
                }

                if (FD_ISSET(mSocket,&wset)&&!FD_ISSET(mSocket,&rset)) {
                    error_log("connect success!");
                    break;
                }
                break;
            }
        }

        return err;
    }

    int GetFd()
    {
        return mSocket;
    }

    bool NoOK()
    {
        return (mClosed || mError != 0);
    }

    int Send(const char *buf, int len, int &sent)
    {
        int err = 0;
        if (NoOK()) {
            return EINVAL;
        }

        sent = send(mSocket, buf, len, 0);
        if (sent < 0) {
            err = errno;
            if (err != EAGAIN &&
                    err != EWOULDBLOCK) {
                error_log("send buffer failed, errno: " << errno
                        << ", errstr: " << strerror(errno));
                mError = err;
            } else {
                err = 0;
            }
        }
        
        return err;
    }

    int SendAll(const char *buf, int len)
    {
        int tosend = len;
        int sent = 0;
        int ret = 0;
        int err = 0;

        if (NoOK()) {
            return EINVAL;
        }

        while(1)
        {
            ret = 0;
            do{
                ret = send(mSocket , buf + sent , tosend , 0);
            } while(ret<0&&(( EAGAIN == errno ) || ( EWOULDBLOCK == errno )));

            if ( ret < 0 ) {
                err = errno;
                mError = err;
                error_log("send buffer failed, errno: " << errno
                        << ", errstr: " << strerror(errno));
                break;
            }

            tosend -= ret;
            sent += ret;

            if ( 0 == tosend ) {
                err = 0;
                break;
            }
        }

        return err;
    }

    bool HasPreRecv()
    {
        return mPreRecv;
    }

    void SetPreRecv(bool has)
    {
        mPreRecv = has;
    }

    /* pre recv the lenth to recv */
    int PreRecv(int& len)
    {
        int err = 0;
        int tempLen = 0;
        char buf[4] ;

        if (NoOK()) {
            return EINVAL;
        }

        do {
            if (mPreRecv) {
                error_log("has pre received, should not do this again!");
                assert(0);
                return 0;
            }
            mPreRecv = true;

            err = RecvAll(buf, sizeof(int));
            if (0 != err) {
                trace_log("PreRecv failed! error: " <<err
                        << ", errstr: " << strerror(err));
                break;
            }

            memcpy(&tempLen, buf, sizeof(int));
            len = ntohl(tempLen);

            debug_log("prerecv len: " << len);
        } while(0);

        return err;
    }

    /* try recv len bytes */
    int Recv(char *buf, int len, int& received)
    {
        int err = 0;
        if (NoOK()) {
            return EINVAL;
        }

        do {
            received = recv(mSocket, buf, len, 0);
            if (received < 0) {
                err = errno;
                if (EAGAIN == err
                        || EWOULDBLOCK == err) {
                    /* this is not error, just reset err and received*/
                    err = 0;
                    received = 0;
                } else {
                    mError = err;
                    error_log("socket recv failed, ip: " << mIP
                            << ", port: " << mPort);
                    break;
                }
            } else if (received == 0) {
                /* this mean that is closed */
                err = EINVAL;
                mClosed = true;
            }
            trace_log("socket received " << received << " bytes!");
        } while(0);

        return err;
    }

    /* recv len bytes unless error occurs */
    int RecvAll(char *buf, int len)
    {
        int toRecv = len;
        int recved = 0;
        int ret = 0;
        int err = 0;
        if (NoOK()) {
            return EINVAL;
        }

        while(1)
        {
            ret = 0;

            do{
                ret = recv(mSocket, buf + recved,toRecv,0);
            } while(ret<0&&(( EAGAIN == errno ) || ( EWOULDBLOCK == errno )));

            if (ret < 0) {
                err = errno;
                mError = err;
                error_log("recv buffer failed, errno: " << errno
                        << ", errstr: " << strerror(errno));
                break;
            } else if (ret == 0) {
                trace_log("client closed!");
                err = EINVAL;
                mClosed = true;
                break;
            }

            toRecv -= ret;
            recved += ret;

            if ( 0 == toRecv ) {
                err = 0;
                break;
            }
        }

        return err;
    }

public:
    std::string mIP;
    int mPort;
    bool mPreRecv;
    int mSocket;
    bool mClosed;
    int mError;
};

#endif


