#include <math.h>
#include <raylib.h>

#define WIDTH 800
#define HEIGHT 800
#define TITLE "Double Pendulum"
#define FPS 480

// === GLOBAL VARIABLES ===
Vector2 origin;
float r1, r2, m1, m2, a1, a2;
float a1_v, a2_v, a1_a, a2_a;

float g = 2500;

RenderTexture2D target;
Vector2 last_point;
int frameCount;

void init(void) {
    origin = (Vector2){WIDTH / 2.0f, HEIGHT / 2.0f};

    r1 = r2 = 160;
    m1 = m2 = 16;
    a1 = M_PI / 2;
    a2 = M_PI / 2;
    a1_v = 0;
    a2_v = 0;
    a1_a = 0;
    a2_a = 0;

    target = LoadRenderTexture(WIDTH, HEIGHT);
    BeginTextureMode(target);
    ClearBackground(BLACK);
    EndTextureMode();
    frameCount = 0;
}

void clean(void) { UnloadRenderTexture(target); }

void update(float dt) {
    // update physics
    float num1 = -g * (2 * m1 + m2) * sinf(a1) - m2 * g * sinf(a1 - 2 * a2) - 2 * sinf(a1 - a2) * m2 * (a2_v * a2_v * r2 + a1_v * a1_v * r1 * cosf(a1 - a2));
    float den1 = r1 * (2 * m1 + m2 - m2 * cosf(2 * a1 - 2 * a2));

    float num2 = 2 * sinf(a1 - a2) * (a1_v * a1_v * r1 * (m1 + m2) + g * (m1 + m2) * cosf(a1) + a2_v * a2_v * r2 * m2 * cosf(a1 - a2));
    float den2 = r2 * (2 * m1 + m2 - m2 * cosf(2 * a1 - 2 * a2));

    a1_a = num1 / den1;
    a2_a = num2 / den2;

    a1_v += a1_a * dt;
    a2_v += a2_a * dt;
    a1 += a1_v * dt;
    a2 += a2_v * dt;

    // update positions
    Vector2 p1 = {r1 * sinf(a1), r1 * cosf(a1)};
    Vector2 p2 = {r2 * sinf(a2), r2 * cosf(a2)};
    p1.x += origin.x;
    p1.y += origin.y;

    p2.x += p1.x;
    p2.y += p1.y;

    // render
    BeginTextureMode(target);
    if (frameCount > 0) { DrawLineEx(last_point, p2, 1, (Color){255, 255, 255, 128}); }
    last_point = p2;
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(
        target.texture,
        (Rectangle){0, 0, WIDTH, -HEIGHT},
        (Rectangle){0, 0, WIDTH, HEIGHT},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    DrawCircleV(origin, 6.0f, WHITE);

    DrawLineEx(origin, p1, 3, WHITE);
    DrawLineEx(p1, p2, 3, WHITE);

    DrawCircleV(p1, m1, WHITE);
    DrawCircleV(p2, m2, WHITE);

    DrawFPS(10, 10);

    EndDrawing();
    frameCount++;
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    init();
    while (!WindowShouldClose()) { update(GetFrameTime()); }

    clean();

    CloseWindow();
    return 0;
}
