#include "../include/options.h"

int set_options(int argc, char const *argv[], Options *options)
{
    if (argc % 2 == 0)
        return ERR_INPUT;
    for (int i = 1; i < argc; i += 2)

        // Initializes each field based on the command-line
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
            if (!(options->flags & (OPT_B | OPT_B_BITS)))
            {
                options->b = atoi(argv[i + 1]);
                options->flags |= OPT_B;
            }
        }
        else if (strcmp("-p_bits", argv[i]) == 0)
        {
            if (!(options->flags & (OPT_P | OPT_P_BITS)))
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
        else if (strcmp("-bit", argv[i]) == 0)
        {
            int bits = atoi(argv[i + 1]);
            if (bits != 16 && bits != 32)
                return ERR_BITS;
            options->bits = bits;
        }
        else
            return ERR_UNRECOGNIZED;
    return 0;
}

#define DEFINE_INIT_PARAM(BIT)                                                    \
    int init_param_##BIT(Options *options, Parameters *param)                     \
    {                                                                             \
        unsigned char flags = options->flags;                                     \
                                                                                  \
        /* If the option is passed as parameter */                                \
        uint32_t p = (flags & OPT_P) ? options->p : 0;                            \
        uint32_t b = (flags & OPT_B) ? options->b : 0;                            \
        ulong p_bits = (flags & OPT_P_BITS) ? options->p_bits : 0;                \
        ulong b_bits = (flags & OPT_B_BITS) ? options->b_bits : 0;                \
                                                                                  \
        switch (flags & (OPT_P | OPT_B | OPT_P_BITS | OPT_B_BITS))                \
        {                                                                         \
        case (OPT_P | OPT_B):                                                     \
            if (b >= p)                                                           \
                return ERR_B_GE_P;                                                \
            param->param_##BIT = init_parameters_##BIT(b, p);                     \
            break;                                                                \
        case (OPT_P | OPT_B_BITS):                                                \
            if (FLINT_BIT_COUNT(p) < b_bits)                                      \
                return ERR_BBITS_GE_PBITS;                                        \
                                                                                  \
            /* Choose new b while b >= p */                                       \
            do                                                                    \
            {                                                                     \
                b = n_randbits(state, b_bits);                                    \
            } while (b >= p);                                                     \
            param->param_##BIT = init_parameters_##BIT(b, p);                     \
            break;                                                                \
        case OPT_P:                                                               \
            param->param_##BIT = init_parameters_##BIT(n_randint(state, p), p);   \
            break;                                                                \
        case (OPT_B | OPT_P_BITS):                                                \
            if (p_bits > 1 && FLINT_BIT_COUNT(b) > p_bits)                        \
                return ERR_BBITS_GE_PBITS;                                        \
                                                                                  \
            /* If there not exists a prime of p_bits bits such that p > b */      \
            if (p_bits > 1 && p_bits < BIT && b >= max_prime_bits(p_bits))        \
                return ERR_NO_PRIME_FOR_BITS;                                     \
            do                                                                    \
            {                                                                     \
                p = rand_prime(p_bits);                                           \
            } while (p <= b);                                                     \
            param->param_##BIT = init_parameters_##BIT(b, p);                     \
            break;                                                                \
        case OPT_B:                                                               \
            ulong bits = FLINT_BIT_COUNT(b);                                      \
            ulong lower = (bits < 2) ? 2 : bits;                                  \
            do                                                                    \
            {                                                                     \
                p = rand_prime(lower + n_randint(state, BIT - lower));            \
            } while (p <= b);                                                     \
            param->param_##BIT = init_parameters_##BIT(b, p);                     \
            break;                                                                \
        case (OPT_P_BITS | OPT_B_BITS):                                           \
            if (p_bits >= 2 && p_bits <= BIT && b_bits <= BIT && p_bits < b_bits) \
                return ERR_BBITS_GE_PBITS;                                        \
            param->param_##BIT = rand_parameters_##BIT(b_bits, p_bits);           \
            break;                                                                \
        case OPT_P_BITS:                                                          \
            param->param_##BIT = rand_parameters_##BIT(BIT + 1, p_bits);          \
            break;                                                                \
        case OPT_B_BITS:                                                          \
            param->param_##BIT = rand_parameters_##BIT(b_bits, 0);                \
            break;                                                                \
        default:                                                                  \
            p = rand_prime(0);                                                    \
            param->param_##BIT = init_parameters_##BIT(n_randint(state, p), p);   \
            break;                                                                \
        }                                                                         \
        return 0;                                                                 \
    }

DEFINE_INIT_PARAM(16)
DEFINE_INIT_PARAM(32)
