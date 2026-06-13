#include "noise.h"

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// https://rigtorp.se/notes/hashing/
uint64_t hash64(uint64_t x)
{
    x ^= x >> 27;
    x *= 0x3C79AC492BA7B653UL;
    x ^= x >> 33;
    x *= 0x1C69B3F74AC4AE35UL;
    x ^= x >> 27;
    return x;
}

float hash64_angle(int x, int y, int seed)
{
    return (float)hash64(hash64(x*2027) ^ hash64(y*15101) ^ hash64(seed))/UINT64_MAX * (2.0f*PI);
}

float dot_product(float a, float b_x, float b_y)
{
    return b_x*cos(a) + b_y*sin(a);
}

float fade(float t)
{
    return 6.0f*t*t*t*t*t
        - 15.0f*t*t*t*t
        + 10.0f*t*t*t;
}

float lerp(float a, float b, float t)
{
    return a + t*(b-a);
}

float noise_perlin_unit(int x, int y, int seed, int lattice_unit)
{
    // top left lattice point
    const int lattice_x = x / lattice_unit;
    const int lattice_y = y / lattice_unit;
    // diff vector
    const int d_x0 = x - (lattice_x+0) * lattice_unit;
    const int d_y0 = y - (lattice_y+0) * lattice_unit;
    const int d_x1 = d_x0 - lattice_unit;
    const int d_y1 = d_y0 - lattice_unit;
    // random angles
    const float angle00 = hash64_angle(lattice_x+0, lattice_y+0, seed);
    const float angle10 = hash64_angle(lattice_x+1, lattice_y+0, seed);
    const float angle01 = hash64_angle(lattice_x+0, lattice_y+1, seed);
    const float angle11 = hash64_angle(lattice_x+1, lattice_y+1, seed);
    // contribution
    const float contr00 = dot_product(angle00, (float)d_x0/lattice_unit, (float)d_y0/lattice_unit);
    const float contr10 = dot_product(angle10, (float)d_x1/lattice_unit, (float)d_y0/lattice_unit);
    const float contr01 = dot_product(angle01, (float)d_x0/lattice_unit, (float)d_y1/lattice_unit);
    const float contr11 = dot_product(angle11, (float)d_x1/lattice_unit, (float)d_y1/lattice_unit);
    // determine 
    const float u = fade((float)d_x0 / lattice_unit);
    const float v = fade((float)d_y0 / lattice_unit);
    return lerp(
        lerp(contr00, contr10, u),
        lerp(contr01, contr11, u),
        v);
}

float noise_perlin(int x, int y, int seed)
{
    const float res =
          0.15 * noise_perlin_unit(x, y, seed, 800)
        + 0.30 * noise_perlin_unit(x, y, seed, 400)
        + 0.25 * noise_perlin_unit(x, y, seed, 200)
        + 0.15 * noise_perlin_unit(x, y, seed, 100)
        + 0.10 * noise_perlin_unit(x, y, seed, 50)
        + 0.05 * noise_perlin_unit(x, y, seed, 25);
    return 1.0 - (
        (res <= 0.0f)
            ? 0.0f
            : (res >= 1.0f)
                ? 1.0f
                : res);
}

