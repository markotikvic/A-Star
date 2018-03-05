#include <stdio.h>
#include <stdlib.h>

#include "a_star.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("a-star <map.txt>\n");
        return 1;
    }

    AStarMap * map = a_star_parse_map(argv[1]);
    if (map == NULL) return -1;
    a_star_print_map_info(map);
    a_star_print_map(map);

    AStarPath *path = a_star_solve_map(map);
    if (path != NULL) {
        printf("\nsolved:\n");
        a_star_print_path(path);
    } else {
        printf("\ncouldn't solve map\n");
    }
    a_star_print_map(map);

    return 0;

}
