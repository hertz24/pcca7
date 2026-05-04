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

ulong rand_prime(ulong bits)
{
    if (bits < 2)
        bits = 2 + n_randint(state, 30);
    return n_randprime(state, bits, 1);
}

ulong max_prime_bits(ulong bits)
{
    if (bits < 2)
    {
        fprintf(stderr, "The number of bits must be greater than 2.\n");
        return 0;
    }
    ulong x = (1 << bits) - 1;
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

#define DEFINE_PARAM(BIT)                                                                                           \
    param##BIT##_t rand_parameters_##BIT(ulong b_bits, ulong p_bits)                                                \
    {                                                                                                               \
        ulong max = BIT - 1;                                                                                        \
        if (p_bits >= 2 && p_bits <= max && b_bits <= max && p_bits < b_bits)                                       \
        {                                                                                                           \
            fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n"); \
            return (param##BIT##_t){0};                                                                             \
        }                                                                                                           \
        if (p_bits < 2 || p_bits > max)                                                                             \
        {                                                                                                           \
            ulong lower = (b_bits < 2 || b_bits > max) ? 2 : b_bits;                                                \
            p_bits = lower + n_randint(state, BIT - lower);                                                         \
        }                                                                                                           \
        if (b_bits > max)                                                                                           \
            b_bits = n_randint(state, p_bits + 1);                                                                  \
        uint32_t p = rand_prime(p_bits);                                                                            \
        if (p == 2 && b_bits == 2)                                                                                  \
        {                                                                                                           \
            printf("p = 2: the number of bits of b was changed by 1.\n");                                           \
            return init_parameters_##BIT(1, 2);                                                                     \
        }                                                                                                           \
        uint32_t b;                                                                                                 \
        do                                                                                                          \
        {                                                                                                           \
            b = n_randbits(state, b_bits);                                                                          \
        } while (p <= b);                                                                                           \
        return init_parameters_##BIT(b, p);                                                                         \
    }                                                                                                               \
                                                                                                                    \
    void print_param_##BIT(FILE *out, param##BIT##_t param)                                                         \
    {                                                                                                               \
        fprintf(out, "b = %u, b_precomp = %u, p = %u\n", param.b, param.b_precomp, param.p);                        \
    }

DEFINE_PARAM(16)
DEFINE_PARAM(32)
