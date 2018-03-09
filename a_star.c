#include "a_star.h"

#define INF 10000
//#define DEBUG

static int heuristic_cost(AStarNode *n1, AStarNode *n2)
{
    int D = 1;

    int dx = abs(n1->pos.x - n2->pos.x);
    int dy = abs(n1->pos.y - n2->pos.y);

    return D * (dx + dy);
}

static bool same_node(AStarNode *n1, AStarNode *n2)
{
    return (n1->pos.x == n2->pos.x && n1->pos.y == n2->pos.y);
}

static int movement_cost(AStarNode *n1, AStarNode *n2)
{
    if (n2-> tag == '#') {
        return INF;
    } else {
        int cost = abs(n1->pos.x - n2->pos.x) + abs(n1->pos.y - n2->pos.y);
        return cost;
    }
}

static AStarSet *new_set(unsigned int size)
{
    if (size < 1) {
        size = 1;
    }
    AStarSet *set = (AStarSet *) malloc(sizeof(AStarSet));
    set->nodes = (AStarNode **) malloc(sizeof(AStarNode) * size);
    set->len = 0;
    set->cap = size;

    return set;
}

static void add_to_set(AStarSet *set, AStarNode *node)
{
    // regrow set if needed by doubling it's capacity
    if (set->cap == set->len) {
        set->cap *= 2;
        set->nodes = realloc(set->nodes, sizeof(AStarNode *) * set->cap);
    }
    set->nodes[set->len] = node;
    set->len++;
}

static AStarNode *node_at(AStarSet *set, int index)
{
    if (index < set->len) {
        return set->nodes[index];
    }

    return NULL;
}

static bool is_in_set(AStarSet *set, AStarNode *node)
{
    for (int i = 0; i < set->len; i++) {
        AStarNode *temp = node_at(set, i);
        if (temp->pos.x == node->pos.x && temp->pos.y == node->pos.y) {
            return true;
        }
    }

    return false;
}

static bool remove_from_set(AStarSet *set, AStarNode *node)
{
    AStarNode *temp;
    for (int i = 0; i < set->len; i++) {
        temp = set->nodes[i];
        if (temp->pos.x == node->pos.x && temp->pos.y == node->pos.y) {
            for (int j = i; j < set->len - 1; j++) {
                set->nodes[j] = set->nodes[j+1];
            }
            set->len--;
            set->nodes[set->len] = NULL;
            return true;
        }
    }

    return false;
}

static AStarNode *min_f_score(AStarSet *set)
{
    AStarNode *temp, *min;
    min = node_at(set, 0);

    for (int i = 0; i < set->len; i++) {
        temp = set->nodes[i];
        if (temp->f_score <= min->f_score) {
            min = temp;
        }
    }

    return min;
}

static bool is_in_map(AStarMap *map, int x, int y) {
    return (x >= 0 && y >= 0 && x < map->w && y < map->h);
}

// find only walkable neighbours
static AStarSet *find_neighbours(AStarMap *map, AStarNode *current)
{
    AStarSet *n = new_set(1);
    int x = current->pos.x;
    int y = current->pos.y;
    
    AStarNode *temp = NULL;
    for (int i = -1; i <= 1; i++) {
        if (i != 0 && is_in_map(map, x+i, y)) {
            temp = &(map->nodes[y][x+i]);
            if (temp->tag != '#') {
                add_to_set(n, temp);
            }
        }
    }
    for (int i = -1; i <= 1; i++) {
        if (i != 0 && is_in_map(map, x, y+i)) {
            temp = &(map->nodes[y+i][x]);
            if (temp->tag != '#') {
                add_to_set(n, temp);
            }
        }
    }

    return n;
}

// steps are in reverse order
static AStarPath *retrace_map(AStarNode *last)
{
    AStarPath *path = (AStarPath *) malloc(sizeof(AStarPath));
    AStarNode *curr = last;

    // mark the steps and count them up
    int x, y, steps;
    for (steps = 0; curr->came_from != NULL; steps++) {
        curr = curr->came_from;
        if (curr->tag != 'G' && curr->tag != 'S') {
            curr->tag = '+';
        }
    }
    steps++;

    // add steps to path
    path->nodes_count = steps;
    path->weight = last->f_score;
    path->steps = (vec2 *) malloc(sizeof(vec2) * steps);
    curr = last;
    path->steps[steps-1] = v2(last->pos.x, last->pos.y);
    for (int i = 2; i <= steps; i++) {
        x = curr->came_from->pos.x;
        y = curr->came_from->pos.y;
        path->steps[steps-i] = v2(x, y);
        curr = curr->came_from;
    }

    return path;
}

