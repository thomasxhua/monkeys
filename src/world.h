#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>

#include "terrain.h"
#include "monkey.h"

typedef struct
{
    Terrain* terrain;
    DynArrayMonkey monkeys;
    uint64_t monkey_next_id;
} World;

World* world_new_initialized(int terrain_width, int terrain_height);
void world_free(World* world);

void world_tick(
      World* world
    , int terrain_width
    , int terrain_height
    , uint64_t* count_death
    , uint64_t* count_newborn
    , DNA* avg_dna);
Monkey* world_monkeys_spawn_newborn(World* world);

#endif // WORLD_H
