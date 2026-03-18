#include <time.h>

#include "test.h"
#include "../include/parameters.h"

int main(void)
{
    srand(time(NULL));
    uint32_t p;
    if (!n_is_prime((p = rand_prime())))
    {
        ERROR("n_is_prime");
        fprintf(stderr, "%d isn't a prime number.\n", p);
        return 1;
    }
    SUCCESS("n_is_prime");
    return 0;
}
