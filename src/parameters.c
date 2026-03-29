#include "../include/parameters.h"

flint_rand_t state;

void rand_init(void)
{
    flint_rand_init(state);
    unsigned long seed = time(NULL);
    flint_randseed(state, seed, seed + 1);
}

uint32_t rand_prime(ulong bits)
{
    if (bits < 2 || bits > 31)
        bits = 2 + rand() % 30;
    uint32_t p = n_randprime(state, bits, 1);
    return p;
}

uint32_t max_prime_bits(ulong bits)
{
    if (bits < 2 || bits > 32)
    {
        fprintf(stderr, "The number of bits must be between 2 and 32.\n");
        return 0;
    }
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

Parameters rand_parameters_p(ulong bits)
{
    uint32_t p = rand_prime(bits);
    Parameters param = init_parameters(n_randint(state, p), p);
    return param;
}

Parameters rand_parameters_b(ulong bits)
{
    uint32_t b = n_randbits(state, bits);
    uint32_t p;
    ulong lower = (bits < 2) ? 2 : bits;
    do
    {
        p = n_randprime(state, lower + rand() % (32 - lower), 1);
    } while (p <= b);
    return init_parameters(b, p);
}

Parameters rand_parameters(ulong p_bits, ulong b_bits)
{
    if (p_bits < b_bits && p_bits >= 2 && p_bits <= 31)
    {
        fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n");
        return (Parameters){0};
    }
    uint32_t p;
    uint32_t b;
    Parameters param;
    if (p_bits >= 2 && p_bits <= 31)
    {
        p = rand_prime(p_bits);
        do
        {
            b = n_randbits(state, b_bits);
        } while (p <= b);
        param = init_parameters(b, p);
    }
    else
        param = rand_parameters_b(b_bits);
    return param;
}

void print_param(Parameters param)
{
    printf("b = %u\nb_precomp = %u\np = %u\n", param.b, param.b_precomp, param.p);
}
