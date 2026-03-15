#include <time.h>

#include "../include/shoup.h"
#include "../include/utils.h"

#define SIZE 100

int main(void)
{
    srand(time(NULL));
    Parameters (*functions[2])(ulong) = {rand_parameters, rand_parameters_b};
    for (int i = 0; i <= 1; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = functions[i](j);
            Vector rand_v = rand_vector(SIZE);

            // Always correct
            Vector ref = naive_scale(param, rand_v);

            Vector results[4] = {
                shoup_scale_ref(param, rand_v),
                shoup_scale(param, rand_v), shoup_scale_mullo(param, rand_v), shoup_scale_avx512(param, rand_v)};
            int error = 0;
            for (int k = 0; k < 4; k++)
            {
                int index = compare_vectors(ref, results[k]);
                if (index != SIZE)
                {
                    error = 1;
                    printf("test_equality: error when multiplying %lu bits and %lu bits for ", nb_bits(*(rand_v.elements + index)), j);
                    switch (k)
                    {
                    case 0:
                        printf("shoup_scale_ref\n");
                        break;
                    case 1:
                        printf("shoup_scale\n");
                        break;
                    case 2:
                        printf("shoup_scale_mullo\n");
                        break;
                    case 3:
                        printf("shoup_scale_avx512\n");
                        break;
                    }
                }
            }
            free_vector(ref);
            for (int k = 0; k < 4; k++)
                free_vector(results[k]);
            if (i == 0)
                j++;
            if (error)
                return 1;
        }
    printf("test_equality: no error\n");
    return 0;
}
