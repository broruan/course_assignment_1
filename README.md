# Snake Game

本项目入口逻辑在 `src\main.c`，头文件 `include\snake.h`。

## 目录结构

- `include/snake.h`：公共常量、数据结构与函数声明
- `src/main.c`：程序入口（负责设置控制台并调用菜单）
- `src/snake.c`：贪吃蛇游戏核心逻辑
- `src/output/snake.exe`：编译产物（可执行文件）
- `txt_files/`：集中放置的 `.txt` 资源文件

## 编译（不使用 makefile）

使用 MinGW 的 `gcc`：

```powershell
& "C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\gcc.exe" -std=c99 -O2 -Wall -Wextra -DWIN32 `
  -I".\include" `
  -o ".\src\output\snake.exe" `
  ".\src\main.c" ".\src\snake.c"
```

## 运行

直接运行生成的：

```powershell
.\src\output\snake.exe
```

