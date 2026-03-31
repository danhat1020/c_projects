#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1000
#define HEIGHT 1000

#define RES 10

static const char *RAMP = " .':,_-^;!\"+<>/\\?=*|{})([]%O0&#$@";

int main(int argc, char **argv) {
  const int ramp_length = strlen(RAMP);

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <image.jpg>\n", argv[0]);
    return 1;
  }

  InitWindow(WIDTH, HEIGHT, "ASCII Art");
  SetTargetFPS(5);

  Image img = LoadImage(argv[1]);
  int img_width = img.width;
  int img_height = img.height;

  unsigned int *pixels = calloc(img.width * img.height * 4, sizeof(unsigned int));

  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      Color c = GetImageColor(img, x, y);
      int idx = (y * img.width + x) * 4;
      pixels[idx + 0] = c.r;
      pixels[idx + 1] = c.g;
      pixels[idx + 2] = c.b;
      pixels[idx + 3] = 255;
    }
  }

  UnloadImage(img);

  Font font = LoadFontEx("./fonts/CascadiaCode-Bold.ttf", RES * 4, NULL, 0);
  Vector2 size = MeasureTextEx(font, "A", font.baseSize / 4.0f, 0.0f);

  int fx = size.x * 0.8;
  int fy = size.y * 0.8;

  int rows = (int)((float)HEIGHT / fy);
  int cols = (int)((float)WIDTH / fx);

  unsigned int *f_pix = calloc(cols * rows * 4, sizeof(unsigned int));

  float rx = (float)img_width / cols;
  float ry = (float)img_height / rows;

  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      int sumr = 0;
      int sumg = 0;
      int sumb = 0;
      int count = 0;

      int img_x = x * rx;
      int img_y = y * ry;

      for (int j = 0; j < (int)ry; j++) {
        for (int i = 0; i < (int)rx; i++) {
          int idx = ((img_y + j) * img_width + (img_x + i)) * 4;

          sumr += (int)(pixels[idx + 0]);
          sumg += (int)(pixels[idx + 1]);
          sumb += (int)(pixels[idx + 2]);
          count++;
        }
      }

      if (count > 0) {
        f_pix[(y * cols + x) * 4 + 0] = (int)((float)sumr / count);
        f_pix[(y * cols + x) * 4 + 1] = (int)((float)sumg / count);
        f_pix[(y * cols + x) * 4 + 2] = (int)((float)sumb / count);
      } else {
        for (int i = 0; i < 4; i++) {
          f_pix[(y * cols + x) * 4 + i] = 0;
        }
      }
      f_pix[(y * cols + x) * 4 + 3] = 255;
    }
  }

  free(pixels);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int y = 0; y < rows; y++) {
      for (int x = 0; x < cols; x++) {
        int idx = (y * cols + x) * 4;
        Color col = {f_pix[idx], f_pix[idx + 1], f_pix[idx + 2], f_pix[idx + 3]};

        float brightness = (col.r + col.g + col.b) / (3.0f * 255.0f);
        brightness = powf(brightness, 1.0);
        int ramp_idx = (int)(brightness * (ramp_length - 1));

        char ch = RAMP[ramp_idx];
        char str[2] = {ch, '\0'};
        DrawTextEx(font, str, (Vector2){x * fx, y * fy}, RES, 0, col);
      }
    }

    EndDrawing();
  }

  free(f_pix);
  UnloadFont(font);
  CloseWindow();
  return 0;
}
