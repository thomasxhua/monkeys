#include "terrain.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "noise.h"

Terrain* terrain_new(int width, int height)
{
    return calloc(width*height, sizeof(Terrain));
}

void terrain_perlin(Terrain* terrain, int width, int height, float water_level, float stone_level, int seed)
{
    assert(terrain != NULL);
    for (int i=0; i<height; ++i)
    {
        for (int j=0; j<width; ++j)
        {
            // generate land and ocean
            float val = noise_perlin(j, i, seed);
            terrain[i*width + j] =
                (val >= water_level)
                    ? TERRAIN_EARTH
                    : (val >= water_level * 0.95)
                        ? TERRAIN_WATER_SHALLOW
                        : (val >= water_level * 0.9)
                            ? TERRAIN_WATER
                            : TERRAIN_WATER_DEEP;
            // generate stone
            val = noise_perlin(j, i, seed ^ 0x420420420);
            if (val < stone_level && terrain[i*width + j] == TERRAIN_EARTH)
                terrain[i*width + j] = TERRAIN_STONE;
        }
    }
}

