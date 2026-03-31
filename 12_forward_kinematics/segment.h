#pragma once
#include <raylib.h>

typedef struct Segment {
    Vector2 a, b;
    float length;
    float angle;
    struct Segment *child;
    float thickness;
} Segment;

Segment *seg_create_root(float x, float y, float length, float angle, float thick);
Segment *seg_create(Segment *parent, float length, float angle, float thick);
void seg_destroy(Segment *seg);

void seg_render(Segment *seg);

void seg_update(Segment *seg, float delta_angle, float dt);
