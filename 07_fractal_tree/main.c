#include "defs.h"
#include <raylib.h>

#define BRANCH_ANGLE (2 * M_PI) / 11.0f
#define RATIO 0.75f

void DrawBranch(f32 x, f32 y, f32 length, f32 angle, f32 thickness, u32 i);

u32 generations = 0;
u32 max = 15;

int main(void) {
    InitWindow(width, height, "Fractal Tree");
    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_K) && generations < max) { generations++; }
        if (IsKeyPressed(KEY_J) && generations > 0) { generations--; }

        DrawText(TextFormat("%d", generations), 10, 10, 20, WHITE);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawBranch(width * 0.5f, height, 180, 0.0f, 20, 0);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawBranch(f32 x, f32 y, f32 length, f32 angle, f32 thickness, u32 i) {
    if (i <= generations) {
        Vector2 *start = malloc(sizeof(Vector2));
        start->x = x;
        start->y = y;

        Vector2 *end = malloc(sizeof(Vector2));
        end->x = x + sinf(angle / 2) * length;
        end->y = y - cosf(angle / 2) * length;

        i32 thick = ceil(thickness);

        u32 alpha = 255 - (i * 10);

        DrawLineEx(*start, *end, thick, (Color){255, 255, 255, alpha});
        DrawBranch(end->x, end->y, length * RATIO, angle + BRANCH_ANGLE, thickness * RATIO, i + 1);
        DrawBranch(end->x, end->y, length * RATIO, angle - BRANCH_ANGLE, thickness * RATIO, i + 1);

        free(end);
        free(start);
    }
    return;
}
