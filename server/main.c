#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

#include <network.h>
#include <protocol.h>
#include <common_math.h>

#include "main.h"
#include "nbody.h"
#include "player.h"

extern Player *player;
extern int players;
static bool game_has_started = false;

Body body[BODIES + PLAYER_MAX + BULLETS] = {
	{
		.position = {0.0, 0.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {14.0, 3.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {-17.0, 36.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {-33.0, 4.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {20.0, -6.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {40.0, 22.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {-42.0, -20},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	}, {
		.position = {30.0, -31.0},
		.velocity = {0.0, 0.0},
		.force = {0.0, 0.0},
		.mass = 10000000.0,
		.radius = 0.65,
		.movable = false,
		.sprite = 0,
	},
};

void prepare_orbit(Body *smaller, Body *larger) {
	double distance, magnitude;
	Point direction, new;
	
	if((direction.x = (smaller->position.x - larger->position.x)) == 0.0)
		direction.y = 0.00000000000001;
	if((direction.y = (smaller->position.y - larger->position.y)) == 0.0)
		direction.y = 0.00000000000001;
	
	double velocity = sqrt(G*(larger->mass + smaller->mass)/sqrt(SUP2(direction.x) + SUP2(direction.y)));
	double angle = math_delta_to_angle(direction.x, direction.y);// + M_PI/2.0;
	
	smaller->velocity.x = velocity*cos(angle);	
	smaller->velocity.y = velocity*sin(angle);	
}

static void set_planet(int i, int orbit_around, double orbit_height,  int sprite, double mass, double radius) {
	double x, y;
	if(i >= BODIES)
		return;
	
	x = body[orbit_around].position.x; + orbit_height/M_SQRT2;
	y = body[orbit_around].position.y + orbit_height/M_SQRT2;
	
	body[i].position.x = x;
	body[i].position.y = y;
	body[i].mass = mass;
	body[i].radius = radius;
	body[i].angle = 0.0;
	body[i].movable = true;
	body[i].sprite = sprite;
	
	prepare_orbit(body + i, body + orbit_around);	
}

static void _send(Body *body, size_t bodies) {
	Packet pack;
	Player *p, *q;
	int i, j;
	Point pre[PRE_SIMULATIONS * players];
	int me[players];
	
	for(q = player, i = 0; q; q = q->next, i++)
		me[i] = q->id;
	
	nbody_pre_simulate(pre, PRE_SIMULATIONS, body, BODIES + players, me, players, 3*60.0);
	
	for(q = player, j = 0; q; q = q->next, j++) {
		pack.type = PACKET_TYPE_OBJECT;
		for(i = 0; i < bodies; i++) {
		//	printf("Body %i: {%f, %f} angle %f\n", i, body[i].position.x, body[i].position.y, body[i].angle);
			pack.object.id = i;
			pack.object.x = body[i].position.x;
			pack.object.y = body[i].position.y;
			pack.object.angle = body[i].angle;
			network_send(q->addr, &pack, sizeof(Packet));
		}
		
		pack.type = PACKET_TYPE_OBJECT;
		for(p = player; p; p = p->next) {
		//	printf("Player %i: {%f, %f} angle %f\n", player->id, body[i].position.x, body[i].position.y, body[i].angle);
			pack.object.id = p->id;
			pack.object.x = p->body->position.x;
			pack.object.y = p->body->position.y;
			pack.object.angle = p->body->angle;
			network_send(q->addr, &pack, sizeof(Packet));
		}
		
		for(i = 0; i < PRE_SIMULATIONS; i++) {
			pack.type = PACKET_TYPE_PRE_SIMULATION;
			pack.simul.id = i;
			pack.simul.x = pre[i + j*PRE_SIMULATIONS].x;
			pack.simul.y = pre[i + j*PRE_SIMULATIONS].y;
			network_send(q->addr, &pack, sizeof(Packet));
		}
		
		pack.type = PACKET_TYPE_PLAYER;
		pack.player.energy = q->body->energy;
		pack.player.accel = sqrt(SUP2(q->body->accel.x) + SUP2(q->body->accel.x)) / (PLAYER_ACCEL*M_SQRT2);
		pack.player.velocity = sqrt(SUP2(q->body->velocity.x) + SUP2(q->body->velocity.x)) / (SPEED_LIMIT*M_SQRT2);
		network_send(q->addr, &pack, sizeof(Packet));
	}
}

static void _setup(Body *body, size_t bodies) {
	Packet pack[2];
	Player *p, *q;
	PacketSetup *ps = &pack[0].setup;
	PacketSetupObject *pso = &pack[1].setup_object;
	int i;
	
	//Fuck multicast
	for(q = player; q; q = q->next) {
		ps->id = q->id;
		ps->type = PACKET_TYPE_SETUP;
		ps->objects = BODIES + players + BULLETS;
		ps->map_width = WIDTH;
		ps->pre_simulations = PRE_SIMULATIONS;
		ps->home = q->id - BODIES;
		network_send(q->addr, ps, sizeof(Packet));
		fprintf(stderr, "Announcing player %i with ip %lu\n", ps->id, q->addr);
		
		pso->type = PACKET_TYPE_SETUP_OBJECT;
		for(i = 0; i < bodies; i++) {
			pso->id = i;
			pso->sprite = body[i].sprite;
			network_send(q->addr, pso, sizeof(Packet));
		}
		
		for(p = player; p; p = p->next) {
			pso->id = p->id;
			pso->sprite = p->body->sprite;
			network_send(q->addr, pso, sizeof(Packet));
		}
	}
}

void *server_main(void *argleblargle) {
	Packet p = {};
	pthread_t pth;
	int i;

	

	/*if(network_init(PORT)) {
		fprintf(stderr, "network problem\n");
		pthread_exit(NULL);
	}*/
	
	
	//pthread_create(&pth, NULL, player_thread, NULL);
	for(i = 0;; i++) {
		if (game_has_started) {
			nbody_calc_forces(body, BODIES + players);
			nbody_move_bodies(body, BODIES + players, 1);
			_send(body, BODIES);
		} else if (!(i & 0x1F)) {
				p.type = PACKET_TYPE_LOBBY;
				p.lobby.begin = 3;
			if (player) {
				strcpy(p.lobby.name, player->pname);
			} else {
				strcpy(p.lobby.name, "Unknown");
			}
				network_broadcast(&p, sizeof(Packet));
		}
		usleep(16666); //60 fps
	}

	pthread_exit(NULL);
}


void server_start_game() {
	_setup(body, BODIES);
	game_has_started = true;
}

static struct Bullet {
	
};
static struct Bullet *bullet;

static void reset_bullet(int i) {
	struct Bullet *b;
	body[BODIES + PLAYER_MAX + i].position.x = -5000.0;
	body[BODIES + PLAYER_MAX + i].position.y = -5000.0;
	body[BODIES + PLAYER_MAX + i].velocity.x = 0.0;
	body[BODIES + PLAYER_MAX + i].velocity.y = 0.0;
	body[BODIES + PLAYER_MAX + i].force.x = 0.0;
	body[BODIES + PLAYER_MAX + i].force.y = 0.0;
	body[BODIES + PLAYER_MAX + i].mass = 0.00000000000001;
	body[BODIES + PLAYER_MAX + i].radius = 0.001;
	body[BODIES + PLAYER_MAX + i].movable = false;
	body[BODIES + PLAYER_MAX + i].sprite = 16;
	
	b = malloc(sizeof(struct Bullet));
	//b->	
}

void server_start() {
	Packet p;
	int i;
	
	/*Of doom*/
	set_planet(8, 0, -3.0, 1, 10000.0, 0.25);
	set_planet(9, 0, -4.0, 1, 10000.0, 0.25);
	set_planet(10, 2, 3.0, 1, 10000.0, 0.25);
	set_planet(11, 2, 2.0, 1, 10000.0, 0.25);
	set_planet(12, 3, 3.0, 1, 10000.0, 0.25);
	set_planet(13, 4, 3.0, 1, 10000.0, 0.25);
	set_planet(14, 4, 2.0, 1, 10000.0, 0.25);
	set_planet(15, 4, 3.0, 1, 10000.0, 0.25);
	set_planet(16, 5, 3.0, 1, 10000.0, 0.25);
	set_planet(17, 6, 5.0, 1, 10000.0, 0.25);
	set_planet(18, 6, 2.0, 1, 10000.0, 0.25);
	set_planet(19, 6, 3.0, 1, 10000.0, 0.25);
	set_planet(20, 6, 4.0, 1, 10000.0, 0.25);
	set_planet(21, 7, 4.0, 1, 10000.0, 0.25);
	set_planet(22, 7, 2.0, 1, 10000.0, 0.25);
	set_planet(23, 7, 3.0, 1, 10000.0, 0.25);

	
	for(i = 0; i < BULLETS; i++) {
		reset_bullet(i);
	}
	
	pthread_t serber;
	pthread_create(&serber, NULL, server_main, NULL);

/*	p.type = PACKET_TYPE_LOBBY;
	p.lobby.begin = 3;
	network_broadcast(&p, sizeof(Packet));*/
}


void server_packet_dispatch(Packet p, unsigned long addr) {
	static bool init = false;

	if (!init) {
		if (p.lobby.type != PACKET_TYPE_LOBBY || p.lobby.begin != 1)
			return;
		
		p.lobby.begin = 6;
		network_send(addr, &p, sizeof(Packet));
		
		Player *pl;
		pl = player_add(addr, 0, 3.0, p.lobby.name);
		
		init = true;
	} else {
		player_thread(p, addr);
	}

}
