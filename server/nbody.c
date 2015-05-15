/**
 * nbody-seq - Sequential implementation of brute force n-body problem
 * nbody-seq.c - Main source file
 * @author Axel Isaksson <axelis@kth.se>, Steven Arnow <stevena@kth.se>
 * @copyright MIT/X11 License, see COPYING
 */

#include <darnit/darnit.h>
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


extern uint8_t score[8];

bool ball_collision_handled(Body *body, int n, int ball, Point deltap, Point deltav) {
	int i;

	for (i = 0; i < n; i++) {
		double distance, dx, dy;

		if (i == ball)
			continue;
		distance = body[ball].radius + body[i].radius;
		dx = body[ball].position.x + deltap.x - body[i].position.x;
		dy = body[ball].position.y + deltap.y - body[i].position.y;
		if (dx * dx + dy * dy > distance * distance)
			continue;
		
		if (body[ball].sprite >= 64 && body[ball].sprite < 73) {
			if ((ball > BULLET_START) && (ball < BULLET_START + BULLETS)) {
				body[i].energy -= 0.1;
			} else {
				double dv, dvx, dvy;
				dvx = body[ball].velocity.x - body[i].velocity.x;
				dvy = body[ball].velocity.y - body[i].velocity.y;
				dv = sqrt(dvx*dvx + dvy*dvy);
				dv *= (body[ball].mass + body[i].mass);
				dv /= 1000000;
				body[ball].energy -= dv;
			}
			if (body[ball].energy < 0)
				body[ball].energy = 0;
		}
		
		if (body[i].sprite >= 64 && body[i].sprite < 73) {
			double dv, dvx, dvy;
			if ((ball > BULLET_START) && (ball < BULLET_START + BULLETS)) {
				body[i].energy -= 0.1;
			} else {
				dvx = body[ball].velocity.x - body[i].velocity.x;
				dvy = body[ball].velocity.y - body[i].velocity.y;
				dv = sqrt(dvx*dvx + dvy*dvy);
				dv *= (body[ball].mass + body[i].mass);
				dv /= 1000000;
				body[i].energy -= dv;
			}
			
			if (body[i].energy < 0)
				body[i].energy = 0;
		}

		if (d_time_get() - body[i].tract.last_attachment < 1500) {
			if (body[i].tract.last_obj - BODIES == ball) {
				fprintf(stderr, "Parked something! %i\n", ball);
				body[i].tract.parked = true;
				score[ball]++;
				return true;
			} return false;
		} if (d_time_get() - body[ball].tract.last_attachment < 1500) {
			if  (body[ball].tract.last_obj - BODIES == i) {
				fprintf(stderr, "Parked something! %i\n", i);
				body[ball].tract.parked = true;
				score[i]++;
				return true;
			} return false;
		}
		
		double vx1, vx2, vy1, vy2, cvx, cvy;

		cvx = deltap.x + body[ball].velocity.x;
		cvy = deltap.y + body[ball].velocity.y;

		vx1 = (cvx * (body[ball].mass - body[i].mass) + 2 * body[i].mass * body[i].velocity.x) / (body[ball].mass + body[i].mass);
		vx2 = (body[i].velocity.x * (body[i].mass - body[ball].mass) + 2 * body[ball].mass * cvx) / (body[ball].mass + body[i].mass);
		vy1 = (cvx * (body[ball].mass - body[i].mass) + 2 * body[i].mass * body[i].velocity.x) / (body[ball].mass + body[i].mass);
		vy2 = (body[i].velocity.x * (body[i].mass - body[ball].mass) + 2 * body[ball].mass * cvx) / (body[ball].mass + body[i].mass);
		vy1 *= 0.7;
		vy2 *= 0.7;
		vx1 *= 0.7;
		vx2 *= 0.7;
		body[ball].velocity.x = vx1;
		body[ball].velocity.y = vy1;
		body[i].velocity.x = vx2;
		body[i].velocity.y = vy2;
		
		return true;
	}

	return false;
}



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

	for (i = 0; i < 8; i++)
		score[i] = 0;
	
	for(i = 0; i < n; i++) {
		if(!body[i].movable)
			goto out;
		if (body[i].tract.parked) {
			score[body[i].tract.last_obj - BODIES]++;
			goto out;
		}
		if (body[i].sprite >= 64 && body[i].sprite < 73) {
			body[i].energy += 0.0001;
			if (body[i].energy > 1.0)
				body[i].energy = 1.0;
		}

		if (body[i].tract.obj) {
			int l;

			l = body[i].tract.obj;
			if (3.0 * body[l].energy < body[i].tract.distance || !body[l].tractor_beam) {
				body[i].tract.last_obj = body[i].tract.obj;
				body[i].tract.obj = 0;
				body[i].tract.last_attachment = d_time_get();
				goto no_attach;
			}
			body[i].position.x = body[l].position.x - cos((body[l].angle - body[i].tract.angle)) * body[i].tract.distance;
			body[i].position.y = body[l].position.y - sin((body[l].angle - body[i].tract.angle)) * body[i].tract.distance;
//			body[i].position = body[body[i].tract.obj].position;
			continue;
		}

		no_attach:
		deltav.x = dt * (body[i].force.x / body[i].mass);
		deltav.y = dt * (body[i].force.y / body[i].mass);
		deltap.x = dt * (body[i].velocity.x + deltav.x/2);
		deltap.y = dt * (body[i].velocity.y + deltav.y/2);
		
		body[i].velocity.x += deltav.x;
		body[i].velocity.y += deltav.y;
		if(body[i].velocity.x > SPEED_LIMIT)
			body[i].velocity.x = SPEED_LIMIT;
		else if(body[i].velocity.x < -SPEED_LIMIT)
			body[i].velocity.x = -SPEED_LIMIT;
		if(body[i].velocity.y > SPEED_LIMIT)
			body[i].velocity.y = SPEED_LIMIT;
		else if(body[i].velocity.y < -SPEED_LIMIT)
			body[i].velocity.y = -SPEED_LIMIT;
		
		if (body[i].tract.parked)
			continue;
		if (ball_collision_handled(body, n, i, deltap, deltav))
			continue;

		bool coll = false;
		if (body[i].position.x + deltap.x > WIDTH) {
			body[i].position.x = WIDTH;
			body[i].velocity.x *= -0.7;
			coll = true;
		} else if (body[i].position.x + deltap.x < -WIDTH) {
			body[i].position.x = -WIDTH;
			body[i].velocity.x *= -0.7;
			coll = true;
		} else {
			body[i].position.x += deltap.x;
		}
		if (body[i].position.y + deltap.y > WIDTH) {
			body[i].position.y = WIDTH;
			body[i].velocity.y *= -0.7;
			coll = true;
		} else if (body[i].position.y + deltap.y < -WIDTH) {
			body[i].position.y = -WIDTH;
			body[i].velocity.y *= -0.7;
			coll = true;
		} else {
			body[i].position.y += deltap.y;
		}
		
		if (body[i].sprite >= 64 && body[i].sprite < 73 && coll) {
			double dv, dvx, dvy;
			dvx = body[i].velocity.x;
			dvy = body[i].velocity.y;
			dv = sqrt(dvx*dvx + dvy*dvy);
			dv *= (body[i].mass);
			dv *= 12;
			body[i].energy -= dv;
			if (body[i].energy < 0)
				body[i].energy = 0;
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
