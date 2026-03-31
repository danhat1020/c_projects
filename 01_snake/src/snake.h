#ifndef SNAKE_H
#define SNAKE_H

#include <raylib.h>

typedef struct {
  Vector2 *body;
  Vector2 size;
  unsigned char dir;
  unsigned int length;
} Snake;

void snake_append(Snake *s, Vector2 pos);

Snake *snake_create(int cols, int rows, int res);
void snake_destroy(Snake *s);

int snake_update(Snake *s, Vector2 *food, int width, int height);
void snake_render(const Snake *s);

#endif // SNAKE_H
