/*
 * ============================================================
 *  贪吃蛇游戏 (Snake Game) - C语言实现
 *  功能: 主菜单、开始游戏、帮助信息、关于信息
 *  编译: gcc snake.c -o snake.exe  (Windows/MinGW)
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>

/* ─── 地图尺寸 ─── */
#define MAP_W  30          /* 逻辑宽度（格数） */
#define MAP_H  20          /* 逻辑高度（格数） */

/* ─── 方向常量 ─── */
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* ─── 地图格子类型 ─── */
#define CELL_EMPTY 0
#define CELL_WALL  1
#define CELL_SNAKE 2
#define CELL_FOOD  3

/* ─── 速度档位（毫秒/帧） ─── */
#define SPEED_SLOW   300
#define SPEED_NORMAL 200
#define SPEED_FAST   130
#define SPEED_TURBO   80

/* ─── 颜色常量（Windows 控制台属性） ─── */
#define CLR_DEFAULT  7
#define CLR_WALL     8    /* 深灰 */
#define CLR_SNAKE_H  10   /* 亮绿 - 蛇头 */
#define CLR_SNAKE_B  2    /* 深绿 - 蛇身 */
#define CLR_FOOD     12   /* 亮红 */
#define CLR_SCORE    14   /* 亮黄 */
#define CLR_TITLE    11   /* 亮青 */
#define CLR_MENU     15   /* 白色 */
#define CLR_SELECT   10   /* 亮绿 - 选中项 */
#define CLR_BORDER   3    /* 深青 - 菜单边框 */
#define CLR_GAMEOVER 12   /* 亮红 */
#define CLR_HINT     8    /* 深灰 - 提示文字 */
#define CLR_INFO     11   /* 亮青 - 信息文字 */

/* ═══════════════════════════════
   数据结构：蛇节点（双向链表）
   ═══════════════════════════════ */
typedef struct Node {
    int x, y;
    struct Node *prev;
    struct Node *next;
} Node;

/* ═══════════════════════════════
   游戏状态结构体
   ═══════════════════════════════ */
typedef struct {
    Node *head;              /* 蛇头指针 */
    Node *tail;              /* 蛇尾指针 */
    int   length;            /* 蛇的长度 */
    int   direction;         /* 当前移动方向 */
    int   next_dir;          /* 下一帧方向（防止同帧反向） */
    int   score;             /* 当前得分 */
    int   high_score;        /* 最高得分（本次运行） */
    int   speed;             /* 当前速度（ms） */
    int   food_x, food_y;    /* 食物坐标 */
    int   map[MAP_H][MAP_W]; /* 地图二维数组 */
    int   game_over;         /* 游戏结束标志 */
    int   paused;            /* 暂停标志 */
} GameState;

/* ─── 全局最高分（跨局） ─── */
static int g_high_score = 0;

/* ════════════════════════════════════════
   工具函数：控制台操作
   ════════════════════════════════════════ */

/* 移动光标到 (x, y) */
static void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

/* 隐藏/显示光标 */
static void set_cursor(int visible) {
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize   = 1;
    ci.bVisible = (BOOL)visible;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

/* 设置控制台字体颜色 */
static void set_color(int attr) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)attr);
}

/* 重置颜色 */
static void reset_color(void) {
    set_color(CLR_DEFAULT);
}

/* 居中打印字符串（控制台宽度 80） */
static void print_center(int y, const char *str, int color) {
    int x = (80 - (int)strlen(str)) / 2;
    if (x < 0) x = 0;
    gotoxy(x, y);
    set_color(color);
    printf("%s", str);
    reset_color();
}

/* 清屏 */
static void clear_screen(void) {
    system("cls");
}

/* 调整控制台窗口大小 */
static void set_console_size(int w, int h) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    /* 先设置缓冲区 */
    COORD bufSize = { (SHORT)w, (SHORT)h };
    SetConsoleScreenBufferSize(hOut, bufSize);
    /* 再设置窗口大小 */
    SMALL_RECT rect = { 0, 0, (SHORT)(w - 1), (SHORT)(h - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &rect);
}

/* ════════════════════════════════════════
   绘制单个格子（地图坐标 → 屏幕坐标）
   每格宽 2 字符，起始 X 偏移 2，Y 偏移 2
   ════════════════════════════════════════ */
