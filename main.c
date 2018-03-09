#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "a_star.h"

typedef struct command_line_arguments {
    vec2 start;
    vec2 goal;
    char *map_file;
} cl_args;

void print_usage(void)
{
    printf("Usage: a-star [OPTIONS] --map=FILE -start=x,y --goal=x,y\n");
    printf("    --map   - map file\n");
    printf("    --start - start position\n");
    printf("    --goal  - goal position\n\n");
    printf("OPTIONS:\n");
    printf("    --help - print this message\n");
}

vec2 parse_vec2(char *str)
{
    vec2 ret;
    char x[10] = { 0 };
    char y[10] = { 0 };

    int c = 0;
    while (str[c++] != ',');

    strncpy(x, str, c);
    ret.x = atoi(x);

    strcpy(y, str+c);
    ret.y = atoi(y);

    return ret;
}

int parse_command_line_args(int argc, char **argv, cl_args *args)
{
    if (argc < 4) {
        print_usage();
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], "--map=") != NULL) {
            char *map_file = argv[i] + strlen("--map=");
            args->map_file = (char *) malloc(strlen(map_file));
            strcpy(args->map_file, map_file);
        } else if (strstr(argv[i], "--start=") != NULL) {
            char *start = argv[i] + strlen("--start=");
            args->start = parse_vec2(start);
        } else if (strstr(argv[i], "--goal=") != NULL) {
            char *goal = argv[i] + strlen("--goal=");
            args->goal = parse_vec2(goal);
        } else {
            printf("\nuknown command: %s\n\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    return 0;
}

cl_args args;

int main(int argc, char **argv)
{
    int r = parse_command_line_args(argc, argv, &args);
    if (r != 0) {
        return r;
    }

    AStarMap * map = a_star_parse_map(args.map_file);
    if (map == NULL) return -1;

    printf("size: %d x %d\n", map->w, map->h);
    a_star_print_map(map);

    AStarPath *path = a_star_solve_map(map, args.start, args.goal);
    if (path != NULL) {
        printf("\nsolved\n");
        //a_star_print_path(path);
    } else {
        printf("\ncouldn't solve map\n");
    }
    printf("tested %d nodes\n", map->tested_count);
    a_star_print_map(map);

    return 0;

}
