#include <time.h>

#include "../include/algo_registry.h"
#include "../include/utils.h"

#define SIZE 100

int main(void)
{
    srand(time(NULL));
    char instruction[5] =
#if NEON
        "NEON";
#elif AVX2
        "AVX2";
#else
        "\0";
#endif
    Parameters (*functions[2])(ulong) = {rand_parameters, rand_parameters_b};
    for (int i = 0; i <= 1; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = functions[i](j);
            Vector rand_v = rand_vector(SIZE);

            // Always correct
            Vector ref = algorithms[0](param, rand_v);

            int error = 0;
            for (int k = 1; k < NB_ALGO; k++)
            {
                Vector result = algorithms[k](param, rand_v);
                int index = compare_vectors(ref, result);
                if (index != SIZE)
                {
                    if (!error)
                        printf("test_equality...\t\e[1m\033[31merror\033[0m\e[m:\n");
                    error = 1;
                    printf("\t- When multiplying %lu bits and %lu bits for ", nb_bits(*(rand_v.elements + index)), j);
                    switch (k)
                    {
                    case 1:
                        printf("shoup_scale_ref\n");
                        break;
                    case 2:
                        printf("shoup_scale_flint\n");
                        break;
                    case 3:
                        printf("shoup_scale (%s)\n", instruction);
                        break;
                    case 4:
                        printf("shoup_scale_mullo (%s)\n", instruction);
                        break;
                    case 5:
                        printf("shoup_scale_avx512\n");
                        break;
                    }
                }
            }
            free_vector(ref);
            if (i == 0)
                j++;
            if (error)
                return 1;
        }
    printf("test_equality...\t\e[1m\033[32mno error\033[0m\e[m\n");
    return 0;
}
