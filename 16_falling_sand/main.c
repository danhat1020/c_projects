#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600
#define TITLE "TITLE"
#define FPS 120

#define GRAV 0.1f
#define MAX_VEL 10.0f

// === GLOBAL VARIABLES ===
typedef enum {
    EMPTY,
    SAND,
} CellType;

CellType *grid;
int *updated;
float *vel;
Color *colors;

int in_bounds(int x, int y);
void update_sand(int x, int y);
void move_cell(int x1, int y1, int x2, int y2);
Color random_sand_col();

void init(void) {
    grid = calloc(WIDTH * HEIGHT, sizeof(CellType));
    updated = calloc(WIDTH * HEIGHT, sizeof(int));
    vel = calloc(WIDTH * HEIGHT, sizeof(float));
    colors = calloc(WIDTH * HEIGHT, sizeof(Color));

    // init grid
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[x + y * WIDTH] = EMPTY;
            updated[x + y * WIDTH] = 0;
        }
    }

    updated[0] = 1;
}

void clean(void) { free(grid), free(updated), free(vel), free(colors); }

void update(void) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        int mx = (int)mouse.x;
        int my = (int)mouse.y;
        int r = 2;
        for (int x = -r; x <= r; x++) {
            for (int y = -r; y <= r; y++) {
                int idx = (mx + x) + (my + y) * WIDTH;
                if (in_bounds(mx + x, my + y) && grid[idx] == EMPTY) {
                    grid[idx] = SAND;
                    colors[idx] = random_sand_col();
                }
            }
        }
    }

    // clear update flags
    memset(updated, 0, sizeof(int) * WIDTH * HEIGHT);
    BeginDrawing();
    ClearBackground(BLACK);
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            if (grid[x + y * WIDTH] == SAND) {
                if (!updated[x + y * WIDTH]) { update_sand(x, y); }
                DrawPixel(x, y, colors[x + y * WIDTH]);
            }
        }
    }

    DrawFPS(10, 10);
    EndDrawing();
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    init();
    while (!WindowShouldClose()) { update(); }

    clean();

    CloseWindow();
    return 0;
}

int in_bounds(int x, int y) { return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT; }

void update_sand(int x, int y) {
    int idx = x + y * WIDTH;

    vel[idx] += GRAV;
    if (vel[idx] > MAX_VEL) { vel[idx] = MAX_VEL; }

    int steps = (int)vel[idx];
    int cx = x, cy = y;

    for (int i = 0; i < steps; i++) {
        int downX = cx;
        int downY = cy + 1;

        if (in_bounds(downX, downY) && grid[downX + downY * WIDTH] == EMPTY) {
            cx = downX, cy = downY;
            continue;
        }

        // try diagonals
        int firstDir = rng() < 0.5f ? 1 : -1;
        int secondDir = -firstDir;

        if (in_bounds(cx + firstDir, cy + 1) && grid[(cx + firstDir) + (cy + 1) * WIDTH] == EMPTY) {
            cx = cx + firstDir;
            cy = cy + 1;
        } else if (in_bounds(cx + secondDir, cy + 1) && grid[(cx + secondDir) + (cy + 1) * WIDTH] == EMPTY) {
            cx = cx + secondDir;
            cy = cy + 1;
        } else {
            vel[idx] = 0; // hit something, reset
            break;
        }
    }

    if (cx != x || cy != y) { move_cell(x, y, cx, cy); }
}

void move_cell(int x1, int y1, int x2, int y2) {
    grid[x2 + y2 * WIDTH] = grid[x1 + y1 * WIDTH];
    grid[x1 + y1 * WIDTH] = EMPTY;

    vel[x2 + y2 * WIDTH] = vel[x1 + y1 * WIDTH];
    vel[x1 + y1 * WIDTH] = 0.0f;

    colors[x2 + y2 * WIDTH] = colors[x1 + y1 * WIDTH];

    updated[x2 + y2 * WIDTH] = 1;
}

Color random_sand_col() {
    int brightness = floor(rng_range(180, 241));
    int r = brightness;
    int g = floor(brightness * 0.85f);
    int b = floor(brightness * 0.5f);
    return (Color){r, g, b, 255};
}
