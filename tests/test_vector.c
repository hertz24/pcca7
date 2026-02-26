#include "../include/calculation.h"

int main(void)
{
    int ret = 0;
    uint32_t p = rand_prime();
    Vector v = rand_vector(100000, p);
    for (int i = 0; i < 100000; i++)
        if (*(v.elements + i) >= p)
        {
            printf("test_vector error: a == %u >= p == %u at index %d\n", *(v.elements + i), p, i);
            ret = 1;
            break;
        }
    free_vector(v);
    if (ret == 0)
        printf("test_vector: no error\n");
    return ret;
}
