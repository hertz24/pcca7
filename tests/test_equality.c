#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"

int main(void)
{
    srand(time(NULL));
    Parameters param = rand_parameters();
    print_param(param);
    Vector rand_v = rand_vector(10, param.p);
    print_vector(rand_v);
    Vector v1 = shoup_scale(param, rand_v);
    Vector v2 = naive_scale(param, rand_v);
    int ret = compare_vectors(v1, v2);
    if (!ret)
        printf("test_equality: no error\n");
    return ret;
}
