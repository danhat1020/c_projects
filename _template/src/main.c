#include <raylib.h>

#define WIDTH 1280
#define HEIGHT 720
#define FPS 60

int main(void) {
  InitWindow(WIDTH, HEIGHT, "");
  SetTargetFPS(FPS);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
