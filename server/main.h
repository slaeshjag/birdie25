#ifndef __SERVER_MAIN_H__
#define ___SERVER_MAIN_H__

#include "player.h"
#include <protocol.h>

//#define BODIES 48
#define	BODIES (48 + 80)
#define STATIONARY 8
#define PRE_SIMULATIONS 20
#define BULLETS 80
#define	BULLET_START (BODIES - BULLETS)

#define BULLET_SPEED 0.3

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

void reset_bullet(int i);
void send_bullets();
int alloc_bullet();


#endif
