#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH 600
#define HEIGHT 600
#define TITLE "Water Simulation"
#define FPS 60

// === GLOBAL VARIABLES ===
float *buf1, *buf2;
float dampening;

void init(void) {
    buf1 = calloc(WIDTH * HEIGHT, sizeof(float));
    buf2 = calloc(WIDTH * HEIGHT, sizeof(float));

    dampening = 0.985;
}

void clean(void) {
    free(buf1), free(buf2);
}

int idx(int i, int j) {
    return i + j * WIDTH;
}

void update(void) {

    if (rng() < 0.6) {
        int ind = floor(rng_range(0, WIDTH * HEIGHT));
        buf1[ind] = 1600.0f;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        buf1[idx(mouse.x, mouse.y)] = 2000.0f;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    for (int j = 0; j < HEIGHT; j++) {
        for (int i = 0; i < WIDTH; i++) {
            int val = (int)buf2[idx(i, j)];
            if (val > 8) {
                val = val > 255 ? 255 : val;
                Color col = {val, val, val, 255};
                DrawPixel(i, j, col);
            }

            // clang-format off
            if (i == 0 || i == WIDTH - 1 || j == 0 || j == HEIGHT - 1) { continue; };
            buf2[idx(i, j)] = (
                               buf1[idx(i - 1, j    )]
                             + buf1[idx(i + 1, j    )]
                             + buf1[idx(i    , j + 1)]
                             + buf1[idx(i    , j - 1)]) / 2.0f - buf2[idx(i, j)];
            buf2[idx(i, j)] *= dampening;
            // clang-format on
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
    while (!WindowShouldClose()) {
        update();
    }

    clean();

    CloseWindow();
    return 0;
}
