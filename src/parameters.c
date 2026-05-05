#include "../include/parameters.h"

flint_rand_t state;

void rand_init(void)
{
    flint_rand_init(state);
    unsigned long seed = time(NULL);
    flint_rand_set_seed(state, seed, seed + 1);
}

void rand_clear(void)
{
    flint_rand_clear(state);
}

uint32_t rand_prime(ulong bits)
{
    if (bits < 2 || bits > 32)
        return 0;
    return n_randprime(state, bits, 1);
}

uint32_t max_prime_bits(ulong bits)
{
    if (bits < 2 || bits > 32)
        return 0;
    uint32_t x = (1 << bits) - 1;
    while (FLINT_BIT_COUNT(x) == bits)
    {
        if (n_is_prime(x))
            return x;
        x -= 2;
    }
    /*
     * NOTE: should never reach
     */
    return 0;
}

Parameters rand_parameters(ulong b_bits, ulong p_bits)
{
    /*
     * NOTE: the maximal number of bits used are 31.
     */
    if (b_bits > 32 || p_bits < 2 || p_bits > 32 || p_bits < b_bits)
        return (Parameters){0};
    if (b_bits == 2 && p_bits == 2)
        return init_parameters(2, 3);
    uint32_t p = rand_prime(p_bits);
    uint32_t b;
    do
    {
        b = n_randbits(state, b_bits);
    } while (p <= b);
    return init_parameters(b, p);
}

void print_param(FILE *out, Parameters param)
{
    fprintf(out, "b = %u, b_precomp = %u, p = %u\n", param.b, param.b_precomp, param.p);
}
