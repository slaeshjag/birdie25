#ifndef __CLIENT_OBJECT_H__
#define	__CLIENT_OBJECT_H__

#define	CLIENT_OBJECT_COORD_SCALE 100.0

#include <darnit/darnit.h>
#include <stdbool.h>

#define SUP2(x) ((x)*(x))
#define DIST(b1, b2) sqrt(SUP2((b1).dx - (b2).dx) + SUP2((b1).dy - (b2).dy))

struct ClientObject {
	int			x;
	int			y;
	double			dx;
	double			dy;
	int			angle;
	
	double			tractor_beam;
	bool			sprite;
	union {
		DARNIT_TILE	*tile;
		DARNIT_SPRITE	*sprite;
	} pic;
};


void object_init(int objects, int _pre_simulations);
void object_init_object(int id, int sprite_id);
void object_update(int id, double x, double y, double angle);
void object_get_coord(int id, int *x, int *y, int *w, int *h);
void object_draw();
void *object_thread(void *arne);
void *object_get_icons();
void object_draw_world_border();
void pre_simulation_draw();

#endif
