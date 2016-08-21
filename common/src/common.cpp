

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <event.h>
#include "common.h"


bool InvalidIP(const std::string& ip)
{
    int ret = 0;
    ret = inet_addr(ip.c_str());
    if ( INADDR_NONE == ret ) {
        return true;
    } else {
        return false;
    }
}

bool InvalidPort(const std::string& port)
{
    int i = 0;
    for ( ;i<port.length(); ++i ) {
        if ( !isdigit(port[i]) ) {
            return true;
        }
    }

    return false;
}

int StringAsInt(const std::string& str)
{
    int i;
    std::stringstream ss;
    ss<<str;
    ss>>i;

    return i;
}

std::string i2s(const int& i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

int s2i(const std::string& s)
{
    int i;
    std::stringstream ss;
    ss<<s;
    ss>>i;
    return i;
}

std::string GetPeerInfo(struct sockaddr_in * paddr)
{
    std::string str;
    str += inet_ntoa(paddr->sin_addr);
    str += ":";
    std::stringstream ss;
    ss<<ntohs(paddr->sin_port);
    str += ss.str();

    return str;
}

std::string GetPeerIP(struct sockaddr_in *paddr)
{
    return std::string(inet_ntoa(paddr->sin_addr));
}

int GetPeerPort(struct sockaddr_in *paddr)
{
    return ntohs(paddr->sin_port);
}

