#pragma once
#include <windows.h>
#include "snake_logic.h"

void ui_init(HINSTANCE hInst, int nCmdShow, GameState* state);
int  ui_run(GameState* state);
void ui_cleanup(void);