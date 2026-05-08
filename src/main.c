#include "snake.h"

#include <windows.h>

int main(void) {
    /* 璁剧疆鎺у埗鍙扮紪鐮佷负 UTF-8 浠ユ敮鎸佷腑鏂囧拰鏂瑰潡瀛楃 */
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    /* 璁剧疆鎺у埗鍙版爣棰?*/
    SetConsoleTitleA("璐悆铔?Snake Game");

    /* 璋冩暣绐楀彛澶у皬锛堝80鍒楋紝楂?6琛岋級 */
    set_console_size(82, 26);

    /* 杩涘叆涓昏彍鍗?*/
    show_main_menu();

    return 0;
}
