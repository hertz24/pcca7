#include <time.h>
#include "../include/calculation.h"

int main(void)
{
    srand(time(NULL));
    Parameters param = rand_parameters();
    if (!n_is_prime(param.p))
    {
        printf("n_is_prime error: %d isn't a prime number.\n", param.p);
        return 1;
    }
    if (param.b >= param.p)
    {
        printf("n_is_prime error: param.b == %u >= param.p == %u\n", param.b, param.p);
        return 1;
    }
    printf("test_param: no error\n");
    return 0;
}
