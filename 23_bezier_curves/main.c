#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

#define WIDTH 1280
#define HEIGHT 720
#define TITLE "Bezier Curves"
#define FPS 120

#define POINT_COUNT WIDTH
#define GRAB_RADIUS 6

Vector2 cubic(const Vector2 p0, const Vector2 p1, const Vector2 p2, const Vector2 p3, float t);

// === GLOBAL VARIABLES ===
Vector2 anchor1, anchor2;
Vector2 control1, control2;
Vector2 *points;

Vector2 *dragging;
Vector2 drag_off;
Vector2 mouse;

void init(void) {
    anchor1 = (Vector2){WIDTH * 0.2f, HEIGHT * 0.5f};
    anchor2 = (Vector2){WIDTH * 0.8f, HEIGHT * 0.5f};
    control1 = (Vector2){WIDTH * 0.4f, HEIGHT * 0.5f};
    control2 = (Vector2){WIDTH * 0.6f, HEIGHT * 0.5f};

    points = malloc(sizeof(Vector2) * POINT_COUNT);

    dragging = NULL;
    drag_off = (Vector2){0, 0};
}

void clean(void) {
    free(points);
}

void update(void) {
    // allow movement of points
    mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 *pts[] = {&anchor1, &control1, &control2, &anchor2};
        for (int i = 0; i < 4; i++) {
            Vector2 *p = pts[i];
            float dx = mouse.x - p->x;
            float dy = mouse.y - p->y;
            if ((dx * dx + dy * dy) <= (GRAB_RADIUS * GRAB_RADIUS)) {
                dragging = p;
                drag_off.x = dx, drag_off.y = dy;
                break;
            }
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { dragging = NULL; }

    if (dragging) {
        dragging->x = Clamp(mouse.x - drag_off.x, GRAB_RADIUS, WIDTH - GRAB_RADIUS);
        dragging->y = Clamp(mouse.y - drag_off.y, GRAB_RADIUS, HEIGHT - GRAB_RADIUS);
    }

    DrawFPS(10, 10);
    DrawText("All points are draggable!", 10, 35, 20, GRAY);

    // draw points and lines
    DrawLineEx(anchor1, control1, 2, (Color){255, 255, 255, 50});
    DrawLineEx(anchor2, control2, 2, (Color){255, 255, 255, 50});
    DrawCircleV(anchor1, GRAB_RADIUS, WHITE);
    DrawCircleV(anchor2, GRAB_RADIUS, WHITE);
    DrawCircleV(control1, GRAB_RADIUS, (Color){120, 120, 120, 255});
    DrawCircleV(control2, GRAB_RADIUS, (Color){120, 120, 120, 255});

    // draw bezier
    for (int i = 0; i < POINT_COUNT; i++) {
        float t = (float)i / (float)POINT_COUNT;

        points[i] = cubic(anchor1, control1, control2, anchor2, t);

        if (i > 0) { DrawLineEx(points[i - 1], points[i], 2, WHITE); }
        if (i == POINT_COUNT - 1) { DrawLineEx(points[i], anchor2, 2, WHITE); }
    }
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

Vector2 cubic(const Vector2 p0, const Vector2 p1, const Vector2 p2, const Vector2 p3, float t) {
    Vector2 final;

    float n = 1.0 - t;

    final.x = pow(n, 3) * p0.x + 3 * pow(n, 2) * t * p1.x + 3 * n * pow(t, 2) * p2.x + pow(t, 3) * p3.x;
    final.y = pow(n, 3) * p0.y + 3 * pow(n, 2) * t * p1.y + 3 * n * pow(t, 2) * p2.y + pow(t, 3) * p3.y;

    return final;
}
