#include <math.h>
#include <random.h>
#include <raylib.h>

#include "snake.h"

#define WIDTH 1280
#define HEIGHT 720
#define FPS 15

#define RES 20
#define COLS (int)((float)WIDTH / (float)RES)
#define ROWS (int)((float)HEIGHT / (float)RES)

void random_food(Vector2 *food) {
  food->x = floor(rng_range(0, COLS)) * RES;
  food->y = floor(rng_range(0, ROWS)) * RES;
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Snake");
  SetTargetFPS(FPS);

  Snake *s = snake_create(COLS, ROWS, RES);
  Vector2 food = {0};
  random_food(&food);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_L) && s->dir != 2) { s->dir = 0; }
    if (IsKeyPressed(KEY_K) && s->dir != 3) { s->dir = 1; }
    if (IsKeyPressed(KEY_J) && s->dir != 0) { s->dir = 2; }
    if (IsKeyPressed(KEY_I) && s->dir != 1) { s->dir = 3; }

    int result = snake_update(s, &food, WIDTH, HEIGHT);
    if (result != 0) {
      random_food(&food);
    }
    if (result == -1) {
      snake_destroy(s);
      s = snake_create(COLS, ROWS, RES);
    }

    BeginDrawing();
    ClearBackground(BLACK);
    DrawRectangleV(food, (Vector2){RES, RES}, RED);
    snake_render(s);
    EndDrawing();
  }

  snake_destroy(s);

  CloseWindow();
  return 0;
}
