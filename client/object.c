#include "object.h"
#include <string.h>
#include <math.h>


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

	sprintf(path, "res/%i.spr", id);
	obj[id].sprite = d_sprite_load(path, 0, DARNIT_PFORMAT_RGBA8);
	return;
}


void object_update(int id, double x, double y, double angle) {
	int xi, yi, ai;

	xi = x * CLIENT_OBJECT_COORD_SCALE;
	yi = y * CLIENT_OBJECT_COORD_SCALE;
	ai = (angle / M_PI * 1800);
	xi -= d_sprite_width(obj[id].sprite);
	yi -= d_sprite_height(obj[id].sprite);
	d_sprite_rotate(obj->sprite, ai);
	d_sprite_move(obj->sprite, xi, yi);

	obj[id].x = xi;
	obj[id].y = yi;
	obj[id].angle = ai;
}


void object_draw() {
	int i;

	for (i = 0; i < objs; i++)
		d_sprite_draw(obj[i].sprite);
}
