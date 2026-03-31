#include "defs.h"

#include "ball.h"

#include "ball.c"

int main(void) {
    InitWindow(width, height, "Bouncing Balls");
    SetTargetFPS(fps);

    Ball *balls = balls_create();

    for (u32 i = 0; i < ball_count; i++) {
        f32 mass = rng_range(0.8, 4);
        f32 r = sqrt(mass) * 25;
        f32 x = rng_range(r, width - r);
        f32 y = rng_range(r, height - r);
        f32 angle = rng_range(0, 2 * M_PI);
        f32 speed = rng_range(2, 3) * 60;
        f32 vx = speed * cos(angle);
        f32 vy = speed * sin(angle);
        balls[i] = (Ball){
            .pos = (Vector2){x, y},
            .vel = (Vector2){vx, vy},
            .mass = mass,
            .radius = r,
        };
    }

    while (!WindowShouldClose()) {
        // update logic/state
        BeginDrawing();
        ClearBackground(BLACK);
        for (u32 i = 0; i < ball_count; i++) {
            ball_update(&balls[i]);
            for (u32 j = i + 1; j < ball_count; j++) {
                ball_resolve_collision(&balls[i], &balls[j]);
            }
            ball_render(&balls[i]);
        }
        EndDrawing();
    }

    balls_destroy(balls);

    CloseWindow();
    return 0;
}
