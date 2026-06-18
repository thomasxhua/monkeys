#include "monkey.h"

DNA dna_combine(const DNA* a, const DNA* b)
{
    assert(a != NULL);
    assert(b != NULL);
    DNA dna;
    dna.move_probability = (a->move_probability + b->move_probability) / 2;
    dna.sight_radius     = (a->sight_radius + b->sight_radius) / 2;
    dna.score_water      = (a->score_water + b->score_water) / 2;
    dna_mutate(&dna, 0.1f);
    return dna;
}

void dna_mutate(DNA* dna, float noise_percent)
{
    assert(dna != NULL);
    float factor;
    factor = 1.0f + (((float)rand()/RAND_MAX) * noise_percent) - 2*noise_percent;
    dna->move_probability *= factor;
    factor = 1.0f + (((float)rand()/RAND_MAX) * noise_percent) - 2*noise_percent;
    dna->sight_radius     *= factor;
    factor = 1.0f + (((float)rand()/RAND_MAX) * noise_percent) - 2*noise_percent;
    dna->score_water      *= factor;
}

