#ifndef TERRAIN_H
#define TERRAIN_H

#include <stdbool.h>

typedef enum
{
    TERRAIN_NULL = 0,
    TERRAIN_EARTH,
    TERRAIN_STONE,
    // from here only water
    TERRAIN_WATER,
    TERRAIN_WATER_SHALLOW,
    TERRAIN_WATER_DEEP,
} Terrain;

Terrain* terrain_new(int width, int height);

void terrain_perlin(Terrain* terrain, int width, int height, float water_level, float stone_level, int seed);

#endif // TERRAIN_H
