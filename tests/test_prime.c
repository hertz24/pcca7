#include "test.h"

int main(void)
{
    srand(time(NULL));
    uint32_t p;
    for (ulong i = 0; i <= 31; i++)
    {
        if (!n_is_prime((p = rand_prime(i))))
        {
            ERROR("n_is_prime");
            fprintf(stderr, "%d isn't a prime number.\n", p);
            return 1;
        }
        if (p >= (1UL << 31))
        {
            ERROR("n_is_prime");
            fprintf(stderr, "%d is greater than or equal to 2^{31}.\n", p);
            return 1;
        }
    }
    SUCCESS("n_is_prime");
    return 0;
}
