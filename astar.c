#include <string.h>

#include "astar.h"

#define INF 10000

static int distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

static int heuristic_cost(int x1, int y1, int x2, int y2) {
    int D = 1;

    return D * distance(x1, y1, x2, y2);
}

static int movement_cost(int x1, int y1, int x2, int y2, char tile) {
    return (tile == TILE_WALL ? INF : distance(x1, y1, x2, y2));
}

static int min_fscore(Map_t *map) {
    int min_index = 0;
    int fmin = map->fscores[0];
    for (int i = 0; i < map->w*map->h; i++) {
        if (map->fscores[i] <= fmin) {
            fmin = map->fscores[i];
            min_index = i;
        }
    }

    return min_index;
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
    int start, goal, w, h, p;
    w = map->w;
    h = map->h;

    for (int i = 0; i < h; i++) {
        if ((p = pos_in_string(map->tiles[i], TILE_START)) != -1) {
            start = i*w+p;
        }
        if ((p = pos_in_string(map->tiles[i], TILE_GOAL)) != -1) {
            goal = i*w+p;
        }
    }
    printf("Start (%d): Y%d X%d\n", start, idxtoy(start,w), idxtox(start,w));
    printf("Goal (%d): Y%d X%d\n", goal, idxtoy(goal,w), idxtox(goal,w));
    //map->open_set[map->open_n++] = 
    return 0;
}

/*
int solve_map(AStarMap *map, AStarPath *path) {
    int sx = map->start->pos.x,
        sy = map->start->pos.y,
        ex = map->goal->pos.x,
        ey = map->goal->pos.y;
    printf("solving for: S(%d, %d) -> G(%d, %d)\n", sx, sy, ex, ey);

    Array *open_array   = new_array(0);
    Array *closed_array = new_array(0);

    // only starting node is in open set at the begging
    array_push(open_array, (void *) map->start);
    map->explored++;

    while (open_array->len > 0) {
        AStarNode *current = min_f_score(open_array);
        if (current == map->goal) {
            retrace_map(current, path);
            free(open_array);
            free(closed_array);
            return 0;
        }

        array_remove(open_array, (void *) current);
        array_push(closed_array, current);

        Array *neighbours = find_neighbours(map, current);
        for (int i = 0; i < neighbours->len; i++) {
#ifdef DEBUG
            print_map(map);
            getchar();
#endif
            AStarNode *n = (AStarNode *) array_at(neighbours, i);
            if (in_array(closed_array, (void *) n)) {
                continue;
            }

            if (!in_array(open_array, (void *) n)) {
                if (n != map->goal) {
                    n->tag = '~';
                }
                array_push(open_array, n);
                map->explored++;
            }

            int temp_g = current->g_score + movement_cost(current, n);
            if (temp_g >= n->g_score) {
                continue;
            }
            n->g_score = temp_g;
            n->f_score = temp_g + heuristic_cost(n, map->goal);
            n->came_from = current;
        }
        free(neighbours);
        neighbours = NULL;
    }
    free(open_array);
    free(closed_array);

    return 1;
}

void print_path(AStarPath *path) {
    printf("path: nodes = %d; weight = %d\n", path->nodes_count, path->weight);
    for (int i = 0; i < path->nodes_count; i++) {
        printf("[%d] (%d, %d)\n", i, path->steps[i].x, path->steps[i].y);
    }
}
*/
