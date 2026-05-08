static int g_high_score = 0;


/* 移动光标�?(x, y) */
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

/* 设置控制台字体颜�?*/
static void set_color(int attr) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)attr);
}

/* 重置颜色 */
static void reset_color(void) {
    set_color(CLR_DEFAULT);
}

/* 居中打印字符串（控制台宽�?80�?*/
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

/* 调整控制台窗口大�?*/
static void set_console_size(int w, int h) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    /* 先设置缓冲区 */
    COORD bufSize = { (SHORT)w, (SHORT)h };
    SetConsoleScreenBufferSize(hOut, bufSize);
    /* 再设置窗口大�?*/
    SMALL_RECT rect = { 0, 0, (SHORT)(w - 1), (SHORT)(h - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &rect);
}
static void draw_cell(int mx, int my, int type) {
    int sx = OFFSET_X + mx * 2;
    int sy = OFFSET_Y + my;
    gotoxy(sx, sy);
    switch (type) {
        case CELL_WALL:
            set_color(CLR_WALL); //定义边界
            printf("██");
            break;
        case CELL_OBSTACLE:
            set_color(CLR_WALL); //障碍物使用与边界相同的颜�?
            printf("██");
            break;
        case CELL_SNAKE:
            /* 蛇头用亮绿，蛇身由调用方�?CELL_SNAKE�?
               头部特殊处理�?draw_full_map */
            set_color(CLR_SNAKE_B);
            printf("▓▓");
            break;
        case CELL_FOOD_1:     //蓝色食物 - 1�?
            set_color(CLR_FOOD_1);
            printf("◆◆");
            break;
        case CELL_FOOD_2:     //紫色食物 - 2�?
            set_color(CLR_FOOD_2);
            printf("◆◆");
            break;
        case CELL_FOOD_3:     //橙色食物 - 3�?
            set_color(CLR_FOOD_3);
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

/* 专门绘制蛇头（颜色区别于蛇身�?*/
static void draw_head_cell(int mx, int my) {
    int sx = OFFSET_X + mx * 2;
    int sy = OFFSET_Y + my;
    gotoxy(sx, sy);
    set_color(CLR_SNAKE_H);
    printf("██");
    reset_color();
}

/* ════════════════════════════════════════
   初始�?/ 绘制地图边界
   ════════════════════════════════════════ */
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
    if (g->food_type == 1)
        draw_cell(g->food_x, g->food_y, CELL_FOOD_1);
    else if (g->food_type == 2)
        draw_cell(g->food_x, g->food_y, CELL_FOOD_2);
    else
        draw_cell(g->food_x, g->food_y, CELL_FOOD_3);
}

/* ════════════════════════════════════════
   生成食物（随机放置在空格上，随机类型�?
   ════════════════════════════════════════ */
static void draw_score_panel(GameState *g) {
    const char *speed_str;

    /* 得分 */
    gotoxy(INFO_X, OFFSET_Y + 1);
    set_color(CLR_SCORE);
    printf("得分: %-5d", g->score);

    /* 最高分 */
    gotoxy(INFO_X, OFFSET_Y + 2);
    set_color(CLR_INFO);
    printf("最�? %-5d", g->high_score);

    /* 长度 */
    gotoxy(INFO_X, OFFSET_Y + 4);
    set_color(CLR_MENU);
    printf("长度: %-5d", g->length);

    /* 速度档位 */
    if      (g->speed == SPEED_TURBO)  speed_str = "极�?★★★★";
    else if (g->speed == SPEED_FAST)   speed_str = "快�?★★�?";
    else if (g->speed == SPEED_NORMAL) speed_str = "普�?★★  ";
    else                               speed_str = "慢�?�?  ";

    gotoxy(INFO_X, OFFSET_Y + 5);
    set_color(CLR_SCORE);
    printf("速度: %s", speed_str);

    /* 操作提示 */
    gotoxy(INFO_X, OFFSET_Y + 8);
    set_color(CLR_HINT);
    printf("方向: WASD / 方向�?);
    gotoxy(INFO_X, OFFSET_Y + 9);
    printf("暂停: P / ESC");
    gotoxy(INFO_X, OFFSET_Y + 10);
    printf("退�? Q");

    reset_color();
}

/* 初始绘制信息区标�?*/
static void draw_info_title(void) {
    gotoxy(INFO_X, OFFSET_Y - 1);
    set_color(CLR_TITLE);
    printf("╔═�?游戏信息 ══�?);
    gotoxy(INFO_X, OFFSET_Y + 12);
    printf("╚══════════════╝");
    reset_color();
}

/* ════════════════════════════════════════
   处理键盘输入（非阻塞�?
   ════════════════════════════════════════ */
static void show_game_over(GameState *g) {
    int cx = OFFSET_X + MAP_W;  /* 地图中央大概 X */

    /* 在地图中央叠�?Game Over �?*/
    int gx = OFFSET_X + MAP_W / 2 - 8;
    int gy = OFFSET_Y + MAP_H / 2 - 3;

    gotoxy(gx, gy);
    set_color(CLR_GAMEOVER);
    printf("╔═══════════════════�?);
    gotoxy(gx, gy + 1);
    printf("�?                  �?);
    gotoxy(gx, gy + 2);
    printf("�?  �?游戏结束 �?  �?);
    gotoxy(gx, gy + 3);
    printf("�?                  �?);

    gotoxy(gx + 2, gy + 4);
    set_color(CLR_SCORE);
    printf("得分: %-5d", g->score);

    gotoxy(gx, gy + 4);
    set_color(CLR_GAMEOVER);
    printf("�?);
    gotoxy(gx + 19, gy + 4);
    printf("�?);

    gotoxy(gx + 2, gy + 5);
    set_color(CLR_INFO);
    printf("最�? %-5d", g_high_score);

    gotoxy(gx, gy + 5);
    set_color(CLR_GAMEOVER);
    printf("�?);
    gotoxy(gx + 19, gy + 5);
    printf("�?);

    gotoxy(gx, gy + 6);
    set_color(CLR_GAMEOVER);
    printf("�?                  �?);
    gotoxy(gx, gy + 7);
    printf("╚═══════════════════�?);

    gotoxy(gx + 1, gy + 6);
    set_color(CLR_HINT);
    printf("按任意键返回主菜�?);

    reset_color();
    (void)cx;  /* 消除未使用警�?*/

    /* 清空输入缓冲 */
    while (_kbhit()) _getch();
    _getch();
}

/* ════════════════════════════════════════
   开始游戏（主游戏循环）
   ════════════════════════════════════════ */
static void draw_menu_box(int x, int y, int w, int h) {
    int i;
    set_color(CLR_BORDER);

    gotoxy(x, y);
    printf("�?);
    for (i = 0; i < w - 2; i++) printf("�?);
    printf("�?);

    for (i = 1; i < h - 1; i++) {
        gotoxy(x, y + i);
        printf("�?);
        gotoxy(x + w - 1, y + i);
        printf("�?);
    }

    gotoxy(x, y + h - 1);
    printf("�?);
    for (i = 0; i < w - 2; i++) printf("�?);
    printf("�?);

    reset_color();
}

/* ════════════════════════════════════════
   主菜�?
   ════════════════════════════════════════ */
static void show_main_menu(void) {
    const char *items[] = {
        "  1.  开始游�? ",
        "  2.  帮助信息  ",
        "  3.  关于信息  ",
        "  4.  退出游�? "
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
        print_center(7, "       �? �? �? �? �?      ", CLR_SCORE);

        /* 菜单�?*/
        draw_menu_box(28, 10, 24, 14);

        /* 菜单�?*/
        for (i = 0; i < n; i++) {
            gotoxy(30, 12 + i * 2);
            if (i == sel) {
                set_color(CLR_SELECT);
                printf("�?%s ◀", items[i]);
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
        print_center(22, "使用 W/S �?�?�?选择,Enter 确认", CLR_HINT);

        reset_color();

        /* 等待输入 */
        key = _getch();
        if (key == 0 || key == 224) {
            ext = _getch();
            if (ext == 72) key = 'w';  /* �?*/
            if (ext == 80) key = 's';  /* �?*/
        }

        switch (key) {
            case 'w': case 'W': sel = (sel - 1 + n) % n; break;
            case 's': case 'S': sel = (sel + 1) % n;      break;
            case '1': sel = 0;  /* fall through */  /* 直接跳转 */ 
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
        printf("【基本操作�?);
        gotoxy(14, 5);  set_color(CLR_MENU);   printf("W / �?     向上移动");
        gotoxy(14, 6);  printf("S / �?     向下移动");
        gotoxy(14, 7);  printf("A / �?     向左移动");
        gotoxy(14, 8);  printf("D / �?     向右移动");
        gotoxy(14, 9); printf("P / ESC    暂停/继续游戏");
        gotoxy(14, 10); printf("Q          退出当前游�?);

        gotoxy(14, 12);
        set_color(CLR_SCORE);
        printf("【游戏规则�?);
        gotoxy(14, 13); set_color(CLR_MENU);   printf("�?控制蛇吃掉食物获得分�?);
        gotoxy(14, 14); printf("  蓝色�?+10�?| 紫色�?+20�?| 橙色�?+30�?);
        gotoxy(14, 15); printf("�?吃到食物后蛇身变长一�?);
        gotoxy(14, 16); printf("�?障碍物大�?x5,随机刷新位置");
        gotoxy(14, 17); printf("�?撞到墙壁或障碍物则游戏结�?);
        gotoxy(14, 18); printf("�?撞到蛇身会失去被碰部位之后的身体");

        gotoxy(14, 19);
        set_color(CLR_SCORE);
        printf("【速度档位�?);
        gotoxy(14, 20); set_color(CLR_MENU);
        printf("0�?慢�?�?30�?普�?�?80�?快�?�?150�?极�?);

        print_center(23, "按任意键返回主菜�?, CLR_HINT);
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

        gotoxy(20, 6);  set_color(CLR_SCORE); printf("游戏名称:  贪吃�?Snake Game");
        gotoxy(20, 7);  set_color(CLR_INFO);  printf("�?   �?  v1.0.0");
        gotoxy(20, 8);  set_color(CLR_MENU);  printf("�?   言:  C ");
        gotoxy(20, 9);  printf("�?   �?  Windows ");
        gotoxy(20, 10); printf("数据结构:  双向链表");
        gotoxy(20, 12); set_color(CLR_HINT);
        printf("本游戏为学习用途，包含:");
        gotoxy(20, 13); printf("主菜单、地图渲染、蛇链表移动");
        gotoxy(20, 14); printf("碰撞检测、得分统计、速度自适应");
        gotoxy(20, 15); set_color(CLR_SCORE);
        printf("历史最高分: %d", g_high_score);

        print_center(19, "按任意键返回主菜�?, CLR_HINT);
        reset_color();
        while (_kbhit()) _getch();
        _getch();
        continue;

DO_EXIT:
        clear_screen();
        set_cursor(1);
        print_center(12, "感谢游玩！再�?(�?ω<�?", CLR_TITLE);
        print_center(13, "", CLR_DEFAULT);
        printf("\n");
        reset_color();
        return;
    }
}

/* ════════════════════════════════════════
   程序入口
   ════════════════════════════════════════ */