vec2 v2(int x, int y)
{
    vec2 v = {x = x, y = y};

    return v;
}

AStarMap *a_star_parse_map(char *fname)
{
    AStarMap *map = (AStarMap *) malloc(sizeof(AStarMap));

    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        fprintf(stderr, "couldn't open map %s\n", fname);
        return NULL;
    }

    // determine map size
    int y = 0, x = 0, c = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') y++;
        else           x++;
    }
    rewind(fp);
    map->h = y;
    map->w = x/y;

    // allocate map nodes
    map->nodes = (AStarNode **) malloc(sizeof(AStarNode *) * map->h);
    for (int i = 0; i < map->h; i++) {
        map->nodes[i] = (AStarNode *) malloc(sizeof(AStarNode) * map->w);
    }

    // populate map
    x = y = c = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            y++;
            x = 0;
            continue;
        }
        map->nodes[y][x].pos = v2(x, y);
        map->nodes[y][x].tag = c;
        // all nodes have infinite f and g scores at the start
        map->nodes[y][x].f_score = INF;
        map->nodes[y][x].g_score = INF;
        map->nodes[y][x].came_from = NULL;
        x++;
    }

    fclose(fp);

    return map;
}

void a_star_print_map(AStarMap *map)
{
    if (map == NULL) return;

    for (int y = 0; y < map->h; y++) {
        for (int x = 0; x < map->w; x++) {
            printf("%c", (map->nodes[y][x]).tag);
        }
        printf("\n");
    }
}

AStarPath *a_star_solve_map(AStarMap *map, vec2 start, vec2 goal)
{
    if (!is_in_map(map, start.x, start.y)) {
        printf("starting point (%d, %d) is not in map range\n", start.x, start.y);
        return NULL;
    }

    if (!is_in_map(map, start.x, start.y)) {
        printf("goal point (%d, %d) is not in map range\n", goal.x, goal.y);
        return NULL;
    }

    printf("solving for: S(%d, %d) -> G(%d, %d)\n", start.x, start.y, goal.x, goal.y);

    AStarSet *open_set   = new_set(10);
    AStarSet *closed_set = new_set(10);

    // set start and goal
    map->start = &(map->nodes[start.y][start.x]);
    map->goal  = &(map->nodes[goal.y][goal.x]);
    map->start->tag = 'S';
    map->goal->tag = 'G';

    // only starting node is in open set at the start
    map->start->f_score = heuristic_cost(map->start, map->goal);
    map->start->g_score = 0;
    add_to_set(open_set, map->start);
    map->tested_count++;


    AStarNode *current;
    while (open_set->len > 0) {
        current = min_f_score(open_set);
        if (same_node(current, map->goal)) {
            return retrace_map(current);
        }

        remove_from_set(open_set, current);
        add_to_set(closed_set, current);

        AStarSet *neighbours = find_neighbours(map, current);
        for (int i = 0; i < neighbours->len; i++) {
#ifdef DEBUG
            a_star_print_map(map);
            getchar();
#endif
            AStarNode *neigh = node_at(neighbours, i);
            if (is_in_set(closed_set, neigh)) {
                continue;
            }

            if (!is_in_set(open_set, neigh)) {
                if (!same_node(neigh, map->goal)) {
                    neigh->tag = '~';
                }
                add_to_set(open_set, neigh);
                map->tested_count++;
            }

            int temp_g = current->g_score + movement_cost(current, neigh);
            if (temp_g >= neigh->g_score) {
                continue;
            }
            neigh->g_score = temp_g;
            neigh->f_score = temp_g + heuristic_cost(neigh, map->goal);
            neigh->came_from = current;
        }
        free(neighbours);
        neighbours = NULL;
    }
    free(open_set);
    free(closed_set);

    return NULL;
}

void a_star_print_path(AStarPath *path)
{
    printf("path: nodes = %d; weight = %d\n", path->nodes_count, path->weight);
    for (int i = 0; i < path->nodes_count; i++) {
        printf("[%d] (%d, %d)\n", i, path->steps[i].x, path->steps[i].y);
    }
}
