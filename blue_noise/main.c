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
    static const float weights[] = {0.299f, 0.587f, 0.114f};
    int n = sizeof(levels) / sizeof(levels[0]);
    int bucket;
    for (int i = 0; i < 3; i++) {
        float weight = weights[(i + 1) % 3] + weights[(i + 2) % 3];
        float multi = powf(weight, 0.06);
        int col = pixels[idx + i] * multi;
        col = col < 0 ? 0 : col;
        bucket = col * n / 256;
        pixels[idx + i] = levels[bucket];
    }
}

void noise(unsigned char *pixels, int idx) {
    float avg = (pixels[idx + 0] + pixels[idx + 1] + pixels[idx + 2]) / 3.0f + 6; // added 6 to squash values in from 0-255 to 6-249
    if (avg > 128) {
        avg = 255 - avg;
    }

    int value = (int)rng_range(-avg / 1.75f, avg / 2.0f); // less range on lower end to counteract unbalanced darkening
    float opacity = 0.8f;

    for (int i = 0; i < 3; i++) {
        int result = pixels[idx + i] + (int)(value * opacity);
        result = result < 0 ? 0 : result;
        result = result > 255 ? 255 : result;
        pixels[idx + i] = (unsigned char)result;
    }
}

void blue_noise(unsigned char *pixels, int x, int y, unsigned char *bn_tex, int bn_w, int bn_h) {
    int idx = (y * WIDTH + x) * 4;

    int noise_idx = ((y % bn_h) * bn_w + (x % bn_w)) * 4;
    int noise_val = bn_tex[noise_idx] - 128; // center around 0
    for (int i = 0; i < 3; i++) {
        int result = pixels[idx + i] + (noise_val * 1.0f);
        result = result < 0 ? 0 : result;
        result = result > 255 ? 255 : result;
        pixels[idx + i] = (unsigned char)result;
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

    unsigned char *original = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
    if (!original) {
        fprintf(stderr, "Memory for pixels could not be allocated\n");
        return 1;
    }
    memcpy(original, img.data, WIDTH * HEIGHT * 4);
    UnloadImage(img);

    unsigned char *pixels = malloc(sizeof(unsigned char) * WIDTH * HEIGHT * 4);
    if (!pixels) {
        fprintf(stderr, "Memory for pix2 could not be allocated\n");
        return 1;
    }
    memcpy(pixels, original, WIDTH * HEIGHT * 4);

    // blue noise
    Image bn_img = LoadImage("assets/blue_noise.png");
    if (!bn_img.data) {
        fprintf(stderr, "Failed to load blue_noise texture\n");
    }
    ImageFormat(&bn_img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    int bn_w = bn_img.width;
    int bn_h = bn_img.height;

    unsigned char *bn_tex = malloc(sizeof(unsigned char) * bn_w * bn_h * 4);
    memcpy(bn_tex, bn_img.data, bn_w * bn_h * 4);

    UnloadImage(bn_img);

    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    Texture2D texture = LoadTextureFromImage((Image){
        .data = original,
        .width = WIDTH,
        .height = HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1,
    });

    int iterations = 0;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            memcpy(pixels, original, WIDTH * HEIGHT * 4);
            if (iterations == 0) {
                for (int y = 0; y < HEIGHT; y++) {
                    for (int x = 0; x < WIDTH; x++) {
                        int idx = (y * WIDTH + x) * 4;
                        threshold_bw(pixels, idx);
                    }
                }
            } else if (iterations == 1) {
                for (int y = 0; y < HEIGHT; y++) {
                    for (int x = 0; x < WIDTH; x++) {
                        int idx = (y * WIDTH + x) * 4;
                        blue_noise(pixels, x, y, bn_tex, bn_w, bn_h);
                        threshold_bw(pixels, idx);
                    }
                }
            } else if (iterations == 2) {
                for (int y = 0; y < HEIGHT; y++) {
                    for (int x = 0; x < WIDTH; x++) {
                        int idx = (y * WIDTH + x) * 4;
                        threshold(pixels, idx);
                    }
                }
            } else if (iterations == 3) {
                for (int y = 0; y < HEIGHT; y++) {
                    for (int x = 0; x < WIDTH; x++) {
                        int idx = (y * WIDTH + x) * 4;

                        // gamma correction
                        float gamma = 1.5f;
                        pixels[idx + 0] = (unsigned char)(powf(pixels[idx + 0] / 255.0f, gamma) * 255.0f);
                        pixels[idx + 1] = (unsigned char)(powf(pixels[idx + 1] / 255.0f, gamma) * 255.0f);
                        pixels[idx + 2] = (unsigned char)(powf(pixels[idx + 2] / 255.0f, gamma) * 255.0f);

                        blue_noise(pixels, x, y, bn_tex, bn_w, bn_h);
                        threshold(pixels, idx);
                    }
                }
            }
            iterations++;
        }

        UpdateTexture(texture, pixels);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        EndDrawing();
    }

    free(original);
    UnloadTexture(texture);

    CloseWindow();
    return 0;
}
