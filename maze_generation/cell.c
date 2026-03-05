#include "cell.h"
#include <raylib.h>

// === GRID ===
Cell *grid_create() {
    Cell *grid = malloc(sizeof(Cell) * GRID_SIZE);

    // clang-format off
    for (i32 j = 0; j < ROWS; j++) {
        for (i32 i = 0; i < COLS; i++) {
            i32 index = i + j * COLS;
            grid[index].i = i;
            grid[index].j = j;
            grid[index].visited = 0;
            for (i32 k = 0; k < 4; k++) { grid[index].walls[k] = 1; }
        }
    }
    // clang-format on

    return grid;
}

void grid_destroy(Cell *grid) { free(grid); }

void grid_render(Cell *grid) {
    for (i32 i = 0; i < GRID_SIZE; i++) {
        cell_render(&grid[i]);
    }
}

// === CELL ===
void cell_render(Cell *cell) {
    i32 x = cell->i * RES;
    i32 y = cell->j * RES;

    // clang-format off
    if (cell->walls[0]) { DrawLine(x, y, x + RES, y, WHITE); }             // top
    if (cell->walls[1]) { DrawLine(x + RES, y, x + RES, y + RES, WHITE); } // right
    if (cell->walls[2]) { DrawLine(x, y + RES, x + RES, y + RES, WHITE); } // bottom
    if (cell->walls[3]) { DrawLine(x, y, x, y + RES, WHITE); }             // left

    if (cell->visited) { DrawRectangle(x, y, RES, RES, (Color){255, 0, 255, 100}); }
    // clang-format on
}

i32 _index(i32 i, i32 j) {
    if (i < 0 || i >= COLS || j < 0 || j >= ROWS) {
        return -1;
    }
    return i + j * COLS;
}

Cell *cell_check_neighbours(Cell *cell, Cell *grid) {
    Cell *neighbours[4];

    i32 i = cell->i;
    i32 j = cell->j;

    i32 n_idx = 0;

    i32 idx_top = _index(i, j - 1);
    i32 idx_right = _index(i + 1, j);
    i32 idx_bottom = _index(i, j + 1);
    i32 idx_left = _index(i - 1, j);

    // clang-format off
    if (idx_top    >= 0 && !grid[idx_top].visited)    { neighbours[n_idx++] = &grid[idx_top];    }
    if (idx_right  >= 0 && !grid[idx_right].visited)  { neighbours[n_idx++] = &grid[idx_right];  }
    if (idx_bottom >= 0 && !grid[idx_bottom].visited) { neighbours[n_idx++] = &grid[idx_bottom]; }
    if (idx_left   >= 0 && !grid[idx_left].visited)   { neighbours[n_idx++] = &grid[idx_left];   }
    // clang-format on

    if (n_idx > 0) {
        i32 n = floor(rng_range(0, n_idx));
        return neighbours[n];
    }
    return cell;
}

void cell_remove_walls(Cell *a, Cell *b) {
    i32 i = a->i - b->i;
    i32 j = a->j - b->j;

    // clang-format off
    switch (i + 2 * j) {
        case  1: { a->walls[3] = 0; b->walls[1] = 0; } break;
        case -1: { a->walls[1] = 0; b->walls[3] = 0; } break;
        case  2: { a->walls[0] = 0; b->walls[2] = 0; } break;
        case -2: { a->walls[2] = 0; b->walls[0] = 0; } break;
    }
    // clang-format on
}
