#include <math.h>
#include <random.h>
#include <raylib.h>

#include "segment.h"

#define WIDTH 800
#define HEIGHT 800
#define SCALE 5 // hi-res multiplier (800 * 5 = 4000)
#define HI_WIDTH (WIDTH * SCALE)
#define HI_HEIGHT (HEIGHT * SCALE)

#define TITLE "Forward Kinematics"
#define FPS 480            // simulation rate (used for fixed dt)
#define FPS2 60            // display rate after drawing finishes
#define STEPS_PER_FRAME 32 // sim steps per display frame — tune this up/down

#define NUM_SEGS 10

#define POINTS_SIZE (1024 * 64)

static Segment *segments[NUM_SEGS];
static float angles[NUM_SEGS];

static RenderTexture2D pattern_tex; // 4000x4000 hi-res canvas
static Vector2 last_point;          // only need the previous tip, not the full history
static int p_index;                 // counts frames drawn (used for progress %)
static int drawing;
static int saved;

static void init(void) {
    float length = 160.0f;
    float ratio = 1.618f;
    float thickness = NUM_SEGS;

    segments[0] = seg_create_root(WIDTH / 2.0f, HEIGHT / 2.0f, length, rng_range(0, 2 * M_PI), thickness);
    angles[0] = rng_range(3, 7);

    for (int i = 1; i < NUM_SEGS; i++) {
        length /= ratio;
        thickness--;
        segments[i] = seg_create(segments[i - 1], length, rng_range(0, 2 * M_PI), thickness);
        angles[i] = rng_range(3, 7);
    }

    // Hi-res render texture for the pattern
    pattern_tex = LoadRenderTexture(HI_WIDTH, HI_HEIGHT);
    BeginTextureMode(pattern_tex);
    ClearBackground(BLACK);
    EndTextureMode();

    last_point = segments[NUM_SEGS - 1]->b;
    p_index = 0;
    drawing = 1;
    saved = 0;
}

static void clean(void) {
    for (int i = NUM_SEGS - 1; i >= 0; i--) {
        seg_destroy(segments[i]);
    }
    UnloadRenderTexture(pattern_tex);
}

static void save_pattern(void) {
    Image img = LoadImageFromTexture(pattern_tex.texture);
    ImageFlipVertical(&img);
    ExportImage(img, "pattern.png");
    UnloadImage(img);
    saved = 1;
}

static void update(void) {
    if (IsKeyPressed(KEY_SPACE)) {
        drawing = 0;
    }

    if (drawing && p_index < POINTS_SIZE) {

        const float fixed_dt = 1.0f / (float)FPS;

        BeginTextureMode(pattern_tex);
        for (int step = 0; step < STEPS_PER_FRAME && p_index < POINTS_SIZE; step++) {

            float accumulated = 0;
            for (int i = 0; i < NUM_SEGS; i++) {
                accumulated += angles[i];
                seg_update(segments[i], accumulated, fixed_dt);
                if (i + 1 < NUM_SEGS) {
                    segments[i + 1]->a = segments[i]->b;
                }
            }

            Vector2 tip = segments[NUM_SEGS - 1]->b;

            if (p_index > 0) {
                Vector2 hi_prev = {last_point.x * SCALE, last_point.y * SCALE};
                Vector2 hi_curr = {tip.x * SCALE, tip.y * SCALE};
                DrawLineEx(hi_prev, hi_curr, 2.0f * SCALE, (Color){255, 255, 255, 128});
            }

            last_point = tip;
            p_index++;
        }
        EndTextureMode();

    } else if (!saved) {
        save_pattern();
        SetTargetFPS(FPS2);
    }

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(
        pattern_tex.texture,
        (Rectangle){0, 0, (float)HI_WIDTH, -(float)HI_HEIGHT},
        (Rectangle){0, 0, (float)WIDTH, (float)HEIGHT},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    if (drawing && p_index < POINTS_SIZE) {
        for (int i = 0; i < NUM_SEGS; i++) {
            seg_render(segments[i]);
        }
    }

    DrawFPS(10, 10);
    DrawText(TextFormat("%.2f%%", (float)p_index / POINTS_SIZE * 100.0f), 10, 35, 20, WHITE);
    if (saved) {
        DrawText("Saved: pattern.png", 10, 60, 20, GREEN);
    }

    EndDrawing();
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
