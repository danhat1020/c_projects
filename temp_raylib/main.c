#include "defs.h"
#include <raylib.h>

// === GLOBAL VARIABLES ===

void init(void) {}

void clean(void) {}

void update(void) {
    // render
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
}

int main(void) {
    // clang-format off
    InitWindow(WIDTH, HEIGHT, "TITLE");
    SetTargetFPS(FPS);

    init();
    while (!WindowShouldClose()) { update(); }
    clean();

    CloseWindow();
    return 0;
    // clang-format on
}
