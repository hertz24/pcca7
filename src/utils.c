#include "../include/utils.h"

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector vector)
{
    clock_t start = clock();
    free_vector(algorithm(param, vector));
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

int compare_vectors(Vector v1, Vector v2)
{
    if (v1.size != v2.size)
    {
        fprintf(stderr, "The size of vectors aren't the same.\n");
        return 0;
    }
    for (ulong i = 0; i < v1.size; i++)
        if (*(v1.elements + i) != *(v2.elements + i))
        {
            printf("v1: %d and v2: %d at index %ld\n", *(v1.elements + i), *(v2.elements + i), i);
            return 1;
        }
    return 0;
}