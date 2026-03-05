#include "defs.h"

#include "cube.h"

#include "cube.c"
#include <raylib.h>

int main(void) {
    InitWindow(width, height, "3D Cube");

    i32 res, color_steps, fps;
    // clang-format off
    if (low_res == 0) {
        res = 1, color_steps = 200, fps = 120;
    } else if (low_res == 1) {
        res = 4, color_steps =  16, fps =  25;
    } else if (low_res == 2) {
        res = 8, color_steps =  12, fps =  15;
    }
    // clang-format on
    RenderTexture2D target = LoadRenderTexture(width / res, height / res);

    SetTargetFPS(fps);

    vec3 *cube = cube_create();
    i32 pause = 0;

    f32 angle = rng_range(0, M_PI * 2);
    f32 rotation_speed = 80.0f;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            pause = 1 - pause;
        }

        if (!pause) {
            angle += 1.0 * GetFrameTime();
            f32 x = rotation_speed * sin(angle) * rng_range(0.8, 1.25);
            f32 y = rotation_speed * sin(angle + (M_PI * 2 / 3)) * rng_range(0.8, 1.25);
            f32 z = rotation_speed * sin(angle + (M_PI * 4 / 3)) * rng_range(0.8, 1.25);

            cube_rotate(cube, x * GetFrameTime(), y * GetFrameTime(), z * GetFrameTime());
        }

        if (low_res > 0) {
            BeginTextureMode(target);
            ClearBackground(BLACK);
            cube_render(cube, color_steps, res);
            EndTextureMode();

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                target.texture,
                (Rectangle){0, 0, (f32)width / res, -(f32)height / res},
                (Rectangle){0, 0, width, height},
                (Vector2){0, 0},
                0.0f,
                WHITE);
            for (i32 i = 0; i < width / res; i++) {
                for (i32 j = 0; j < height / res; j++) {
                    DrawRectangleLines(i * res, j * res, res, res, (Color){20, 20, 20, 30 * low_res});
                }
            }
            EndDrawing();
        } else {
            BeginDrawing();
            ClearBackground(BLACK);
            cube_render(cube, color_steps, res);

            i32 frames = ceil(1 / GetFrameTime());
            DrawText(TextFormat("%d", frames), 10, 10, 40, WHITE);

            EndDrawing();
        }
    }

    cube_destroy(cube);

    CloseWindow();
    return 0;
}
