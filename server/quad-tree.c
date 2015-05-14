/**
 * quad-tree.c - Helper functions for quad tree data structure
 * @author Axel Isaksson <axelis@kth.se>, Steven Arnow <stevena@kth.se>
 * @copyright MIT/X11 License, see COPYING
 */

#include <stdlib.h>
#include <stdbool.h>
#include "quad-tree.h"

/**
 * Add a body to a quad tree. The center of mass of the nodes will be
 * calculated along the way.
 * @param tree Pointer to a quad tree data structure.
 * @param body Body to add to the tree
 * @param x X coordinate of the area covered by the tree
 * @param y Y coordinate of the area covered by the tree
 * @param w Width of the area covered by the tree
 * @param h Height of the area covered by the tree
 */
void quad_tree_add(QuadTree **tree, Body *body, double x, double y, double w, double h) {
	double m;
	int xquad;
	int yquad;
	
	if(!(tree && body))
		return;
	
	if(!*tree) {
		/*Create a new leaf node*/
		*tree = calloc(1, sizeof(QuadTree));
		(*tree)->body = body;
		(*tree)->mass.mass = body->mass;
		(*tree)->mass.x = body->position.x;
		(*tree)->mass.y = body->position.y;
		return;
	}
	
	/*Determine which subtree to use*/
	QuadTree **quadrant[2][2] = {{&(*tree)->nw, &(*tree)->ne}, {&(*tree)->sw, &(*tree)->se}};
	/*Update center of mass*/
	m = (*tree)->mass.mass + body->mass;
	(*tree)->mass.x = (body->position.x * body->mass + (*tree)->mass.x * (*tree)->mass.mass)/m;
	(*tree)->mass.y = (body->position.y * body->mass + (*tree)->mass.y * (*tree)->mass.mass)/m;
	(*tree)->mass.mass = m;
	xquad = body->position.x > (x + w/2.0);
	yquad = body->position.y > (y + h/2.0);
	
	quad_tree_add(quadrant[yquad][xquad],body, x + (w/2.0)*xquad, y + (h/2.0)*yquad, w/2.0, h/2.0);
	
	/*Convert the node to an internal node*/
	if((body = (*tree)->body))
		return;
	
	(*tree)->body = NULL;
	
	xquad = body->position.x > (x + w/2.0);
	yquad = body->position.y > (y + h/2.0);
	
	quad_tree_add(quadrant[yquad][xquad],body, x + (w/2.0)*xquad, y + (h/2.0)*yquad, w/2.0, h/2.0);
}

/**
 * Free an entire quad tree and NULL its pointer
 * @param tree Pointer to a quad tree data structure.
 */
void quad_tree_free(QuadTree **tree) {
	if(!(tree && *tree))
		return;
	
	quad_tree_free(&(*tree)->nw);
	quad_tree_free(&(*tree)->ne);
	quad_tree_free(&(*tree)->sw);
	quad_tree_free(&(*tree)->se);
	
	free(*tree);
	*tree = NULL;
}
