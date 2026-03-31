#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_param: the print statements in the functions under test are not silenced.\n");
    rand_init();
    for (ulong i = 0; i <= 32; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = rand_parameters(i, j);
            if (i > j && i <= 31 && j >= 2 && j <= 31 && param.p != 0)
            {
                FAIL("test_param", &err);
                fprintf(stderr, "param should be {0}.\n");
                ret = 1;
                goto end;
            }
            else if (param.p == 0)
                continue;
            if (!n_is_prime(param.p))
            {
                FAIL("test_param", &err);
                fprintf(stderr, "%d isn't a prime number.\n", param.p);
                ret = 1;
                goto end;
            }
            if (param.p >= (1UL << 31))
            {
                FAIL("test_param", &err);
                fprintf(stderr, "%d is greater than or equal to 2^{31}.\n", param.p);
                ret = 1;
                goto end;
            }
            if (param.b >= param.p)
            {
                FAIL("test_param", &err);
                fprintf(stderr, "param.b == %u >= param.p == %u\n", param.b, param.p);
                ret = 1;
                goto end;
            }
        }
    SUCCESS("test_param", &out);
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
