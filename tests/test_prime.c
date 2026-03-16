#include <time.h>

#include "../include/parameters.h"

int main(void)
{
    srand(time(NULL));
    uint32_t p;
    if (!n_is_prime((p = rand_prime())))
    {
        printf("n_is_prime...\t\e[1m\033[31merror\033[0m\e[m: %d isn't a prime number.\n", p);
        return 1;
    }
    printf("test_prime...\t\e[1m\033[32mno error\033[0m\e[m\n");
    return 0;
}
