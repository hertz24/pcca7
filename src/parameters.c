#include "../include/parameters.h"

uint32_t rand_prime(void)
{
    FLINT_TEST_INIT(state);
    uint32_t p = n_randprime(state, 2 + rand() % 31, 1);
    FLINT_TEST_CLEAR(state);
    return p;
}

Parameters rand_parameters_b(ulong bits)
{
    FLINT_TEST_INIT(state);
    uint32_t b = n_randbits(state, bits);
    uint32_t p;
    if (bits == 1)
        bits++;
    ulong limit = (33 - bits);
    do
    {
        p = n_randprime(state, bits + rand() % limit, 1);
    } while (p <= b);
    FLINT_TEST_CLEAR(state);
    return init_parameters(b, p);
}

Parameters rand_parameters(ulong bits)
{
    FLINT_TEST_INIT(state);
    uint32_t p = (bits <= 1) ? rand_prime() : n_randprime(state, bits, 1);
    Parameters param = init_parameters(n_randint(state, p), p);
    FLINT_TEST_CLEAR(state);
    return param;
}

void print_param(Parameters param)
{
    printf("b = %u\nb_precomp = %u\np = %u\n", param.b, param.b_precomp, param.p);
}