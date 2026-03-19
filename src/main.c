#include <time.h>

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
    ulong bits = 0;
    for (int i = 1; i < argc; i += 2)
        if (strcmp("-p", argv[i]) == 0)
        {
            if ((opts & (1 | 4 | 8)) == 0)
            {
                p = atoi(argv[i + 1]);
                if (!n_is_prime(p))
                {
                    fprintf(stderr, "p must be a prime number.\n");
                    return 1;
                }
                opts |= 1;
            }
        }
        else if (strcmp("-b", argv[i]) == 0)
        {
            if ((opts & (2 | 4 | 8)) == 0)
            {
                b = atoi(argv[i + 1]);
                opts |= 2;
            }
        }
        else if (strcmp("-p_bits", argv[i]) == 0)
        {
            if (opts == 0)
            {
                bits = strtoul(argv[i + 1], NULL, 10);
                opts |= 4;
            }
        }
        else if (strcmp("-b_bits", argv[i]) == 0)
        {
            if (opts == 0)
            {
                bits = strtoul(argv[i + 1], NULL, 10);
                opts |= 8;
            }
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
    case 4:
        param = rand_parameters(bits);
        break;
    case 1:
        b = n_randint(state, p);
        break;
    case 2:
        ulong b_bits = nb_bits(b);
        ulong limit = (33 - b_bits);
        do
        {
            p = n_randprime(state, b_bits + rand() % limit, 1);
        } while (p <= b);
        break;
    case 8:
        param = rand_parameters_b(bits);
        break;
    }
    FLINT_TEST_CLEAR(state);
    if ((opts & (1 | 2)))
    {
        if (b >= p)
        {
            fprintf(stderr, "b must be less than p.\n");
            return 1;
        }
        param = init_parameters(b, p);
    }
    Algorithm graph1[] = {algorithms[0], algorithms[1], algorithms[2]
#if NEON || AVX2
                          ,
                          algorithms[3]
#endif
#if AVX512
                          ,
                          algorithms[6]
#endif
    };
    int ret = generate_curve(scale, points, param, graph1, TAB_SIZE(graph1));
#if NEON || AVX2
    ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[3], algorithms[4]}, 2);
    ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[3], algorithms[5]}, 2);
    if (b == 1)
        ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[3], algorithms[6]}, 2);
#endif
#if AVX512
    ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[7], algorithms[8]}, 2);
#endif
    return ret;
}
