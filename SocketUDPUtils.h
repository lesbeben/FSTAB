#ifndef SOCKETTCPUTILS_h
#define SOCKETTCPUTILS_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "SocketUDP.h"

SocketUDP* allocSocketUDP();

int initSockAddr(const char* adresse, int port, struct sockaddr_in* in);

id getIdBySockAddr(const struct sockaddr_in* in);

int getPortByAddr(const struct sockaddr_in* in);

#endif 
 
