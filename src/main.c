/**
 * @file main.c
 * @brief Main benchmark program for modular algorithms.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#include <time.h>

#include "../include/graph.h"

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
    ulong p_bits;
    ulong b_bits;
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
            if (!(options->flags & (OPT_P | OPT_P_BITS)))
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
        {
            fprintf(stderr, "b must be less than p.\n");
            return 1;
        }
        else
            *param = init_parameters(b, p);
        break;
    case (OPT_P | OPT_B_BITS):
        if (nb_bits(p) < b_bits)
        {
            fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n");
            return 1;
        }
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
        if (nb_bits(b) > p_bits)
        {
            fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n");
            return 1;
        }
        do
        {
            p = n_randbits(state, p_bits);
        } while (p <= b);
        *param = init_parameters(b, p);
        break;
    case OPT_B:
        ulong bits = nb_bits(b);
        uint32_t p;
        ulong lower = (bits < 2) ? 2 : bits;
        do
        {
            p = n_randprime(state, lower + rand() % (33 - lower), 1);
        } while (p <= b);
        *param = init_parameters(b, p);
        break;
    case (OPT_P_BITS | OPT_B_BITS):
        *param = rand_parameters(p_bits, b_bits);
        break;
    case OPT_P_BITS:
        *param = rand_parameters_p(p_bits);
        break;
    case OPT_B_BITS:
        *param = rand_parameters_b(b_bits);
        break;
    default:
        *param = rand_parameters_p(0);
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
#endif
    return ret;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    rand_init();
    if (argc % 2 == 0)
    {
        fprintf(stderr, "Error input arguments.\n");
        return 1;
    }
    int ret;
    Options options = {0, 0, 0, 1, 100, 0, 0};
    Parameters param;
    if ((ret = set_options(argc, argv, &options)))
        goto end;
    if ((ret = init_param(&options, &param)))
        goto end;
    ret = generate_graphs(options, param);
end:
    return ret;
}
