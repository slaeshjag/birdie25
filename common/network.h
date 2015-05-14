#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <stdlib.h>

#define PORT 1337

int network_init(int port);
void network_close();
int network_broadcast(void *buf, size_t bufsize);
int network_send(unsigned long to, void *buf, size_t bufsize);
unsigned long network_recv(void *buf, size_t bufsize);

#endif
