// math operations
#include "math-ops.h"

uint32_t xorshift32(uint32_t* rng) {
    uint32_t x = *rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *rng = x;
    return x;
}

double random(uint32_t* rng) {
    return (double)xorshift32(rng) / std::numeric_limits<uint32_t>::max();
}