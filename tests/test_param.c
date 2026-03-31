#include "test.h"

int main(void)
{
    int ret = 0;
    int out = dup(STDOUT_FILENO);
    int err = dup(STDERR_FILENO);
    int null = open("/dev/null", O_WRONLY);
    dup2(null, STDOUT_FILENO);
    dup2(null, STDERR_FILENO);
    close(null);
    rand_init();
    for (ulong i = 0; i <= 32; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = rand_parameters(i, j);

            // If i > j, then rand_parameters returns (Parameters){0}
            if (i > j && i <= 31 && j >= 2 && j <= 31 && param.p != 0)
            {
                fflush(stderr);
                dup2(err, STDERR_FILENO);
                ERROR("test_param");
                fprintf(stderr, "param should be {0}.\n");
                ret = 1;
                goto end;
            }
            else
                continue;
            if (!n_is_prime(param.p))
            {
                fflush(stderr);
                dup2(err, STDERR_FILENO);
                ERROR("test_param");
                fprintf(stderr, "%d isn't a prime number.\n", param.p);
                ret = 1;
                goto end;
            }
            if (param.p >= (1UL << 31))
            {
                fflush(stderr);
                dup2(err, STDERR_FILENO);
                ERROR("test_param");
                fprintf(stderr, "%d is greater than or equal to 2^{31}.\n", param.p);
                ret = 1;
                goto end;
            }
            if (param.b >= param.p)
            {
                fflush(stderr);
                dup2(err, STDERR_FILENO);
                ERROR("test_param");
                fprintf(stderr, "param.b == %u >= param.p == %u\n", param.b, param.p);
                ret = 1;
                goto end;
            }
        }
    fflush(stdout);
    dup2(out, STDOUT_FILENO);
    SUCCESS("test_param");
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
