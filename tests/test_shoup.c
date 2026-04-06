#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_shoup: the print statements in the functions under test are not silenced.\n");
    rand_init();
    for (int i = 0; i < NB_TESTS; i++)
        for (ulong j = 0; j <= 32; j++)
            for (ulong k = 0; k <= 32; k++)
                for (ulong l = 0; l <= 32; l++)
                {
                    Parameters param = rand_parameters(j, k);
                    if (j > k && j <= 31 && k >= 2 && k <= 31)
                        continue;
                    uint32_t a = n_randbits(state, l);
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
