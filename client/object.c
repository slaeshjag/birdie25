#include "object.h"
#include <string.h>
#include <math.h>
#include <network.h>
#include <protocol.h>

extern unsigned long sip;
static struct ClientObject *obj;
static int objs;


void object_init(int objects) {
	obj = malloc(sizeof(*obj) * objects);
	memset(obj, 0, sizeof(*obj) * objects);
	objs = objects;
	return;
}


void object_init_object(int id, int sprite_id) {
	char path[512];

	sprintf(path, "res/%i.spr", sprite_id);
	obj[id].sprite = d_sprite_load(path, 0, DARNIT_PFORMAT_RGBA8);
	return;
}


void object_update(int id, double x, double y, double angle) {
	int xi, yi, ai;

	xi = x * CLIENT_OBJECT_COORD_SCALE;
	yi = y * CLIENT_OBJECT_COORD_SCALE;
	ai = (angle / M_PI * 1800);
	xi -= d_sprite_width(obj[id].sprite) / 2;
	yi -= d_sprite_height(obj[id].sprite) / 2;
	d_sprite_rotate(obj[id].sprite, ai);
	d_sprite_move(obj[id].sprite, xi, yi);

	obj[id].x = xi;
	obj[id].y = yi;
	obj[id].angle = ai;
//	fprintf(stderr, "Object %i at %i, %i @ %i\n", id, xi, yi, ai);
}


void object_get_coord(int id, int *x, int *y, int *w, int *h) {
	*w = d_sprite_width(obj[id].sprite);
	*h = d_sprite_height(obj[id].sprite);

	*x = obj[id].x;
	*y = obj[id].y;
}


void object_draw() {
	int i;

	for (i = 0; i < objs; i++)
		d_sprite_draw(obj[i].sprite);
}


void *object_thread(void *arne) {
	Packet pack;
	for (;;) {
		if (network_recv(&pack, sizeof(Packet)) != sip)
			continue;
		
		switch(pack.type) {
			case PACKET_TYPE_OBJECT:
				object_update(pack.object.id, pack.object.x, pack.object.y, pack.object.angle);
				break;
			case PACKET_TYPE_SETUP_OBJECT:
				object_init_object(pack.setup_object.id, pack.setup_object.sprite);
				break;
		}
	}
}
