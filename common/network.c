#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "network.h"
//#include "main.h"

#ifdef _WIN32

//#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>

#define socklen_t int

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <protocol.h>
#include <sys/select.h>

#define SOCKET int
#define closesocket close
#define INVALID_SOCKET -1

#endif

struct Peer {
	struct sockaddr_in addr;
};

static struct Peer broadcast;

static SOCKET sock = -1;
static int port;

extern int server_forward_pipe[2];
extern bool we_are_hosting_a_game;

unsigned long network_local_ip() {
	struct in_addr addr;
	inet_pton(AF_INET, "127.0.0.1", &addr);
	return addr.s_addr;
}

int network_poll() {
	fd_set fds;
	struct timeval tv = {
		.tv_sec = 0,
		.tv_usec = 0,
	};
	FD_ZERO(&fds);
	FD_SET(sock, &fds);
	return select(sock + 1, &fds, NULL, NULL, &tv);
}

int network_init(int _port) {
	int broadcast_enabled = 1;
	int reuse_enabled = 1;
	struct hostent *broadcasthost;
	
	if(sock != INVALID_SOCKET)
		return 0;
	
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
	};
	
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		printf("fail\n");
		return -1;
	}
	
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcast_enabled, sizeof(broadcast_enabled));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *) &reuse_enabled, sizeof(broadcast_enabled));
	
	if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == INVALID_SOCKET) {
		closesocket(sock);
		printf("nope\n");
		return -1;
	}
	
	port = _port;
	
	broadcast.addr.sin_family = AF_INET;
	broadcast.addr.sin_port = htons(port);
	broadcasthost = gethostbyname("255.255.255.255");
	broadcast.addr.sin_addr = **((struct in_addr **) (broadcasthost->h_addr_list));
	//inet_pton(AF_INET, "255.255.255.255", &broadcast.addr.sin_addr);
	
	return 0;
}

void network_close() {
	closesocket(sock);
	sock = -1;
}

int network_broadcast(void *buf, size_t bufsize) {
	//printf("sending \n");
	return sendto(sock, buf, bufsize, 0, (struct sockaddr *) &broadcast.addr, sizeof(broadcast.addr));
}

int network_send(unsigned long to, void *buf, size_t bufsize) {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = to;
	return sendto(sock, buf, bufsize, 0, (struct sockaddr *) &addr, sizeof(addr));
}

unsigned long network_recv(void *buf, size_t bufsize) {
	int datalen;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	
	recvfrom(sock, buf, bufsize, 0, (struct sockaddr *) &addr, &addrlen);
	if (we_are_hosting_a_game)
		server_packet_dispatch(*((Packet *) buf));

	printf("recv\n");
	return addr.sin_addr.s_addr;	
}
