#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <network.h>
#include <protocol.h>

#include "main.h"
#include "nbody.h"

Body body[BODIES] = {
	{
		.position = {0.0, 0.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 100000.0,
		.movable = false,
	}, {
		.position = {1.0, 0.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000.0,
		.movable = true,
	}
};

unsigned long peer;

void _send(Body *body, size_t bodies) {
	PacketObject po = {.type = PACKET_TYPE_OBJECT};
	int i;
	for(i = 0; i < bodies; i++) {
		printf("Body %i: {%f, %f} angle %f\n", i, body[i].position.x, body[i].position.y, body[i].angle);
		po.id = i;
		po.x = body[i].position.x;
		po.y = body[i].position.y;
		po.angle = 0.0;
		network_send(peer, &po, sizeof po);
	}
}

int main(int argc, char **argv) {
	PacketLobby pl = {};
	PacketSetup ps = {};
	
	body[1].velocity.y = sqrt(G*(body[1].mass + body[2].mass)/DIST(body[0], body[1]));
	
	if(network_init(PORT)) {
		fprintf(stderr, "network problem\n");
		return 1;
	}
	
	do {
		peer = network_recv(&pl, sizeof pl);
	} while(pl.type != PACKET_TYPE_LOBBY && pl.begin != 1);
	printf("begin\n");
	pl.begin = 2;
	network_send(peer, &pl, sizeof pl);
	
	ps.type = PACKET_TYPE_SETUP;
	ps.objects = BODIES;
	network_send(peer, &ps, sizeof ps);
	
	for(;;) {
		nbody_calc_forces(body, BODIES);
		nbody_move_bodies(body, BODIES, 1);
		_send(body, BODIES);
		usleep(16666); //60 fps
	}
	return 0;
}