#define OFFSET_X 2   /* 地图在屏幕的起始列 */
#define OFFSET_Y 2   /* 地图在屏幕的起始行 */

static void draw_cell(int mx, int my, int type) {
    int sx = OFFSET_X + mx * 2;
    int sy = OFFSET_Y + my;
    gotoxy(sx, sy);
    switch (type) {
        case CELL_WALL:
            set_color(CLR_WALL);
            printf("██");
            break;
        case CELL_SNAKE:
            /* 蛇头用亮绿，蛇身由调用方传 CELL_SNAKE，
               头部特殊处理见 draw_full_map */
            set_color(CLR_SNAKE_B);
            printf("▓▓");
            break;
        case CELL_FOOD:
            set_color(CLR_FOOD);
            printf("◆◆");
            break;
        case CELL_EMPTY:
        default:
            set_color(CLR_DEFAULT);
            printf("  ");
            break;
    }
    reset_color();
}

/* 专门绘制蛇头（颜色区别于蛇身） */
static void draw_head_cell(int mx, int my) {
    int sx = OFFSET_X + mx * 2;
    int sy = OFFSET_Y + my;
    gotoxy(sx, sy);
    set_color(CLR_SNAKE_H);
    printf("██");
    reset_color();
}

/* ════════════════════════════════════════
   初始化 / 绘制地图边界
   ════════════════════════════════════════ */
static void init_map(GameState *g) {
    int x, y;
    memset(g->map, 0, sizeof(g->map));

    /* 设置边界墙 */
    for (x = 0; x < MAP_W; x++) {
        g->map[0][x]         = CELL_WALL;
        g->map[MAP_H - 1][x] = CELL_WALL;
    }
    for (y = 0; y < MAP_H; y++) {
        g->map[y][0]         = CELL_WALL;
        g->map[y][MAP_W - 1] = CELL_WALL;
    }
}

/* 绘制完整地图（初始化时调用一次） */
static void draw_full_map(GameState *g) {
    int x, y;
    Node *cur;

    for (y = 0; y < MAP_H; y++)
        for (x = 0; x < MAP_W; x++)
            draw_cell(x, y, g->map[y][x]);

    /* 绘制蛇身 */
    for (cur = g->tail; cur != g->head; cur = cur->prev)
        draw_cell(cur->x, cur->y, CELL_SNAKE);

    /* 蛇头高亮 */
    draw_head_cell(g->head->x, g->head->y);

    /* 绘制食物 */
    draw_cell(g->food_x, g->food_y, CELL_FOOD);
}

/* ════════════════════════════════════════
   生成食物（随机放置在空格上）
   ════════════════════════════════════════ */
static void generate_food(GameState *g) {
    int x, y;
    do {
        x = rand() % (MAP_W - 2) + 1;
        y = rand() % (MAP_H - 2) + 1;
    } while (g->map[y][x] != CELL_EMPTY);

    g->food_x = x;
    g->food_y = y;
    g->map[y][x] = CELL_FOOD;
}

/* ════════════════════════════════════════
   蛇链表操作
   ════════════════════════════════════════ */

/* 在头部新增节点 */
static void push_head(GameState *g, int x, int y) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) return;
    n->x    = x;
    n->y    = y;
    n->prev = NULL;
    n->next = g->head;
    if (g->head) g->head->prev = n;
    g->head = n;
    if (!g->tail) g->tail = n;
    g->length++;
    g->map[y][x] = CELL_SNAKE;
}

/* 移除尾部节点 */
static void pop_tail(GameState *g) {
    Node *old;
    if (!g->tail) return;
    old = g->tail;
    g->map[old->y][old->x] = CELL_EMPTY;
    draw_cell(old->x, old->y, CELL_EMPTY);

    g->tail = old->prev;
    if (g->tail) g->tail->next = NULL;
    else         g->head       = NULL;
    free(old);
    g->length--;
}

/* 释放蛇全部节点 */
static void free_snake(GameState *g) {
    Node *cur = g->tail;
    while (cur) {
        Node *prev = cur->prev;
        free(cur);
        cur = prev;
    }
    g->head = g->tail = NULL;
    g->length = 0;
}

/* ════════════════════════════════════════
   初始化蛇（3节，位于地图中央，朝右）
   ════════════════════════════════════════ */
