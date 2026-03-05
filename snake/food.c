#include "food.h"
#include <raylib.h>

void food_render(vec2f *food) {
    DrawRectangle(food->x * res + 1, food->y * res + 1, res - 2, res - 2, RED);
}

void food_rand(vec2f *food) {
    food->x = floor(rng_range(0, cols));
    food->y = floor(rng_range(0, rows));
}
