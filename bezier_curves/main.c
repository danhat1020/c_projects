#include "defs.h"

#include "line.c"
#include "line.h"
#include <raylib.h>

int main(void) {
    InitWindow(width, height, "Bezier Curves");
    SetTargetFPS(fps);

    i32 follow_mouse = 0;

    vec2f a = {width * 0.35f, height * 0.5f};
    vec2f b = {width * 0.65f, height * 0.5f};
    vec2f mid = {width * 0.5f, height * 0.5f};

    line l1 = {a, mid};
    line l2 = {mid, b};

    Vector2 mouse;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_SPACE)) {
            if (follow_mouse) {
                follow_mouse = false;
                ShowCursor();
            } else {
                follow_mouse = true;
                HideCursor();
            }
        }

        if (follow_mouse) {
            mouse = GetMousePosition();

            mid = (vec2f){mouse.x, mouse.y};
        }

        DrawCircle(mid.x, mid.y, 3, WHITE);

        l1 = (line){a, mid};
        l2 = (line){mid, b};

        line_draw(&l1);
        line_draw(&l2);

        curve_draw(a, b, mid);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