static void init_snake(GameState *g) {
    int cx = MAP_W / 2;
    int cy = MAP_H / 2;

    g->head      = NULL;
    g->tail      = NULL;
    g->length    = 0;
    g->direction = DIR_RIGHT;
    g->next_dir  = DIR_RIGHT;

    /* 从尾到头压入（尾→身→头） */
    push_head(g, cx - 2, cy);  /* 尾 */
    push_head(g, cx - 1, cy);  /* 身 */
    push_head(g, cx,     cy);  /* 头 */
}

/* ════════════════════════════════════════
   速度调整：根据得分提升速度
   ════════════════════════════════════════ */
static void adjust_speed(GameState *g) {
    if      (g->score >= 150) g->speed = SPEED_TURBO;
    else if (g->score >= 80)  g->speed = SPEED_FAST;
    else if (g->score >= 30)  g->speed = SPEED_NORMAL;
    else                      g->speed = SPEED_SLOW;
}

/* ════════════════════════════════════════
   显示得分面板（右侧信息区）
   ════════════════════════════════════════ */
#define INFO_X  (OFFSET_X + MAP_W * 2 + 2)  /* 右侧信息起始列 */

static void draw_score_panel(GameState *g) {
    const char *speed_str;

    /* 得分 */
    gotoxy(INFO_X, OFFSET_Y + 1);
    set_color(CLR_SCORE);
    printf("得分: %-5d", g->score);

    /* 最高分 */
    gotoxy(INFO_X, OFFSET_Y + 2);
    set_color(CLR_INFO);
    printf("最高: %-5d", g->high_score);

    /* 长度 */
    gotoxy(INFO_X, OFFSET_Y + 4);
    set_color(CLR_MENU);
    printf("长度: %-5d", g->length);

    /* 速度档位 */
    if      (g->speed == SPEED_TURBO)  speed_str = "极速 ★★★★";
    else if (g->speed == SPEED_FAST)   speed_str = "快速 ★★★ ";
    else if (g->speed == SPEED_NORMAL) speed_str = "普通 ★★  ";
    else                               speed_str = "慢速 ★   ";

    gotoxy(INFO_X, OFFSET_Y + 5);
    set_color(CLR_SCORE);
    printf("速度: %s", speed_str);

    /* 操作提示 */
    gotoxy(INFO_X, OFFSET_Y + 8);
    set_color(CLR_HINT);
    printf("方向: WASD / 方向键");
    gotoxy(INFO_X, OFFSET_Y + 9);
    printf("暂停: P / ESC");
    gotoxy(INFO_X, OFFSET_Y + 10);
    printf("退出: Q");

    reset_color();
}

/* 初始绘制信息区标题 */
static void draw_info_title(void) {
    gotoxy(INFO_X, OFFSET_Y - 1);
    set_color(CLR_TITLE);
    printf("╔══ 游戏信息 ══╗");
    gotoxy(INFO_X, OFFSET_Y + 12);
    printf("╚══════════════╝");
    reset_color();
}

/* ════════════════════════════════════════
   处理键盘输入（非阻塞）
   ════════════════════════════════════════ */
static void handle_input(GameState *g) {
    int key, ext;

    if (!_kbhit()) return;

    key = _getch();

    /* 方向键是扩展键，需读第二字节 */
    if (key == 0 || key == 224) {
        ext = _getch();
        switch (ext) {
            case 72: key = 'w'; break;  /* ↑ */
            case 80: key = 's'; break;  /* ↓ */
            case 75: key = 'a'; break;  /* ← */
            case 77: key = 'd'; break;  /* → */
            default: return;
        }
    }

    switch (key) {
        /* ─── 方向检测：防止 180° 反向 ─── */
        case 'w': case 'W':
            if (g->direction != DIR_DOWN)  g->next_dir = DIR_UP;
            break;
        case 's': case 'S':
            if (g->direction != DIR_UP)    g->next_dir = DIR_DOWN;
            break;
        case 'a': case 'A':
            if (g->direction != DIR_RIGHT) g->next_dir = DIR_LEFT;
            break;
        case 'd': case 'D':
            if (g->direction != DIR_LEFT)  g->next_dir = DIR_RIGHT;
            break;

        /* ─── 暂停 ─── */
        case 'p': case 'P': case 27:  /* ESC */
            g->paused = !g->paused;
            gotoxy(INFO_X, OFFSET_Y + 14);
            if (g->paused) {
                set_color(CLR_GAMEOVER);
                printf("  ██ 已暂停 ██  ");
            } else {
                set_color(CLR_DEFAULT);
                printf("               ");
            }
            reset_color();
            break;

        /* ─── 退出 ─── */
        case 'q': case 'Q':
            g->game_over = 1;
            break;
    }
}

