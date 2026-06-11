#include <windows.h>
#include <time.h>
#include "snake_logic.h"
#include "snake_ui.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show) {
    srand((unsigned)time(NULL));

    GameState state;
    ui_init(hInst, show, &state);
    logic_init(&state);

    int res = ui_run(&state);
    ui_cleanup();
    return res;
}