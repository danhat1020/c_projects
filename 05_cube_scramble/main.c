#include "move.h"

int main(void) {
    Move *move_str = move_str_create();

    move_str_add_moves(move_str);
    move_str_print(move_str);
    move_str_destroy(move_str);

    return 0;
}
