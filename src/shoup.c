#include "../include/shoup.h"

uint32_t shoup_algorithm(uint32_t a, uint32_t b, uint64_t p)
{
    assert(is_prime(p));
    uint64_t b_bis = ((uint64_t)b << 32) / p;
    uint64_t q = (a * b_bis) >> 32;
    uint64_t c = (a * b - q * p) % (1UL << 32);
    if (c >= p)
        c -= p;
    return c;
}