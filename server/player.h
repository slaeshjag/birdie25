#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "nbody.h"

#define PLAYER_ACCEL .00001
#define PLAYER_MAX 16

typedef struct Player Player;
struct Player {
	int id;
	unsigned long addr;
	char *pname;
	
	Body *body;
	double energy;
	Player *next;
};

void player_thread(Packet pack, unsigned long addr);
Player *player_add(unsigned long addr, double x, double y, double vel_x, double vel_y, const char *pname);
bool player_check_coordinate_tractor_beam(double trac_x, double trac_y, double angle, double length, double point_x, double point_y);

#endif
