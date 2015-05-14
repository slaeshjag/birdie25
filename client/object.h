#ifndef __CLIENT_OBJECT_H__
#define	__CLIENT_OBJECT_H__

#define	CLIENT_OBJECT_COORD_SCALE 100.0

#include <darnit/darnit.h>
#include <stdbool.h>

struct ClientObject {
	int			x;
	int			y;
	double			dx;
	double			dy;
	int			angle;

	bool			sprite;
	union {
		DARNIT_TILE	*tile;
		DARNIT_SPRITE	*sprite;
	} pic;
};


void object_init(int objects);
void object_init_object(int id, int sprite_id);
void object_update(int id, double x, double y, double angle);
void object_get_coord(int id, int *x, int *y, int *w, int *h);
void object_draw();
void *object_thread(void *arne);


#endif
