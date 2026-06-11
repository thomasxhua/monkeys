#include "terrain.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "noise.h"

Terrain* terrain_new(int width, int height)
{
    return calloc(width*height, sizeof(Terrain));
}

void terrain_perlin(Terrain* terrain, int width, int height, float water_level, int seed)
{
    assert(terrain);
    for (int i=0; i<height; ++i)
    {
        for (int j=0; j<width; ++j)
        {
            const float val = noise_perlin(j, i, seed);
            terrain[i*width + j] = (val >= water_level)
                ? TERRAIN_EARTH
                : TERRAIN_WATER;
        }
    }
}

