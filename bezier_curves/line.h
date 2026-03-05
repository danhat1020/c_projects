#pragma once
#include "defs.h"

typedef struct {
    f32 x, y;
} vec2f;

typedef struct {
    vec2f a, b;
} line;

void line_draw(line *l);
vec2f line_lerp(line *l, f32 ratio);
void curve_draw(vec2f start, vec2f end, vec2f point);
