#include <time.h>

#include "../include/parameters.h"

int main(void)
{
    srand(time(NULL));
    uint32_t p;
    if (!n_is_prime((p = rand_prime())))
    {
        printf("n_is_prime error: %d isn't a prime number.\n", p);
        return 1;
    }
    printf("test_prime: no error\n");
    return 0;
}
