#include "world.h"

World* world_new_initialized(int terrain_width, int terrain_height)
{
    World* world = malloc(sizeof(World));
    assert(world != NULL);
    world->terrain = terrain_new(terrain_width, terrain_height);
    dyn_array_cell_alloc(&world->cells, 1024);
    return world;
}

void world_free(World* world)
{
    assert(world != NULL);
    assert(world->terrain != NULL);
    free(world->terrain);
    dyn_array_cell_free(&world->cells);
    free(world);
}

