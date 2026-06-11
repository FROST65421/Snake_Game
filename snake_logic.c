#include "snake_logic.h"
#include <stdlib.h>

static void spawn_food(GameState* s) {
    int tries = 0;
    while (tries < 200) {
        s->food.x = rand() % GRID_W;
        s->food.y = rand() % GRID_H;
        bool on_snake = false;
        for (int i = 0; i < s->length; i++) {
            if (s->body[i].x == s->food.x && s->body[i].y == s->food.y) {
                on_snake = true; break;
            }
        }
        if (!on_snake) return;
        tries++;
    }
}

void logic_init(GameState* s) {
    s->length = 3;
    s->body[0] = (Vec2){ 5, 7 };
    s->body[1] = (Vec2){ 4, 7 };
    s->body[2] = (Vec2){ 3, 7 };
    s->dir_x = 1; s->dir_y = 0;
    s->next_dir_x = 1; s->next_dir_y = 0;
    s->score = 0;
    s->game_over = false;
    spawn_food(s);
}

void logic_set_direction(GameState* s, int dx, int dy) {
    if (s->game_over) return;
    // Запрет разворота на 180 градусов
    if (dx == -s->dir_x && dy == -s->dir_y) return;
    s->next_dir_x = dx;
    s->next_dir_y = dy;
}

void logic_update(GameState* s) {
    if (s->game_over) return;

    s->dir_x = s->next_dir_x;
    s->dir_y = s->next_dir_y;

    Vec2 new_head = { s->body[0].x + s->dir_x, s->body[0].y + s->dir_y };

    // Столкновение со стеной
    if (new_head.x < 0 || new_head.x >= GRID_W ||
        new_head.y < 0 || new_head.y >= GRID_H) {
        s->game_over = true; return;
    }
    // Столкновение с хвостом
    for (int i = 0; i < s->length; i++) {
        if (s->body[i].x == new_head.x && s->body[i].y == new_head.y) {
            s->game_over = true; return;
        }
    }

    // Сдвиг тела
    for (int i = s->length; i > 0; i--) s->body[i] = s->body[i - 1];
    s->body[0] = new_head;

    // Поедание еды
    if (new_head.x == s->food.x && new_head.y == s->food.y) {
        s->length++;
        s->score++;
        spawn_food(s);
    }
}

void logic_reset(GameState* s) {
    logic_init(s);
}