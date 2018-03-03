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
    bool open;
    bool closed;
    float f_value;
    int h_cost;  // cost from this vertex node to the goal
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
    int open_count;
    vec2 start, goal;
    AStarNode **nodes;
} AStarMap;

AStarMap *a_star_parse_map(char *fname);
void a_star_print_map(AStarMap *map);
bool a_star_solve_map(AStarMap *map);

#endif /* A_STAR_H */
