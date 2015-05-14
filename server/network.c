#include <stdio.h>
#include <stdlib.h>
#include "nbody.h"


void network_init() {
	
}

void network_recv() {
	
}

void network_send(Body *body, size_t bodies) {
	int i;
	for(i = 0; i < bodies; i++) {
		printf("Body %i: {%f, %f} angle %f\n", i, body[i].position.x, body[i].position.y, body[i].angle);
	}
}
