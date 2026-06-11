\# Snake Game - Win32 API



\## Описание

Классическая игра Змейка на чистом C и Win32 API



\## Управление

\- WASD или Стрелки — движение

\- Пробел — перезапуск после Game Over

\- Esc — выход



\## Сборка

```cmd

cl main.c snake\_logic.c snake\_ui.c gdi32.lib user32.lib /Fe:snake.exe /W3


МОДУЛИ:

**snake\_logic.c — игровая логика, коллизии, спавн еды**

**snake\_ui.c — окно Win32, отрисовка GDI, ввод**

