#include "test.h"

int main(void)
{
    Parameters (*functions[2])(ulong) = {rand_parameters_p, rand_parameters_b};
    for (int i = 0; i <= 2; i++)
        for (ulong j = 0; j <= 31; j++)
        {
            Parameters param = (i < 2) ? functions[i](j) : init_parameters(1, rand_prime(j));
            Vector rand_v = rand_vector(VECTOR_SIZE);

            // Always correct for naive algorithm
            Vector ref = algorithms[0].address(param, rand_v);

            int error = 0;
            for (int k = 1; k < NB_ALGO; k++)
            {
                if ((k == 6 || k == NB_ALGO - 1) && i < 2)
                    continue;
                Vector result = algorithms[k].address(param, rand_v);
                int index = compare_vectors(ref, result);
                if (index != VECTOR_SIZE)
                {
                    if (!error)
                    {
                        ERROR("test_equality");
                        fprintf(stderr, "\n");
                    }
                    error = 1;
                    fprintf(stderr, "\t- When multiplying %lu bits and %lu bits for %s\n", FLINT_BIT_COUNT(*(rand_v.elements + index)), FLINT_BIT_COUNT(param.b), algorithms[k].name);
                }
                free_vector(result);
            }
            free_vector(rand_v);
            free_vector(ref);
            if (error)
                return 1;
        }
    SUCCESS("test_equality");
    return 0;
}
