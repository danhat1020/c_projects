#pragma once
#include "defs.h"

typedef struct {
    Vector2 pos;
    Vector2 vel;
    f32 mass;
    f32 radius;
} Ball;

Ball *balls_create(void);
void balls_destroy(Ball *balls);

void ball_render(const Ball *ball);
void ball_update(Ball *ball);
void ball_resolve_collision(Ball *a, Ball *b);
