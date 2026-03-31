#include "test.h"

int main(void)
{
    for (int i = 0; i <= 33; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = rand_parameters(i, j);
            if (!n_is_prime(param.p))
            {
                ERROR("test_param");
                fprintf(stderr, "%d isn't a prime number.\n", param.p);
                return 1;
            }
            if (param.p >= (1UL << 31))
            {
                ERROR("test_param");
                fprintf(stderr, "%d is greater than or equal to 2^{31}.\n", param.p);
                return 1;
            }
            if (param.b >= param.p)
            {
                ERROR("test_param");
                fprintf(stderr, "param.b == %u >= param.p == %u\n", param.b, param.p);
                return 1;
            }
        }
    SUCCESS("test_param");
    return 0;
}
