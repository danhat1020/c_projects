#include <random.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1000
#define HEIGHT 1000
#define TITLE "Blue Noise"
#define FPS 60

#define mult 40.0f

// === GLOBAL FUNCTIONS ===
int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <image.jpg>\n", argv[0]);
    return 1;
  }

  // image
  Image img = LoadImage(argv[1]);
  if (!img.data) {
    fprintf(stderr, "Failed to load image: %s\n", argv[1]);
    return 1;
  }
  ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  ImageResizeNN(&img, WIDTH, HEIGHT);

  unsigned char *pixels = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
  if (!pixels) {
    fprintf(stderr, "Memory for pixels could not be allocated\n");
    return 1;
  }
  memcpy(pixels, img.data, WIDTH * HEIGHT * 4);
  UnloadImage(img);

  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(FPS);

  Texture2D texture = LoadTextureFromImage((Image){
      .data = pixels,
      .width = WIDTH,
      .height = HEIGHT,
      .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
      .mipmaps = 1,
  });

  int averaged = 0;
  Color col = {0};

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_SPACE)) {
      float *sum = calloc(3, sizeof(float));
      int count = 0;
      for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4) {
        for (int n = 0; n < 3; n++) {
          sum[n] += pixels[i + n];
        }
        count++;
      }
      for (int n = 0; n < 3; n++) {
        sum[n] /= (float)count;
      }
      col = (Color){(int)sum[0], (int)sum[1], (int)sum[2], 255};
      free(sum);
      averaged = 1;
    }

    UpdateTexture(texture, pixels);
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(texture, 0, 0, WHITE);

    if (averaged) {
      DrawRectangle(0, 0, WIDTH, HEIGHT, col);
      DrawRectangle(0, 0, 114, 82, BLACK);
      DrawText(TextFormat("Red  : %d", col.r), 10, 10, 20, RED);
      DrawText(TextFormat("Green: %d", col.g), 10, 34, 20, GREEN);
      DrawText(TextFormat("Blue : %d", col.b), 10, 58, 20, BLUE);
    }

    EndDrawing();
  }

  free(pixels);
  UnloadTexture(texture);

  CloseWindow();
  return 0;
}
