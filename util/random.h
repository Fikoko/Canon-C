#ifndef CANON_C_UTIL_RANDOM_H
#define CANON_C_UTIL_RANDOM_H

#include <stdint.h>

/*
    random.h — Minimal, deterministic PRNG (PCG32)

    No global state — fully explicit.
    Fast, good quality, small code.
*/

typedef struct {
    uint64_t state;
    uint64_t inc;
} Random;

/* Seed the generator */
static inline void random_seed(Random* r, uint64_t seed, uint64_t seq)
{
    if (!r) return;
    r->state = 0U;
    r->inc = (seq << 1u) | 1u;
    random_u32(r);  // warm-up
    r->state += seed;
    random_u32(r);
}

/* Next 32-bit value */
static inline uint32_t random_u32(Random* r)
{
    if (!r) return 0;
    uint64_t old_state = r->state;
    r->state = old_state * 6364136223846793005ULL + r->inc;
    uint32_t xor_shifted = (uint32_t)(((old_state >> 18u) ^ old_state) >> 27u);
    uint32_t rot = (uint32_t)(old_state >> 59u);
    return (xor_shifted >> rot) | (xor_shifted << ((-rot) & 31));
}

/* Uniform [0, bound) */
static inline uint32_t random_range(Random* r, uint32_t bound)
{
    if (!r || bound == 0) return 0;
    uint32_t threshold = -bound % bound;
    uint32_t val;
    do {
        val = random_u32(r);
    } while (val < threshold);
    return val % bound;
}

/* Float in [0.0, 1.0) */
static inline double random_double(Random* r)
{
    return (double)random_u32(r) / (double)4294967296.0;  // 2^32
}

#endif /* CANON_C_UTIL_RANDOM_H */
