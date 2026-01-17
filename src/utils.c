#include "../include/utils.h"

int is_prime(uint64_t p)
{
    if (p <= 1)
        return 0;
    for (uint32_t i = 2; i * i <= p; i++)
        if (p % i == 0)
            return 0;
    return 1;
}