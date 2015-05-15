#include "object.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <network.h>
#include <protocol.h>
#include "main.h"
#include "player.h"

#define SUP2(x) ((x)*(x))
#define DIST(b1, b2) sqrt(SUP2((b1).dx - (b2).dx) + SUP2((b1).dy - (b2).dy))

static struct ClientObject *obj;
int objs;
static DARNIT_TILESHEET *planets;
static DARNIT_TILESHEET *icons;

static double object_scale;
static double coordinate_scale;

void object_get_coord(int id, int *x, int *y, int *w, int *h);
extern bool we_are_hosting_a_game;

void player_thread(Packet pack, unsigned long addr);

void object_init(int objects) {
	obj = malloc(sizeof(*obj) * objects);
	memset(obj, 0, sizeof(*obj) * objects);
	objs = objects;
	planets = d_render_tilesheet_load("res/planets.png", 256, 256, DARNIT_PFORMAT_RGBA8);
	icons = d_render_tilesheet_load("res/icons.png", 32, 32, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_scale_algorithm(planets, DARNIT_SCALE_LINEAR);
	coordinate_scale = CLIENT_OBJECT_COORD_SCALE;
	object_scale = 0.3;
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

	xi = x * coordinate_scale;
	yi = y * coordinate_scale;
	ai = (angle / M_PI * 1800);

	object_get_coord(id, &arne, &goesta, &gw, &gh);
	
	xi -= gw / 2;
	yi -= gh / 2;
	
	if (obj[id].sprite) {
		d_sprite_rotate(obj[id].pic.sprite, ai);
		d_sprite_move(obj[id].pic.sprite, xi, yi);
	} else {
		#if 0
		int cx, cy;
		old_scale = object_scale;
		object_scale = 1.0;
		object_get_coord(if, &arne, &goesta, &ogw, &ogh);
		object_scale = old_scale;

		cx = (ogw - gw) / 2;
		cy = (ogh - gh) / 2;
		#endif
		d_render_tile_move(obj[id].pic.tile, 0, xi, yi);
		d_render_tile_size_set(obj[id].pic.tile, 0, gw, gh);
	}

	obj[id].x = xi;
	obj[id].y = yi;
	obj[id].dx = x;
	obj[id].dy = y;
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

	double min_distance = HUGE_VAL, dist;
	int closest = 0;
	int id = player_get();
	for (i = 0; i < objs; i++) {
		if (obj[i].sprite) {
			printf("%i is sprite\n", i);
			continue;
		}
		if ((dist = DIST(obj[i], obj[id])) < min_distance) {
			min_distance = dist;
			closest = i;
		}
	}
	//coordinate_scale = 30.0*min_distance;//pow(M_E, -0.01*min_distance);
	printf("scale %f, %f, closest is %i\n", coordinate_scale, min_distance, closest);
}


void *object_thread(void *arne) {
	Packet pack;
	unsigned long addr;
	int datalen;
	for (;;) {
		addr = network_recv(&pack, sizeof(Packet));
		
		//if (we_are_hosting_a_game) {
			//player_thread(pack, addr);
			/*datalen = sizeof(Packet);

			write(server_forward_pipe[1], &datalen, sizeof(datalen));
			write(server_forward_pipe[1], &addr, sizeof(addr));
			write(server_forward_pipe[1], &pack, sizeof(pack));*/
		//}

		if (addr != sip)
			continue;
		
		switch(pack.type) {
			case PACKET_TYPE_OBJECT:
				object_update(pack.object.id, pack.object.x, pack.object.y, pack.object.angle);
				break;
			case PACKET_TYPE_SETUP_OBJECT:
				object_init_object(pack.setup_object.id, pack.setup_object.sprite);
				//camera_init(pack.setup.id);
				break;
		}
	}
}


void *object_get_icons() {
	return icons;
}
