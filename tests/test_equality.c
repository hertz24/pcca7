#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_equality: the print statements in the functions under test are not silenced.\n");
    rand_init();
    for (int i = 0; i < NB_TESTS; i++)
        for (ulong j = 0; j <= 32; j++)
            for (ulong k = 0; k <= 32; k++)
            {
                Parameters param = rand_parameters(j, k);
                if (j > k && j <= 31 && k >= 2 && k <= 31)
                    continue;
                Vector rand_v = rand_vector(VECTOR_SIZE);

                // Always correct for naive algorithm
                Vector ref = algorithms[0].address(param, rand_v);

                int error = 0;
                for (int l = 1; l < NB_ALGO; l++)
                {
                    if ((l == 7 || l == NB_ALGO - 1))
                        continue;
                    Vector result = algorithms[l].address(param, rand_v);
                    int index = compare_vectors(ref, result);
                    if (index != VECTOR_SIZE)
                    {
                        if (!error)
                        {
                            FAIL("test_equality", &err);
                            fprintf(stderr, "\n");
                        }
                        error = 1;
                        fprintf(stderr, "\tFor ");
                        print_param(stderr, param);
                        fprintf(stderr, "\t\texpected: ");
                        print_vector(stderr, ref);
                        fprintf(stderr, "\t\tobtained: ");
                        print_vector(stderr, result);
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
