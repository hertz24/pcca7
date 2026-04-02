#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_prime: the print statements in the functions under test are not silenced.\n");
    rand_init();
    uint32_t p;
    for (int i = 0; i < NB_TESTS; i++)
        for (ulong j = 0; j <= 32; j++)
        {
            if (!n_is_prime((p = rand_prime(j))))
            {
                FAIL("test_prime", &err);
                fprintf(stderr, "%d isn't a prime number.\n", p);
                ret = 1;
                goto end;
            }
            if (p >= (1UL << 31))
            {
                FAIL("test_prime", &err);
                fprintf(stderr, "%d is greater than or equal to 2^{31}.\n", p);
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
