#include <math.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 800
#define TITLE "Cube Wave"
#define FPS 120

#define radians(angle) ((float)(angle) / 180 * M_PI)

#define DIM 15
#define AMOUNT (DIM * 10)

#define TOP_COLOR (Color){130, 186, 180, 255}
#define LEFT_COLOR (Color){63, 84, 132, 255}
#define RIGHT_COLOR (Color){230, 228, 176, 255}
// #e6e4b0

// === GLOBAL VARIABLES ===
float angle;

float *lengths;

void init(void) {
    angle = 0.0f;

    lengths = calloc(DIM * DIM, sizeof(float));
}

void clean(void) { free(lengths); }

void update(void) {
    // update lengths
    for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
            int idx = i + j * DIM;
            float diffx = (i + 0.5f) - (DIM / 2.0f);
            float diffy = (j + 0.5f) - (DIM / 2.0f);
            float diff = diffx * diffx + diffy * diffy;
            float offset = 4.5 * M_PI * diff / (DIM * DIM);

            lengths[idx] = AMOUNT * 0.8f * sinf(angle + offset);
        }
    }

    angle -= radians(150) * GetFrameTime();

    // render
    BeginDrawing();
    ClearBackground(WHITE);

    float size = 20.0f;

    float x = WIDTH / 2.0f;
    float y = AMOUNT * 1.35;

    for (int i = 0; i < DIM * DIM; i++) {
        float multi = 1.4f;
        // top
        float y_ = y - (lengths[i] / 2.0f);
        Vector2 a = {x, y_};
        Vector2 b = {x - size, y_ - size / 2.0f};
        Vector2 c = {x, y_ - size};
        Vector2 d = {x + size, y_ - size / 2.0f};
        DrawTriangle(a, c, b, TOP_COLOR);
        DrawTriangle(a, d, c, TOP_COLOR);
        // left
        c = b;
        Vector2 e = a;
        e.y += lengths[i] + AMOUNT * multi;
        c.y += lengths[i] + AMOUNT * multi;
        DrawTriangle(a, b, e, LEFT_COLOR);
        DrawTriangle(b, c, e, LEFT_COLOR);
        // right
        c = d;
        c.y += lengths[i] + AMOUNT * multi;
        DrawTriangle(a, e, c, RIGHT_COLOR);
        DrawTriangle(c, d, a, RIGHT_COLOR);

        // move x and y
        x -= size;
        y += size / 2.0f;
        if (i % DIM == DIM - 1) {
            x += size * (DIM + 1);
            y -= size * ((DIM - 1) / 2.0f);
        }
    }

    DrawText("cube wave by @beesandbombs", 10, 10, 30, BLACK);
    DrawText("https://beesandbombs.tumblr.com/post/149654056864/cube-wave", 10, 45, 20, GRAY);

    EndDrawing();
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
