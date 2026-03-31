#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH 1000
#define HEIGHT 1000
#define W (WIDTH * 4)
#define H (HEIGHT * 4)
#define FPS 240

#define N 8
#define POINT_SIZE 1

Vector2 lerp(const Vector2 *a, const Vector2 *b, float t) {
  Vector2 final = {b->x - a->x, b->y - a->y};
  final.x *= t, final.y *= t;
  final.x += a->x, final.y += a->y;
  return final;
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "");
  SetTargetFPS(FPS);

  Vector2 *points = malloc(sizeof(Vector2) * N);
  RenderTexture2D target = LoadRenderTexture(W, H);

  points[0] = (Vector2){0, 0};
  points[1] = (Vector2){W / 2.0f, 0};

  points[2] = (Vector2){W, 0};
  points[3] = (Vector2){W, H / 2.0f};

  points[4] = (Vector2){W, H};
  points[5] = (Vector2){W / 2.0f, H};

  points[6] = (Vector2){0, H};
  points[7] = (Vector2){0, H / 2.0f};

  int index = floor(rng_range(0, N));
  Vector2 current = points[index];
  Vector2 next = {0};

  BeginTextureMode(target);
  for (int i = 0; i < N; i++) {
    Color col = {128 * (1 - (points[i].x / W)) + 128, 0, 255 * (points[i].x / W), 255};
    DrawCircleV(points[i], POINT_SIZE, col);
  }
  EndTextureMode();

  while (!WindowShouldClose()) {
    BeginTextureMode(target);
    for (int i = 0; i < 1000; i++) {
      int index = floor(rng_range(0, N));
      Vector2 next = points[index];
      current = lerp(&current, &next, 2.0f / 3.0f);

      Color col = {128 * (1 - (current.x / W)) + 128, 0, 255 * (current.x / W), 255};
      DrawCircleV(current, POINT_SIZE, col);
    }
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(
        target.texture,
        (Rectangle){0, 0, W, -H},
        (Rectangle){0, 0, WIDTH, HEIGHT},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    EndDrawing();
  }

  free(points);

  CloseWindow();
  return 0;
}
