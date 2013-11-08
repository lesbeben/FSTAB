#ifndef SOCKETUDP_H
#define SOCKETUDP_H

typedef struct {
	char* name;
	char* ip;
	int port;
} id;

typedef struct {
	int socket;
	id local;
} SocketUDP;
 
SocketUDP* creerSocketUDP();

SocketUDP* creerSocketUDPattache(const char* adresse, int port);

char* getLocalName(SocketUDP* socket);

char* getLocalIP(SocketUDP* socket);

int getLocalPort(SocketUDP* socket);

int writeToSocketUDP(SocketUDP* socket, const char* adresse, int port, char* buffer, int length);

/**
 * 'adresse' doit etre initialisé et contenir 15 octets.
 */
int readFromSocketUDP(SocketUDP* socket, char* buffer, int length, 
						char* adresse, int* port, int timeout);

int closeSocketUDP(SocketUDP* socket);

#endif
