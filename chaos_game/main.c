#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH 1280
#define HEIGHT 720
#define TITLE "Chaos Game"
#define FPS 240

// === GLOBAL VARIABLES ===
typedef struct {
    RenderTexture2D target;
    Rectangle source, dest;
    Vector2 origin;
    float rotation;
    Color tint;
} Tex;

Vector2 *points;
Vector2 current;
Tex rtex;

void init(void) {
    points = calloc(3, sizeof(Vector2));
    points[0] = (Vector2){0, HEIGHT};
    points[1] = (Vector2){WIDTH / 2.0f, 0};
    points[2] = (Vector2){WIDTH, HEIGHT};

    int r = floor(rng_range(0, 3));
    current = points[r];

    rtex = (Tex){
        .target = LoadRenderTexture(WIDTH, HEIGHT),
        .source = {0, 0, WIDTH, -HEIGHT},
        .dest = {0, 0, WIDTH, HEIGHT},
        .origin = {0, 0},
        .rotation = 0.0f,
        .tint = WHITE,
    };
}

void clean(void) {
    free(points);
    UnloadRenderTexture(rtex.target);
}

void update(void) {
    BeginTextureMode(rtex.target);
    for (int i = 0; i < 100; i++) {
        int r = floor(rng_range(0, 3));
        Vector2 next = points[r];

        next.x = (next.x + current.x) / 2;
        next.y = (next.y + current.y) / 2;

        current = next;

        int min = 40;
        float ratio = (float)next.x / (float)WIDTH;
        int red = (255 - min) * (1 - ratio) + min;
        int blue = (255 - min) * ratio + min;

        DrawCircle(current.x, current.y, 0.71f, (Color){red, min, blue, 200});
    }
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(
        rtex.target.texture,
        rtex.source,
        rtex.dest,
        rtex.origin,
        rtex.rotation,
        rtex.tint);
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
