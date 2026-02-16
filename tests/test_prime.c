#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"

int main(void)
{
    srand(time(NULL));
    int n = 100;
    uint32_t p;
    if (!n_is_prime((p = rand_prime(n))))
    {
        printf("n_is_prime error: %d isn't a prime number.\n", p);
        return 1;
    }
    return 0;
}
