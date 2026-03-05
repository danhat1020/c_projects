#include "snake.h"
#include "food.h"
#include <raylib.h>

void snake_push(Snake *snake, vec2f pos) {
    snake->body[snake->length++] = pos;
}

Snake *snake_create() {
    Snake *snake = malloc(sizeof(Snake));

    snake->body = malloc(sizeof(vec2f) * cols * rows);
    snake->length = 0;
    snake->dir = -1;

    snake_push(snake, (vec2f){cols / 2.0f, rows / 2.0f});

    return snake;
}

void snake_destroy(Snake *snake) {
    free(snake);
}

void snake_reset(Snake *snake) {
    snake->body[0] = (vec2f){cols / 2.0f, rows / 2.0f};
    snake->length = 1;
    snake->dir = -1;
}

void snake_render(Snake *snake) {
    for (i32 i = 0; i < snake->length - 1; i++) {
        vec2f *a = &snake->body[i];
        vec2f *b = &snake->body[i + 1];
        i32 diff = (b->x - a->x) + (b->y - a->y) * 2;

        u16 ax = a->x * res;
        u16 ay = a->y * res;

        // clang-format off
        switch(diff) {
            case  1: { DrawRectangle(ax + 1, ay + 1, res, res - 2, GREEN); } break; // right
            case -1: { DrawRectangle(ax - 1, ay + 1, res, res - 2, GREEN); } break; // left
            case  2: { DrawRectangle(ax + 1, ay + 1, res - 2, res, GREEN); } break; // down
            case -2: { DrawRectangle(ax + 1, ay - 1, res - 2, res, GREEN); } break; // up
        }
        // clang-format on
    }
    vec2f *tail = &snake->body[snake->length - 1];
    DrawRectangle(tail->x * res + 1, tail->y * res + 1, res - 2, res - 2, GREEN);
}

void snake_update(Snake *snake, vec2f *food) {
    vec2f *head = &snake->body[0];
    vec2f tail = snake->body[snake->length - 1];

    for (u32 i = snake->length - 1; i >= 1; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    // clang-format off
    switch (snake->dir) {
        case 0: { head->x++; } break;
        case 1: { head->y++;} break;
        case 2: { head->x--;} break;
        case 3: { head->y--;} break;
    }
    // clang-format on

    if (head->x == food->x && head->y == food->y) {
        food_rand(food);
        snake_push(snake, tail);
        return;
    }

    if (head->x < 0 || head->x >= cols || head->y < 0 || head->y >= rows) {
        food_rand(food);
        snake_reset(snake);
        return;
    }

    for (i16 i = 1; i < snake->length - 1; i++) {
        vec2f *cell = &snake->body[i];
        if (head->x == cell->x && head->y == cell->y) {
            food_rand(food);
            snake_reset(snake);
            return;
        }
    }
}
