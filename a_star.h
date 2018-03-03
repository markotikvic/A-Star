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
    char symbol;
    float f_value;
    int g_score;  // cost from starting point to the this vertex node
    int f_score;
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

AStarMap *a_star_parse_map(char *fname);
bool a_star_solve_map(AStarMap *map);
void a_star_print_map(AStarMap *map);
void a_star_print_map_info(AStarMap *map);

#endif /* A_STAR_H */
