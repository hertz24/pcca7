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

Vector rand_vector(ulong size)
{
    Vector v = init_vector(size);
    FLINT_TEST_INIT(state);
    for (ulong i = 0; i < size; i++)
        *(v.elements + i) = n_randbits(state, n_randint(state, 33));
    FLINT_TEST_CLEAR(state);
    return v;
}

void free_vector(Vector v)
{
    free(v.elements);
}

void print_vector(FILE *out, Vector v)
{
    fprintf(out, "[");
    for (ulong i = 0; i < v.size - 1; i++)
        fprintf(out, "%u, ", *(v.elements + i));
    fprintf(out, "%u]\n", *(v.elements + v.size - 1));
}

int compare_vectors(Vector v1, Vector v2)
{
    if (v1.size != v2.size)
        return 0;
    for (ulong i = 0; i < v1.size; i++)
        if (*(v1.elements + i) != *(v2.elements + i))
            return 0;
    return 1;
}

__attribute__((optimize("no-tree-vectorize"))) Vector naive_scale(Parameters param, Vector v)
{
    Vector res = init_vector(v.size);
    for (ulong i = 0; i < v.size; i++)
        *(res.elements + i) = ((uint64_t)*(v.elements + i) * param.b) % param.p;
    return res;
}
