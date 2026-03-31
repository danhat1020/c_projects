#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdlib.h>

#define RES 2
#define COLS 600
#define ROWS 400

#define WIDTH (COLS * RES)
#define HEIGHT (ROWS * RES)
#define TITLE "Water Simulation"
#define FPS 60

#define IDX(i, j) (int)((i) + (j) * COLS)

// === GLOBAL VARIABLES ===
float *buf1, *buf2;
float dampening;

void init(void) {
  buf1 = calloc(COLS * ROWS, sizeof(float));
  buf2 = calloc(COLS * ROWS, sizeof(float));

  dampening = 0.975;
}

void clean(void) { free(buf1), free(buf2); }

void update(void) {
  if (rng() < 0.4) {
    int ind = floor(rng_range(0, COLS * ROWS));
    buf1[ind] = 1800.0f;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 mouse = GetMousePosition();
    buf1[IDX(mouse.x / RES, mouse.y / RES)] = 2000.0f;
  }

  BeginDrawing();
  ClearBackground(BLACK);
  for (int j = 0; j < ROWS; j++) {
    for (int i = 0; i < COLS; i++) {
      int val = (int)buf2[IDX(i, j)];
      if (val > 8) {
        val = val > 255 ? 255 : val;
        Color col = {val, val, val, 255};
        DrawRectangle(i * RES, j * RES, RES, RES, col);
      }

      if (i == 0 || i == COLS - 1 || j == 0 || j == ROWS - 1) { continue; };
      buf2[IDX(i, j)] = (buf1[IDX(i - 1, j)] + buf1[IDX(i + 1, j)] + buf1[IDX(i, j + 1)] + buf1[IDX(i, j - 1)]) / 2.0f - buf2[IDX(i, j)];
      buf2[IDX(i, j)] *= dampening;
    }
  }
  DrawFPS(10, 10);
  EndDrawing();

  float *tmp = buf1;
  buf1 = buf2;
  buf2 = tmp;
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(FPS);

  init();
  while (!WindowShouldClose()) { update(); }

  clean();

  CloseWindow();
  return 0;
}
