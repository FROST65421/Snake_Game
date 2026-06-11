#pragma once
#include <stdbool.h>

#define GRID_W 20
#define GRID_H 15
#define CELL_SIZE 32
#define MAX_SNAKE_LEN (GRID_W * GRID_H)

typedef struct { int x, y; } Vec2;

typedef struct {
    Vec2 body[MAX_SNAKE_LEN];
    int length;
    Vec2 food;
    int dir_x, dir_y;      // Текущее направление
    int next_dir_x, next_dir_y; // Буфер ввода (защита от разворота на 180)
    int score;
    bool game_over;
} GameState;

void logic_init(GameState* s);
void logic_set_direction(GameState* s, int dx, int dy);
void logic_update(GameState* s);
void logic_reset(GameState* s);