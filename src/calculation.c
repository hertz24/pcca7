#include "../include/calculation.h"

int is_prime(uint64_t p)
{
    if (p <= 1)
        return 0;
    for (uint32_t i = 2; i * i <= p; i++)
        if (p % i == 0)
            return 0;
    return 1;
}

static uint64_t pre_computation(uint32_t b, uint64_t p)
{
    assert(is_prime(p));

    // b * 2^32 / p
    return ((uint64_t)b << 32) / p;
}

Parameters init_parameters(int size, uint32_t b, uint64_t p)
{
    assert(is_prime(p));
    Parameters param = {.tab_a = malloc(size * sizeof(uint32_t)), .size = size, .b = b, .p = p};
    if (param.tab_a == NULL)
    {
        perror("init_parameters param.tab_a malloc");
        return (Parameters){0};
    }
    for (int i = 0; i < size; i++)
        *(param.tab_a + i) = rand();
    param.b_bis = pre_computation(b, p);
    return param;
}

void free_parameters(Parameters param)
{
    free(param.tab_a);
}

Vector init_vector(int size)
{
    Vector vector = {malloc(size * sizeof(uint64_t)), size};
    if (vector.elements == NULL)
    {
        perror("init_vector vector.elements malloc");
        return (Vector){0};
    }
    return vector;
}

void free_vector(Vector vector)
{
    free(vector.elements);
}

void print_vector(Vector vector)
{
    printf("[");
    for (int i = 0; i < vector.size - 1; i++)
        printf("%ld, ", *(vector.elements + i));
    printf("%ld]\n", *(vector.elements + vector.size - 1));
}