#include <math.h>
#include <raylib.h>

#define WIDTH 1280
#define HEIGHT 720
#define TITLE "Pi from Colliding Blocks"
#define FPS 120
#define EXP 6.0

typedef struct {
    double x, y;
    double w, h;
    double vel;
    double mass;
} Block;

int main(void) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    Block a = {
        .x = WIDTH / 4.0,
        .y = HEIGHT / 2.0,
        .w = 50,
        .h = 50,
        .vel = 0.0,
        .mass = 1.0,
    };

    Block b = {
        .x = WIDTH / 2.0,
        .y = HEIGHT / 2.0,
        .w = 100,
        .h = 100,
        .vel = -1.0,
        .mass = pow(100.0, EXP),
    };

    int collision_count = 0;

    int steps = (int)round(powf(12.0, EXP));

    while (!WindowShouldClose()) {
        double sdt = 1.0 / steps;

        for (int s = 0; s < steps; s++) {
            a.x += a.vel * sdt;
            b.x += b.vel * sdt;

            if (b.x <= a.x + a.w && a.vel > b.vel) {
                // collide
                double ma = a.mass, mb = b.mass;
                double va = a.vel, vb = b.vel;
                a.vel = (va * (ma - mb) + 2.0 * mb * vb) / (ma + mb);
                b.vel = (vb * (mb - ma) + 2.0 * ma * va) / (ma + mb);

                collision_count++;
            }

            if (a.x <= 0.0 && a.vel < 0.0) {
                a.vel = -a.vel;
                collision_count++;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawLineEx((Vector2){0, HEIGHT / 2.0f}, (Vector2){WIDTH, HEIGHT / 2.0f}, 2, (Color){255, 255, 255, 80});

        Rectangle a_rec = {a.x, a.y - (a.h / 2.0f), a.w, a.h};
        DrawRectangleRec(a_rec, BLUE);
        DrawRectangleLinesEx(a_rec, 1, WHITE);
        DrawText(TextFormat("%.0f kg", a.mass), a.x, a_rec.y - 24, 20, WHITE);

        Rectangle b_rec = {b.x, b.y - (b.h / 2.0f), b.w, b.h};
        DrawRectangleRec(b_rec, BLUE);
        DrawRectangleLinesEx(b_rec, 1, WHITE);
        DrawText(TextFormat("%.0f kg", b.mass), b.x, b_rec.y - 24, 20, WHITE);

        DrawText(TextFormat("Collisions: %d", collision_count), 10, 10, 30, RAYWHITE);
        DrawFPS(10, HEIGHT - 24);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
