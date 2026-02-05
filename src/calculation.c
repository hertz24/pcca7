#include "../include/calculation.h"

uint32_t rand_prime(int n)
{
    n_primes_t iter;
    n_primes_init(iter);
    n = rand() % n;
    uint32_t p;
    int i = 0;
    do
    {
        p = n_primes_next(iter);
    } while (i++ < n);
    n_primes_clear(iter);
    return p;
}

Parameters init_parameters(uint32_t b, uint32_t p)
{
    assert(n_is_prime(p) && b < p);
    return (Parameters){b, ((uint64_t)b << 32) / p, p};
}

void print_param(Parameters param)
{
    printf("b = %u\nb_bis = %u\np = %u\n", param.b, param.b_bis, param.p);
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

Vector rand_vector(int size, uint32_t p)
{
    Vector vector = init_vector(size);
    for (int i = 0; i < size; i++)
        *(vector.elements + i) = rand() % p;
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
        printf("%d, ", *(vector.elements + i));
    printf("%d]\n", *(vector.elements + vector.size - 1));
}

Vector naive_scalar_product(Vector vector, uint32_t b, uint32_t p)
{
    for (int i = 0; i < vector.size; i++)
        *(vector.elements + i) = *(vector.elements + i) * b % p;
    return vector;
}