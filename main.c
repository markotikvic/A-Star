#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "a_star.h"

//#define DEBUG

void print_usage(void);

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: a-star <map_file>\n\n");
        return 1;
    }

    AStarMap map = {0};
    if (parse_map(&map, argv[1]) != 0) {
        fprintf(stderr, "can't parse map\n");
        return 1;
    }

    printf("size: %d x %d\n", map.w, map.h);
    print_map(&map);

    AStarPath path = {0};
    if (solve_map(&map, &path) == 0) {
#ifdef DEBUG
        print_path(path);
#endif
    } else {
        printf("\ncouldn't solve map\n");
    }
    printf("explored %d/%d nodes\n", map.explored, map.w*map.h);
    print_map(&map);

    return 0;
}
