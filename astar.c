#include <string.h>
#include <math.h>

#include "astar.h"

static float distance(int n1, int n2, int w) {
    int x1 = idxtox(n1,w);
    int y1 = idxtoy(n1,w);
    int x2 = idxtox(n2,w);
    int y2 = idxtoy(n2,w);
    return abs(x1 - x2) + abs(y1 - y2);
}

static float h_cost(int n1, int n2, int w) {
    float D = 1.0f;

    return D * distance(n1, n2, w);
}

static float move_cost(int n1, int n2, int w, char tile) {
    return (tile == TILE_WALL ? INFINITY : distance(n1, n2, w));
}

static int min_fscore_node(Map_t *map) {
    int min_index = 0;
    float fmin = INFINITY;

    for (int i = 0; i < map->w*map->h; i++) {
        if (map->open_set[i] == 1) {
            if (map->fscores[i] <= fmin) {
                fmin = map->fscores[i];
                min_index = i;
            }
        }
    }

    return min_index;
}

static void retrace_map(Map_t *map, int current) {
    int w = map->w;
    char c = TILE_GOAL;
    while (current != -1) {
        int x, y;
        current = map->came_from[current];
        y = idxtoy(current,w);
        x = idxtox(current,w);
        c = map->tiles[y][x];
        if (c == TILE_START) {
            break;
        }
        map->tiles[y][x] = '@';
    }
}

int find_neighbours(Map_t *map, int current, int *neighbours) {
    int up, down, left, right;//, upl, upr, downl, downr;
    int neigh_n = 0;

    left  = current - 1;
    right = current + 1;
    up    = current - map->w;
    down  = current + map->w;
    //upl   = up - 1;
    //upr   = up + 1;
    //downl = down - 1;
    //downr = down + 1;

    // if not in first column
    if (idxtox(current,map->w) != 0) {
        neighbours[neigh_n++] = left;
    }

    // if not in last column
    if (idxtox(current,map->w) != (map->w - 1)) {
        neighbours[neigh_n++] = right;
    }

    // if not in first row
    if (idxtoy(current,map->w) != 0) {
        neighbours[neigh_n++] = up;
    }

    // if not in last row
    if (idxtoy(current,map->w) != (map->h - 1)) {
        neighbours[neigh_n++] = down;
    }

    return neigh_n;
}

int parse_map(Map_t *map, FILE *fp) {
    int w, h;

    fscanf(fp, "%dx%d\n", &h, &w);
    map->w = w;
    map->h = h;

    map->tiles = (char **) malloc(h * sizeof(char *));
    for (int i = 0; i < h; i++) {
        map->tiles[i] = (char *) malloc(w * sizeof(char));
        fscanf(fp, "%s\n", map->tiles[i]);
    }
    map->fscores = (float *) malloc(w*h*sizeof(float));
    map->gscores = (float *) malloc(w*h*sizeof(float));
    map->open_set = (int *) malloc(w*h*sizeof(int));
    map->came_from = (int *) malloc(w*h*sizeof(int));

    // set default values for map
    for (int i = 0; i < w*h; i++) {
        map->gscores[i] = INFINITY;
        map->fscores[i] = INFINITY;
        map->open_set[i] = 0;
        map->came_from[i] = -1;
    }
    map->open_n = 0;

    rewind(fp);

    return 0;
}

void print_map(Map_t *map) {
    for (int i = 0; i < map->h; i++) {
        for (int j = 0; j < map->w; j++) {
            printf("%c", map->tiles[i][j]);
        }
        printf("\n");
    }
}

void free_map_buffers(Map_t *map) {
    for (int i = 0; i < map->h; i++) {
        free(map->tiles[i]);
    }
    free(map->tiles);
    free(map->fscores);
    free(map->gscores);
    free(map->open_set);
    free(map->came_from);
}

int pos_in_string(char *s, char c) {
    int pos = -1;
    for (int i = 0; i < strlen(s); i++) {
        if (s[i] == c) {
            pos = i;
        }
    }
    return pos;
}

int solve_map(Map_t *map) {
    // find start and goal index
    int s, g, w, h, p;
    w = map->w;
    h = map->h;

    for (int i = 0; i < h; i++) {
        if ((p = pos_in_string(map->tiles[i], TILE_START)) != -1) {
            s = i*w+p;
        }
        if ((p = pos_in_string(map->tiles[i], TILE_GOAL)) != -1) {
            g = i*w+p;
        }
    }

    map->open_set[s] = 1;
    map->open_n++;
    map->gscores[s] = 0.0f;
    map->fscores[s] = h_cost(s, g, w);

    while (map->open_n) {
        int current = min_fscore_node(map);
        if (current == g) {
            printf("done!\n");
            retrace_map(map, current);
            return 1;
        }

        map->open_set[current] = 0;
        map->open_n--;
        int neighbours[8] = {0};
        int neigh_n = find_neighbours(map, current, neighbours);
        for (int i = 0; i < neigh_n; i++) {
            int n = neighbours[i];
            int tile = map->tiles[idxtoy(n,w)][idxtox(n,w)];
            float tent_gscore = map->gscores[current] + move_cost(current, n, w, tile);
            if (tent_gscore < map->gscores[n]) {
                map->came_from[n] = current;
                map->gscores[n] = tent_gscore;
                map->fscores[n] = tent_gscore + h_cost(n, g, w);
                if (map->open_set[n] != 1) {
                    map->open_set[n] = 1;
                    map->open_n++;
                }
            }
        }
    }


    printf("can't solve!\n");
    return 0;
}
