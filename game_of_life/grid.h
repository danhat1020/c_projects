#pragma once
#include "defs.h"

typedef struct {
    u32 cols, rows;
    u32 *main;
    u32 *temp;
} Grid;

Grid *grid_create(i32 cols, i32 rows);
void grid_set(Grid *grid);
void grid_update(Grid *grid);
void grid_render(Grid *grid);
void grid_destroy(Grid *grid);
