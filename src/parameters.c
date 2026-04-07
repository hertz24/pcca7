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
    if (bits < 2 || bits > 31)
        bits = 2 + n_randint(state, 30);
    return n_randprime(state, bits, 1);
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

Parameters rand_parameters(ulong b_bits, ulong p_bits)
{
    if (p_bits >= 2 && p_bits <= 31 && b_bits <= 31 && p_bits < b_bits)
    {
        fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n");
        return (Parameters){0};
    }
    if (p_bits < 2 || p_bits > 31)
    {
        ulong lower = (b_bits < 2 || b_bits > 31) ? 2 : b_bits;
        p_bits = lower + n_randint(state, 32 - lower);
    }
    if (b_bits > 31)
        b_bits = n_randint(state, p_bits + 1);
    uint32_t p = rand_prime(p_bits);
    if (p == 2 && b_bits == 2)
    {
        printf("p = 2: the number of bits of b was changed by 1.\n");
        return init_parameters(1, 2);
    }
    uint32_t b;
    do
    {
        b = n_randbits(state, b_bits);
    } while (p <= b);
    return init_parameters(b, p);
}

void print_param(Parameters param)
{
    printf("b = %u\nb_precomp = %u\np = %u\n", param.b, param.b_precomp, param.p);
}
