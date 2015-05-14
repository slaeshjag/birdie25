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

#include "main.h"
#include "nbody.h"
#include "player.h"

extern Player *player;
extern int players;
static bool game_has_started = false;

Body body[BODIES + PLAYER_MAX] = {
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
		.sprite = 0,
	}
};

static void _send(Body *body, size_t bodies) {
	Packet pack = {PACKET_TYPE_OBJECT};
	PacketObject *po = &pack.object;
	Player *p, *q;
	
	int i;
	for(q = player; q; q = q->next) {
		for(i = 0; i < bodies; i++) {
			printf("Body %i: {%f, %f} angle %f\n", i, body[i].position.x, body[i].position.y, body[i].angle);
			po->id = i;
			po->x = body[i].position.x;
			po->y = body[i].position.y;
			po->angle = body[i].angle;
			network_send(q->addr, po, sizeof(Packet));
		}
		
		for(p = player; p; p = p->next) {
			printf("Player %i: {%f, %f} angle %f\n", player->id, body[i].position.x, body[i].position.y, body[i].angle);
			po->id = p->id;
			po->x = p->body->position.x;
			po->y = p->body->position.y;
			po->angle = p->body->angle;
			network_send(q->addr, po, sizeof(Packet));
		}
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
		ps->objects = BODIES + players;
		ps->width = WIDTH;
		ps->height = HEIGHT;
		network_send(q->addr, ps, sizeof(Packet));
		
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
				strcpy(p.lobby.name, "Arne");
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


void server_start() {
	Packet p;

	body[1].velocity.y = sqrt(G*(body[0].mass + body[1].mass)/DIST(body[0], body[1]));
	body[2].velocity.y = sqrt(G*(body[0].mass + body[2].mass)/DIST(body[0], body[2]));
	body[3].velocity.y = sqrt(G*(body[2].mass + body[3].mass)/DIST(body[2], body[3]));
	
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

		printf("begin\n");
		p.lobby.begin = 6;
		network_send(addr, &p, sizeof(Packet));
		
		player_add(addr, 1.0, 2.0, p.lobby.name);
		init = true;
	} else {
		player_thread(p, addr);
	}

}
