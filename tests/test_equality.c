#include <time.h>

#include "../include/shoup.h"

int main(void)
{
    srand(time(NULL));
    Parameters param = rand_parameters();
    Vector rand_v = rand_vector(10000);
    Vector v1 = naive_scale(param, rand_v);
    Vector v2 = shoup_scale_ref(param, rand_v);
    Vector v3 = shoup_scale(param, rand_v);
    Vector v4 = shoup_scale_mullo(param, rand_v);
    Vector v5 = shoup_scale_avx512(param, rand_v);
    int ret = compare_vectors(v1, v2) || compare_vectors(v2, v3) || compare_vectors(v3, v4) || compare_vectors(v4, v5);
    if (!ret)
        printf("test_equality: no error\n");
    else
    {
        printf("test_equality: error:\n");
        print_param(param);
        print_vector(v1);
        print_vector(v2);
        print_vector(v3);
        print_vector(v4);
        print_vector(v5);
    }
    free_vector(v1);
    free_vector(v2);
    free_vector(v3);
    free_vector(v4);
    free_vector(v5);
    return ret;
}
