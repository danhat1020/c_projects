#include "defs.h"
#include <raylib.h>

#include "food.c"
#include "snake.c"

int main(void) {
    InitWindow(width, height, "Snake Game");
    SetTargetFPS(fps);

    for (i32 i = 0; i < 10; i++) {
        printf("%f\n", rng());
    }

    Snake *snake = snake_create();
    vec2f *food = malloc(sizeof(vec2f));

    food_rand(food);

    while (!WindowShouldClose()) {
        // clang-format off
        if (IsKeyDown(KEY_RIGHT) && snake->dir != 2) { snake->dir = 0; }
        if (IsKeyDown(KEY_DOWN)  && snake->dir != 3) { snake->dir = 1; }
        if (IsKeyDown(KEY_LEFT)  && snake->dir != 0) { snake->dir = 2; }
        if (IsKeyDown(KEY_UP)    && snake->dir != 1) { snake->dir = 3; }
        // clang-format on

        snake_update(snake, food);

        BeginDrawing();
        ClearBackground(BLACK);

        for (i8 i = 1; i < cols; i++) {
            for (i8 j = 1; j < rows; j++) {
                DrawCircle(i * res, j * res, 1, (Color){255, 255, 255, 50});
            }
        }

        food_render(food);
        snake_render(snake);

        EndDrawing();
    }

    snake_destroy(snake);

    CloseWindow();
    return 0;
}
