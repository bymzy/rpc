

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


