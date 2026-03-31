#include "move.h"
#include <random.h>

Move *move_str_create() {
    Move *move_str = malloc(sizeof(Move) * 20);
    return move_str;
}

void move_str_destroy(Move *move_str) {
    free(move_str);
}

void move_str_print(Move *move_str) {
    char *moves = "LRUDBF";
    char *additions = " '2";

    char *string = malloc(sizeof(char) * 61);
    if (!string) { return; }
    i32 index = 0;

    for (u32 i = 0; i < 20; i++) {
        Move curr = move_str[i];
        string[index++] = moves[curr.dir];
        string[index++] = additions[curr.addition];
        if (curr.addition) { string[index++] = ' '; }
    }
    string[index] = '\0';

    printf("%s\n", string);
    free(string);
}

void move_str_add_moves(Move *move_str) {
    for (i32 i = 0; i < 20; i++) {
        i32 dir = floor(rng_range(0, 6));
        i32 addition = floor(rng_range(0, 3));
        i32 axis = dir / 2.0f;
        if (i != 0) {
            dir = move_str[i - 1].dir;
            addition = floor(rng_range(0, 3));
            axis = dir / 2.0f;
            if (i - 2 >= 0) {
                do {
                    dir = floor(rng_range(0, 6));
                    axis = dir / 2.0f;
                } while (move_str[i - 1].axis == axis || move_str[i - 2].dir == dir || move_str[i - 1].dir == dir);
            } else if (i - 1 >= 0) {
                do {
                    dir = floor(rng_range(0, 6));
                    axis = dir / 2.0f;
                } while (move_str[i - 1].dir == dir);
            }
        }

        Move move = {
            .dir = dir,
            .addition = addition,
            .axis = axis,
        };

        move_str[i] = move;
    }
}
