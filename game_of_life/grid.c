#include "grid.h"
#include <raylib.h>

i32 _cell_count_neighbours(Grid *grid, i32 i, i32 j) {
    i32 sum = 0;

    for (i32 x = -1; x < 2; x++) {
        for (i32 y = -1; y < 2; y++) {
            if (x == 0 && y == 0) {
                continue;
            }
            i32 a = i + x, b = j + y;
            if (a >= 0 && a < (i32)grid->cols && b >= 0 && b < (i32)grid->rows) {
                sum += grid->main[a + b * grid->cols];
            }
        }
    }

    return sum;
}

Grid *grid_create(i32 cols, i32 rows) {
    Grid *grid = malloc(sizeof(Grid));
    grid->cols = cols;
    grid->rows = rows;

    grid->main = malloc(sizeof(i32) * cols * rows);
    grid->temp = malloc(sizeof(i32) * cols * rows);

    return grid;
}

void grid_set(Grid *grid) {
    for (u32 j = 0; j < grid->rows; j++) {
        for (u32 i = 0; i < grid->cols; i++) {
            grid->main[i + j * grid->cols] = floor(rng_range(0, 2));
            grid->temp[i + j * grid->cols] = floor(rng_range(0, 2));
        }
    }
}

void grid_update(Grid *grid) {
    memcpy(grid->temp, grid->main, grid->rows * grid->cols * sizeof(u32));

    for (u32 j = 0; j < grid->rows; j++) {
        for (u32 i = 0; i < grid->cols; i++) {
            const u32 cell = grid->main[i + j * grid->cols];
            i32 neighbours = _cell_count_neighbours(grid, i, j);

            if (cell == 0 && neighbours == 0) {
                continue;
            }

            if (cell == 1) {
                if (neighbours < 2 || neighbours > 3) {
                    grid->temp[i + j * grid->cols] = 0;
                }
            } else {
                if (neighbours == 3) {
                    grid->temp[i + j * grid->cols] = 1;
                }
            }
        }
    }

    memcpy(grid->main, grid->temp, grid->rows * grid->cols * sizeof(u32));
}

void grid_render(Grid *grid) {
    for (u32 j = 0; j < grid->rows; j++) {
        for (u32 i = 0; i < grid->cols; i++) {
            if (grid->main[i + j * grid->cols] == 1) {
                DrawRectangle(i * res, j * res, res, res, (Color){210, 100, 255, 255});
            }
        }
    }
}

void grid_destroy(Grid *grid) {
    free(grid->main);
    free(grid->temp);
    free(grid);
}
