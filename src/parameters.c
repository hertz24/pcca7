#include "../include/parameters.h"

uint32_t rand_prime(void)
{
    FLINT_TEST_INIT(state);
    uint32_t p = n_randprime(state, rand() % 32, 1);
    FLINT_TEST_CLEAR(state);
    return p;
}

Parameters rand_parameters(void)
{
    FLINT_TEST_INIT(state);
    uint32_t p = rand_prime();
    Parameters param = init_parameters(n_randint(state, p), p);
    FLINT_TEST_CLEAR(state);
    return param;
}

void print_param(Parameters param)
{
    printf("b = %u\nb_precomp = %u\np = %u\n", param.b, param.b_precomp, param.p);
}