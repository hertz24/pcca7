#include <time.h>

#include "test.h"
#include "../include/parameters.h"

int main(void)
{
    srand(time(NULL));
    uint32_t p;
    for (ulong i = 0; i <= 32; i++)
        if (!n_is_prime((p = rand_prime(i))))
        {
            ERROR("n_is_prime");
            fprintf(stderr, "%d isn't a prime number.\n", p);
            return 1;
        }
    SUCCESS("n_is_prime");
    return 0;
}
