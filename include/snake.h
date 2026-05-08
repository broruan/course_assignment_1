#pragma once

#include <stddef.h>

/* 游戏画面尺寸 */
#define MAP_W 30
#define MAP_H 20

/* 方向 */
#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

/* 地图单元类型 */
#define CELL_EMPTY 0
#define CELL_WALL 1
#define CELL_SNAKE 2
#define CELL_FOOD_1 3
#define CELL_FOOD_2 4
#define CELL_FOOD_3 5
#define CELL_OBSTACLE 6

/* 难度/速度 */
#define SPEED_SLOW 300
#define SPEED_NORMAL 200
#define SPEED_FAST 130
#define SPEED_TURBO 80

/* 控制台颜色属性 */
#define CLR_DEFAULT 7
#define CLR_WALL 8
#define CLR_SNAKE_H 10
#define CLR_SNAKE_B 2
#define CLR_FOOD_1 9
#define CLR_FOOD_2 13
#define CLR_FOOD_3 6
#define CLR_SCORE 14
#define CLR_TITLE 11
#define CLR_MENU 15
#define CLR_SELECT 10
#define CLR_BORDER 3
#define CLR_GAMEOVER 12
#define CLR_HINT 8
#define CLR_INFO 11

typedef struct Node {
    int x, y;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int length;
    int direction;
    int next_dir;
    int score;
    int high_score;
    int speed;
    int food_x, food_y;
    int food_type;
    int map[MAP_H][MAP_W];
    int game_over;
    int paused;
    int obstacle_x, obstacle_y;
    int obstacle_horizontal;
} GameState;

/* 全局最高分（跨多局保持） */
extern int g_high_score;

/* UI / 绘制 / 菜单 */
void gotoxy(int x, int y);
void set_cursor(int visible);
void set_color(int attr);
void reset_color(void);
void print_center(int y, const char *str, int color);
void clear_screen(void);
void set_console_size(int w, int h);

void draw_cell(int mx, int my, int type);
void draw_head_cell(int mx, int my);
void draw_full_map(GameState *g);

void draw_score_panel(GameState *g);
void draw_info_title(void);
void show_game_over(GameState *g);

void draw_menu_box(int x, int y, int w, int h);
void show_main_menu(void);

/* 地图/道具 */
void init_map(GameState *g);
void generate_food(GameState *g);
void generate_obstacle(GameState *g);

/* 蛇逻辑/输入 */
void init_snake(GameState *g);
void free_snake(GameState *g);
void handle_input(GameState *g);
void move_snake(GameState *g);

/* 游戏主流程 */
void start_game(void);

/* 控制台布局（原单文件中的 OFFSET_X/Y/INFO_X） */
#define OFFSET_X 2
#define OFFSET_Y 2
#define INFO_X (OFFSET_X + MAP_W * 2 + 2)

