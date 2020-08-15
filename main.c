#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astar.h"

//#define DEBUG

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: a-star <map_file>\n\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "can't open file\n");
    }

    Map_t map;
    if (parse_map(&map, fp) != 0) {
        fprintf(stderr, "can't parse map\n");
        return 0;
    }

    printf("%dx%d (HxW)\n", map.h, map.w);
    print_map(&map);
    solve_map(&map);
    free_map_buffers(&map);

    return 0;
}
