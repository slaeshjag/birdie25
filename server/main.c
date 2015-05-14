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

void _send(Body *body, size_t bodies) {
	int i;
	for(i = 0; i < bodies; i++) {
		printf("Body %i: {%f, %f} angle %f\n", i, body[i].position.x, body[i].position.y, body[i].angle);
	}
}

int main(int argc, char **argv) {
	PacketLobby pack = {};
	unsigned long peer;
	
	body[1].velocity.y = sqrt(G*(body[1].mass + body[2].mass)/DIST(body[0], body[1]));
	
	if(network_init(PORT)) {
		fprintf(stderr, "network problem\n");
		return 1;
	}
	
	do {
		peer = network_recv(&pack, sizeof pack);
	} while(pack.type != PACKET_TYPE_LOBBY && pack.begin != 1);
	printf("begin\n");
	pack.begin = 2;
	network_send(peer, &pack, sizeof pack);
	
	for(;;) {
		nbody_calc_forces(body, BODIES);
		nbody_move_bodies(body, BODIES, 1);
		_send(body, BODIES);
		usleep(16666); //60 fps
	}
	return 0;
}
