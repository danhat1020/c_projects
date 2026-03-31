#include <random.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1024
#define HEIGHT WIDTH
#define TITLE "Blue Noise"
#define FPS 60

#define mult 40.0f

// === GLOBAL FUNCTIONS ===
void threshold_bw(unsigned char *pixels, int idx) {
  static const unsigned char levels[] = {0, 255};
  int n = sizeof(levels) / sizeof(levels[0]);
  int avg = (int)(0.299f * pixels[idx + 0] + 0.587f * pixels[idx + 1] + 0.114f * pixels[idx + 2]) - (mult / 2.0f);
  avg *= (255.0f - (mult / 2.0f)) / 255.0f;
  avg = avg < 0 ? 0 : avg;
  int bucket = avg * n / 256;
  for (int i = 0; i < 3; i++) {
    pixels[idx + i] = levels[bucket];
  }
}

void threshold(unsigned char *pixels, int idx) {
  static const unsigned char levels[] = {0, 255};
  for (int i = 0; i < 3; i++) {
    int bucket = pixels[idx + i] * (sizeof(levels) / sizeof(levels[0])) / 256;
    pixels[idx + i] = levels[bucket];
  }
}

void noise(unsigned char *pixels, int idx) {
  float avg = (pixels[idx + 0] + pixels[idx + 1] + pixels[idx + 2]) / 3.0f + 6; // added 6 to squash values in from 0-255 to 6-249
  if (avg > 128) { avg = 255 - avg; }

  int value = (int)rng_range(-avg / 1.75f, avg / 2.0f); // less range on lower end to counteract unbalanced darkening
  float opacity = 0.8f;

  for (int i = 0; i < 3; i++) {
    int result = pixels[idx + i] + (int)(value * opacity);
    result = result < 0 ? 0 : result;
    result = result > 255 ? 255 : result;
    pixels[idx + i] = (unsigned char)result;
  }
}

void blue_noise(unsigned char *pixels, int x, int y, int width, unsigned char *bn_tex, int bn_w, int bn_h) {
  int idx = (y * width + x) * 4;

  int noise_idx = ((y % bn_h) * bn_w + (x % bn_w)) * 4;
  int noise_val = bn_tex[noise_idx] - 128; // center around 0

  for (int i = 0; i < 3; i++) {
    int result = pixels[idx + i] + noise_val;
    result = result < 0 ? 0 : result;
    result = result > 255 ? 255 : result;
    pixels[idx + i] = (unsigned char)result;
  }
}

