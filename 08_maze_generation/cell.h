#pragma once
#include "defs.h"

typedef struct {
    i32 i, j;
    i32 visited;
    i32 walls[4];
} Cell;

// === GRID ===
Cell *grid_create();
void grid_destroy(Cell *grid);

void grid_render(Cell *grid);

// === CELL ===
void cell_render(Cell *cell);
Cell *cell_check_neighbours(Cell *cell, Cell *grid);
void cell_remove_walls(Cell *current, Cell *next);
