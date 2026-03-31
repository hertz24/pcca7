#include "test.h"

int main(void)
{
    for (ulong i = 0; i <= 33; i++)
        for (ulong j = 0; j <= 32; j++)
            for (ulong k = 0; k <= 32; k++)
            {
                Parameters param = rand_parameters(i, j);
                uint32_t a = n_randbits(state, k);
                uint32_t ref = (uint64_t)a * param.b % param.p;
                uint32_t shoup = shoup_ref(a, param.b, param.b_precomp, param.p);
                if (ref != shoup)
                {
                    ERROR("test_shoup");
                    fprintf(stderr, "result obtained %u instead of %u for %u * %u mod %u (multipliying %lu bits and %lu bits mod %lu bits)\n", shoup, ref, a, param.b, param.p, FLINT_BIT_COUNT(a), FLINT_BIT_COUNT(param.b), FLINT_BIT_COUNT(param.p));
                    return 1;
                }
            }
    SUCCESS("test_shoup");
    return 0;
}
