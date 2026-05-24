#include "snake.h"

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void init_map(GameState *g) {
    int x, y;
    memset(g->map, 0, sizeof(g->map));

    for (x = 0; x < MAP_W; x++) {
        g->map[0][x] = CELL_WALL;
        g->map[MAP_H - 1][x] = CELL_WALL;
    }
    for (y = 0; y < MAP_H; y++) {
        g->map[y][0] = CELL_WALL;
        g->map[y][MAP_W - 1] = CELL_WALL;
    }
}

void draw_full_map(GameState *g); /* 确保链接（声明在头文件中） */

void generate_food(GameState *g) {
    int x, y;

    do {
        x = rand() % (MAP_W - 2) + 1;
        y = rand() % (MAP_H - 2) + 1;
    } while (g->map[y][x] != CELL_EMPTY);

    g->food_x = x;
    g->food_y = y;

    {
        int food_rand = rand() % 100;
        if (food_rand < 50) {
            g->food_type = 1;
            g->map[y][x] = CELL_FOOD_1;
        } else if (food_rand < 80) {
            g->food_type = 2;
            g->map[y][x] = CELL_FOOD_2;
        } else {
            g->food_type = 3;
            g->map[y][x] = CELL_FOOD_3;
        }
    }
}

static void clear_obstacle(GameState *g) {
    int i;

    if (g->obstacle_x == 0 && g->obstacle_y == 0)
        return;

    if (g->obstacle_horizontal) {
        for (i = 0; i < 5; i++) {
            g->map[g->obstacle_y][g->obstacle_x + i] = CELL_EMPTY;
            draw_cell(g->obstacle_x + i, g->obstacle_y, CELL_EMPTY);
        }
    } else {
        for (i = 0; i < 5; i++) {
            g->map[g->obstacle_y + i][g->obstacle_x] = CELL_EMPTY;
            draw_cell(g->obstacle_x, g->obstacle_y + i, CELL_EMPTY);
        }
    }
}

void generate_obstacle(GameState *g) {
    int x, y, i, valid;
    int horizontal;

    clear_obstacle(g);
    horizontal = rand() % 2;

    do {
        valid = 1;
        if (horizontal) {
            x = rand() % (MAP_W - 7) + 2;
            y = rand() % (MAP_H - 4) + 2;

            for (i = 0; i < 5; i++) {
                if (g->map[y][x + i] != CELL_EMPTY) {
                    valid = 0;
                    break;
                }
            }
        } else {
            x = rand() % (MAP_W - 4) + 2;
            y = rand() % (MAP_H - 7) + 2;

            for (i = 0; i < 5; i++) {
                if (g->map[y + i][x] != CELL_EMPTY) {
                    valid = 0;
                    break;
                }
            }
        }
    } while (!valid);

    g->obstacle_x = x;
    g->obstacle_y = y;
    g->obstacle_horizontal = horizontal;

    if (horizontal) {
        for (i = 0; i < 5; i++) {
            g->map[y][x + i] = CELL_OBSTACLE;
            draw_cell(x + i, y, CELL_OBSTACLE);
        }
    } else {
        for (i = 0; i < 5; i++) {
            g->map[y + i][x] = CELL_OBSTACLE;
            draw_cell(x, y + i, CELL_OBSTACLE);
        }
    }
}

