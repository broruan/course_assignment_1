#include "snake.h"

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

static void push_head(GameState *g, int x, int y) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n)
        return;
    n->x = x;
    n->y = y;
    n->prev = NULL;
    n->next = g->head;
    if (g->head)
        g->head->prev = n;
    g->head = n;
    if (!g->tail)
        g->tail = n;
    g->length++;
    g->map[y][x] = CELL_SNAKE;
}

static void pop_tail(GameState *g) {
    Node *old;
    if (!g->tail)
        return;
    old = g->tail;
    g->map[old->y][old->x] = CELL_EMPTY;
    draw_cell(old->x, old->y, CELL_EMPTY);

    g->tail = old->prev;
    if (g->tail)
        g->tail->next = NULL;
    else
        g->head = NULL;
    free(old);
    g->length--;
}

void free_snake(GameState *g) {
    Node *cur = g->tail;
    while (cur) {
        Node *prev = cur->prev;
        free(cur);
        cur = prev;
    }
    g->head = g->tail = NULL;
    g->length = 0;
}

void init_snake(GameState *g) {
    int cx = MAP_W / 2;
    int cy = MAP_H / 2;

    g->head = NULL;
    g->tail = NULL;
    g->length = 0;
    g->direction = DIR_RIGHT;
    g->next_dir = DIR_RIGHT;

    push_head(g, cx - 2, cy);
    push_head(g, cx - 1, cy);
    push_head(g, cx, cy);
}

static void adjust_speed(GameState *g) {
    if (g->score >= 150)
        g->speed = SPEED_TURBO;
    else if (g->score >= 80)
        g->speed = SPEED_FAST;
    else if (g->score >= 30)
        g->speed = SPEED_NORMAL;
    else
        g->speed = SPEED_SLOW;
}

void handle_input(GameState *g) {
    int key, ext;

    if (!_kbhit())
        return;

    key = _getch();

    if (key == 0 || key == 224) {
        ext = _getch();
        switch (ext) {
            case 72:
                key = 'w';
                break;
            case 80:
                key = 's';
                break;
            case 75:
                key = 'a';
                break;
            case 77:
                key = 'd';
                break;
            default:
                return;
        }
    }

    switch (key) {
        case 'w':
        case 'W':
            if (g->direction != DIR_DOWN)
                g->next_dir = DIR_UP;
            break;
        case 's':
        case 'S':
            if (g->direction != DIR_UP)
                g->next_dir = DIR_DOWN;
            break;
        case 'a':
        case 'A':
            if (g->direction != DIR_RIGHT)
                g->next_dir = DIR_LEFT;
            break;
        case 'd':
        case 'D':
            if (g->direction != DIR_LEFT)
                g->next_dir = DIR_RIGHT;
            break;

        case 'p':
        case 'P':
        case 27:
            g->paused = !g->paused;
            gotoxy(INFO_X, OFFSET_Y + 14);
            if (g->paused) {
                set_color(CLR_GAMEOVER);
                printf("  鈻堚枅 宸叉殏鍋?鈻堚枅  ");
            } else {
                set_color(CLR_DEFAULT);
                printf("               ");
            }
            reset_color();
            break;

        case 'q':
        case 'Q':
            g->game_over = 1;
            break;
    }
}

void move_snake(GameState *g) {
    int nx, ny;
    int cell;

    if (g->paused || g->game_over)
        return;

    g->direction = g->next_dir;

    nx = g->head->x;
    ny = g->head->y;
    switch (g->direction) {
        case DIR_UP:
            ny--;
            break;
        case DIR_DOWN:
            ny++;
            break;
        case DIR_LEFT:
            nx--;
            break;
        case DIR_RIGHT:
            nx++;
            break;
    }

    cell = g->map[ny][nx];
    if (cell == CELL_WALL || cell == CELL_OBSTACLE) {
        g->game_over = 1;
        return;
    }

    if (cell == CELL_SNAKE) {
        Node *cur = g->tail;
        Node *collision_node = NULL;

        while (cur) {
            if (cur->x == nx && cur->y == ny) {
                collision_node = cur;
                break;
            }
            cur = cur->prev;
        }

        if (collision_node) {
            while (g->tail && (g->tail->y != collision_node->y || g->tail->x != collision_node->x)) {
                pop_tail(g);
            }
            if (g->tail) {
                pop_tail(g);
            }
        }

        if (g->length < 2) {
            g->game_over = 1;
            return;
        }
    }

    if (cell == CELL_FOOD_1 || cell == CELL_FOOD_2 || cell == CELL_FOOD_3) {
        int base_score = 10;
        int multiplier = (cell == CELL_FOOD_1) ? 1 : (cell == CELL_FOOD_2) ? 2 : 3;

        g->score += base_score * multiplier;
        if (g->score > g->high_score)
            g->high_score = g->score;
        if (g->score > g_high_score)
            g_high_score = g->score;

        push_head(g, nx, ny);
        draw_head_cell(nx, ny);

        if (g->head->next)
            draw_cell(g->head->next->x, g->head->next->y, CELL_SNAKE);

        if (g->head->next && g->head->next->prev)
            draw_cell(g->head->next->prev->x, g->head->next->prev->y, CELL_SNAKE);

        draw_score_panel(g);

        generate_obstacle(g);
        generate_food(g);
        draw_cell(g->food_x, g->food_y, g->map[g->food_y][g->food_x]);

        adjust_speed(g);
        draw_score_panel(g);
    } else {
        pop_tail(g);

        push_head(g, nx, ny);
        draw_head_cell(nx, ny);

        if (g->head->next)
            draw_cell(g->head->next->x, g->head->next->y, CELL_SNAKE);

        draw_score_panel(g);
    }
}

