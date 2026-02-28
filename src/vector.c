#include "../include/vector.h"

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

int compare_vectors(Vector v1, Vector v2)
{
    if (v1.size != v2.size)
    {
        fprintf(stderr, "The size of vectors isn't the same.\n");
        return 0;
    }
    for (ulong i = 0; i < v1.size; i++)
        if (*(v1.elements + i) != *(v2.elements + i))
            return 1;
    return 0;
}

Vector naive_scale(Parameters param, Vector v)
{
    Vector res = init_vector(v.size);
    for (ulong i = 0; i < v.size; i++)
        *(res.elements + i) = ((uint64_t)*(v.elements + i) * param.b) % param.p;
    return res;
}