#ifndef __NBODY_H_
#define __NBODY_H_

#include <stdbool.h>

#define SUP2(x) ((x)*(x))
#define DIST(b1, b2) sqrt(SUP2((b1).position.x - (b2).position.x) + SUP2((b1).position.y - (b2).position.y))

#define G (6.67e-11)
#define MERGE_RADIUS 0.001

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Point Point;
struct Point {
	double x;
	double y;
};

typedef struct Body Body;
struct Body {
	Point position;
	Point velocity;
	Point force;
	double mass;
	double angle;
	bool movable;
};

typedef struct Rectangle Rectangle;
struct Rectangle {
	double x;
	double y;
	double w;
	double h;
};

void nbody_calc_forces(Body *body, int n);
void nbody_move_bodies(Body *body, int n, double dt);

#endif
