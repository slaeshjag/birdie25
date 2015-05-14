#include <math.h>
#include <stdlib.h>
#include <network.h>
#include <protocol.h>
#include "player.h"
#include "main.h"


Player *player;
int players;
extern Body body[];

Player *player_add(unsigned long addr, double x, double y) {
	Player *p;
	p = malloc(sizeof(Player));
	p->addr = addr;
	p->id = BODIES + players;
	p->body = body + BODIES + players;
	p->body->position.x = x;
	p->body->position.y = y;
	p->body->sprite = 64 + players;
	
	p->next = player;
	players++;
	player = p;
	return p;
}

void *player_thread(void *arg) {
	Packet pack;
	Player *p;
	unsigned long ip;
	
	for(;;) {
		ip = network_recv(&pack, sizeof(Packet));
		switch(pack.type) {
			case PACKET_TYPE_CLIENT:
				for(p = player; p; p = p->next)
					if(p->addr == ip)
						break;
				if(!p)
					break;
				p->body->angle = pack.client.angle;
				if(pack.client.button.forward) {
					p->body->accel.x = PLAYER_ACCEL*cos(pack.client.angle);
					p->body->accel.y = PLAYER_ACCEL*sin(pack.client.angle);
				}
				if(pack.client.button.backward) {
					p->body->accel.x = -PLAYER_ACCEL*cos(pack.client.angle);
					p->body->accel.y = -PLAYER_ACCEL*sin(pack.client.angle);
				}
				break;
		}
	}
	
	return NULL;
}
