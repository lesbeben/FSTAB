#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>

#include "SocketUDPUtils.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 512
#endif
 
SocketUDP* allocSocketUDP() {
    SocketUDP* sock = malloc(sizeof (SocketUDP));
    sock->socket = -1;
    sock->local.ip = NULL;
    sock->local.name = NULL;
    sock->local.port = -1;
    return sock;
}

int initSockAddr(const char* adresse, int port, struct sockaddr_in* in) {
    if ((adresse != NULL) && (strlen(adresse) <= 0)) {
        adresse = NULL;
    }

    struct addrinfo hint;
    memset(&hint, 0, sizeof (struct addrinfo));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    struct addrinfo* res = NULL;
    int s;
    char service[10];
    sprintf(service, "%d", port);
    if ((s = getaddrinfo(adresse, service, &hint, &res)) == -1) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }
    memcpy(in, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    return 0;
}

id getIdBySockAddr(const struct sockaddr_in* in) {
    id i = {NULL, NULL, -1};

    char* ip = malloc(BUFFER_SIZE * sizeof (char));
    char port[BUFFER_SIZE];
    getnameinfo((struct sockaddr*) in, (socklen_t) sizeof (struct sockaddr_in),
            ip, BUFFER_SIZE,
            port, BUFFER_SIZE,
            NI_NUMERICHOST | NI_NUMERICSERV);
    i.ip = ip;
    i.port = strtol(port, NULL, 0);

    return i;
}

int getPortByAddr(const struct sockaddr_in* in) {
    char buff[BUFFER_SIZE];
    int err = getnameinfo(
            (struct sockaddr*) in, (socklen_t) sizeof (struct sockaddr_in),
            NULL, 0,
            buff, BUFFER_SIZE,
            NI_DGRAM | NI_NUMERICSERV);
    if (err == -1) {
        perror("getnameinfo");
        return -1;
    }
    int port = strtol(buff, NULL, 0);
    return port;
}
