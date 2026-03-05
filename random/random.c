#include "random.h"
#include <stdio.h>
#include <time.h>

static unsigned int xo_state[4];
static int initialized = 0;

static inline unsigned int rotl(unsigned int x, int k) {
    return (x << k) | (x >> (32 - k));
}

static unsigned int xoshiro128p(void) {
    unsigned int result = xo_state[0] + xo_state[3];
    unsigned int t = xo_state[1] << 9;

    xo_state[2] ^= xo_state[0];
    xo_state[3] ^= xo_state[1];
    xo_state[1] ^= xo_state[2];
    xo_state[0] ^= xo_state[3];
    xo_state[2] ^= t;
    xo_state[3] = rotl(xo_state[3], 11);

    return result;
}

static void seed_rng_with_seed(unsigned int seed) {
    for (int i = 0; i < 4; i++) {
        seed += 0x9e3779b9u;
        unsigned int z = seed;
        z = (z ^ (z >> 16)) * 0x85ebca6bu;
        z = (z ^ (z >> 13)) * 0xc2b2ae35u;
        xo_state[i] = z ^ (z >> 16);
    }
}

static void seed_rng(void) {
    unsigned int seed;
    int seeded = 0;

    FILE *f = fopen("/dev/urandom", "rb");
    if (f) {
        if (fread(&seed, sizeof(seed), 1, f)) {
            seeded = 1;
        }
        fclose(f);
    }

    if (!seeded) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        seed = (unsigned int)(ts.tv_sec ^ ts.tv_nsec);
    }
    seed_rng_with_seed(seed);
}

static void ensure_seeded(void) {
    if (initialized) {
        return;
    }
    seed_rng();
    initialized = 1;
}

float rng(void) {
    ensure_seeded();
    return (float)(xoshiro128p() >> 8) / (float)(1 << 24);
}

float rng_range(float a, float b) {
    ensure_seeded();
    float val = (float)(xoshiro128p() >> 8) / (float)(1 << 24);
    return a + val * (b - a);
}
