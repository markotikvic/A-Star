#include "a_star.h"

#define INF 10000

static int heuristic_cost(AStarNode *n1, AStarNode *n2) {
    int D = 1;

    int dx = abs(n1->pos.x - n2->pos.x);
    int dy = abs(n1->pos.y - n2->pos.y);

    return D * (dx + dy);
}

static int movement_cost(AStarNode *n1, AStarNode *n2) {
    if (n2-> tag == '#') {
        return INF;
    }

    return abs(n1->pos.x - n2->pos.x) + abs(n1->pos.y - n2->pos.y);
}

static Array *new_array(int cap) {
    Array *arr = (Array *) malloc(sizeof(Array));
    arr->cap = cap < 1 ? 1 : cap;
    arr->len = 0;
    arr->items = (void *) malloc(sizeof(void *) * arr->cap);

    return arr;
}

static void *array_at(Array *arr, int index) {
    return arr->items[index];
}

static void array_set(Array *arr, int index, void *v) {
    arr->items[index] = v;
}

static void array_push(Array *arr, void *node) {
    // regrow set if needed by doubling it's capacity
    if (arr->cap == arr->len) {
        arr->cap *= 2;
        arr->items = realloc(arr->items, sizeof(void *)*arr->cap);
    }
    array_set(arr, arr->len, node);
    arr->len++;
}

static int in_array(Array *arr, void *node) {
    for (int i = 0; i < arr->len; i++) {
        if (array_at(arr, i) == node) {
            return 1;
        }
    }

    return 0;
}

static void array_remove(Array *arr, void *node) {
    for (int i = 0; i < arr->len; i++) {
        if (array_at(arr, i) == node) {
            for (int j = i; j < arr->len - 1; j++) {
                array_set(arr, j, array_at(arr, j+1));
            }
            arr->len--;
            array_set(arr, arr->len, 0);
        }
    }
}

static AStarNode *min_f_score(Array *arr) {
    AStarNode *min = (AStarNode *) array_at(arr, 0);

    for (int i = 0; i < arr->len; i++) {
        AStarNode *n = (AStarNode *) array_at(arr, i);
        if (n->f_score <= min->f_score) {
            min = n;
        }
    }

    return min;
}

static int in_map(AStarMap *map, int x, int y) {
    return (x >= 0 && y >= 0 && x < map->w && y < map->h);
}

// find only walkable neighbours
static Array *find_neighbours(AStarMap *map, AStarNode *node) {
    Array *neighbours = new_array(0);

    int x = node->pos.x;
    int y = node->pos.y;

    if (in_map(map, x-1, y) && map->nodes[y][x-1].tag != '#') {
        array_push(neighbours, (void *) &(map->nodes[y][x-1]));
    }

    if (in_map(map, x+1, y) && map->nodes[y][x+1].tag != '#') {
        array_push(neighbours, (void *) &(map->nodes[y][x+1]));
    }

    if (in_map(map, x, y-1) && map->nodes[y-1][x].tag != '#') {
        array_push(neighbours, (void *) &(map->nodes[y-1][x]));
    }

    if (in_map(map, x, y+1) && map->nodes[y+1][x].tag != '#') {
        array_push(neighbours, (void *) &(map->nodes[y+1][x]));
    }

    return neighbours;
}

// steps are in reverse order
static void retrace_map(AStarNode *last, AStarPath *path) {
    AStarNode *curr = last;

    // mark the steps and count them up
    int x, y, steps;
    for (steps = 0; curr->came_from != NULL; steps++) {
        curr = curr->came_from;
        if (curr->tag != 'G' && curr->tag != 'S') {
            curr->tag = '*';
        }
    }
    steps++;

    // add steps to path
    path->nodes_count = steps;
    path->weight = last->f_score;
    path->steps = (Vec2 *) malloc(sizeof(Vec2) * steps);
    curr = last;
    path->steps[steps-1] = (Vec2) {last->pos.x, last->pos.y};
    for (int i = 2; i <= steps; i++) {
        x = curr->came_from->pos.x;
        y = curr->came_from->pos.y;
        path->steps[steps-i] = (Vec2) {x, y};
        curr = curr->came_from;
    }
}

int parse_map(AStarMap *map, char *fname) {
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        fprintf(stderr, "couldn't open map %s\n", fname);
        return 1;
    }

    // determine map size
    int y = 0, x = 0, c = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            y++;
        } else {
            x++;
        }
    }
    map->h = y;
    map->w = x/y;

    // allocate map nodes
    map->nodes = (AStarNode **) malloc(sizeof(AStarNode *) * map->h);
    for (int i = 0; i < map->h; i++) {
        map->nodes[i] = (AStarNode *) malloc(sizeof(AStarNode) * map->w);
    }

    // populate map
    int found_s = false, found_g = false;
    x = y = c = 0;
    rewind(fp);
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            y++;
            x = 0;
            continue;
        }

        map->nodes[y][x].pos = (Vec2) {x, y};
        map->nodes[y][x].tag = c;
        // all nodes have infinite f and g scores at the start
        map->nodes[y][x].f_score = INF;
        map->nodes[y][x].g_score = INF;
        map->nodes[y][x].came_from = NULL;

        // set start and goal nodes
        if (c == 'S') {
            map->start = &(map->nodes[y][x]);
            map->start->tag = 'S';
            map->start->g_score = 0;
            found_s = true;
        }

        if (c == 'G') {
            map->goal = &(map->nodes[y][x]);
            map->goal->tag = 'G';
            found_g = true;
        }

        x++;
    }

    if (!found_s) {
        fprintf(stderr, "start not defined\n");
        return 1;
    }

    if (!found_g) {
        fprintf(stderr, "goal not defined\n");
        return 1;
    }

    map->start->f_score = heuristic_cost(map->start, map->goal);

    fclose(fp);

    return 0;
}

void print_map(AStarMap *map) {
    for (int y = 0; y < map->h; y++) {
        for (int x = 0; x < map->w; x++) {
            printf("%c", (map->nodes[y][x]).tag);
        }
        printf("\n");
    }
}

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
