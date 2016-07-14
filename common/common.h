#ifndef __COMMON_H
#define __COMMON_H

#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"


bool InvalidIP(const std::string& ip);
bool InvalidPort(const std::string& port);
int StringAsInt(const std::string& str);
std::string GetPeerInfo(struct sockaddr_in *);


#endif

