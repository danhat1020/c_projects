#pragma once

#include <math.h>
#include <random.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef int32_t i32;
typedef uint32_t u32;

const i32 width = 1280;
const i32 height = 720;
const i32 fps = 60;

const i32 res = 2;
const i32 cols = width / res;
const i32 rows = height / res;
