#ifndef __MAIN_H__
#define __MAIN_H__

#include "player.h"
#include <protocol.h>

#define BODIES 32
#define STATIONARY 8
#define PRE_SIMULATIONS 20
#define BULLETS 80

#define WIDTH 50.0
#define SPEED_LIMIT 0.1

typedef struct Client Client;
struct Client {
	unsigned long addr;
};

void prepare_orbit(Body *smaller, Body *larger);
void server_packet_dispatch(Packet p, unsigned long addr);
void server_start();
void server_start_game();

#endif
