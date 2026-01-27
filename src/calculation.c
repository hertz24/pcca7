#include "../include/calculation.h"

Parameters init_parameters(uint32_t b, uint64_t p)
{
    assert(n_is_prime(p));
    return (Parameters){b, ((uint64_t)b << 32) / p, p};
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

Vector rand_vector(int size)
{
    Vector vector = init_vector(size);
    for (int i = 0; i < size; i++)
        *(vector.elements + i) = rand();
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