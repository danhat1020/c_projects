#include "snake.h"
#include <raylib.h>
#include <stdlib.h>

void snake_append(Snake *s, Vector2 pos) {
  pos.x *= s->size.x, pos.y *= s->size.y;
  s->body[s->length++] = pos;
}

Snake *snake_create(int cols, int rows, int res) {
  Snake *s = malloc(sizeof(Snake));

  s->body = malloc(sizeof(Vector2) * cols * rows);
  s->size = (Vector2){res, res};
  s->dir = 4;
  s->length = 0;

  snake_append(s, (Vector2){cols / 2.0f, rows / 2.0f});

  return s;
}

void snake_destroy(Snake *s) {
  free(s->body);
  free(s);
}

int snake_update(Snake *s, Vector2 *food, int width, int height) {
  int return_val = 0;
  Vector2 *head = &s->body[0];

  for (unsigned int i = s->length - 1; i >= 1; i--) {
    s->body[i] = s->body[i - 1];
  }

  switch (s->dir) {
    case 0: head->x += s->size.x; break;
    case 1: head->y += s->size.y; break;
    case 2: head->x -= s->size.x; break;
    case 3: head->y -= s->size.y; break;
  }

  if (head->x == food->x && head->y == food->y) {
    snake_append(s, s->body[s->length - 1]);
    return_val = 1;
  }

  if (head->x < 0 || head->x >= width || head->y < 0 || head->y >= height) {
    return_val = -1;
  }

  for (unsigned int i = 1; i < s->length; i++) {
    Vector2 *cell = &s->body[i];
    if (head->x == cell->x && head->y == cell->y) {
      return_val = -1;
    }
  }

  return return_val;
}

void snake_render(const Snake *s) {
  for (unsigned int i = 0; i < s->length; i++) {
    DrawRectangleV(s->body[i], s->size, GREEN);
  }
}
