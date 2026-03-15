#include "../include/curve.h"

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    if (argc % 2 == 0)
    {
        fprintf(stderr, "Error input arguments.\n");
        return 1;
    }
    uint32_t p = 0;
    uint32_t b = 0;
    unsigned char opts = 0;
    int scale = 1;
    ulong points = 100;
    for (int i = 1; i < argc; i += 2)
        if (strcmp("-p", argv[i]) == 0)
        {
            p = atoi(argv[i + 1]);
            if (!n_is_prime(p))
            {
                fprintf(stderr, "p must be a prime number.\n");
                return 1;
            }
            opts |= 1;
        }
        else if (strcmp("-b", argv[i]) == 0)
        {
            b = atoi(argv[i + 1]);
            opts |= 2;
        }
        else if (strcmp("-scale", argv[i]) == 0)
            scale = atoi(argv[i + 1]);
        else if (strcmp("-pts", argv[i]) == 0)
            points = strtoul(argv[i + 1], NULL, 10);
        else
        {
            fprintf(stderr, "Option %s unrecognized.\n", argv[i]);
            return 1;
        }
    Parameters param;
    FLINT_TEST_INIT(state);
    switch (opts)
    {
    case 0:
        param = rand_parameters(0);
        break;
    case 1:
        b = n_randint(state, p);
        break;
    case 2:
        ulong bits = nb_bits(b);
        ulong limit = (33 - bits);
        do
        {
            p = n_randprime(state, bits + rand() % limit, 1);
        } while (p <= b);
        break;
    }
    FLINT_TEST_CLEAR(state);
    if (opts != 0)
    {
        if (b >= p)
        {
            fprintf(stderr, "b must be less than p.\n");
            return 1;
        }
        param = init_parameters(b, p);
    }
    return generate_curve(scale, points, param);
}
