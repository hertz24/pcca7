#include "../include/option.h"

int set_options(int argc, char const *argv[], Options *options)
{
    if (argc % 2 == 0)
        return ERR_INPUT;
    for (int i = 1; i < argc; i += 2)
        if (strcmp("-p", argv[i]) == 0)
        {
            if (!(options->flags & (OPT_P | OPT_P_BITS)))
            {
                options->p = atoi(argv[i + 1]);
                if (!n_is_prime(options->p))
                    return ERR_PRIME;
                options->flags |= OPT_P;
            }
        }
        else if (strcmp("-b", argv[i]) == 0)
        {
            if ((options->flags & (OPT_B | OPT_B_BITS)) == 0)
            {
                options->b = atoi(argv[i + 1]);
                options->flags |= OPT_B;
            }
        }
        else if (strcmp("-p_bits", argv[i]) == 0)
        {
            if ((options->flags & (OPT_P | OPT_P_BITS)) == 0)
            {
                options->p_bits = strtoul(argv[i + 1], NULL, 10);
                options->flags |= OPT_P_BITS;
            }
        }
        else if (strcmp("-b_bits", argv[i]) == 0)
        {
            if (!(options->flags & (OPT_B | OPT_B_BITS)))
            {
                options->b_bits = strtoul(argv[i + 1], NULL, 10);
                options->flags |= OPT_B_BITS;
            }
        }
        else if (strcmp("-scale", argv[i]) == 0)
            options->scale = atoi(argv[i + 1]);
        else if (strcmp("-pts", argv[i]) == 0)
            options->points = strtoul(argv[i + 1], NULL, 10);
        else
            return ERR_UNRECOGNIZED;
    return 0;
}

int init_param(Options *options, Parameters *param)
{
    unsigned char flags = options->flags;
    uint32_t p = options->p;
    uint32_t b = options->b;
    ulong p_bits = options->p_bits;
    ulong b_bits = options->b_bits;
    switch (flags & (OPT_P | OPT_B | OPT_P_BITS | OPT_B_BITS))
    {
    case (OPT_P | OPT_B):
        if (b >= p)
            return ERR_B_GE_P;
        *param = init_parameters(b, p);
        break;
    case (OPT_P | OPT_B_BITS):
        if (FLINT_BIT_COUNT(p) < b_bits)
            return ERR_BBITS_GE_PBITS;
        do
        {
            b = n_randbits(state, b_bits);
        } while (b >= p);
        *param = init_parameters(b, p);
        break;
    case OPT_P:
        *param = init_parameters(_n_randint(state, p), p);
        break;
    case (OPT_B | OPT_P_BITS):
        if (p_bits > 1 && FLINT_BIT_COUNT(b) > p_bits)
            return ERR_BBITS_GE_PBITS;
        if (p_bits > 1 && p_bits < 32 && b >= max_prime_bits(p_bits))
            return ERR_NO_PRIME_FOR_BITS;
        do
        {
            p = rand_prime(p_bits);
        } while (p <= b);
        *param = init_parameters(b, p);
        break;
    case OPT_B:
        ulong bits = FLINT_BIT_COUNT(b);
        ulong lower = (bits < 2) ? 2 : bits;
        do
        {
            p = rand_prime(lower + _n_randint(state, 32 - lower));
        } while (p <= b);
        *param = init_parameters(b, p);
        break;
    case (OPT_P_BITS | OPT_B_BITS):
        if (p_bits >= 2 && p_bits <= 32 && b_bits <= 32 && p_bits < b_bits)
            return ERR_BBITS_GE_PBITS;
        *param = rand_parameters(b_bits, p_bits);
        break;
    case OPT_P_BITS:
        *param = rand_parameters(33, p_bits);
        break;
    case OPT_B_BITS:
        *param = rand_parameters(b_bits, 0);
        break;
    default:
        p = rand_prime(0);
        *param = init_parameters(_n_randint(state, p), p);
        break;
    }
    return 0;
}

int generate_graphs(Options options, Parameters param)
{
    ulong scale = options.scale;
    ulong points = options.points;
    Algorithm graph1[] = {algorithms[0], algorithms[1], algorithms[2]
#if NEON || AVX2
                          ,
                          algorithms[3]
#endif
#if AVX512
                          ,
                          algorithms[7]
#endif
    };
    int ret = generate_graph(scale, points, param, graph1, TAB_SIZE(graph1));
#if NEON || AVX2
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[4]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[5]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[6]}, 2);
#endif
#if AVX512
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[7], algorithms[8]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[7], algorithms[9]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[7], algorithms[10]}, 2);
#endif
    return ret ? ERR_GEN_GRAPHS : 0;
}
