#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include "SocketUDP.h"
#include "SocketUDPUtils.h"
 
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 512
#endif

SocketUDP* creerSocketUDP() {
	SocketUDP* soc = allocSocketUDP();
	soc->socket = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (soc->socket == -1) {
		perror("socket");
		free(soc);
		soc = NULL;
	}
	return soc;
}

SocketUDP* creerSocketUDPattache(const char* adresse, int port) {
	SocketUDP* soc = creerSocketUDP();
	if (soc == NULL) {
		return NULL;
	}
	struct sockaddr_in in;
	if (initSockAddr(adresse, port, &in) == -1) {
		fprintf(stderr, "Erreur d'initilisation de l'adresse\n");
		free(soc);
		return NULL;
	}
	
	if (bind(soc->socket, (struct sockaddr*) &in, sizeof(struct sockaddr_in)) == -1) {
		perror("bind");
		free(soc);
		return NULL;
	}
	
	soc->local = getIdBySockAddr(&in);
	
	return soc;
}

char* getLocalName(SocketUDP* socket) {
	char* res = socket->local.name;
	if ((res == NULL) && (socket->socket != -1)) {
		res = getLocalIP(socket);
		if (res != NULL) {
			char buffer[BUFFER_SIZE];
			struct sockaddr_in in;
			memset(&in, 0, sizeof(struct sockaddr_in));
			in.sin_family = AF_INET;
			in.sin_addr.s_addr = inet_addr(res);
			int err = getnameinfo(
							(struct sockaddr*) &in, sizeof(struct sockaddr_in),
						   buffer, BUFFER_SIZE,
						   NULL, 0, NI_DGRAM);
			if (err == -1) {
				perror("getnameinfo");
				return NULL;
			}
			socket->local.name = malloc((strlen(buffer) + 1) * sizeof(char));
			strcpy(socket->local.name, buffer);
			res = socket->local.name;
		}
	}
	return res;
}

char* getLocalIP(SocketUDP* socket) {
	char* res = socket->local.ip;
	if ((res == NULL) && (socket->socket != -1)) {
		struct sockaddr_in in;
		int size = sizeof(struct sockaddr_in);
		if (getpeername(socket->socket, (struct sockaddr*) &in, (socklen_t*)&size) == -1) {
			perror("getpeername");
			return NULL;
		}
		char buffer[BUFFER_SIZE];
		int err = getnameinfo(
						(struct sockaddr*) &in, sizeof(struct sockaddr_in),
					   buffer, BUFFER_SIZE,
					   NULL, 0, NI_DGRAM | NI_NUMERICHOST);
		if (err == -1) {
			perror("getnameinfo");
			return NULL;
		}
		socket->local.ip = malloc((strlen(buffer) + 1) * sizeof(char));
		strcpy(socket->local.ip, buffer);
		res = socket->local.ip;
	}
	return res;
}

int getLocalPort(SocketUDP* socket) {
	int port = socket->local.port;
	
	if ((port == -1) && (socket->socket != -1)) {
		struct sockaddr_in in;
		int size = sizeof(struct sockaddr_in);
		if (getpeername(socket->socket, (struct sockaddr*) &in, (socklen_t*)&size) == -1) {
			perror("getpeername");
			return -1;
		}
		port = getPortByAddr(&in);
	}
	
	return port;
}

int writeToSocketUDP(SocketUDP* socket, const char* adresse, int port, char* buffer, int length) {
	if (socket->socket == -1) {
		fprintf(stderr, "Impossible d'envoyer des donnees sur une socket non ouverte\n");
		return -1;
	}
	struct sockaddr_in in;
	if (initSockAddr(adresse, port, &in) == -1) {
		fprintf(stderr, "Erreur d'initilisation de l'adresse\n");
		return -1;
	}
	int n = sendto(socket->socket, buffer, length, 0, 
					(struct sockaddr*) &in, sizeof(struct sockaddr_in));
	if (n == -1) {
		perror("sendto");
		return -1;
	}
	return n;
}

int readFromSocketUDP(SocketUDP* socket, char* buffer, int length, 
						char* adresse, int* port, int timeout) {
	if (socket->socket == -1) {
		fprintf(stderr, "Impossible d'e recevoir des donnees sur une socket non ouverte\n");
		return -1;
	}
	fd_set set;
	FD_ZERO(&set);
	FD_SET(socket->socket, &set);
	int err = 0;
	
	if (timeout > 0) {
		struct timeval time;
		time.tv_sec = timeout;
		time.tv_usec = 0;
		err = select(socket->socket + 1, &set, NULL, NULL, &time);
	} else {
		err = select(socket->socket + 1, &set, NULL, NULL, NULL);
	}
	if (err == -1) {
		perror("select");
		return -1;
	} else if (err == 0) {
		return 0;
	}
	
	struct sockaddr_in in;
	int size = sizeof(struct sockaddr_in);
	int n = recvfrom(socket->socket, buffer, length, 0, 
					(struct sockaddr*) &in, (socklen_t*) &size);
	if (n == -1) {
		perror("recvfrom");
		return -1;
	}
	if ((adresse != NULL) && (port != NULL)) {
		id i = getIdBySockAddr(&in);
		if (adresse != NULL) {
			strcpy(adresse, i.ip);
		}
		if (port != NULL) {
			*port = i.port;
		}
	}

	
	return n;
}

int closeSocketUDP(SocketUDP* socket) {
	int err = 0;
	shutdown(socket->socket, SHUT_RDWR);
	
	err = close(socket->socket);
	
	if (socket->local.name != NULL) {
		free(socket->local.name);
	}
	if (socket->local.ip != NULL) {
		free(socket->local.ip);
	}
	
	free(socket);
	return err;
}
