#pragma once
#include "defs.h"

typedef struct {
    vec2f *body;
    u16 length;
    i16 dir;
} Snake;

void snake_push(Snake *snake, vec2f pos);
Snake *snake_create();
void snake_destroy(Snake *snake);
void snake_reset(Snake *snake);

void snake_render(Snake *snake);
void snake_update(Snake *snake, vec2f *food);
