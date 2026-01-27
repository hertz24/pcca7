#include "../include/utils.h"

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector vector)
{
    clock_t start = clock();
    algorithm(param, vector);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}