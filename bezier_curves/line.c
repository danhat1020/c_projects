#include "line.h"
#include <raylib.h>

void line_draw(line *l) {
    DrawLine(l->a.x, l->a.y, l->b.x, l->b.y, (Color){255, 255, 255, 128});
}

vec2f line_lerp(line *l, f32 ratio) {
    vec2f final = {l->b.x - l->a.x, l->b.y - l->a.y};

    final.x *= ratio;
    final.y *= ratio;

    final.x += l->a.x;
    final.y += l->a.y;

    return final;
}

void curve_draw(vec2f start, vec2f end, vec2f point) {
    f32 max = 1000.0f;
    line l1 = {start, point};
    line l2 = {point, end};
    for (u32 i = 0; i < max; i++) {
        // lerp both lines, lerp line between those, draw point
        vec2f l1_lerp = line_lerp(&l1, i / max);
        vec2f l2_lerp = line_lerp(&l2, i / max);
        line final = {l1_lerp, l2_lerp};
        vec2f final_lerp = line_lerp(&final, i / max);
        DrawCircle(final_lerp.x, final_lerp.y, 1, (Color){255, 255, 255, 200});
    }
}
