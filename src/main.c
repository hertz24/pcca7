#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = rand_parameters(100000);
    print_param(param1);
    Vector v1 = rand_vector(1, param1.p);
    printf("Naive scalar product: %.20f ms\n", time_algorithm(naive_scalar_product, param1, v1));
    printf("Shoup scalar product: %.20f ms\n", time_algorithm(shoup_scalar, param1, v1));
    free_vector(v1);
    generate_curve(1, 100);
    return 0;
}
