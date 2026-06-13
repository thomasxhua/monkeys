#ifndef WORLD_H
#define WORLD_H

#include "terrain.h"
#include "cell.h"

typedef struct
{
    Terrain* terrain;
    DynArrayCell cells;
} World;

World* world_new_initialized(int terrain_width, int terrain_height);
void world_free(World* world);

#endif // WORLD_H
