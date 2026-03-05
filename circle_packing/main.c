#include "defs.h"
#include <raylib.h>

#include "circle.h"

// === GLOBAL VARIABLES ===
Circle *circles;
i32 c_idx;

void init(void) {
    circles = malloc(sizeof(Circle) * 1024);
    c_idx = 0;
}

void clean(void) {
    free(circles);
}

void update(void) {
    if (c_idx < 1024 && rng() < 0.5) {
        circles[c_idx++] = create_circle(floor(rng_range(0, WIDTH)), floor(rng_range(0, HEIGHT)));
        for (i32 i = 0; i < c_idx - 1; i++) {
            Circle a = circles[i];
            Circle b = circles[c_idx - 1];
            f32 dist = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
            if (dist < a.radius + b.radius) {
                circles[c_idx++] = create_circle(floor(rng_range(0, WIDTH)), floor(rng_range(0, HEIGHT)));
            }
        }
        printf("%d\n", c_idx);
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
}

int main(void) {
    // clang-format off
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    init();
    while (!WindowShouldClose()) { update(); }
    clean();

    CloseWindow();
    return 0;
    // clang-format on
}
