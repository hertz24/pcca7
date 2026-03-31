#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_equality: the print statements in the functions under test are not silenced.\n");
    rand_init();
    for (ulong i = 0; i <= 32; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            Parameters param = rand_parameters(i, j);
            if (i > j && i <= 31 && j >= 2 && j <= 31 && param.p != 0)
            {
                FAIL("test_param", &err);
                fprintf(stderr, "param should be {0}.\n");
                ret = 1;
                goto end;
            }
            else if (param.p == 0)
                continue;
            Vector rand_v = rand_vector(VECTOR_SIZE);

            // Always correct for naive algorithm
            Vector ref = algorithms[0].address(param, rand_v);

            int error = 0;
            for (int k = 1; k < NB_ALGO; k++)
            {
                if ((k == 6 || k == NB_ALGO - 1))
                    continue;
                Vector result = algorithms[k].address(param, rand_v);
                int index = compare_vectors(ref, result);
                if (index != VECTOR_SIZE)
                {
                    if (!error)
                    {
                        FAIL("test_equality", &err);
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
            {
                ret = 1;
                goto end;
            }
        }
    SUCCESS("test_equality", &out);
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
