#include "object.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <network.h>
#include <protocol.h>

extern unsigned long sip;
static struct ClientObject *obj;
static int objs;
static DARNIT_TILESHEET *planets;

static double object_scale;
static double coordinate_scale;

void object_get_coord(int id, int *x, int *y, int *w, int *h);


void object_init(int objects) {
	obj = malloc(sizeof(*obj) * objects);
	memset(obj, 0, sizeof(*obj) * objects);
	objs = objects;
	planets = d_render_tilesheet_load("res/planets.png", 256, 256, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_scale_algorithm(planets, DARNIT_SCALE_LINEAR);
	coordinate_scale = CLIENT_OBJECT_COORD_SCALE;
	object_scale = 0.5;
	return;
}


void object_init_object(int id, int sprite_id) {
	char path[512];

	if (sprite_id < 64) {
		obj[id].sprite = false;
		obj[id].pic.tile = d_render_tile_new(1, planets);
		d_render_tile_set(obj[id].pic.tile, 0, sprite_id);
	} else {
		sprintf(path, "res/%i.spr", sprite_id - 64);
		obj[id].pic.sprite = d_sprite_load(path, 0, DARNIT_PFORMAT_RGBA8);
		obj[id].sprite = true;
	}

	return;
}


void object_update(int id, double x, double y, double angle) {
	int xi, yi, ai, arne, goesta, gw, gh;

	xi = x * CLIENT_OBJECT_COORD_SCALE;
	yi = y * CLIENT_OBJECT_COORD_SCALE;
	ai = (angle / M_PI * 1800);
	
	object_get_coord(id, &arne, &goesta, &gw, &gh);
	xi -= gw / 2;
	yi -= gh / 2;
	if (obj[id].sprite) {
		d_sprite_rotate(obj[id].pic.sprite, ai);
		d_sprite_move(obj[id].pic.sprite, xi, yi);
	} else {
		int cx, cy;

		cx = xi + (gw * (object_scale - 1.));
		cy = yi + (gh * (object_scale - 1.));
		d_render_tile_move(obj[id].pic.tile, 0, cx, cy);
		d_render_tile_size_set(obj[id].pic.tile, 0, ((double) object_scale * gw), ((double) object_scale * gh));
	}

	obj[id].x = xi;
	obj[id].y = yi;
	obj[id].angle = ai;
//	fprintf(stderr, "Object %i at %i, %i @ %i\n", id, xi, yi, ai);
}


void object_get_coord(int id, int *x, int *y, int *w, int *h) {
	if (obj[id].sprite) {
		*w = d_sprite_width(obj[id].pic.sprite);
		*h = d_sprite_height(obj[id].pic.sprite);
	} else {
		int tw, th;
		d_render_tilesheet_geometrics(planets, NULL, NULL, &tw, &th);
		*w = object_scale * tw;
		*h = object_scale * th;
	}

	*x = obj[id].x;
	*y = obj[id].y;
}


void object_draw() {
	int i;

	for (i = 0; i < objs; i++) {
		if (obj[i].sprite)
			d_sprite_draw(obj[i].pic.sprite);
		else
			d_render_tile_draw(obj[i].pic.tile, 1);
	}
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
