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
            for (ulong k = 0; k <= 32; k++)
            {
                Parameters param = rand_parameters(i, j);
                uint32_t a = n_randbits(state, k);
                uint32_t ref = (uint64_t)a * param.b % param.p;
                uint32_t shoup = shoup_ref(a, param.b, param.b_precomp, param.p);
                if (ref != shoup)
                {
                    fflush(stderr);
                    dup2(err, STDERR_FILENO);
                    ERROR("test_shoup");
                    fprintf(stderr, "result obtained %u instead of %u for %u * %u mod %u (multipliying %lu bits and %lu bits mod %lu bits)\n", shoup, ref, a, param.b, param.p, FLINT_BIT_COUNT(a), FLINT_BIT_COUNT(param.b), FLINT_BIT_COUNT(param.p));
                    ret = 1;
                    goto end;
                }
            }
    fflush(stdout);
    dup2(out, STDOUT_FILENO);
    SUCCESS("test_shoup");
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
