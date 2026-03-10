#include <raylib.h>

#define WIDTH 1280
#define HEIGHT 720
#define TITLE "TITLE"
#define FPS 60

// === GLOBAL VARIABLES ===

void init(void) {}

void clean(void) {}

void update(void) {}

int main(void) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    init();
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        update();
        EndDrawing();
    }

    clean();

    CloseWindow();
    return 0;
}
