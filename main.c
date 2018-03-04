#include <stdio.h>
#include <stdlib.h>

#include "a_star.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("a-star map_file.txt\n");
        return 1;
    }

    AStarMap * map = a_star_parse_map(argv[1]);
    if (map == NULL) return -1;
    a_star_print_map_info(map);
    a_star_print_map(map);
    if (a_star_solve_map(map)) {
        printf("\nsolved:\n");
        a_star_print_map(map);
    } else {
        printf("couldn't solve map\n");
    }

    return 0;

}
