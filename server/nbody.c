/**
 * nbody-seq - Sequential implementation of brute force n-body problem
 * nbody-seq.c - Main source file
 * @author Axel Isaksson <axelis@kth.se>, Steven Arnow <stevena@kth.se>
 * @copyright MIT/X11 License, see COPYING
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <protocol.h>
#include <math.h>
#include "nbody.h"
#include "main.h"

/**
 * Calculate the force vectors for an array of bodies
 * @param body The array of bodies to be processed
 * @param n Size of the body array
 */
void nbody_calc_forces(Body *body, int n) {
	int i, j;
	double distance, magnitude;
	Point direction;
	
	for(i = 0; i < (n - 1); i++) {
		for(j = i + 1; j < n; j++) {
			if((distance = sqrt(SUP2(body[i].position.x - body[j].position.x) + SUP2(body[i].position.y - body[j].position.y))) < MERGE_RADIUS)
				continue; //Prevent division by zero by disallowing move when distance = 0
			magnitude = (G * body[i].mass * body[j].mass) / SUP2(distance);
			direction.x = body[j].position.x - body[i].position.x;
			direction.y = body[j].position.y - body[i].position.y;
			
			body[i].force.x += magnitude * direction.x / distance + body[i].mass*body[i].accel.x;
			body[j].force.x -= magnitude * direction.x / distance + body[j].mass*body[j].accel.x;
			body[i].force.y += magnitude * direction.y / distance + body[i].mass*body[i].accel.y;
			body[j].force.y -= magnitude * direction.y / distance + body[j].mass*body[j].accel.y;
		}
	}
}

/**
 * Calculate the new speed and position vectors of an array of bodies,
 * reset their force vectors
 * @param body The array of bodies to be processed
 * @param n Size of the body array
 * @param dt Time delta of the simulation step
 */
void nbody_move_bodies(Body *body, int n, double dt) {
	Point deltav, deltap;
	int i;
	
	for(i = 0; i < n; i++) {
		if(!body[i].movable)
			goto out;
		
		deltav.x = dt * (body[i].force.x / body[i].mass);
		deltav.y = dt * (body[i].force.y / body[i].mass);
		deltap.x = dt * (body[i].velocity.x + deltav.x/2);
		deltap.y = dt * (body[i].velocity.y + deltav.y/2);
		
		body[i].velocity.x += deltav.x;
		body[i].velocity.y += deltav.y;
		if (body[i].position.x + deltap.x > WIDTH) {
			fprintf(stderr, "KRASH!\n");
			body[i].position.y = WIDTH;
		} else {
			body[i].position.x += deltap.x;
		}
		if (body[i].position.y + deltap.y > WIDTH) {
			fprintf(stderr, "KRASH!\n");
			body[i].position.y = WIDTH;
		} else {
			body[i].position.y += deltap.y;
		}

		out:
		body[i].force.x = 0.0;
		body[i].force.y = 0.0;
	}
}

void nbody_pre_simulate(Point *out, int no, Body *b, int nb, int *me, int nm, double time) {
	Body *copy;
	double dt, t;
	int i, j, d;
	
	copy = malloc(sizeof(Body) * nb);
	memcpy(copy, b, sizeof(Body) * nb);
	
	dt = time / ((double) no);
	for(t = 0, d = 0; t < time; t += dt, d++) {
		nbody_calc_forces(copy, nb);
		nbody_move_bodies(copy, nb, dt);
		for(i = 0; i < nm; i++) {
			j = me[i];
			out[i*no + d].x = copy[j].position.x;
			out[i*no + d].y = copy[j].position.y;
		}
	}
	
	free(copy);
}

#if 0
/**
 * Main program entry point. Parse command line arguments, create array of
 * bodies for simulation, start timers and run simulation.
 */
int main(int argc, char **argv) {
	int n;
	long long endtime;
	double dt = 0.01;
	long long t;
	double start_time, end_time;
	int i;
	
	/*Parse arguments*/
	switch(argc) {
		case 4:
			srand(atoi(argv[3]));
		case 3:
			endtime = atoll(argv[2]);
			n = atoi(argv[1]);
			break;
		default:
			fprintf(stderr, "Usage: nbody <number of bodies> <number of steps> [random seed]\n");
			return 1;
	}
	
	if(n <= 0)
		return 1;
	
	#ifdef VISUAL
	visualize_init();
	#endif
	Body body[n];
	memset(body, 0, sizeof(Body) * n);
	/*Randomize bodies' starting position*/
	for(i = 0; i < n; i++) {
		body[i].position.x = (((double) (rand() % 2000)) - 1000.0) / 1000.0;
		body[i].position.y = (((double) (rand() % 2000)) - 1000.0) / 1000.0;
		body[i].mass = rand() % 10000 + 5000;
	}
	
	/*Run simulation*/
	start_time = timer();
	for(t = 0; t < endtime; t++) {
		calc_forces(body, n);
		move_bodies(body, n, dt);
		#ifdef VISUAL
		visualize_step(body, n, ((double) HEIGHT)/3.0, NULL);
		#endif
	}
	end_time = timer();
	
	fprintf(stdout, "Time used: %.4fs, args: ", end_time - start_time);
	for (i = 0; i < argc; i++)
		fprintf(stdout, "'%s' ", argv[i]);
	fprintf(stdout, "\n");
	
	return 0;
}
#endif
