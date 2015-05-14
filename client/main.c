#include <darnit/darnit.h>
#include "object.h"
#include <protocol.h>
#include <network.h>
#include <pthread.h>

unsigned long sip;

enum GameState {
	GAME_STATE_SELECT_NAME,
	GAME_STATE_LOBBY,
	GAME_STATE_HOST,
	GAME_STATE_PLAYING,
};


int main(int argc, char **argv) {
	int i;
	PacketLobby pl = { PACKET_TYPE_LOBBY, 1 };
	PacketSetup ps;

	d_init_custom("Jymdsjepp^wArs", 1280, 720, 0, "birdie25", NULL);

	network_init(PORT);
	network_broadcast(&pl, sizeof(pl));
	
	pl.begin = 0;
	do {
		loop:
		sip = network_recv(&pl, sizeof(pl));
		if (pl.type != PACKET_TYPE_LOBBY)
			goto loop;
	} while(pl.begin != 2);

	ps.type = PACKET_TYPE_LOBBY;
	do {
		network_recv(&ps, sizeof(ps));
	} while(ps.type != PACKET_TYPE_SETUP);

	object_init(ps.objects);
	camera_init(0);

	for (i = 0; i < ps.objects; i++)
		object_init_object(i, 1);

	pthread_t tid;
	pthread_create(&tid, NULL, object_thread, NULL);

	d_render_blend_enable();

	for (;;) {
		d_render_begin();
		handle_camera();
		object_draw();
		d_render_end();
		d_loop();
	}
}
