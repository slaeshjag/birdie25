#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "nbody.h"

#define PLAYER_ACCEL 1.0
#define PLAYER_MAX 16

typedef struct Player Player;
struct Player {
	int id;
	unsigned long addr;
	
	Body *body;
	
	Player *next;
};

void *player_thread(void *);
Player *player_add(unsigned long addr, double x, double y);

#endif
