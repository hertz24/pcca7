#include "../include/vector.h"

#define DEFINE_VECTOR(BIT, CAST)                                                                                              \
    vector##BIT##_t init_vector_##BIT(ulong size)                                                                             \
    {                                                                                                                         \
        vector##BIT##_t v = {malloc(size * sizeof(uint##BIT##_t)), size};                                                     \
        if (v.elements == NULL)                                                                                               \
        {                                                                                                                     \
            perror("init_vector vector.elements malloc");                                                                     \
            return (vector##BIT##_t){0};                                                                                      \
        }                                                                                                                     \
        return v;                                                                                                             \
    }                                                                                                                         \
                                                                                                                              \
    vector##BIT##_t rand_vector_##BIT(ulong size)                                                                             \
    {                                                                                                                         \
        vector##BIT##_t v = init_vector_##BIT(size);                                                                          \
        FLINT_TEST_INIT(state);                                                                                               \
        for (ulong i = 0; i < size; i++)                                                                                      \
            *(v.elements + i) = n_randbits(state, n_randint(state, 33));                                                      \
        FLINT_TEST_CLEAR(state);                                                                                              \
        return v;                                                                                                             \
    }                                                                                                                         \
                                                                                                                              \
    void free_vector_##BIT(vector##BIT##_t v)                                                                                 \
    {                                                                                                                         \
        free(v.elements);                                                                                                     \
    }                                                                                                                         \
                                                                                                                              \
    void print_vector_##BIT(FILE *out, vector##BIT##_t v)                                                                     \
    {                                                                                                                         \
        fprintf(out, "[");                                                                                                    \
        for (ulong i = 0; i < v.size - 1; i++)                                                                                \
            fprintf(out, "%u, ", *(v.elements + i));                                                                          \
        fprintf(out, "%u]\n", *(v.elements + v.size - 1));                                                                    \
    }                                                                                                                         \
                                                                                                                              \
    int compare_vectors_##BIT(vector##BIT##_t v1, vector##BIT##_t v2)                                                         \
    {                                                                                                                         \
        if (v1.size != v2.size)                                                                                               \
            return 0;                                                                                                         \
        for (ulong i = 0; i < v1.size; i++)                                                                                   \
            if (*(v1.elements + i) != *(v2.elements + i))                                                                     \
                return 0;                                                                                                     \
        return 1;                                                                                                             \
    }                                                                                                                         \
                                                                                                                              \
    __attribute__((optimize("no-tree-vectorize"))) vector##BIT##_t naive_scale_##BIT(param##BIT##_t param, vector##BIT##_t v) \
    {                                                                                                                         \
        vector##BIT##_t res = init_vector_##BIT(v.size);                                                                      \
        for (ulong i = 0; i < v.size; i++)                                                                                    \
            *(res.elements + i) = ((CAST) * (v.elements + i) * param.b) % param.p;                                            \
        return res;                                                                                                           \
    }

DEFINE_VECTOR(16, uint32_t)
DEFINE_VECTOR(32, uint64_t)
