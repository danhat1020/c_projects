#pragma once
#include "defs.h"

typedef struct {
    f32 x, y, radius;
    i32 growing;
} Circle;

Circle create_circle(f32 x, f32 y);

void circle_render(Circle *circle);
void circle_grow(Circle *circle);
