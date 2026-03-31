#pragma once

float perlin_1d(float x);
float perlin_2d(float x, float y);
float perlin_2d_control(float x, float y, int octaves, float persistence, float lacunarity);
