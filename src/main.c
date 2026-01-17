#include "../include/shoup.h"
#include "../include/calculation.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(100, 1828122989, 97);
    printf("%d\n", param1.b);
    printf("%ld\n", param1.b_bis);
    printf("%ld\n", param1.p);
    Vector v = shoup_scalar_algorithm(param1);
    print_vector(v);
    return 0;
}