void mean_filter(unsigned char *pixels, unsigned char *out, int width, int height, int radius) {
  memcpy(out, pixels, width * height * 4);
  for (int y = radius; y < height - radius; y++) {
    for (int x = radius; x < width - radius; x++) {
      int idx = (y * width + x) * 4;
      for (int c = 0; c < 3; c++) {
        int sum = 0;
        int count = 0;
        for (int dy = -radius; dy <= radius; dy++) {
          for (int dx = -radius; dx <= radius; dx++) {
            sum += pixels[((y + dy) * width + (x + dx)) * 4 + c];
            count++;
          }
        }

        out[idx + c] = (unsigned char)((float)sum / count); // mean
      }
      out[idx + 3] = pixels[idx + 3]; // alpha passthrough
    }
  }
}

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

  // original pixel array
  unsigned char *original = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
  if (!original) {
    fprintf(stderr, "Memory for pixels could not be allocated\n");
    return 1;
  }
  memcpy(original, img.data, WIDTH * HEIGHT * 4);
  UnloadImage(img);

  // threshold pixel array
  unsigned char *thresh = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
  if (!thresh) {
    fprintf(stderr, "Memory for thresh could not be allocated\n");
    return 1;
  }
  memcpy(thresh, original, WIDTH * HEIGHT * 4);
  // threshold + mean pixel array
  unsigned char *threshmean = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
  if (!threshmean) {
    fprintf(stderr, "Memory for threshmean could not be allocated\n");
    return 1;
  }
  memcpy(threshmean, original, WIDTH * HEIGHT * 4);
  // blue noise pixel array
  unsigned char *boriginal = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
  if (!boriginal) {
    fprintf(stderr, "Memory for boriginal could not be allocated\n");
    return 1;
  }
  memcpy(boriginal, original, WIDTH * HEIGHT * 4);
  // blue noise + threshold pixel array
  unsigned char *bthresh = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
  if (!bthresh) {
    fprintf(stderr, "Memory for bthresh could not be allocated\n");
    return 1;
  }
  memcpy(bthresh, original, WIDTH * HEIGHT * 4);
  // blue noise + threshold + mean pixel array
  unsigned char *bthreshmean = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
  if (!bthreshmean) {
    fprintf(stderr, "Memory for bthreshmean could not be allocated\n");
    return 1;
  }
  memcpy(bthreshmean, original, WIDTH * HEIGHT * 4);

  // blue noise image
  Image bn_img = LoadImage("textures/blue_noise.png");
  if (!bn_img.data) { fprintf(stderr, "Failed to load blue_noise texture\n"); }
  ImageFormat(&bn_img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  int bn_w = bn_img.width;
  int bn_h = bn_img.height;

  // blue noise pixel array
  unsigned char *bn_tex = malloc(sizeof(unsigned char) * bn_w * bn_h * 4);
  memcpy(bn_tex, bn_img.data, bn_w * bn_h * 4);
  UnloadImage(bn_img);

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int idx = (y * WIDTH + x) * 4;

      blue_noise(boriginal, x, y, WIDTH, bn_tex, bn_w, bn_h);
      blue_noise(bthresh, x, y, WIDTH, bn_tex, bn_w, bn_h);
      blue_noise(bthreshmean, x, y, WIDTH, bn_tex, bn_w, bn_h);

      threshold(thresh, idx);
      threshold(threshmean, idx);
      threshold(bthresh, idx);
      threshold(bthreshmean, idx);
    }
  }

  unsigned char *tm_copy = malloc(WIDTH * HEIGHT * 4 * sizeof(unsigned char));
  unsigned char *btm_copy = malloc(WIDTH * HEIGHT * 4 * sizeof(unsigned char));
  if (!tm_copy) {
    fprintf(stderr, "Memory for tm_copy could not be allocated\n");
    return 1;
  }
  if (!btm_copy) {
    fprintf(stderr, "Memory for btm_copy could not be allocated\n");
    return 1;
  }
  memcpy(tm_copy, threshmean, WIDTH * HEIGHT * 4);
  memcpy(btm_copy, bthreshmean, WIDTH * HEIGHT * 4);
  mean_filter(tm_copy, threshmean, WIDTH, HEIGHT, 1);
  mean_filter(btm_copy, bthreshmean, WIDTH, HEIGHT, 1);
  free(tm_copy);
  free(btm_copy);

  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(FPS);

  Texture2D texture = LoadTextureFromImage((Image){
      .data = original,
      .width = WIDTH,
      .height = HEIGHT,
      .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
      .mipmaps = 1,
  });

  int num = 0;
  int last_num = 0;

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_L) && num < 7) { num++; }
    if (IsKeyPressed(KEY_H) && num > 0) { num--; }
    if (num != last_num) {
      switch (num) {
        case 1: UpdateTexture(texture, thresh); break;
        case 2: UpdateTexture(texture, threshmean); break;
        case 4: UpdateTexture(texture, boriginal); break;
        case 5: UpdateTexture(texture, bthresh); break;
        case 6: UpdateTexture(texture, bthreshmean); break;
        default: UpdateTexture(texture, original); break;
      }
    }

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(texture, 0, 0, WHITE);

    // draw label
    switch (num) {
      case 1: // threshold
        DrawRectangle(0, 0, 208, 40, BLACK);
        DrawText("Effect: Threshold", 10, 10, 20, WHITE);
        break;
      case 2: // threshold + mean
        DrawRectangle(0, 0, 286, 40, BLACK);
        DrawText("Effect: Threshold + mean", 10, 10, 20, WHITE);
        break;
      case 4: // blue noise
        DrawRectangle(0, 0, 208, 40, BLACK);
        DrawText("Effect: Blue noise", 10, 10, 20, WHITE);
        break;
      case 5: // blue noise + threshold
        DrawRectangle(0, 0, 334, 40, BLACK);
        DrawText("Effect: Blue noise + threshold", 10, 10, 20, WHITE);
        break;
      case 6: // blue noise + threshold + mean
        DrawRectangle(0, 0, 410, 40, BLACK);
        DrawText("Effect: Blue noise + threshold + mean", 10, 10, 20, WHITE);
        break;
      default:
        DrawRectangle(0, 0, 178, 40, BLACK);
        DrawText("Effect: Original", 10, 10, 20, WHITE);
        break;
    }

    EndDrawing();

    last_num = num;
  }

  free(thresh), free(threshmean), free(boriginal), free(bthresh), free(bthreshmean);
  free(original);
  UnloadTexture(texture);

  CloseWindow();
  return 0;
}
