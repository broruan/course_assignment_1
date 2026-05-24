#include "snake.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void start_game(void) {
    GameState g;
    DWORD last_tick, now;

    memset(&g, 0, sizeof(g));
    g.high_score = g_high_score;
    g.speed = SPEED_SLOW;
    g.game_over = 0;
    g.paused = 0;

    srand((unsigned int)time(NULL));

    clear_screen();
    set_cursor(0);

    gotoxy(OFFSET_X, OFFSET_Y - 2);
    set_color(CLR_TITLE);
    printf("璐?鍚?铔? Snake Game");
    reset_color();

    init_map(&g);
    init_snake(&g);

    g.obstacle_x = 0;
    g.obstacle_y = 0;
    generate_obstacle(&g);

    generate_food(&g);

    draw_full_map(&g);
    draw_info_title();
    draw_score_panel(&g);

    last_tick = GetTickCount();

    while (!g.game_over) {
        handle_input(&g);

        now = GetTickCount();
        if (!g.paused && (now - last_tick) >= (DWORD)g.speed) {
            move_snake(&g);
            last_tick = now;
        }

        Sleep(10);
    }

    if (g.score > 0 || g.length > 3) {
        show_game_over(&g);
    } else {
        show_game_over(&g);
    }

    free_snake(&g);
    set_cursor(1);
}

