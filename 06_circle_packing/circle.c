#include "circle.h"

#include <raylib.h>

Circle create_circle(f32 x, f32 y) {
    Circle circle = {x, y, 0, 1};
    return circle;
}

void circle_render(Circle *circle) {
    if (circle->radius > 0) {
        DrawCircle(circle->x, circle->y, circle->radius, (Color){60, 60, 60, 255});
        DrawCircleLines(circle->x, circle->y, circle->radius, RAYWHITE);
    }
}

void _edges(Circle *circle) {
    i32 x = circle->x, y = circle->y, r = circle->radius;
    if (x - r < 0 || y - r < 0 || x + r > WIDTH || y + r > HEIGHT) { circle->growing = 0; }
}

void circle_grow(Circle *circle) {
    _edges(circle);
    if (circle->growing) { circle->radius++; }
}
