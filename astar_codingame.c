#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TILE_WALL    '#'
#define TILE_SPACE   '.'
#define TILE_UNKNOWN '?'
#define TILE_START   'P'
#define TILE_GOAL1   'C'
#define TILE_GOAL2   'T'

char TILE_GOAL = 0;

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
    float cost = 0.0f;

    switch (tile) {
        case TILE_WALL:
            cost = INFINITY;
            break;
        case TILE_SPACE:
        case TILE_START:
        case TILE_GOAL1:
        case TILE_GOAL2:
            cost = distance(n1, n2, w);
            break;
        case TILE_UNKNOWN:
            cost = 3*distance(n1, n2, w);
            break;
        default:
            cost = INFINITY;
            break;
    }

    return cost;
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

static void retrace_map(Map_t *map, int current, int path[100]) {
    int i = 0;
    int w = map->w;
    char c = TILE_GOAL;
    path[i++] = current;
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
        path[i++] = current;
    }

    // reverse map so that the next move is first
    for (int j = 0; j < i; j++) {
        int temp = path[j];
        path[j] = path[i - j - 1];
        path[i] = temp;
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

#define MAX_ROWS 100
#define MAX_COLS 200

int parse_map(Map_t *map, int h, int w, char temp_map[MAX_ROWS][MAX_COLS]) {
    map->w = w;
    map->h = h;

    map->tiles = (char **) malloc(h * sizeof(char *));
    for (int i = 0; i < h; i++) {
        map->tiles[i] = (char *) malloc(w * sizeof(char));
        memcpy(map->tiles[i], temp_map[i], w);
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

    return 0;
}

void print_map(Map_t *map) {
    for (int i = 0; i < map->h; i++) {
        for (int j = 0; j < map->w; j++) {
            fprintf(stderr, "%c", map->tiles[i][j]);
        }
        fprintf(stderr, "\n");
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

int solve_map(Map_t *map, int path[100]) {
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
            //fprintf(stderr, "done!\n");
            retrace_map(map, current, path);
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


    fprintf(stderr, "can't solve!\n");
    return 0;
}

int main()
{
    // number of rows.
    int h, w, a;
    int real_goal_on_map = 0;
    int rgx = 0, rgy = 0;
    int fake_goal_reached = 1;
    int fgx = 0, fgy = 0;

    char temp_map[MAX_ROWS][MAX_COLS] = {{0}};
    Map_t map;
    int path[100] = {0};
    

    srand(h*w*a);

    scanf("%d%d%d\n", &h, &w, &a);
    //fprintf(stderr, "%d %d %d\n", h, w, a);

    TILE_GOAL = TILE_GOAL1;

    // game loop
    while (1) {
        int start_y, start_x;
        scanf("%d%d\n", &start_y, &start_x);
        for (int i = 0; i < h; i++) {
            scanf("%s", temp_map[i]);
            for (int j = 0; j < w; j++) {
                if (temp_map[i][j] == TILE_GOAL) {
                    real_goal_on_map = 1;
                    rgx = j;
                    rgy = i;
                }
            }
        }

        if (start_x == rgx && start_y == rgy) {
            TILE_GOAL = TILE_GOAL2;
        }
        if (start_x == fgx && start_y == fgy) {
            fake_goal_reached = 1;
        }

        if (temp_map[fgy][fgx] == TILE_WALL) {
            fake_goal_reached = 1;
        };

        if (!real_goal_on_map) {
            if (fake_goal_reached) {
                fake_goal_reached = 0;
                fgx = rand() % w; // fake goal x
                fgy = rand() % h; // fake goal y
                char tmp = temp_map[fgy][fgx];
                while (tmp == TILE_WALL || tmp == 'T' || tmp == TILE_SPACE) {
                    fgx = rand() % w;
                    fgy = rand() % h;
                    tmp = temp_map[fgy][fgx];
                }
            }
            temp_map[fgy][fgx] = TILE_GOAL;
        }

        temp_map[start_y][start_x] = TILE_START;

        parse_map(&map, h, w, temp_map);
        print_map(&map);

        if (solve_map(&map, path)) {
            int next_x = idxtox(path[0], w);
            int next_y = idxtoy(path[0], w);

            //fprintf(stderr, "next: %d %d\n", next_x, next_y);

            if (next_x > start_x) {
                printf("RIGHT\n");
            } else if (next_x < start_x) {
                printf("LEFT\n");
            } else if (next_y > start_y) {
                printf("DOWN\n");
            } else if (next_y < start_y) {
                printf("UP\n");
            } else {
                fprintf(stderr, "impossible next move\n");
            }
        }
        free_map_buffers(&map);
        memset(path, 0, 100*sizeof(int));
    }

    return 0;
}
