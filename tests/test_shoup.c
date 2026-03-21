#include <time.h>

#include "test.h"
#include "../include/shoup.h"
#include "../include/utils.h"

int main(void)
{
    srand(time(NULL));
    Parameters (*functions[2])(ulong) = {rand_parameters, rand_parameters_b};
    FLINT_TEST_INIT(state);
    for (int i = 0; i <= 1; i++)
        for (ulong j = 0; j <= 32; j++)
            for (ulong k = 0; k <= 32; k++)
            {
                Parameters param = functions[i](j);
                uint32_t a = n_randbits(state, k);
                uint32_t ref = (uint64_t)a * param.b % param.p;
                uint32_t shoup = shoup_ref(a, param.b, param.b_precomp, param.p);
                if (ref != shoup)
                {
                    ERROR("test_shoup");
                    fprintf(stderr, "result obtained %u instead of %u for %u * %u mod %u (multipliying %lu bits and %lu bits mod %lu bits)\n", shoup, ref, a, param.b, param.p, nb_bits(a), nb_bits(param.b), nb_bits(param.p));
                    return 1;
                }
            }
    FLINT_TEST_CLEAR(state);
    SUCCESS("test_shoup");
    return 0;
}
