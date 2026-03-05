#pragma once

#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

const i32 width = 800;
const i32 height = 800;

const f32 distance = 4.2f;
const f32 fov_angle = 60.0f;

const i32 low_res = 2;

typedef struct {
    f32 x, y;
} vec2;

typedef struct {
    f32 x, y, z;
} vec3;
