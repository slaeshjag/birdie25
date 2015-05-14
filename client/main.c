#include <darnit/darnit.h>
#include "object.h"
#include <protocol.h>
#include <network.h>


enum GameState {
	GAME_STATE_SELECT_NAME,
	GAME_STATE_LOBBY,
	GAME_STATE_HOST,
	GAME_STATE_PLAYING,
};


int main(int argc, char **argv) {
	int i;
	PacketLobby pl = { PACKET_TYPE_LOBBY, 1 };

	d_init_custom("Jymdsjepp^wArs", 1280, 720, 0, "birdie25", NULL);

	object_init(4);
	for (i = 0; i < 4; i++) {
		d_render_offset(0, 0);
		object_init_object(i, 1);
		object_update(i, 200.0 * i, 360.0, 0.0);
	}
	
	network_init(PORT);
	network_broadcast(&pl, sizeof(pl));
	
	pl.begin = 0;
	unsigned long sip;
	do {
		loop:
		sip = network_recv(&pl, sizeof(pl));
		if (pl.type != PACKET_TYPE_LOBBY)
			goto loop;
	} while(pl.begin != 2);


	for (;;) {
		d_render_begin();
		object_draw();
		d_render_end();
		d_loop();
	}
}
