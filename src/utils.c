#include "../include/utils.h"

static void test_algorithm(void *arg, ulong count)
{
    void **data = (void **)arg;
    Vector (*algorithm)(Parameters, Vector) = *data;
    Parameters param = *((Parameters *)*(data + 1));
    Vector v = *((Vector *)*(data + 2));
    for (ulong i = 0; i < count; i++)
    {
        prof_start();
        free_vector(algorithm(param, v));
        prof_stop();
    }
}

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector v)
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
    *(data + 2) = &v;
    prof_repeat(&min, &max, test_algorithm, data);
    free(data);
    return min;
}
