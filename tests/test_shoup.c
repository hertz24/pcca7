#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_shoup: the print statements in the functions under test are not silenced.\n");
    rand_init();
    for (ulong i = 0; i <= 32; i++)
        for (ulong j = 0; j <= 32; j++)
            for (ulong k = 0; k <= 32; k++)
            {
                Parameters param = rand_parameters(i, j);
                if (i > j && i <= 31 && j >= 2 && j <= 31 && param.p != 0)
                {
                    FAIL("test_shoup", &err);
                    fprintf(stderr, "param should be {0}.\n");
                    ret = 1;
                    goto end;
                }
                else if (param.p == 0)
                    continue;
                uint32_t a = n_randbits(state, k);
                uint32_t ref = (uint64_t)a * param.b % param.p;
                uint32_t shoup = shoup_ref(a, param.b, param.b_precomp, param.p);
                if (ref != shoup)
                {
                    FAIL("test_shoup", &err);
                    fprintf(stderr, "result obtained %u instead of %u for %u * %u mod %u (multipliying %lu bits and %lu bits mod %lu bits)\n", shoup, ref, a, param.b, param.p, FLINT_BIT_COUNT(a), FLINT_BIT_COUNT(param.b), FLINT_BIT_COUNT(param.p));
                    ret = 1;
                    goto end;
                }
            }
    SUCCESS("test_shoup", &out);
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
