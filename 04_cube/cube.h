#pragma once
#include "defs.h"

typedef struct {
    i32 index;
    f32 z;
} Face;

vec3 *cube_create();
void cube_render(vec3 *cube, i32 color_steps, i32 res);
void cube_rotate(vec3 *cube, f32 x, f32 y, f32 z);
void cube_destroy(vec3 *cube);
