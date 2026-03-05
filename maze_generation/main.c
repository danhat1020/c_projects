#include "defs.h"
#include <raylib.h>

#include "cell.h"

// === GLOBAL VARIABLES ===
Cell *grid;
Cell *current;

Cell *stack[GRID_SIZE];
i32 stack_idx;

void stack_push(Cell *cell);
Cell *stack_pop();

void init(void) {
    grid = grid_create();

    current = &grid[0];
    current->visited = 1;

    stack_idx = 0;
}

void clean(void) {
    grid_destroy(grid);
}

void update(void) {
    Cell *next = cell_check_neighbours(current, grid);
    if (next != current) {
        next->visited = 1;

        stack_push(current);

        cell_remove_walls(current, next);

        current = next;
    } else if (stack_idx > 0) {
        current = stack_pop();
    }
}

void render(void) {
    grid_render(grid);

    // highlight current
    if (stack_idx != 0) {
        i32 x = current->i * RES;
        i32 y = current->j * RES;
        DrawRectangle(x, y, RES, RES, (Color){255, 255, 255, 100});
    }
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Maze Generation");
    SetTargetFPS(FPS);

    init();

    while (!WindowShouldClose()) {
        update();
        BeginDrawing();
        ClearBackground(BLACK);
        render();
        EndDrawing();
    }

    clean();

    CloseWindow();
    return 0;
}

void stack_push(Cell *cell) {
    stack[stack_idx++] = cell;
}

Cell *stack_pop() {
    return stack[--stack_idx];
}
