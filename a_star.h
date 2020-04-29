#ifndef A_STAR_H
#define A_STAR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int x, y;
} Vec2;

typedef struct a_star_node {
    Vec2 pos;
    char tag;
    int g_score; // cost from start to the this vertex node
    int f_score; // cost from this vertex node to the goal
    struct a_star_node *came_from;
} AStarNode;

typedef struct {
    void **items;
    int len;
    int cap;
} Array;

typedef struct map {
    int w, h;
    int explored;
    AStarNode *start, *goal;
    AStarNode **nodes;
} AStarMap;

typedef struct path {
    int weight;
    int nodes_count;
    Vec2 *steps; // steps are reversed
} AStarPath;

int parse_map(AStarMap *map, char *fname);
int solve_map(AStarMap *map, AStarPath *path);
void print_map(AStarMap *map);
void print_path(AStarPath *path);

#endif /* A_STAR_H */