/* ════════════════════════════════════════
   蛇移动核心逻辑（每帧调用一次）
   ════════════════════════════════════════ */
static void move_snake(GameState *g) {
    int nx, ny;
    int cell;

    if (g->paused || g->game_over) return;

    /* 更新实际方向 */
    g->direction = g->next_dir;

    /* 计算蛇头下一步坐标 */
    nx = g->head->x;
    ny = g->head->y;
    switch (g->direction) {
        case DIR_UP:    ny--; break;
        case DIR_DOWN:  ny++; break;
        case DIR_LEFT:  nx--; break;
        case DIR_RIGHT: nx++; break;
    }

    /* ─── 死亡判定：撞墙或撞自己 ─── */
    cell = g->map[ny][nx];
    if (cell == CELL_WALL || cell == CELL_SNAKE) {
        g->game_over = 1;
        return;
    }

    /* ─── 吃食物检测 ─── */
    if (cell == CELL_FOOD) {
        /* 得分统计 */
        g->score += 10;
        if (g->score > g->high_score)
            g->high_score = g->score;
        if (g->score > g_high_score)
            g_high_score = g->score;

        /* 吃食物不缩尾，蛇变长 */
        push_head(g, nx, ny);
        draw_head_cell(nx, ny);

        /* 原蛇头变为蛇身 */
        if (g->head->next)
            draw_cell(g->head->next->x, g->head->next->y, CELL_SNAKE);

        /* 生成新食物 */
        generate_food(g);
        draw_cell(g->food_x, g->food_y, CELL_FOOD);

        /* 速度调整 */
        adjust_speed(g);
        draw_score_panel(g);
    } else {
        /* ─── 普通移动：蛇身向前一步 ─── */
        /* 先移除尾部（erase 旧尾） */
        pop_tail(g);

        /* 添加新头 */
        push_head(g, nx, ny);
        draw_head_cell(nx, ny);

        /* 原蛇头变为蛇身 */
        if (g->head->next)
            draw_cell(g->head->next->x, g->head->next->y, CELL_SNAKE);

        draw_score_panel(g);
    }
}

/* ════════════════════════════════════════
   显示游戏结束画面
   ════════════════════════════════════════ */
static void show_game_over(GameState *g) {
    int cx = OFFSET_X + MAP_W;  /* 地图中央大概 X */

    /* 在地图中央叠加 Game Over 框 */
    int gx = OFFSET_X + MAP_W / 2 - 8;
    int gy = OFFSET_Y + MAP_H / 2 - 3;

    gotoxy(gx, gy);
    set_color(CLR_GAMEOVER);
    printf("╔═══════════════════╗");
    gotoxy(gx, gy + 1);
    printf("║                   ║");
    gotoxy(gx, gy + 2);
    printf("║   ★ 游戏结束 ★   ║");
    gotoxy(gx, gy + 3);
    printf("║                   ║");

    gotoxy(gx + 2, gy + 4);
    set_color(CLR_SCORE);
    printf("得分: %-5d", g->score);

    gotoxy(gx, gy + 4);
    set_color(CLR_GAMEOVER);
    printf("║");
    gotoxy(gx + 19, gy + 4);
    printf("║");

    gotoxy(gx + 2, gy + 5);
    set_color(CLR_INFO);
    printf("最高: %-5d", g_high_score);

    gotoxy(gx, gy + 5);
    set_color(CLR_GAMEOVER);
    printf("║");
    gotoxy(gx + 19, gy + 5);
    printf("║");

    gotoxy(gx, gy + 6);
    set_color(CLR_GAMEOVER);
    printf("║                   ║");
    gotoxy(gx, gy + 7);
    printf("╚═══════════════════╝");

    gotoxy(gx + 1, gy + 6);
    set_color(CLR_HINT);
    printf("按任意键返回主菜单");

    reset_color();
    (void)cx;  /* 消除未使用警告 */

    /* 清空输入缓冲 */
    while (_kbhit()) _getch();
    _getch();
}

/* ════════════════════════════════════════
   开始游戏（主游戏循环）
   ════════════════════════════════════════ */
