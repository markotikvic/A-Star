#ifndef ASTAR_H
#define ASTAR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TILE_WALL  '#'
#define TILE_SPACE '.'
#define TILE_START 'S'
#define TILE_GOAL  'G'

#define idxtox(i,w) (i%w)
#define idxtoy(i,w) (i/w)

typedef struct {
    int w, h;
    char **tiles;
    int *open_set;
    int open_n;
    float *gscores;
    float *fscores;
    int *came_from;
    int *path;
} Map_t;

int parse_map(Map_t *map, FILE *fp);
int solve_map(Map_t *map);
void print_map(Map_t *map);
void free_map_buffers(Map_t *map);

#endif /* ASTAR_H */
