#include "world.h"

#include <math.h>
#include <memory.h>

#define MONKEYS_SOCIAL_DISTANCE 1
#define MONKEYS_BIRTH_RADIUS    1

World* world_new_initialized(int terrain_width, int terrain_height)
{
    World* world = malloc(sizeof(World));
    assert(world != NULL);
    world->terrain = terrain_new(terrain_width, terrain_height);
    dyn_array_monkey_alloc(&world->monkeys, 1024);
    world->monkey_next_id = 0;
    return world;
}

void world_free(World* world)
{
    assert(world != NULL);
    assert(world->terrain != NULL);
    free(world->terrain);
    dyn_array_monkey_free(&world->monkeys);
    free(world);
}

int sign(int x)
{
    return (x < 0) ? 1 : (x > 0) ? -1 : 0;
}

bool is_within(int a, int b, int err)
{
    return (a - b) <= err || (b - a) <= err;
}

void world_tick(
      World* world
    , int terrain_width
    , int terrain_height
    , uint64_t* count_death
    , uint64_t* count_newborn
    , DNA* dna_avg)
{
    assert(world != NULL);
    assert(world->terrain != NULL);
    uint64_t count_dna_avg;
    if (dna_avg)
    {
        memset(dna_avg, 0, sizeof(typeof(*dna_avg)));
        count_dna_avg = 0;
    }
    // monkeys
    for (size_t i=0; i<world->monkeys.size; ++i)
    {
        Monkey* monkey = &world->monkeys.data[i];
        // add
        if (dna_avg)
        {
            dna_avg->move_probability += monkey->dna.move_probability;
            dna_avg->sight_radius     += monkey->dna.sight_radius;
            dna_avg->score_water      += monkey->dna.score_water;
            ++count_dna_avg;
        }
        // check aliveness
        if (false
            // fell of the edge
            || 0 > monkey->pos_x || monkey->pos_x >= terrain_width
            || 0 > monkey->pos_y || monkey->pos_y >= terrain_height
            // in water
            || world->terrain[(monkey->pos_y * terrain_width) + monkey->pos_x] >= TERRAIN_WATER
            // old age
            || (   monkey->age >= MONKEY_AGE_OLD
                && ((float)rand()/RAND_MAX) <= MONKEY_PROBABILITY_DYING_FROM_OLD_AGE)
        ){
            dyn_array_monkey_remove_unordered(&world->monkeys, i);
            if (count_death)
                ++(*count_death);
        }
        // age
        ++monkey->age;
        // looking
        // movement
        bool has_moved = false;
        for (size_t j=0; j<world->monkeys.size; ++j)
        {
            if (j == i)
                continue;
            Monkey* other_monkey = &world->monkeys.data[j];
            // choose movement by gene
            const float probability = (float)rand()/RAND_MAX;
            // TODO:
            //   each gets a score
            //   each square is ranked ?
            const int d_x  = monkey->pos_x - other_monkey->pos_x;
            const int d_y  = monkey->pos_y - other_monkey->pos_y;
            const int dist = (int)sqrt(d_x*d_x + d_y*d_y);
            if (   MONKEYS_SOCIAL_DISTANCE < dist
                && dist <= monkey->dna.sight_radius + MONKEYS_SOCIAL_DISTANCE)
            {
                if (monkey->dna.move_probability <= probability)
                {
                    monkey->pos_x += (rand() % (2 + 1) -1) * sign(d_x);
                    monkey->pos_y += (rand() % (2 + 1) -1) * sign(d_y);
                    has_moved = true;
                }
            }
            // birth children?
            if (true
                // adults
                && MONKEY_AGE_ADULT <= monkey->age       && monkey->age       < MONKEY_AGE_OLD
                && MONKEY_AGE_ADULT <= other_monkey->age && other_monkey->age < MONKEY_AGE_OLD
                // less than max children
                && monkey->count_child       < MONKEY_COUNT_CHILD_MAX
                && other_monkey->count_child < MONKEY_COUNT_CHILD_MAX
                // are in birthing range
                && monkey->pos_x == other_monkey->pos_x
                && monkey->pos_y == other_monkey->pos_y)
            {
                Monkey* baby_monkey = world_monkeys_spawn_newborn(world);
                baby_monkey->pos_x  = monkey->pos_x;
                baby_monkey->pos_y  = monkey->pos_y;
                baby_monkey->dna    = dna_combine(&monkey->dna, &other_monkey->dna);
                ++monkey->count_child;
                ++other_monkey->count_child;
                if (count_newborn)
                    ++(*count_newborn);
            }
        }
        if (!has_moved)
        {
            // random movement
            if (monkey->dna.move_probability <= (float)rand()/RAND_MAX)
            {
                monkey->pos_x += rand() % (2 + 1) -1;
                monkey->pos_y += rand() % (2 + 1) -1;
            }
        }
    }
    if (dna_avg && count_dna_avg != 0)
    {
        dna_avg->move_probability /= count_dna_avg;
        dna_avg->sight_radius     /= count_dna_avg;
        dna_avg->score_water      /= count_dna_avg;
    }
}

Monkey* world_monkeys_spawn_newborn(World* world)
{
    assert(world != NULL);
    Monkey monkey = {0};
    monkey.id     = world->monkey_next_id++;
    return dyn_array_monkey_append(&world->monkeys, monkey);
}

