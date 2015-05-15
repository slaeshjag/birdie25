#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <protocol.h>
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
	p->pname = strdup(pname);
	
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
					p->body->accel.x = -PLAYER_ACCEL*cos(pack.client.angle);
					p->body->accel.y = -PLAYER_ACCEL*sin(pack.client.angle);
				}
				if(pack.client.button.backward) {
					p->body->accel.x = PLAYER_ACCEL*cos(pack.client.angle);
					p->body->accel.y = PLAYER_ACCEL*sin(pack.client.angle);
				}
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
					fprintf(stderr, "Telling player about %s addr %i\n", pack.lobby.name, addr);
					network_send(addr, &pack, sizeof(pack));
				}

				break;
		}
//	}
	
//	return NULL;
}
