#include "../include/calculation.h"

uint32_t rand_prime()
{
    FLINT_TEST_INIT(state);
    uint32_t p = n_randprime(state, 31, 1);
    FLINT_TEST_CLEAR(state);
    return p;
}

Parameters init_parameters(uint32_t b, uint32_t p)
{
    assert(n_is_prime(p) && b < p);
    return (Parameters){b, ((uint64_t)b << 32) / p, p};
}

Parameters rand_parameters()
{
    FLINT_TEST_INIT(state);
    uint32_t p = rand_prime();
    Parameters param = init_parameters(n_randint(state, p), p);
    FLINT_TEST_CLEAR(state);
    return param;
}

void print_param(Parameters param)
{
    printf("b = %u\nb_bis = %u\np = %u\n", param.b, param.b_bis, param.p);
}

Vector init_vector(ulong size)
{
    Vector v = {malloc(size * sizeof(uint64_t)), size};
    if (v.elements == NULL)
    {
        perror("init_vector vector.elements malloc");
        return (Vector){0};
    }
    return v;
}

Vector rand_vector(ulong size, uint32_t p)
{
    Vector v = init_vector(size);
    for (ulong i = 0; i < size; i++)
        *(v.elements + i) = rand() % p;
    return v;
}

void free_vector(Vector v)
{
    free(v.elements);
}

void print_vector(Vector v)
{
    printf("[");
    for (ulong i = 0; i < v.size - 1; i++)
        printf("%d, ", *(v.elements + i));
    printf("%d]\n", *(v.elements + v.size - 1));
}

Vector naive_scalar_product(Parameters param, Vector v)
{
    Vector res = init_vector(v.size);
    for (ulong i = 0; i < v.size; i++)
        *(res.elements + i) = ((uint64_t)*(v.elements + i) * param.b) % param.p;
    return res;
}