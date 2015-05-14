#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
		.velocity = {0.0, 0.001},
		.force = {0.0, 0.0},
		.mass = 10000.0,
		.movable = true,
	}
};

int main(int argc, char **argv) {
	Rectangle rect;
	double theta = 0.0;
	//double dt = 0;
	
	network_init();
	
	for(;;) {
		network_recv();
		nbody_calc_forces(body, BODIES);
		nbody_move_bodies(body, BODIES, 1);
		network_send(body, BODIES);
		usleep(16666); //60 fps
	}
	return 0;
}
