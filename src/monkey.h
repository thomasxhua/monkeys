#ifndef MONKEY_H
#define MONKEY_H

#include <stdint.h>

#include "dyn_array.h"

#define MONKEY_SIGHT_RADIUS_DEFAULT 3.0f

#define MONKEY_AGE_ADULT 500
#define MONKEY_AGE_OLD   800
#define MONKEY_PROBABILITY_DYING_FROM_OLD_AGE 0.01f

typedef struct
{
    float move_probability;
    float sight_radius;
    // preferences in [-1.0, 1.0]
    float score_water;
} DNA;

DNA dna_combine(const DNA* a, const DNA* b);
void dna_mutate(DNA* dna, float noise_percent);

typedef struct
{
    // fix
    uint64_t id;
    // stats (current state)
    int pos_x, pos_y;
    int age;
    // ... hp ...
    DNA dna;
} Monkey;

DEFINE_DYN_ARRAY(Monkey, DynArrayMonkey, dyn_array_monkey)

#endif // MONKEY_H
