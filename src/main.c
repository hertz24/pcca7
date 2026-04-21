/**
 * @file main.c
 * @brief Main benchmark program for modular algorithms.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#include "../include/options.h"

static int generate_graphs(Options options, Parameters param)
{
    ulong scale = options.scale;
    ulong points = options.points;
    AlgorithmID graph_1[] = {NAIVE_SCALE, SHOUP_SCALE_REF, SHOUP_SCALE_FLINT
#if NEON
                             ,
                             SHOUP_SCALE_NEON
#elif AVX2
                             ,
                             SHOUP_SCALE_MULLO_V2_AVX2
#endif
#if AVX512
                             ,
                             SHOUP_SCALE_MULLO_V2_AVX512
#endif
    };

    // Continues to generate graphs even if there is an error
    int ret = generate_graph(scale, points, param, graph_1, TAB_SIZE(graph_1));
#if NEON
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_NEON, UNROLLING_SHOUP_SCALE_NEON}, 2);
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_NEON, SHOUP_SCALE_MULLO_NEON}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_NEON, SHOUP_B1_SCALE_NEON}, 2);
#elif AVX2
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_AVX2, UNROLLING_SHOUP_SCALE_AVX2}, 2);
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_AVX2, SHOUP_SCALE_MULLO_AVX2}, 2);
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_MULLO_AVX2, SHOUP_SCALE_MULLO_V2_AVX2}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_AVX2, SHOUP_B1_SCALE_AVX2}, 2);
#endif
#if AVX512
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_AVX512, UNROLLING_SHOUP_SCALE_AVX512}, 2);
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_AVX512, SHOUP_SCALE_MULLO_AVX512}, 2);
    ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_MULLO_AVX512, SHOUP_SCALE_MULLO_V2_AVX512}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (AlgorithmID[]){SHOUP_SCALE_AVX512, SHOUP_B1_SCALE_AVX512}, 2);
#endif
    return ret ? ERR_GEN_GRAPHS : 0;
}

int main(int argc, char const *argv[])
{
    rand_init();
    int ret = init_data_tab();
    if (ret)
        goto end;
    Options options = {.flags = 0, .scale = 1, .points = 100};
    Parameters param;
    if ((ret = set_options(argc, argv, &options)))
        goto end;
    if ((ret = init_param(&options, &param)))
        goto end;
    ret = generate_graphs(options, param);
end:
    free_data_tab();
    rand_clear();
    if (ret)
        PRINT_ERROR(ret);
    return ret;
}
