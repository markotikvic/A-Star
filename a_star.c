#include "a_star.h"

#define INF 10000

static vec2 v2(int x, int y)
{
    vec2 v = {x = x, y = y};
    return v;
}

static int min(int a, int b)
{
    if (a < b) return a;
    else return b;
}

// diagonal distance with D = 1 and D2 = 1
static int chebyshev_distance(vec2 p1, vec2 p2)
{
    int D = 1, D2 = 1;

    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);

    return D * (dx + dy) + (D2 - 2 * D) * min(dx, dy);
}

static int heuristic(AStarNode *n1, AStarNode *n2)
{
    return chebyshev_distance(n1->pos, n2->pos);
}

static int movement_cost(AStarNode *n1, AStarNode *n2)
{
    if (n1->symbol == '#' || n2->symbol == '#') {
        return INF;
    } else {
        return 1;
    }

    return 0;
}

static AStarSet *new_set(int size)
{
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

static void remove_from_set(AStarSet *set, AStarNode *node)
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
            break;
        }
    }
}

static AStarNode *min_f_score(AStarSet *set)
{
    AStarNode *temp, *min;
    min = node_at(set, 0);

    for (int i = 0; i < set->len; i++) {
        temp = set->nodes[i];
        if (temp->f_score < min->f_score) {
            min = temp;
        }
    }
    return min;
}

// find only walkable neighbours
static AStarSet *find_neighbours(AStarMap *map, AStarNode *current)
{
    AStarSet *n = new_set(8);
    int x = current->pos.x;
    int y = current->pos.y;

    // cardinal directions
    AStarNode *temp = NULL;
    if (x > 0) {
        temp = &(map->nodes[y][x-1]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }
    if (x < map->w - 1) {
        temp = &(map->nodes[y][x+1]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }
    if (y > 0) {
        temp = &(map->nodes[y-1][x]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }
    if (y < map->h - 1) {
        temp = &(map->nodes[y+1][x]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }
    // diagonals
    if (x > 0 && y > 0) {
        temp = &(map->nodes[y-1][x-1]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }
    if (x < map->w - 1 && y < map->h - 1) {
        temp = &(map->nodes[y+1][x+1]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }
    if (x > 0 && y < map->h - 1) {
        temp = &(map->nodes[y+1][x-1]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }
    if (x < map->w - 1 && y > 0) {
        temp = &(map->nodes[y-1][x+1]);
        if (temp->symbol != '#') add_to_set(n, temp);
    }

    return n;
}

// steps are in reverse order
static AStarPath *retrace_map(AStarMap *map, AStarNode *last)
{
    AStarPath *path = (AStarPath *) malloc(sizeof(AStarPath));
    AStarNode *curr = last;
    int x, y, steps, weight = 0;
    for (steps = 0; curr->symbol != 'S'; steps++) {
        weight =+ curr->f_score;
        x = curr->came_from.x;
        y = curr->came_from.y;
        curr = &(map->nodes[y][x]);
        if (curr->symbol != 'G' && curr->symbol != 'S') {
            curr->symbol = '+';
        }
    }

    steps++; // +1 to account for end node
    path->steps_count = steps;
    path->weight = weight;
    path->steps = (vec2 *) malloc(sizeof(vec2) * steps);
    curr = last;
    path->steps[0] = v2(last->pos.x, last->pos.y);
    for (int i = 1; curr->symbol != 'S'; i++) {
        x = curr->came_from.x;
        y = curr->came_from.y;
        path->steps[i] = v2(x, y);
        curr = &(map->nodes[y][x]);
    }

    return path;
}

static bool same_node(AStarNode *n1, AStarNode *n2)
{
    if (n1->pos.x == n2->pos.x && n1->pos.y == n2->pos.y) {
        return true;
    }
    return false;
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
        x++;
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
        map->nodes[y][x].symbol = c;
        // all nodes have infinite f and g scores at the start
        map->nodes[y][x].f_score = INF;
        map->nodes[y][x].g_score = INF;
        if (c == 'G') {
            map->goal = &(map->nodes[y][x]);
        }
        if (c == 'S') {
            map->start = &(map->nodes[y][x]);
            map->start->g_score = 0;
        }
        x++;
    }

    fclose(fp);

    return map;
}

void a_star_print_map_info(AStarMap *map)
{
    vec2 s = map->start->pos;
    vec2 g = map->goal->pos;
    printf("size:  %d x %d\n", map->w, map->h);
    printf("start: (%d, %d)\n", s.x, s.y);
    printf("goal:  (%d, %d)\n", g.x, g.y);
}

void a_star_print_map(AStarMap *map)
{
    for (int y = 0; y < map->h; y++) {
        for (int x = 0; x < map->w; x++) {
            printf("%c", (map->nodes[y][x]).symbol);
        }
        printf("\n");
    }
}

AStarPath *a_star_solve_map(AStarMap *map)
{
    AStarSet *open_set   = new_set(10);
    AStarSet *closed_set = new_set(10);

    // only starting node is in open set at the start
    map->start->f_score = heuristic(map->start, map->goal);
    add_to_set(open_set, map->start);


    AStarNode *current;
    while (open_set->len > 0) {
        current = min_f_score(open_set);
        if (same_node(current, map->goal)) {
            return retrace_map(map, current);
        }

        remove_from_set(open_set, current);
        add_to_set(closed_set, current);

        AStarSet *neighbours = find_neighbours(map, current);
        for (int i = 0; i < neighbours->len; i++) {
            AStarNode *neigh = node_at(neighbours, i);

            if (is_in_set(closed_set, neigh)) continue;

            if (!is_in_set(open_set, neigh)) {
                add_to_set(open_set, neigh);
            }

            int tentative_g = current->g_score + movement_cost(current, neigh);
            // not worth it
            if (tentative_g >= neigh->g_score) continue;

            neigh->g_score = tentative_g;
            neigh->f_score = neigh->g_score + heuristic(neigh, map->goal);
            neigh->came_from = current->pos;
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
    printf("path: steps = %d, weight = %d\n", path->steps_count, path->weight);
    for (int i = path->steps_count, j = 0; i > 0; i--, j++) {
        printf("[%d] (%d, %d)\n", j+1, path->steps[i-1].x, path->steps[i-1].y);
    }
}
