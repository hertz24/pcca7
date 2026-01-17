#include "../include/utils.h"

double time_algorithm(Vector (*algorithm)(Parameters), Parameters param)
{
    clock_t start = clock();
    algorithm(param);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}