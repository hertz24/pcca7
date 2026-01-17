#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(100, 812112989, 97);
    Vector v = shoup_scalar_algorithm(param1);
    printf("%f s\n", time_algorithm(shoup_scalar_algorithm, param1));
    print_vector(v);
    return 0;
}
