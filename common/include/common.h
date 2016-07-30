#ifndef __COMMON_H
#define __COMMON_H

#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


bool InvalidIP(const std::string& ip);
bool InvalidPort(const std::string& port);
int StringAsInt(const std::string& str);
std::string GetPeerInfo(struct sockaddr_in *);
std::string GetPeerIP(struct sockaddr_in *paddr);
int GetPeerPort(struct sockaddr_in *paddr);


#endif


