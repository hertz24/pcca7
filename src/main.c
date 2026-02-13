#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"
#include "flint/nmod.h"
#include "flint/ulong_extras.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(96, rand_prime(1000000));
    print_param(param1);
    Vector rand_v = rand_vector(50, param1.p);
    print_vector(rand_v);
    Vector v = shoup_scalar(param1, rand_v);
    printf("%.20f s\n", time_algorithm(shoup_scalar, param1, rand_v));
    print_vector(v);
    printf("\n");
    Vector v1 = naive_scalar_product(param1, rand_v);
    printf("%.20f s\n", time_algorithm(naive_scalar_product, param1, rand_v));
    print_vector(v1);
    printf("%d\n", compare_vectors(v, v1));
    return 0;
}
