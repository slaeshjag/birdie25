#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

#include <network.h>
#include <protocol.h>

#include "main.h"
#include "nbody.h"
#include "player.h"

extern Player *player;
extern int players;

Body body[BODIES] = {
	{
		.position = {0.0, 0.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {1.0, 0.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000.0,
		.movable = true,
		.sprite = 1,
	}, {
		.position = {3.0, 0.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000.0,
		.movable = true,
		.sprite = 1,
	}, {
		.position = {3.01, 0.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 1000.0,
		.movable = true,
		.sprite = 64,
	}
};

unsigned long peer;

static void _send(Body *body, size_t bodies) {
	Packet pack = {PACKET_TYPE_OBJECT};
	PacketObject *po = &pack.object;
	
	int i;
	for(i = 0; i < bodies; i++) {
		printf("Body %i: {%f, %f} angle %f\n", i, body[i].position.x, body[i].position.y, body[i].angle);
		po->id = i;
		po->x = body[i].position.x;
		po->y = body[i].position.y;
		po->angle = body[i].angle;
		network_send(peer, po, sizeof(Packet));
	}
}

static void _setup(Body *body, size_t bodies) {
	Packet pack[2];
	PacketSetup *ps = &pack[0].setup;
	PacketSetupObject *pso = &pack[1].setup_object;
	int i;
	
	ps->type = PACKET_TYPE_SETUP;
	ps->objects = BODIES;
	network_send(peer, ps, sizeof(Packet));
	
	pso->type = PACKET_TYPE_SETUP_OBJECT;
	for(i = 0; i < bodies; i++) {
		pso->id = i;
		pso->sprite = body[i].sprite;
		network_send(peer, pso, sizeof(Packet));
	}
}

int main(int argc, char **argv) {
	Packet p = {};
	pthread_t pth;
	
	body[1].velocity.y = sqrt(G*(body[0].mass + body[1].mass)/DIST(body[0], body[1]));
	body[2].velocity.y = sqrt(G*(body[0].mass + body[2].mass)/DIST(body[0], body[2]));
	body[3].velocity.y = sqrt(G*(body[2].mass + body[3].mass)/DIST(body[2], body[3]));
	
	if(network_init(PORT)) {
		fprintf(stderr, "network problem\n");
		return 1;
	}
	
	p.type = PACKET_TYPE_LOBBY;
	p.lobby.begin = 3;
	network_broadcast(&p, sizeof(Packet));
	do {
		peer = network_recv(&p, sizeof(Packet));
	} while(p.lobby.type != PACKET_TYPE_LOBBY || p.lobby.begin != 1);
	printf("begin\n");
	p.lobby.begin = 2;
	network_send(peer, &p, sizeof(Packet));
	_setup(body, BODIES);
	
	player_init(1);
	player[0].addr = peer;
	player[0].body = body + 3;
	pthread_create(&pth, NULL, player_thread, NULL);
	
	for(;;) {
		nbody_calc_forces(body, BODIES);
		nbody_move_bodies(body, BODIES, 1);
		_send(body, BODIES);
		usleep(16666); //60 fps
	}
	return 0;
}
