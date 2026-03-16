#include <time.h>

#include "../include/shoup.h"
#include "../include/utils.h"

#if AVX512
#define NB_ALGO 4
#else
#define NB_ALGO 3
#endif

#define SIZE 100

int main(void)
{
    srand(time(NULL));
    char instruction[5] =
#if NEON
        "NEON";
#else
        "AVX2";
#endif
    Parameters (*functions[2])(ulong) = {rand_parameters, rand_parameters_b};
    for (int i = 0; i <= 1; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = functions[i](j);
            Vector rand_v = rand_vector(SIZE);

            // Always correct
            Vector ref = naive_scale(param, rand_v);

            Vector results[NB_ALGO] = {
                shoup_scale_ref(param, rand_v),
#if NEON
                shoup_scale_neon(param, rand_v),
                shoup_scale_mullo_neon(param, rand_v)
#else
                shoup_scale_avx2(param, rand_v),
                shoup_scale_mullo_avx2(param, rand_v)
#endif
#if AVX512
                    ,
                shoup_scale_avx512(param, rand_v)
#endif
            };
            int error = 0;
            for (int k = 0; k < NB_ALGO; k++)
            {
                int index = compare_vectors(ref, results[k]);
                if (index != SIZE)
                {
                    if (!error)
                        printf("test_equality...\t\e[1m\033[31merror\033[0m\e[m:\n");
                    error = 1;
                    printf("\t- When multiplying %lu bits and %lu bits for ", nb_bits(*(rand_v.elements + index)), j);
                    switch (k)
                    {
                    case 0:
                        printf("shoup_scale_ref\n");
                        break;
                    case 1:
                        printf("shoup_scale (%s)\n", instruction);
                        break;
                    case 2:
                        printf("shoup_scale_mullo (%s)\n", instruction);
                        break;
                    case 3:
                        printf("shoup_scale_avx512\n");
                        break;
                    }
                }
            }
            free_vector(ref);
            for (int k = 0; k < NB_ALGO; k++)
                free_vector(results[k]);
            if (i == 0)
                j++;
            if (error)
                return 1;
        }
    printf("test_equality...\t\e[1m\033[32mno error\033[0m\e[m\n");
    return 0;
}
