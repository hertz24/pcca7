#include "test.h"

int main(void)
{
    int ret = 0;
    int out = dup(STDOUT_FILENO);
    int err = dup(STDERR_FILENO);
    int null = open("/dev/null", O_WRONLY);
    dup2(null, STDOUT_FILENO);
    dup2(null, STDERR_FILENO);
    close(null);
    rand_init();
    for (ulong i = 0; i <= 32; i++)
        for (ulong j = i; j <= 32; j++)
        {
            Parameters param = rand_parameters(i, j);
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
                        fflush(stderr);
                        dup2(err, STDERR_FILENO);
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
            {
                ret = 1;
                goto end;
            }
        }
    fflush(stdout);
    dup2(out, STDOUT_FILENO);
    SUCCESS("test_equality");
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
