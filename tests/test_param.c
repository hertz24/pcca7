#include <time.h>

#include "../include/parameters.h"

int main(void)
{
    srand(time(NULL));
    Parameters (*functions[2])(ulong) = {rand_parameters, rand_parameters_b};
    char *names[2] = {"rand_parameters", "rand_parameters_b"};
    for (int i = 0; i <= 1; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = functions[i](j);
            if (!n_is_prime(param.p))
            {
                printf("test_param error for %s: %d isn't a prime number.\n", names[i], param.p);
                return 1;
            }
            if (param.b >= param.p)
            {
                printf("test_param error for %s: param.b == %u >= param.p == %u\n", names[i], param.b, param.p);
                return 1;
            }
            if (i == 0)
                j++;
        }
    printf("test_param: no error\n");
    return 0;
}
