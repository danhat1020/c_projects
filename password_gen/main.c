#include <math.h>
#include <random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *allowed_letter = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const char *allowed_special = "!?@#$\"'^()[]%";
char *password;

int main(void) {
    const int letter_count = strlen(allowed_letter);
    const int special_count = strlen(allowed_special);

    int max_characters = 20;
    int max_special = 4;
    password = malloc((max_characters + 1) * sizeof(char));

    for (int i = 0; i < max_characters - max_special; i++) {
        int char_idx = floor(rng_range(0, letter_count));
        password[i] = allowed_letter[char_idx];
    }
    for (int i = 0; i < max_special; i++) {
        int char_idx = floor(rng_range(0, special_count));
        password[i + max_characters - max_special] = allowed_special[char_idx];
    }
    password[max_characters] = '\0';

    printf("Your password is: %s\n", password);

    free(password);
}
