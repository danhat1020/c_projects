#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1000
#define HEIGHT 1000
#define FPS 60

#define COLS 7
#define ROWS 6
#define SPACE_W (int)((float)(WIDTH - 20) / COLS)
#define RADIUS (SPACE_W * 0.4f)

#define IDX(i, j) ((j) * COLS + (i))

typedef enum {
  EMPTY = 0,
  P_RED,
  P_YELLOW,
} Piece;

int clamp(int val, int a, int b) {
  if (val < a) return a;
  else if (val > b) return b;
  return val;
}

int check_win(Piece *board, int i, int j) {
  Vector2 directions[] = {{1, 0}, {0, 1}, {1, 1}, {-1, 1}};
  int curr = board[IDX(i, j)];
  for (int d = 0; d < 4; d++) {
    int count = 1;
    Vector2 *dir = &directions[d];

    for (int n = 1; n < 4; n++) {
      int c = i + dir->x * n;
      int r = j + dir->y * n;
      int next = board[IDX(c, r)];
      if (c < 0 || c >= COLS || r < 0 || r >= ROWS || next != curr) { break; }
      count++;
    }
    for (int n = 1; n < 4; n++) {
      int c = i - dir->x * n;
      int r = j - dir->y * n;
      int next = board[IDX(c, r)];
      if (c < 0 || c >= COLS || r < 0 || r >= ROWS || next != curr) { break; }
      count++;
    }
    if (count >= 4) { return curr; }
  }
  return 0;
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Connect Four");
  SetTargetFPS(FPS);

  Piece *board = calloc(ROWS * COLS, sizeof(Piece));

  Color board_col = {0, 40, 200, 255};
  Color space_col = {0, 4, 16, 255};
  Color red_col = {240, 80, 80, 255};
  Color yellow_col = {240, 200, 80, 255};

  int turn = floor(rng_range(1, 3));

  float mx;
  int curr_col;
  int placed_col = 0, placed_row = 0;

  int winner = 0;

  while (!WindowShouldClose()) {
    if (winner == 0) {
      mx = GetMousePosition().x;
      curr_col = clamp(floor((mx - 10) * COLS / WIDTH), 0, COLS - 1);

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // put piece in space
        int placed = 0;
        int row = ROWS - 1;
        while (!placed) {
          if (board[row * COLS + curr_col] == EMPTY) {
            board[row * COLS + curr_col] = turn;
            placed_col = curr_col;
            placed_row = row;
            placed = 1;
            break;
          }
          row--;
          if (row < 0) { break; }
        }
        if (placed) { turn = 3 - turn; }
      }

      winner = check_win(board, placed_col, placed_row);
      if (winner == 1) { printf("Winner: RED\n"); }
      if (winner == 2) { printf("Winner: YELLOW\n"); }
    }

    BeginDrawing();
    ClearBackground(BLACK);

    // draw board
    DrawRectangle(10, 10 + SPACE_W, WIDTH - 20, HEIGHT - 20 - SPACE_W, board_col);

    // draw spaces
    for (int j = 0; j < ROWS; j++) {
      for (int i = 0; i < COLS; i++) {
        int space = board[j * COLS + i];
        Color col;
        if (space == EMPTY) col = space_col;
        else if (space == P_RED) col = red_col;
        else col = yellow_col;

        float x = SPACE_W * (0.5f + i) + 10;
        float y = SPACE_W * (1.5f + j) + 10;
        DrawCircle(x, y, RADIUS, col);
      }
    }

    // draw next
    if (winner == 0) {
      float n_x = curr_col * SPACE_W + (SPACE_W / 2.0f) + 10;
      float n_y = SPACE_W * 0.5f + 10;
      Color col = turn == 1 ? red_col : yellow_col;
      DrawCircle(n_x, n_y, RADIUS, col);
    }

    EndDrawing();
  }

  free(board);

  CloseWindow();
  return 0;
}
