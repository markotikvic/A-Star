#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "a_star.h"

//#define DEBUG

void print_usage(void);

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    AStarMap *map = a_star_parse_map(argv[1]);
    if (map == NULL) {
        fprintf(stderr, "can't parse map\n");
        return 1;
    }

    printf("size: %d x %d\n", map->w, map->h);
    a_star_print_map(map);

    AStarPath *path = a_star_solve_map(map);
    if (path != NULL) {
        printf("\nsolved\n");
        //a_star_print_path(path);
    } else {
        printf("\ncouldn't solve map\n");
    }
    printf("explored %d/%d nodes\n", map->explored, map->w*map->h);
    a_star_print_map(map);

    return 0;
}

void print_usage(void) {
    printf("Usage: a-star [OPTIONS] [FILE]\n\n");
    printf("OPTIONS:\n");
    printf("    --help - print this message and exit\n");
}
