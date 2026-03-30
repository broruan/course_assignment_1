#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// 平台相关头文件
    #include <conio.h>
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#define WIDTH 40
#define HEIGHT 20
#define MAX_SNAKE_LEN WIDTH * HEIGHT

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[MAX_SNAKE_LEN];
    int length;
    int dx, dy;
} Snake;

// 全局游戏状态
Snake snake;
Point food;
bool gameOver = false;
int score = 0;
int gameSpeed = 100; // 毫秒

void init_game() {
    snake.length = 3;
    snake.dx = 1; snake.dy = 0; // 初始向右
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = 5 - i;
        snake.body[i].y = HEIGHT / 2;
    }
    
    srand(time(NULL));
    food.x = rand() % (WIDTH - 2) + 1;
    food.y = rand() % (HEIGHT - 2) + 1;
    gameOver = false;
    score = 0;
}

void draw() {
    
        printf("\033[H"); 
    

    // 绘制上边框
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("-");
    printf("+\n");

    // 绘制游戏区域
    for (int y = 0; y < HEIGHT; y++) {
        printf("|");
        for (int x = 0; x < WIDTH; x++) {
            bool isSnake = false;
            bool isHead = false;
            
            // 检查是否是蛇身
            for (int i = 0; i < snake.length; i++) {
                if (snake.body[i].x == x && snake.body[i].y == y) {
                    isSnake = true;
                    if (i == 0) isHead = true;
                    break;
                }
            }

            if (isHead) printf("@");      // 蛇头
            else if (isSnake) printf("o"); // 蛇身
            else if (food.x == x && food.y == y) printf("*"); // 食物
            else printf(" ");             // 空白
        }
        printf("|\n");
    }

    // 绘制下边框
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("-");
    printf("+\n");
    
    printf("Score: %d (Use WASD or Arrows, Q to quit)\n", score);
    fflush(stdout); // 强制刷新输出缓冲区
}

void input() {
    int key = 0;
    
    
        if (_kbhit()) {
            key = _getch();
            if (key == 0 || key == 224) { // 处理方向键前缀
                key = _getch();
                if (key == 72) key = 'w'; // Up
                if (key == 80) key = 's'; // Down
                if (key == 77) key = 'd'; // Right
                if (key == 75) key = 'a'; // Left
            } else {
                key = tolower(key);
            }
        }

    if (key == 'q') gameOver = true;
    
    // 防止反向移动
    if (key == 'w' && snake.dy != 1) { snake.dx = 0; snake.dy = -1; }
    else if (key == 's' && snake.dy != -1) { snake.dx = 0; snake.dy = 1; }
    else if (key == 'a' && snake.dx != 1) { snake.dx = -1; snake.dy = 0; }
    else if (key == 'd' && snake.dx != -1) { snake.dx = 1; snake.dy = 0; }
}

void update() {
    // 计算新头部位置
    Point newHead = {
        snake.body[0].x + snake.dx,
        snake.body[0].y + snake.dy
    };

    // 撞墙检测
    if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT) {
        gameOver = true;
        return;
    }

    // 撞自己检测
    for (int i = 0; i < snake.length; i++) {
        if (newHead.x == snake.body[i].x && newHead.y == snake.body[i].y) {
            gameOver = true;
            return;
        }
    }

    // 移动身体：从尾部向前复制
    for (int i = snake.length; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = newHead;

    // 吃食物
    if (newHead.x == food.x && newHead.y == food.y) {
        snake.length++;
        score += 10;
        // 重新生成食物 (简单随机，可能生成在蛇身上，严谨版需检查)
        food.x = rand() % (WIDTH - 2) + 1;
        food.y = rand() % (HEIGHT - 2) + 1;
    }
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[2J\033[H");
    #endif
}

void show_instructions() {
    clear_screen();
    printf("=== 游戏说明 ===\n");
    printf("1. 使用 WASD 或方向键控制蛇的移动。\n");
    printf("2. 吃到 '*' 可以得分并变长。\n");
    printf("3. 碰到墙或者自己的身体游戏结束。\n");
    printf("4. 按 Q 退出游戏。\n\n");
    printf("按任意键返回菜单...");
    _getch();
}

int show_menu() {
    while (1) {
        clear_screen();
        printf("=== 贪吃蛇 游戏 ===\n");
        printf("1. 开始游戏（普通 100ms）\n");
        printf("2. 开始游戏（快速 60ms）\n");
        printf("3. 开始游戏（极速 30ms）\n");
        printf("4. 游戏说明\n");
        printf("5. 退出\n");
        printf("请选择 (1-5): ");

        int key = _getch();
        if (key == '1') { gameSpeed = 100; return 1; }
        if (key == '2') { gameSpeed = 60; return 1; }
        if (key == '3') { gameSpeed = 30; return 1; }
        if (key == '4') {
            show_instructions();
            continue;
        }
        if (key == '5' || key == 'q' || key == 'Q') return 0;
    }
}

int main() {
    #ifndef _WIN32
        init_terminal();
        // 注册退出时恢复终端
        atexit(restore_terminal);
    #endif

    while (1) {
        // 1) 显示主菜单，选择返回 false 时退出程序
        if (!show_menu()) break;

        // 2) 初始化游戏状态（蛇位置、食物、分数）
        init_game();

        // 3) 游戏主循环，按 gameSpeed 控制帧率
        while (!gameOver) {
            // 取消每帧清屏，防止闪烁；draw() 内已移动光标到起始位置
            draw();              // 在屏幕起始位置绘制整帧内容
            input();             // 读取按键并更新移动方向/退出信号
            update();            // 计算蛇移动、碰撞、吃食物逻辑
            SLEEP_MS(gameSpeed); // 休眠控制速度，由菜单选项设定（30ms/60ms/100ms）
        }

        // 4) 游戏结束，清屏显示最终结果
        clear_screen();
        draw();
        printf("\nGAME OVER! Final Score: %d\n", score);
        printf("按任意键返回菜单...");
        _getch();
                                                  
        // 5) 复位 gameOver，回到菜单继续下一局
        gameOver = false;
    }

    #ifndef _WIN32
        restore_terminal();
    #endif

    return 0;
}