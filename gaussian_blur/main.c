#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 600
#define HEIGHT 600
#define TITLE "Gaussian Blur"
#define FPS 60

float *build_kernel(int radius, float sigma) {
    int size = 2 * radius + 1;
    float *kernel = malloc(sizeof(float) * size * size);
    float sum = 0.0f;

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            float exponent = -(x * x + y * y) / (2.0f * sigma * sigma);
            int idx = (y + radius) * size + (x + radius);
            float value = expf(exponent);
            kernel[idx] = value;
            sum += value;
        }
    }

    for (int i = 0; i < size * size; i++) {
        kernel[i] /= sum;
    }

    return kernel;
}

void gaussian(int *arr, int w, int h, int radius) {
    float sigma = radius / 2.0f;
    float *kernel = build_kernel(radius, sigma);
    int size = 2 * radius + 1;

    int *temp = malloc(sizeof(int) * w * h * 4);
    memcpy(temp, arr, sizeof(int) * w * h * 4);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            float r = 0, g = 0, b = 0, a = 0;

            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    int k_idx = (ky + radius) * size + (kx + radius);
                    float weight = kernel[k_idx];

                    int nx = (x + kx < 0) ? 0 : (x + kx >= w ? w - 1 : x + kx);
                    int ny = (y + ky < 0) ? 0 : (y + ky >= h ? h - 1 : y + ky);
                    int n_idx = (ny * w + nx) * 4;
                    r += temp[n_idx + 0] * weight;
                    g += temp[n_idx + 1] * weight;
                    b += temp[n_idx + 2] * weight;
                    a += temp[n_idx + 3] * weight;
                }
            }

            arr[idx + 0] = (int)r;
            arr[idx + 1] = (int)g;
            arr[idx + 2] = (int)b;
            arr[idx + 3] = (int)a;
        }
    }

    free(temp);
    free(kernel);
}

Image img;

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <image.jpg>\n", argv[0]);
        return 1;
    }

    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    img = LoadImage(argv[1]);

    int *pixels = calloc(img.width * img.height * 4, sizeof(int));

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

    int w = img.width, h = img.height;
    UnloadImage(img);

    gaussian(pixels, w, h, 8);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int idx = (y * w + x) * 4;
                Color c = {pixels[idx], pixels[idx + 1], pixels[idx + 2], pixels[idx + 3]};
                DrawPixel(x, y, c);
            }
        }

        EndDrawing();
    }

    free(pixels);

    CloseWindow();
    return 0;
}
