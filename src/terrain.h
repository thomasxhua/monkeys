#ifndef TERRAIN_H
#define TERRAIN_H

typedef enum
{
    TERRAIN_NULL = 0,
    TERRAIN_EARTH,
    TERRAIN_WATER,
} Terrain;

Terrain* terrain_new(int width, int height);
void terrain_perlin(Terrain* terrain, int width, int height, float water_level, int seed);

#endif // TERRAIN_H
