#pragma once
#include "cube.h"
#include <raylib.h>

const f32 angle_radians = (fov_angle / 180) * M_PI;

vec2 _point_convert_2d(const vec3 corner, i32 res) {
    vec3 final = corner;
    final.z += distance;

    f32 x = (width / 2.0f) * final.x / (final.z * tan(angle_radians / 2.0f)) + width / 2.0f;
    f32 y = (height / 2.0f) * final.y / (final.z * tan(angle_radians / 2.0f)) + height / 2.0f;

    return (vec2){x / res, y / res};
}

vec2 *_cube_convert_2d(vec3 *cube, i32 res) {
    vec2 *points = malloc(sizeof(vec2) * 8);

    points[0] = _point_convert_2d(cube[0], res);
    points[1] = _point_convert_2d(cube[1], res);
    points[2] = _point_convert_2d(cube[2], res);
    points[3] = _point_convert_2d(cube[3], res);
    points[4] = _point_convert_2d(cube[4], res);
    points[5] = _point_convert_2d(cube[5], res);
    points[6] = _point_convert_2d(cube[6], res);
    points[7] = _point_convert_2d(cube[7], res);

    return points;
}

f32 _face_calc_z(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *d) {
    f32 z_val = (a->z + b->z + c->z + d->z) / 4;
    return z_val;
}

void _cube_draw_face(const vec2 *a, const vec2 *b, const vec2 *c, const vec2 *d, f32 z, i32 color_steps) {
    Vector2 _a = {a->x, a->y};
    Vector2 _b = {b->x, b->y};
    Vector2 _c = {c->x, c->y};
    Vector2 _d = {d->x, d->y};

    i32 value = -z * color_steps;
    value *= (i32)(200 / color_steps);
    value += 40;
    Color col = {value, value, value, value};
    DrawTriangle(_a, _c, _b, col);
    DrawTriangle(_a, _d, _c, col);
}

void _cube_render_side(vec3 *cube, i32 side, i32 color_steps, i32 res) {
    vec2 *points = _cube_convert_2d(cube, res);

    f32 z_val = 0;
    switch (side) {
        case 0: // left
            z_val = _face_calc_z(&cube[4], &cube[0], &cube[2], &cube[6]);
            _cube_draw_face(&points[4], &points[0], &points[2], &points[6], z_val, color_steps);
            break;
        case 1: // right
            z_val = _face_calc_z(&cube[1], &cube[5], &cube[7], &cube[3]);
            _cube_draw_face(&points[1], &points[5], &points[7], &points[3], z_val, color_steps);
            break;
        case 2: // top
            z_val = _face_calc_z(&cube[4], &cube[5], &cube[1], &cube[0]);
            _cube_draw_face(&points[4], &points[5], &points[1], &points[0], z_val, color_steps);
            break;
        case 3: // bottom
            z_val = _face_calc_z(&cube[2], &cube[3], &cube[7], &cube[6]);
            _cube_draw_face(&points[2], &points[3], &points[7], &points[6], z_val, color_steps);
            break;
        case 4: // front
            z_val = _face_calc_z(&cube[0], &cube[1], &cube[3], &cube[2]);
            _cube_draw_face(&points[0], &points[1], &points[3], &points[2], z_val, color_steps);
            break;
        case 5: // back
            z_val = _face_calc_z(&cube[5], &cube[4], &cube[6], &cube[7]);
            _cube_draw_face(&points[5], &points[4], &points[6], &points[7], z_val, color_steps);
            break;
    }

    free(points);
}

// [ ===- MAIN FUNCTIONS -=== ]
vec3 *cube_create() {
    vec3 *cube = malloc(sizeof(vec3) * 8);

    cube[0] = (vec3){-1.0f, -1.0f, -1.0f}; //  LEFT,    TOP, FRONT
    cube[1] = (vec3){1.0f, -1.0f, -1.0f};  // RIGHT,    TOP, FRONT
    cube[2] = (vec3){-1.0f, 1.0f, -1.0f};  //  LEFT, BOTTOM, FRONT
    cube[3] = (vec3){1.0f, 1.0f, -1.0f};   // RIGHT, BOTTOM, FRONT
    cube[4] = (vec3){-1.0f, -1.0f, 1.0f};  //  LEFT,    TOP,  BACK
    cube[5] = (vec3){1.0f, -1.0f, 1.0f};   // RIGHT,    TOP,  BACK
    cube[6] = (vec3){-1.0f, 1.0f, 1.0f};   //  LEFT, BOTTOM,  BACK
    cube[7] = (vec3){1.0f, 1.0f, 1.0f};    // RIGHT, BOTTOM,  BACK

    return cube;
}

void cube_render(vec3 *cube, i32 color_steps, i32 res) {
    Face *faces = malloc(sizeof(Face) * 6);

    vec3 *face_verts[6][4] = {
        {&cube[4], &cube[0], &cube[2], &cube[6]}, // left
        {&cube[1], &cube[5], &cube[7], &cube[3]}, // right
        {&cube[4], &cube[5], &cube[1], &cube[0]}, // top
        {&cube[2], &cube[3], &cube[7], &cube[6]}, // bottom
        {&cube[0], &cube[1], &cube[3], &cube[2]}, // front
        {&cube[5], &cube[4], &cube[6], &cube[7]}, // back
    };

    for (u32 i = 0; i < 6; i++) {
        faces[i].index = i;
        faces[i].z = _face_calc_z(
            face_verts[i][0],
            face_verts[i][1],
            face_verts[i][2],
            face_verts[i][3]);
    }

    for (u32 i = 0; i < 5; i++) {
        for (u32 j = i + 1; j < 6; j++) {
            if (faces[j].z > faces[i].z) {
                Face tmp = faces[i];
                faces[i] = faces[j];
                faces[j] = tmp;
            }
        }
    }

    for (u32 i = 3; i < 6; i++) {
        _cube_render_side(cube, faces[i].index, color_steps, res);
    }
}

void cube_rotate(vec3 *cube, f32 x, f32 y, f32 z) {
    x = (x / 180) * M_PI;
    y = (y / 180) * M_PI;
    z = (z / 180) * M_PI;

    f32 cx = cosf(x), sx = sinf(x);
    f32 cy = cosf(y), sy = sinf(y);
    f32 cz = cosf(z), sz = sinf(z);

    f32 r[3][3] = {
        {cy * cz, cy * sz, -sy},
        {sx * sy * cz - cx * sz, sx * sy * sz + cx * cz, sx * cy},
        {cx * sy * cz + sx * sz, cx * sy * sz - sx * cz, cx * cy},
    };

    for (u32 i = 0; i < 8; i++) {
        vec3 *p = &cube[i];
        cube[i] = (vec3){
            r[0][0] * p->x + r[0][1] * p->y + r[0][2] * p->z,
            r[1][0] * p->x + r[1][1] * p->y + r[1][2] * p->z,
            r[2][0] * p->x + r[2][1] * p->y + r[2][2] * p->z,
        };
    }
}

void cube_destroy(vec3 *cube) { free(cube); }
