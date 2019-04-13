#ifndef __A_STAR_H__
#define __A_STAR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct point {
    int x, y;
} point; 

typedef struct a_star_node {
    point pos;
    char tag;
    int g_score; // cost from start to the this vertex node
    int f_score; // cost from this vertex node to the goal
    struct a_star_node *came_from;
} AStarNode;

typedef struct node_set {
    AStarNode **nodes;
    int len;
    int cap;
} AStarSet;

typedef struct map {
    int w, h;
    int explored;
    AStarNode *start, *goal;
    AStarNode **nodes;
} AStarMap;

typedef struct path {
    int weight;
    int nodes_count;
    point *steps; // steps are reversed
} AStarPath;

point point2D(int x, int y);
AStarMap  *a_star_parse_map(char *fname);
AStarPath *a_star_solve_map(AStarMap *map);
void a_star_print_map(AStarMap *map);
void a_star_print_path(AStarPath *path);

#endif /* A_STAR_H */
