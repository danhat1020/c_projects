#include "perlin.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

static int perm[512];
static int PERLIN_INITIALIZED = 0;

static float fade(float t) { return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); }
static float lerp(float a, float b, float t) { return a + t * (b - a); }

static void perlin_init(unsigned int seed) {
    if (!PERLIN_INITIALIZED) {
        // fill with 0..255
        for (int i = 0; i < 256; i++) {
            perm[i] = i;
        }

        // fisher-yates shuffle using seed
        srand(seed);
        for (int i = 255; i >= 0; i--) {
            int j = rand() % (i + 1);
            int tmp = perm[i];
            perm[i] = perm[j];
            perm[j] = tmp;
        }

        // double it to avoid needing modulo on every lookup
        for (int i = 0; i < 256; i++) {
            perm[256 + i] = perm[i];
        }

        PERLIN_INITIALIZED = 1;
    }
}

// 1d gradient : maps a hashed integer to a gradient of -1 or +1
static float grad1(int hash, float x) { return (hash & 1) ? x : -x; }

// 1d perlin
float perlin_1d(float x) {
    perlin_init(time(NULL));
    // step 1: find the integer cell and local offset
    int xi = (int)floor(x) & 255;
    float xf = x - floor(x);

    // step 2: smooth the local offset
    float u = fade(xf);

    // step 3: hash the two surrounding integers to get gradients
    int a = perm[xi];     // hash of left point
    int b = perm[xi + 1]; // hash of right point

    // step 4: dot gradient with offset, then interpolate
    // left point: offset from left = xf
    // right point: offset from right = xf - 1
    return lerp(grad1(a, xf), grad1(b, xf - 1.0), u);
}

// 2d gradient : maps a hash to one of 8 evenly-spaced unit gradient vectors
//               using the classic (-/+1, -/+1) set is fast and avoids trig
static float grad2(int hash, float x, float y) {
    // clang-format off
    switch (hash & 7) { // possible gradients
        case 0: return x + y;  // ( 1,  1)
        case 1: return -x + y; // (-1,  1)
        case 2: return x - y;  // ( 1, -1)
        case 3: return -x - y; // (-1, -1)
        case 4: return x;      // ( 1,  0)
        case 5: return -x;     // (-1,  0)
        case 6: return y;      // ( 0,  1)
        case 7: return -y;     // ( 0, -1)
        default: return 0;
    }
    // clang-format on
}

// 2d perlin
float perlin_2d_natural(float x, float y) {
    perlin_init(time(NULL));
    // step 1: integer cell coordinates and fractional offsets
    int xi = (int)floor(x) & 255;
    int yi = (int)floor(y) & 255;
    float xf = x - floor(x);
    float yf = y - floor(y);

    // step 2: smooth the offsets
    float u = fade(xf);
    float v = fade(yf);

    // step 3: hash all 4 corners of the unit square
    int aa = perm[perm[xi] + yi];
    int ab = perm[perm[xi] + yi + 1];
    int ba = perm[perm[xi + 1] + yi];
    int bb = perm[perm[xi + 1] + yi + 1];

    // step 4: dot each corner's gradient with the offset TO that corner
    float dot_aa = grad2(aa, xf, yf);             // bottom left
    float dot_ab = grad2(ab, xf, yf - 1.0);       // top    left
    float dot_ba = grad2(ba, xf - 1.0, yf);       // bottom right
    float dot_bb = grad2(bb, xf - 1.0, yf - 1.0); // top    right

    // step 5: bilinear interpolation
    // first interpolate along bottom edge, and then top, then between them
    float top = lerp(dot_aa, dot_ba, u);
    float bottom = lerp(dot_ab, dot_bb, u);
    return lerp(top, bottom, v);
}

float perlin_2d(float x, float y) {
    int octaves = 4;
    float persistence = 0.4;
    float lacunarity = 2.0;
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float max_value = 0.0;

    for (int i = 0; i < octaves; i++) {
        value += perlin_2d_natural(x * frequency, y * frequency) * amplitude;
        max_value += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value / max_value; // norm to -1, 1
}

// octaves     - how many layers                  (more = more detail)
// persistence - how much each octave contributes (typically 0.5)
// lacunarity  - frequency multiplier per octave  (typically 2.0)
float perlin_2d_control(float x, float y, int octaves, float persistence, float lacunarity) {
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float max_value = 0.0;

    for (int i = 0; i < octaves; i++) {
        value += perlin_2d_natural(x * frequency, y * frequency) * amplitude;
        max_value += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value / max_value; // norm to -1, 1
}
