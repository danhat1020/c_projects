#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1280
#define HEIGHT 720
#define FPS 60

#define RES 2
#define COLS (int)((float)WIDTH / (float)RES)
#define ROWS (int)((float)HEIGHT / (float)RES)

int count_neighbours(unsigned char *grid, int i, int j) {
  int sum = 0;
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      int a = i + x, b = j + y;
      if (a >= 0 && b >= 0 && a < COLS && b < ROWS) { sum += grid[b * COLS + a]; }
    }
  }
  return sum - grid[j * COLS + i];
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Conway's Game of Life");
  SetTargetFPS(FPS);

  int size = COLS * ROWS;

  unsigned char *grid = malloc(sizeof(unsigned char) * size);
  unsigned char *temp = malloc(sizeof(unsigned char) * size);

  for (int i = 0; i < size; i++) {
    grid[i] = floor(rng_range(0, 2));
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    memcpy(temp, grid, sizeof(unsigned char) * size);
    for (int j = 0; j < ROWS; j++) {
      for (int i = 0; i < COLS; i++) {
        int sum = count_neighbours(grid, i, j);

        int index = j * COLS + i;
        if (grid[index]) {
          if (sum < 2 || sum > 3) { temp[index] = 0; }
        } else {
          if (sum == 3) { temp[index] = 1; }
        }

        // draw grid
        if (temp[j * COLS + i]) { DrawRectangle(i * RES, j * RES, RES, RES, GetColor(0xB8B8B8FF)); }
      }
    }
    memcpy(grid, temp, sizeof(unsigned char) * size);

    EndDrawing();
  }

  free(temp);
  free(grid);

  CloseWindow();
  return 0;
}
