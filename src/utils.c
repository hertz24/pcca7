#include "../include/utils.h"

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

static void test_algorithm(void *arg, ulong count)
{
    void **data = (void **)arg;
    Vector (*algorithm)(Parameters, Vector) = *data;
    Parameters param = *((Parameters *)*(data + 1));
    Vector vector = *((Vector *)*(data + 2));
    for (ulong i = 0; i < count; i++)
    {
        prof_start();
        free_vector(algorithm(param, vector));
        prof_stop();
    }
}

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector vector)
{
    double min, max;
    void **data = malloc(3 * sizeof(void *));
    if (data == NULL)
    {
        perror("time_algorithm data malloc");
        return -1;
    }
    *data = algorithm;
    *(data + 1) = &param;
    *(data + 2) = &vector;
    prof_repeat(&min, &max, test_algorithm, data);
    free(data);
    return min;
}