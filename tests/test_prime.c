#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_prime: the print statements in the functions under test are not silenced.\n");
    rand_init();
    for (int i = 0; i < NB_TESTS; i++)
        for (ulong j = 0; j <= 64; j++)
        {
            uint32_t p = rand_prime(j);
            if (((j < 2 || j > 32) && p != 0) || ((j >= 2 && j <= 32) && !n_is_prime(p)))
            {
                FAIL("test_prime", &err);
                if ((j < 2 || j > 32) || p != 0)
                    fprintf(stderr, "%lu is not in [2, 32] and must return 0.\n", j);
                fprintf(stderr, "%d isn't a prime number.\n", p);
                ret = 1;
                goto end;
            }
        }
    SUCCESS("test_prime", &out);
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