static void start_game(void) {
    GameState g;
    DWORD last_tick, now;

    /* ── 初始化游戏状态 ── */
    memset(&g, 0, sizeof(g));
    g.high_score = g_high_score;
    g.speed      = SPEED_SLOW;
    g.game_over  = 0;
    g.paused     = 0;

    srand((unsigned int)time(NULL));

    clear_screen();
    set_cursor(0);

    /* ── 绘制地图标题 ── */
    gotoxy(OFFSET_X, OFFSET_Y - 2);
    set_color(CLR_TITLE);
    printf("贪 吃 蛇  Snake Game");
    reset_color();

    /* ── 初始化地图 ── */
    init_map(&g);

    /* ── 初始化蛇 ── */
    init_snake(&g);

    /* ── 生成第一个食物 ── */
    generate_food(&g);

    /* ── 完整绘制一次地图 ── */
    draw_full_map(&g);
    draw_info_title();
    draw_score_panel(&g);

    /* ── 游戏主循环 ── */
    last_tick = GetTickCount();

    while (!g.game_over) {
        /* 处理输入 */
        handle_input(&g);

        /* 计时控制移动速度 */
        now = GetTickCount();
        if (!g.paused && (now - last_tick) >= (DWORD)g.speed) {
            move_snake(&g);
            last_tick = now;
        }

        Sleep(10);  /* 降低 CPU 占用 */
    }

    /* ── 游戏结束 ── */
    if (g.score > 0 || g.length > 3) {
        show_game_over(&g);
    } else {
        /* 按 Q 直接退出时跳过结束画面 */
        show_game_over(&g);
    }

    free_snake(&g);
    set_cursor(1);
}

/* ════════════════════════════════════════
   绘制主菜单通用边框
   ════════════════════════════════════════ */
static void draw_menu_box(int x, int y, int w, int h) {
    int i;
    set_color(CLR_BORDER);

    gotoxy(x, y);
    printf("╔");
    for (i = 0; i < w - 2; i++) printf("═");
    printf("╗");

    for (i = 1; i < h - 1; i++) {
        gotoxy(x, y + i);
        printf("║");
        gotoxy(x + w - 1, y + i);
        printf("║");
    }

    gotoxy(x, y + h - 1);
    printf("╚");
    for (i = 0; i < w - 2; i++) printf("═");
    printf("╝");

    reset_color();
}

/* ════════════════════════════════════════
   主菜单
   ════════════════════════════════════════ */
