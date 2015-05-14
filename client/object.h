#ifndef __CLIENT_OBJECT_H__
#define	__CLIENT_OBJECT_H__

#define	CLIENT_OBJECT_COORD_SCALE 1.0

#include <darnit/darnit.h>

struct ClientObject {
	int			x;
	int			y;
	int			angle;
	DARNIT_SPRITE		*sprite;
};


void object_init(int objects);
void object_init_object(int id, int sprite_id);
void object_update(int id, double x, double y, double angle);
void object_draw();


#endif
