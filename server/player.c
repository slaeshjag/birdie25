#include <math.h>
#include <stdlib.h>
#include <network.h>
#include <protocol.h>
#include "player.h"


Player *player;
int players;

int player_init(int _players) { 
	player = calloc(_players, sizeof(Player));
	players = _players;
}

void player_set_accel(int id, double velocity) {
	
}

void *player_thread(void *arg) {
	Packet pack;
	unsigned long ip;
	int i;
	
	for(;;) {
		ip = network_recv(&pack, sizeof(Packet));
		
		switch(pack.type) {
			case PACKET_TYPE_CLIENT:
				for(i = 0; i < players; i ++)
					if(player[i].addr == ip)
						break;
				player[i].body->angle = pack.client.angle;
				if(pack.client.button.forward) {
					player[i].body->accel.x = PLAYER_ACCEL*cos(pack.client.angle);
					player[i].body->accel.y = PLAYER_ACCEL*sin(pack.client.angle);
				}
				if(pack.client.button.backward) {
					player[i].body->accel.x = -PLAYER_ACCEL*cos(pack.client.angle);
					player[i].body->accel.y = -PLAYER_ACCEL*sin(pack.client.angle);
				}
				break;
		}
	}
	
	return NULL;
}
