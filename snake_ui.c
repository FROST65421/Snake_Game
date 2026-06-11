#include "snake_ui.h"
#include <stdio.h>

/* Минимально необходимые static внутри модуля (разрешено ТЗ) */
static HWND g_hwnd = NULL;
static HDC  g_hdc_mem = NULL;
static HBITMAP g_hbm_mem = NULL;
static int g_width = GRID_W * CELL_SIZE;
static int g_height = GRID_H * CELL_SIZE;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    /* Получаем указатель на состояние через GWLP_USERDATA */
    GameState* state = (GameState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_KEYDOWN:
        if (state) {
            switch (wParam) {
            case VK_UP:    case 'W': logic_set_direction(state, 0, -1); break;
            case VK_DOWN:  case 'S': logic_set_direction(state, 0, 1); break;
            case VK_LEFT:  case 'A': logic_set_direction(state, -1, 0); break;
            case VK_RIGHT: case 'D': logic_set_direction(state, 1, 0); break;
            case VK_SPACE: case VK_RETURN:
                if (state->game_over) logic_reset(state); break;
            case VK_ESCAPE: PostQuitMessage(0); break;
            }
        }
        return 0;
    case WM_SIZE:
        g_width = LOWORD(lParam);
        g_height = HIWORD(lParam);
        if (g_hbm_mem) DeleteObject(g_hbm_mem);
        if (g_hdc_mem) DeleteDC(g_hdc_mem);
        {
            HDC hdc = GetDC(hwnd);
            g_hdc_mem = CreateCompatibleDC(hdc);
            g_hbm_mem = CreateCompatibleBitmap(hdc, g_width, g_height);
            SelectObject(g_hdc_mem, g_hbm_mem);
            ReleaseDC(hwnd, hdc);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ui_init(HINSTANCE hInst, int nCmdShow, GameState* state) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = TEXT("SnakeWin32");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);

    RECT rc = { 0, 0, g_width, g_height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindow(wc.lpszClassName, TEXT("Snake - C/Win32 API"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, hInst, NULL);

    /* Привязываем состояние к окну */
    SetWindowLongPtr(g_hwnd, GWLP_USERDATA, (LONG_PTR)state);

    HDC hdc = GetDC(g_hwnd);
    g_hdc_mem = CreateCompatibleDC(hdc);
    g_hbm_mem = CreateCompatibleBitmap(hdc, g_width, g_height);
    SelectObject(g_hdc_mem, g_hbm_mem);
    ReleaseDC(g_hwnd, hdc);
}

static void ui_render(HDC hdc, const GameState* s) {
    RECT bg = { 0, 0, g_width, g_height };
    FillRect(hdc, &bg, (HBRUSH)GetStockObject(BLACK_BRUSH));

    /* Сетка */
    HPEN grid_pen = CreatePen(PS_SOLID, 1, RGB(30, 30, 30));
    HPEN old_pen = SelectObject(hdc, grid_pen);
    for (int x = 0; x <= g_width; x += CELL_SIZE) { MoveToEx(hdc, x, 0, NULL); LineTo(hdc, x, g_height); }
    for (int y = 0; y <= g_height; y += CELL_SIZE) { MoveToEx(hdc, 0, y, NULL); LineTo(hdc, g_width, y); }
    SelectObject(hdc, old_pen); DeleteObject(grid_pen);

    /* Змейка */
    HBRUSH snake_brush = CreateSolidBrush(RGB(50, 205, 50));
    old_pen = (HPEN)SelectObject(hdc, snake_brush);
    for (int i = 0; i < s->length; i++) {
        int px = s->body[i].x * CELL_SIZE;
        int py = s->body[i].y * CELL_SIZE;
        Rectangle(hdc, px + 1, py + 1, px + CELL_SIZE - 1, py + CELL_SIZE - 1);
    }
    SelectObject(hdc, old_pen); DeleteObject(snake_brush);

    /* Еда */
    HBRUSH food_brush = CreateSolidBrush(RGB(255, 69, 0));
    SelectObject(hdc, food_brush);
    Ellipse(hdc, s->food.x * CELL_SIZE + 4, s->food.y * CELL_SIZE + 4,
        (s->food.x + 1) * CELL_SIZE - 4, (s->food.y + 1) * CELL_SIZE - 4);
    SelectObject(hdc, old_pen); DeleteObject(food_brush);

    /* Текст */
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    char txt[40];
    int len = sprintf(txt, "Score: %d", s->score);
    TextOutA(hdc, 10, 10, txt, len);

    if (s->game_over) {
        len = sprintf(txt, "GAME OVER! [SPACE] Restart");
        TextOutA(hdc, g_width / 2 - 90, g_height / 2, txt, len);
    }
}

int ui_run(GameState* state) {
    MSG msg;
    LARGE_INTEGER freq, prev;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

    const float TICK_DT = 0.08f; /* ~12.5 кадров логики в секунду */
    float accumulator = 0.0f;

    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = (float)(now.QuadPart - prev.QuadPart) / (float)freq.QuadPart;
        prev = now;
        if (dt > 0.2f) dt = 0.2f; /* Защита от "спирали смерти" */

        accumulator += dt;
        while (accumulator >= TICK_DT) {
            logic_update(state);
            accumulator -= TICK_DT;
        }

        /* Двойная буферизация */
        if (g_hdc_mem) {
            ui_render(g_hdc_mem, state);
            HDC hdc = GetDC(g_hwnd);
            BitBlt(hdc, 0, 0, g_width, g_height, g_hdc_mem, 0, 0, SRCCOPY);
            ReleaseDC(g_hwnd, hdc);
        }
    }
    return (int)msg.wParam;
}

void ui_cleanup(void) {
    if (g_hbm_mem) DeleteObject(g_hbm_mem);
    if (g_hdc_mem) DeleteDC(g_hdc_mem);
}