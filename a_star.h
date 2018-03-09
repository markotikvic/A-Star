#ifndef A_STAR_H
#define A_STAR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct vector2 {
    int x, y;
} vec2; 

typedef struct a_star_node {
    vec2 pos;
    char tag;
    int g_score; // cost from star to the this vertex node
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
    int tested_count;
    AStarNode *start, *goal;
    AStarNode **nodes;
} AStarMap;

typedef struct path {
    int weight;
    int nodes_count;
    vec2 *steps; // steps are reversed
} AStarPath;

vec2 v2(int x, int y);
AStarMap  *a_star_parse_map(char *fname);
AStarPath *a_star_solve_map(AStarMap *map, vec2 start, vec2 goal);
void a_star_print_map(AStarMap *map);
void a_star_print_path(AStarPath *path);

#endif /* A_STAR_H */
