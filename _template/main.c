#include <raylib.h>

#define WIDTH 1280
#define HEIGHT 720
#define TITLE "TITLE"
#define FPS 60

// === GLOBAL FUNCTIONS ===
// === GLOBAL VARIABLES ===

int main(void) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
