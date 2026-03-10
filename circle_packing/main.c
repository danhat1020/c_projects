#include "defs.h"
#include <raylib.h>

#include "circle.h"

#define SIZE (1024 * 2)

// === GLOBAL VARIABLES ===
Circle *circles;
i32 c_idx;

void init(void) {
    circles = calloc(SIZE, sizeof(Circle));
    c_idx = 0;
    printf("\n");
}

void clean(void) {
    free(circles);
    printf("\n\n");
}

void update(void) {
    if (c_idx < SIZE) {
        Circle a = create_circle(floor(rng_range(0, WIDTH)), floor(rng_range(0, HEIGHT)));
        for (i32 i = 0; i < c_idx; i++) {
            Circle b = circles[i];
            f32 dist = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
            if (dist < a.radius + b.radius) {
                a = create_circle(floor(rng_range(0, WIDTH)), floor(rng_range(0, HEIGHT)));
                i = -1;
            }
        }
        circles[c_idx++] = a;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    for (i32 i = 0; i < c_idx; i++) {
        for (i32 j = 0; j < c_idx; j++) {
            if (i == j) {
                continue;
            }
            Circle a = circles[i], b = circles[j];
            f32 dist = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
            if (dist <= a.radius + b.radius) {
                circles[i].growing = 0;
                circles[j].growing = 0;
            }
        }
        circle_grow(&circles[i]);
        circle_render(&circles[i]);
    }
    EndDrawing();

    printf("\r%.2f%%", 100.0f * (float)c_idx / (float)SIZE);
    fflush(stdout);
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
