#ifndef __MAIN_H__
#define __MAIN_H__

#include "player.h"
#include <protocol.h>

#define BODIES 4

#define WIDTH 100
#define HEIGHT 100

typedef struct Client Client;
struct Client {
	unsigned long addr;
};


void server_packet_dispatch(Packet p, unsigned long addr);
void server_start();
void server_start_game();

#endif
