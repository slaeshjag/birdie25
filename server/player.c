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

Player *player_add(unsigned long addr, int home, double spawn_height, const char *pname) {
	Player *p;
	p = malloc(sizeof(Player));
	p->addr = addr;
	p->id = BODIES + players;
	p->body = body + BODIES + players;

	p->body->sprite = 64 + players;
	p->body->mass = 1.0;
	p->body->movable = true;
	p->body->radius = 0.1;
	p->pname = strdup(pname);
	
	p->body->energy = 1.0;
	p->body->position.x = body[home].position.x + spawn_height/M_SQRT2;
	p->body->position.y = body[home].position.y + spawn_height/M_SQRT2;
	
	prepare_orbit(p->body, body + home);
	
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
				
				if (pack.client.button.beam) {
					player_attach_asteroid(p);
				}
				
				p->body->tractor_beam = pack.client.button.beam;
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
				p = player_add(addr, players, 3.0, pack.lobby.name);
				
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

	da = math_delta_to_angle(dx, -dy);
	if (da > 2 * M_PI)
		da -= M_PI * 2;
	a1 = angle - M_PI/11;
	a2 = angle + M_PI/11;
	
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


void player_attach_asteroid(Player *p) {
	int i, player;

	player = p->id;

	for (i = 0; i < BODIES; i++) {
		if (!body[i].tract.obj) {
			if (body[i].sprite < 74)
				continue;
			double trac_x, trac_y, length;
			trac_x = body[player].position.x, trac_y = body[player].position.y;
			length = p->body->energy * 3.0f;
			if (!player_check_coordinate_tractor_beam(trac_x, trac_y, body[player].angle, length, body[i].position.x, body[i].position.y))
				continue;

			double dx, dy;
			dx = -(trac_x - body[i].position.x);
			dy = -(trac_y - body[i].position.y);
			body[i].tract.obj = p->id;
			body[i].tract.distance = sqrt(dx*dx+dy*dy);
			body[i].tract.angle = math_delta_to_angle(dx, dy);
		} else
			continue;
	}
}
