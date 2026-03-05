#include "defs.h"

#include "grid.c"

int main(void) {
    InitWindow(width, height, "Conway's Game of Life");
    SetTargetFPS(fps);

    Grid *grid = grid_create(cols, rows);
    grid_set(grid);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            grid_set(grid);
        }

        grid_update(grid);

        BeginDrawing();
        ClearBackground((Color){30, 0, 45, 255});

        grid_render(grid);

        EndDrawing();
    }

    grid_destroy(grid);

    CloseWindow();
    return 0;
}
