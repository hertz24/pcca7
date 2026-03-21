#include <time.h>

#include "../include/curve.h"

#define OPT_P 1
#define OPT_B 2
#define OPT_P_BITS 4
#define OPT_B_BITS 8

typedef struct
{
    uint32_t p;
    uint32_t b;
    unsigned char flags;
    int scale;
    ulong points;
    ulong bits;
} Options;

/**
 * @brief Initializes the struct of @p options
 *
 * @param argc The number of elements in @p argv
 * @param argv The array containing the command line
 * @param[in, out] options The pointer to the @c Options structure which will be filled based on the command line
 *
 * @retval 0 success
 * @retval 1 error
 */
int set_options(int argc, char const *argv[], Options *options)
{
    for (int i = 1; i < argc; i += 2)
        if (strcmp("-p", argv[i]) == 0)
        {
            if ((options->flags & (OPT_P | OPT_P_BITS | OPT_B_BITS)) == 0)
            {
                options->p = atoi(argv[i + 1]);
                if (!n_is_prime(options->p))
                {
                    fprintf(stderr, "p must be a prime number.\n");
                    return 1;
                }
                options->flags |= OPT_P;
            }
        }
        else if (strcmp("-b", argv[i]) == 0)
        {
            if ((options->flags & (OPT_B | OPT_P_BITS | OPT_B_BITS)) == 0)
            {
                options->b = atoi(argv[i + 1]);
                options->flags |= OPT_B;
            }
        }
        else if (strcmp("-p_bits", argv[i]) == 0)
        {
            if (!options->flags)
            {
                options->bits = strtoul(argv[i + 1], NULL, 10);
                options->flags |= OPT_P_BITS;
            }
        }
        else if (strcmp("-b_bits", argv[i]) == 0)
        {
            if (options->flags == 0)
            {
                options->bits = strtoul(argv[i + 1], NULL, 10);
                options->flags |= OPT_B_BITS;
            }
        }
        else if (strcmp("-scale", argv[i]) == 0)
            options->scale = atoi(argv[i + 1]);
        else if (strcmp("-pts", argv[i]) == 0)
            options->points = strtoul(argv[i + 1], NULL, 10);
        else
        {
            fprintf(stderr, "Option %s unrecognized.\n", argv[i]);
            return 1;
        }
    return 0;
}

/**
 * @brief Completes the options by generating the missing parameter.
 *
 * This function automatically generates either @c p or @c b depending on which one is already set in the options structure.
 *
 * @param[in, out] options The pointer to the @c Options structure
 *
 * @retval 0 success
 * @retval 1 error
 */
int complete_options(Options *options)
{
    FLINT_TEST_INIT(state);
    unsigned char flags = options->flags & (OPT_P | OPT_B);
    if (flags == OPT_P)
        options->b = n_randint(state, options->p);
    else if (flags == OPT_B)
    {
        ulong b_bits = nb_bits(options->b);
        ulong limit = (33 - b_bits);
        do
        {
            options->p = n_randprime(state, b_bits + rand() % limit, 1);
        } while (options->p <= options->b);
    }
    FLINT_TEST_CLEAR(state);
    if (options->b >= options->p)
    {
        fprintf(stderr, "b must be less than p.\n");
        return 1;
    }
    return 0;
}

Parameters init_param(Options options)
{
    unsigned char flags = options.flags;
    if (!flags || flags == OPT_P_BITS)
        return rand_parameters(options.bits);
    else if ((flags & (OPT_P | OPT_B)))
        return init_parameters(options.b, options.p);
    else
        return rand_parameters_b(options.bits);
}

int generate_graph(Options options, Parameters param)
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
    int ret = generate_curve(scale, points, param, graph1, TAB_SIZE(graph1));
#if NEON || AVX2
    ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[3], algorithms[4]}, 2);
    ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[3], algorithms[5]}, 2);
    if (options.b == 1)
        ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[3], algorithms[6]}, 2);
#endif
#if AVX512
    ret |= generate_curve(scale, points, param, (Algorithm[]){algorithms[7], algorithms[8]}, 2);
#endif
    return ret;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    if (argc % 2 == 0)
    {
        fprintf(stderr, "Error input arguments.\n");
        return 1;
    }
    int ret;
    Options options = {0, 0, 0, 1, 100, 0};
    if ((ret = set_options(argc, argv, &options)))
        goto end;
    if ((ret = complete_options(&options)))
        goto end;
    Parameters param = init_param(options);
    ret = generate_graph(options, param);
end:
    return ret;
}