static void show_main_menu(void) {
    const char *items[] = {
        "  1.  开始游戏  ",
        "  2.  帮助信息  ",
        "  3.  关于信息  ",
        "  4.  退出游戏  "
    };
    int n = 4, sel = 0, key, ext, i;

    while (1) {
        clear_screen();
        set_cursor(0);

        /* ASCII 标题艺术 */
        set_color(CLR_TITLE);
        print_center(2, " ____  _   _    _    _  _______ ", CLR_TITLE);
        print_center(3, "/ ___|| \\ | |  / \\  | |/ / ____|", CLR_TITLE);
        print_center(4, "\\___ \\|  \\| | / _ \\ | ' /|  _|  ", CLR_TITLE);
        print_center(5, " ___) | |\\  |/ ___ \\| . \\| |___ ", CLR_TITLE);
        print_center(6, "|____/|_| \\_/_/   \\_\\_|\\_\\_____|", CLR_TITLE);
        print_center(7, "       贪  吃  蛇  游  戏       ", CLR_SCORE);

        /* 菜单框 */
        draw_menu_box(28, 10, 24, 14);

        /* 菜单项 */
        for (i = 0; i < n; i++) {
            gotoxy(30, 12 + i * 2);
            if (i == sel) {
                set_color(CLR_SELECT);
                printf("▶ %s ◀", items[i]);
            } else {
                set_color(CLR_MENU);
                printf("  %s  ", items[i]);
            }
        }

        /* 最高分 */
        gotoxy(30, 11 + n * 2 + 1);
        set_color(CLR_HINT);
        printf("  历史最高分: %d", g_high_score);

        /* 底部提示 */
        print_center(22, "使用 W/S 或 ↑/↓ 选择，Enter 确认", CLR_HINT);

        reset_color();

        /* 等待输入 */
        key = _getch();
        if (key == 0 || key == 224) {
            ext = _getch();
            if (ext == 72) key = 'w';  /* ↑ */
            if (ext == 80) key = 's';  /* ↓ */
        }

        switch (key) {
            case 'w': case 'W': sel = (sel - 1 + n) % n; break;
            case 's': case 'S': sel = (sel + 1) % n;      break;
            case '1': sel = 0; /* 直接跳转 */
            case '\r': case '\n':
                switch (sel) {
                    case 0: start_game();     break;
                    case 1:
                        goto DO_HELP;
                    case 2: goto DO_ABOUT;
                    case 3: goto DO_EXIT;
                }
                break;
            case '2': goto DO_HELP;
            case '3': goto DO_ABOUT;
            case '4': case 'q': case 'Q': goto DO_EXIT;
        }
        continue;

DO_HELP:
        /* 帮助信息 */
        clear_screen();
        set_cursor(1);
        draw_menu_box(10, 2, 60, 20);

        print_center(3,  "══════ 帮助信息 ══════", CLR_TITLE);

        gotoxy(14, 5);
        set_color(CLR_SCORE);
        printf("【基本操作】");
        gotoxy(14, 6);  set_color(CLR_MENU);   printf("W / ↑      向上移动");
        gotoxy(14, 7);  printf("S / ↓      向下移动");
        gotoxy(14, 8);  printf("A / ←      向左移动");
        gotoxy(14, 9);  printf("D / →      向右移动");
        gotoxy(14, 10); printf("P / ESC    暂停/继续游戏");
        gotoxy(14, 11); printf("Q          退出当前游戏");

        gotoxy(14, 13);
        set_color(CLR_SCORE);
        printf("【游戏规则】");
        gotoxy(14, 14); set_color(CLR_MENU);   printf("● 控制蛇吃掉红色◆食物，每个+10分");
        gotoxy(14, 15); printf("● 吃到食物后蛇身变长一节");
        gotoxy(14, 16); printf("● 撞到墙壁或蛇身则游戏结束");

        gotoxy(14, 18);
        set_color(CLR_SCORE);
        printf("【速度档位】");
        gotoxy(14, 19); set_color(CLR_MENU);
        printf("0分:慢速 → 30分:普通 → 80分:快速 → 150分:极速");

        print_center(21, "按任意键返回主菜单", CLR_HINT);
        reset_color();
        while (_kbhit()) _getch();
        _getch();
        continue;

DO_ABOUT:
        /* 关于信息 */
        clear_screen();
        set_cursor(1);
        draw_menu_box(15, 3, 50, 16);

        print_center(4,  "══════ 关于信息 ══════", CLR_TITLE);

        gotoxy(20, 6);  set_color(CLR_SCORE); printf("游戏名称:  贪吃蛇 Snake Game");
        gotoxy(20, 7);  set_color(CLR_INFO);  printf("版    本:  v1.0.0");
        gotoxy(20, 8);  set_color(CLR_MENU);  printf("语    言:  C (C99)");
        gotoxy(20, 9);  printf("平    台:  Windows (conio.h)");
        gotoxy(20, 10); printf("数据结构:  双向链表");
        gotoxy(20, 12); set_color(CLR_HINT);
        printf("本游戏为学习用途，包含:");
        gotoxy(20, 13); printf("主菜单、地图渲染、蛇链表移动");
        gotoxy(20, 14); printf("碰撞检测、得分统计、速度自适应");
        gotoxy(20, 15); set_color(CLR_SCORE);
        printf("历史最高分: %d", g_high_score);

        print_center(19, "按任意键返回主菜单", CLR_HINT);
        reset_color();
        while (_kbhit()) _getch();
        _getch();
        continue;

DO_EXIT:
        clear_screen();
        set_cursor(1);
        print_center(12, "感谢游玩！再见 (｡>ω<｡)", CLR_TITLE);
        print_center(13, "", CLR_DEFAULT);
        printf("\n");
        reset_color();
        return;
    }
}

/* ════════════════════════════════════════
   程序入口
   ════════════════════════════════════════ */
int main(void) {
    /* 设置控制台编码为 UTF-8 以支持中文和方块字符 */
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    /* 设置控制台标题 */
    SetConsoleTitleA("贪吃蛇 Snake Game");

    /* 调整窗口大小（宽80列，高26行） */
    set_console_size(82, 26);

    /* 进入主菜单 */
    show_main_menu();

    return 0;
}
