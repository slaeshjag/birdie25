#ifndef __QUAD_TREE_H_
#define __QUAD_TREE_H_

#include "nbody.h"

typedef struct MassCenter MassCenter;
struct MassCenter {
	double x;
	double y;
	double mass;
};

typedef struct QuadTree QuadTree;
struct QuadTree {
	MassCenter mass;
	Body *body;
	
	QuadTree *nw;
	QuadTree *ne;
	QuadTree *sw;
	QuadTree *se;
};

void quad_tree_add(QuadTree **tree, Body *body, double x, double y, double w, double h);
void quad_tree_free(QuadTree **tree);

#endif
