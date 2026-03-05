#pragma once
#include "defs.h"

typedef struct {
    i32 dir;
    i32 addition;
    i32 axis;
} Move;

Move *move_str_create();
void move_str_destroy(Move *move_str);
void move_str_print(Move *move_str);
void move_str_add_moves(Move *move_str);
