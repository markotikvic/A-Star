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
    vec2 came_from;
    char tag;
    int g_score; // cost from star to the this vertex node
    int f_score; // cost from this vertex node to the goal
} AStarNode;

typedef struct node_set {
    AStarNode **nodes;
    int len;
    int cap;
} AStarSet;

typedef struct map {
    int w, h;
    AStarNode *start, *goal;
    AStarNode **nodes;
} AStarMap;

typedef struct path {
    int weight;
    int steps_count;
    vec2 *steps; // steps are reversed
} AStarPath;

AStarMap  *a_star_parse_map(char *fname);
AStarPath *a_star_solve_map(AStarMap *map);
void a_star_print_map(AStarMap *map);
void a_star_print_map_info(AStarMap *map);
void a_star_print_path(AStarPath *path);

#endif /* A_STAR_H */
