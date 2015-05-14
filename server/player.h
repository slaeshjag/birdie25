#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "nbody.h"

#define PLAYER_ACCEL 5.0

typedef struct Player Player;
struct Player {
	int id;
	unsigned long addr;
	
	Body *body;
};

int player_init(int);
void *player_thread(void *);

#endif
