#include "../include/shoup.h"

uint64_t shoup_algorithm(uint32_t a, uint32_t b, uint64_t b_bis, uint64_t p)
{
    assert(is_prime(p));

    // TODO: overflow to fix
    // a * b_bis / 2^32
    uint64_t q = (a * b_bis) >> 32;

    // (a * b - q * p) % 2^32
    uint64_t c = (a * b - q * p) % (1UL << 32);

    if (c >= p)
        c -= p;
    return c;
}

Vector shoup_scalar_algorithm(Parameters param)
{
    int size = param.size;
    Vector vector = init_vector(size);
    for (int i = 0; i < size; i++)
        *(vector.elements + i) = shoup_algorithm(*(param.tab_a + i), param.b, param.b_bis, param.p);
    return vector;
}