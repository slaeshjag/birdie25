#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <protocol.h>
#include <common_math.h>
#include "player.h"
#include "main.h"


Player *player;
int players;
extern Body body[];

Player *player_add(unsigned long addr, double x, double y, double vel_x, double vel_y, const char *pname) {
	Player *p;
	p = malloc(sizeof(Player));
	p->addr = addr;
	p->id = BODIES + players;
	p->body = body + BODIES + players;
	p->body->position.x = x;
	p->body->position.y = y;
	p->body->velocity.x = vel_x;
	p->body->velocity.y = vel_y;
	p->body->sprite = 64 + players;
	p->body->mass = 1.0;
	p->body->movable = true;
	p->body->radius = 0.1;
	p->pname = strdup(pname);
	
	p->energy = 1.0;
	
	p->next = player;
	players++;
	player = p;
	return p;
}


void player_broadcast_package(Packet pack) {
	Player *p;

	for (p = player; p; p = p->next)
		network_send(p->addr, &pack, sizeof(pack));
	return;
}



void player_thread(Packet pack, unsigned long addr) {
	Player *p;
	
//	for(;;) {
		switch(pack.type) {
			case PACKET_TYPE_CLIENT:
				for(p = player; p; p = p->next)
					if(p->addr == addr)
						break;
				if(!p)
					break;
				p->body->angle = pack.client.angle;
				/* HACK: Below accelerations have sign flipped. I don't know why it works, but it does */
				if(pack.client.button.forward) {
					p->body->da += PLAYER_ACCEL/100.0;
				} else
					p->body->da -= PLAYER_ACCEL/30.0;
				
				if(p->body->da >= PLAYER_ACCEL)
					p->body->da = PLAYER_ACCEL;
				else if(p->body->da <= 0)
					p->body->da = 0;
				
				p->body->accel.x = -p->body->da*cos(pack.client.angle);
				p->body->accel.y = -p->body->da*sin(pack.client.angle);
				break;
			case PACKET_TYPE_LOBBY:
				if (pack.lobby.begin != 5)
					break;

				fprintf(stderr, "A player joined\n");
				// TODO: Put this somewhere near the home planet
				double vel;
				Body b;
				b.position.x = 0.0;
				b.position.y = -1.0;
				vel = sqrt(G*(body[0].mass + 1.0)/DIST(body[0], b));
				p = player_add(addr, 0.0, 1.0, vel, 0.0, pack.lobby.name);
				pack.lobby.begin = 6;
				player_broadcast_package(pack);
				fprintf(stderr, "Broadcasting %s\n", pack.lobby.name);

				Player *next;
				for (next = player; next; next = next->next) {
					if (next->id == p->id)
						continue;
					pack.type = PACKET_TYPE_LOBBY;
					pack.lobby.begin = 6;
					strcpy(pack.lobby.name, next->pname);
					fprintf(stderr, "Telling player about %s addr %lu\n", pack.lobby.name, addr);
					network_send(addr, &pack, sizeof(pack));
				}

				break;
		}
//	}
	
//	return NULL;
}


bool player_check_coordinate_tractor_beam(double trac_x, double trac_y, double angle, double length, double point_x, double point_y) {
	double dx, dy, da, a1, a2;

	dx = point_x - trac_x;
	dy = point_y - trac_y;
	dx *= -1;
	dy *= -1;
	if (dx*dx + dy*dy >= length*length)
		return false;

	da = math_delta_to_angle(dx, dy);
	a1 = angle - M_PI/10;
	a2 = angle + M_PI/10;
	
	if (a1 < 0) {
		a1 += M_PI * 2;
		if (da >= a1 && da <= a2)
			return false;
		return true;
	} else if (a2 > 2 * M_PI) {
		a2 -= M_PI * 2;
		if (da >= a1 || da < a2)
			return true;
		return false;
	} else {
		if (da >= a1 && da <= a2)
			return true;
		return false;
	}
}
