#include <time.h>

#include "test.h"
#include "../include/parameters.h"

int main(void)
{
    srand(time(NULL));
    Parameters (*functions[2])(ulong) = {rand_parameters_p, rand_parameters_b};
    char *names[2] = {"rand_parameters", "rand_parameters_b"};
    for (int i = 0; i <= 1; i++)
        for (ulong j = 0; j <= 31; j++)
        {
            Parameters param = functions[i](j);
            if (!n_is_prime(param.p))
            {
                ERROR("test_param");
                fprintf(stderr, "for %s, %d isn't a prime number.\n", names[i], param.p);
                return 1;
            }
            if (param.p >= (1UL << 31))
            {
                ERROR("test_param");
                fprintf(stderr, "for %s, %d is greater than or equal to 2^{31}.\n", names[i], param.p);
                return 1;
            }
            if (param.b >= param.p)
            {
                ERROR("test_param");
                fprintf(stderr, "for %s, param.b == %u >= param.p == %u\n", names[i], param.b, param.p);
                return 1;
            }
        }
    SUCCESS("test_param");
    return 0;
}
