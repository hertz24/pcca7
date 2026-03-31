#include "test.h"

int main(void)
{
    int ret = 0;
    rand_init();
    uint32_t p;
    for (ulong i = 0; i <= 32; i++)
    {
        if (!n_is_prime((p = rand_prime(i))))
        {
            ERROR("n_is_prime");
            fprintf(stderr, "%d isn't a prime number.\n", p);
            ret = 1;
            goto end;
        }
        if (p >= (1UL << 31))
        {
            ERROR("n_is_prime");
            fprintf(stderr, "%d is greater than or equal to 2^{31}.\n", p);
            ret = 1;
            goto end;
        }
    }
    SUCCESS("n_is_prime");
end:
    rand_clear();
    return ret;
}
