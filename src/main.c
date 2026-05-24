#include "snake.h"

#include <windows.h>

int main(void) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);


    SetConsoleTitleA("Snake Game");


    set_console_size(82, 26);


    show_main_menu();

    return 0;
}
