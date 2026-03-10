#include <math.h>
#include <raylib.h>

#include <perlin.h>

#define WIDTH 800
#define HEIGHT 800
#define TITLE "Polar Perlin Noise Loop"
#define FPS 120

#define angle_count 2000

float map(float val, float a, float b, float c, float d);

// === GLOBAL VARIABLES ===
int angles[angle_count];
float offset = 0.0;

void init(void) {}

void clean(void) {}

void update(void) {
    float lastx, lasty;
    for (float i = 0; i < angle_count; i++) {
        float a = map(i, 0, angle_count, 0, M_PI * 2);
        float xoff = map(cosf(a), -1, 1, 0, 5);
        float yoff = map(sinf(a), -1, 1, 0, 5);
        float r = map(perlin_2d(xoff + offset, yoff + offset), -1, 1, 250, 350);

        float x = WIDTH / 2.0 + r * cosf(a);
        float y = HEIGHT / 2.0 + r * sinf(a);

        if (i != 0) {
            DrawLine(lastx, lasty, x, y, RAYWHITE);
        }

        lastx = x;
        lasty = y;
    }
    offset += 0.008;
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    init();
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        update();
        EndDrawing();
    }

    clean();

    CloseWindow();
    return 0;
}

float map(float val, float a, float b, float c, float d) {
    float diff1 = (b - a);
    float diff2 = (d - c);
    float result = val - a;
    result *= diff2 / diff1;
    return result + c;
}
