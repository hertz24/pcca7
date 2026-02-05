#include "../include/utils.h"

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector vector)
{
    clock_t start = clock();
    algorithm(param, vector);
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
    for (int i = 0; i < v1.size; i++)
        if (*(v1.elements + i) != *(v2.elements + i))
        {
            printf("v1: %d and v2: %d at index %d\n", *(v1.elements + i), *(v2.elements + i), i);
            return 0;
        }
    return 1;
}