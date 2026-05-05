#include "../include/options.h"

int set_options(int argc, char const *argv[], Options *options)
{
    if (argc % 2 == 0)
        return ERR_INPUT;
    for (int i = 1; i < argc; i += 2)

        // Initializes each field based on the command-line
        if (*argv[i + 1] == '-')
            return ERR_NEGATIVE_NUMBER;
        else if (strcmp("-p", argv[i]) == 0)
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
                if (options->p_bits < 2 || options->p_bits > 31)
                    return INVALID_P_BITS;
                options->flags |= OPT_P_BITS;
            }
        }
        else if (strcmp("-b_bits", argv[i]) == 0)
        {
            if (!(options->flags & (OPT_B | OPT_B_BITS)))
            {
                options->b_bits = strtoul(argv[i + 1], NULL, 10);
                if (options->b_bits > 31)
                    return INVALID_B_BITS;
                options->flags |= OPT_B_BITS;
            }
        }
        else if (strcmp("-scale", argv[i]) == 0)
        {
            if (!(options->flags & OPT_SCALE))
            {
                options->scale = atoi(argv[i + 1]);
                options->flags |= OPT_SCALE;
            }
        }
        else if (strcmp("-pts", argv[i]) == 0)
        {
            if (!(options->flags & OPT_PTS))
            {
                options->points = strtoul(argv[i + 1], NULL, 10);
                if (options->points > MAX_NUMBER_PTS)
                    return INVALID_NUMBER_PTS;
                options->flags |= OPT_PTS;
            }
        }
        else
            return ERR_UNRECOGNIZED;
    return 0;
}

int init_param(Options *options, Parameters *param)
{
    unsigned char flags = options->flags;

    // If the option is passed as parameter
    uint32_t p = (flags & OPT_P) ? options->p : 0;
    uint32_t b = (flags & OPT_B) ? options->b : 0;
    ulong p_bits = (flags & OPT_P_BITS) ? options->p_bits : 0;
    ulong b_bits = (flags & OPT_B_BITS) ? options->b_bits : 0;

    ulong lower;
    switch (flags & (OPT_P | OPT_B | OPT_P_BITS | OPT_B_BITS))
    {
    case (OPT_P | OPT_B):
        if (b >= p)
            return ERR_B_GE_P;
        *param = init_parameters(b, p);
        break;
    case (OPT_P | OPT_B_BITS):
        if (FLINT_BIT_COUNT(p) < b_bits)
            return ERR_B_BITS_GE_P_BITS;

        // Only the case where p == 2 and b_bits == 2
        else if (FLINT_BIT_COUNT(p) == b_bits && p == 2)
            return ERR_NO_B_FOR_P;

        // Choose new b while b >= p
        do
        {
            b = n_randbits(state, b_bits);
        } while (b >= p);
        *param = init_parameters(b, p);
        break;
    case OPT_P:
        *param = init_parameters(n_randint(state, p), p);
        break;
    case (OPT_B | OPT_P_BITS):
        if (FLINT_BIT_COUNT(b) > p_bits)
            return ERR_B_BITS_GE_P_BITS;

        // If there not exists a prime of p_bits bits such that p > b
        if (b >= max_prime_bits(p_bits))
            return ERR_NO_PRIME_FOR_BITS;
        do
        {
            p = rand_prime(p_bits);
        } while (p <= b);
        *param = init_parameters(b, p);
        break;
    case OPT_B:
        if (b >= max_prime_bits(31))
            return ERR_NO_PRIME_FOR_BITS;
        ulong bits = FLINT_BIT_COUNT(b);
        lower = (bits < 2) ? 2 : bits;
        do
        {
            p = rand_prime(lower + n_randint(state, 32 - lower));
        } while (p <= b);
        *param = init_parameters(b, p);
        break;
    case (OPT_P_BITS | OPT_B_BITS):
        if (p_bits < b_bits)
            return ERR_B_BITS_GE_P_BITS;
        *param = rand_parameters(b_bits, p_bits);
        break;
    case OPT_P_BITS:
        *param = rand_parameters(n_randint(state, p_bits + 1), p_bits);
        break;
    case OPT_B_BITS:
        lower = (b_bits < 2) ? 2 : b_bits;
        do
        {
            p_bits = lower + n_randint(state, 32 - lower);
        } while (p_bits <= b_bits);
        *param = rand_parameters(b_bits, p_bits);
        break;
    default:
        p_bits = 2 + n_randint(state, 30);
        b_bits = n_randint(state, p_bits + 1);
        *param = rand_parameters(b_bits, p_bits);
        break;
    }
    return 0;
}
