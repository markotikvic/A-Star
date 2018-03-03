#include "a_star.h"

#define INF 10000

static int min_i(int a, int b)
{
    if (a < b) return a;
    else return b;
}

static vec2 v2(int x, int y)
{
    vec2 v = {x = x, y = y};
    return v;
}

static int chebyshev_distance(vec2 p1, vec2 p2)
{
    // diagonal distance with D = 1 and D2 = 1 is called Chebyshev distance
    int D = 1, D2 = 1;

    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);

    return D * (dx + dy) + (D2 - 2 * D) * min_i(dx, dy);
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

static AStarSet *find_neighbours(AStarMap *map, AStarNode *current)
{
    AStarSet *n = new_set(8);
    int x = current->pos.x;
    int y = current->pos.y;

    // cardinal directions
    if (x > 0)
        add_to_set(n, &(map->nodes[y][x-1]));
    if (x < map->w - 1)
        add_to_set(n, &(map->nodes[y][x+1]));
    if (y > 0)
        add_to_set(n, &(map->nodes[y-1][x]));
    if (y < map->h - 1)
        add_to_set(n, &(map->nodes[y+1][x]));
    // diagonals
    if (x > 0 && y > 0)
        add_to_set(n, &(map->nodes[y-1][x-1]));
    if (x < map->w - 1 && y < map->h - 1)
        add_to_set(n, &(map->nodes[y+1][x+1]));
    if (x > 0 && y < map->h - 1)
        add_to_set(n, &(map->nodes[y+1][x-1]));
    if (x < map->w - 1 && y > 0)
        add_to_set(n, &(map->nodes[y-1][x+1]));

    return n;
}

static void retrace_map(AStarMap *map, AStarNode *last)
{
    AStarNode *curr = last;
    int x, y;
    while (curr->symbol != 'S') {
        x = curr->came_from.x;
        y = curr->came_from.y;
        curr = &(map->nodes[y][x]);
        if (curr->symbol != 'G' && curr->symbol != 'S') {
            curr->symbol = '+';
        }
    }
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

void a_star_print_map(AStarMap *map)
{
    printf("w = %d, h = %d\n", map->w, map->h);
    printf("start (%d, %d), goal (%d, %d)\n",
            map->start->pos.x, map->start->pos.y, map->goal->pos.x, map->goal->pos.y);
    for (int y = 0; y < map->h; y++) {
        for (int x = 0; x < map->w; x++) {
            printf("%c", (map->nodes[y][x]).symbol);
        }
        printf("\n");
    }
}

bool a_star_solve_map(AStarMap *map)
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
            retrace_map(map, current);
            return true;
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
            neigh->came_from = current->pos;

            //add_to_set(came_from, current);
            neighbours->nodes[i]->g_score = tentative_g;
            neighbours->nodes[i]->f_score = neigh->g_score + heuristic(neigh, map->goal);
        }
        free(neighbours);
        neighbours = NULL;
    }
    free(open_set);
    free(closed_set);

    return false;
}
