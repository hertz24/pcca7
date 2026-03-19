#include <time.h>

#include "test.h"
#include "../include/algo_registry.h"
#include "../include/utils.h"

int main(void)
{
    srand(time(NULL));
    Parameters (*functions[2])(ulong) = {rand_parameters, rand_parameters_b};
    for (int i = 0; i <= 1; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = functions[i](j);
            Vector rand_v = rand_vector(VECTOR_SIZE);

            // Always correct for naive algorithm
            Vector ref = algorithms[0].address(param, rand_v);

            int error = 0;
            for (int k = 1; k < NB_ALGO; k++)
            {
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
                    fprintf(stderr, "\t- When multiplying %lu bits and %lu bits for %s\n", nb_bits(*(rand_v.elements + index)), nb_bits(param.b), algorithms[k].name);
                }
                free_vector(result);
            }
            free_vector(rand_v);
            free_vector(ref);
            if (i == 0)
                j++;
            if (error)
                return 1;
        }
    SUCCESS("test_equality");
    return 0;
}
