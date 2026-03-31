#include "segment.h"
#include <math.h>
#include <raylib.h>
#include <stdlib.h>

static void _seg_calc_b(Segment *seg) {
    seg->b.x = seg->a.x + seg->length * cosf(seg->angle);
    seg->b.y = seg->a.y + seg->length * sinf(seg->angle);
}

Segment *seg_create_root(float x, float y, float length, float angle, float thick) {
    Segment *seg = malloc(sizeof(Segment));

    seg->a = (Vector2){x, y};
    seg->length = length;
    seg->angle = angle;
    seg->thickness = thick;
    seg->child = NULL;

    _seg_calc_b(seg);
    return seg;
}

Segment *seg_create(Segment *parent, float length, float angle, float thick) {
    Segment *seg = malloc(sizeof(Segment));

    seg->a = parent->b;
    seg->length = length;
    seg->angle = parent->angle + angle;
    seg->thickness = thick;
    seg->child = NULL;
    parent->child = seg;

    _seg_calc_b(seg);
    return seg;
}

void seg_destroy(Segment *seg) {
    free(seg);
}

void seg_render(Segment *seg) {
    DrawLineEx(seg->a, seg->b, seg->thickness, (Color){255, 255, 255, 180});
}

void seg_update(Segment *seg, float delta_angle, float dt) {
    seg->angle += delta_angle * dt / 5.0f;
    seg->b.x = seg->a.x + seg->length * cosf(seg->angle);
    seg->b.y = seg->a.y + seg->length * sinf(seg->angle);
}
