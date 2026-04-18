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
    Algorithm graph1[] = {algorithms[NAIVE_SCALE], algorithms[SHOUP_SCALE_REF], algorithms[SHOUP_SCALE_FLINT]
#if NEON
                          ,
                          algorithms[SHOUP_SCALE_NEON]
#elif AVX2
                          ,
                          algorithms[SHOUP_SCALE_AVX2]
#endif
#if AVX512
                          ,
                          algorithms[SHOUP_SCALE_AVX512]
#endif
    };
    int ret = generate_graph(scale, points, param, graph1, TAB_SIZE(graph1));
#if NEON
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_NEON], algorithms[UNROLLING_SHOUP_SCALE_NEON]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_NEON], algorithms[SHOUP_SCALE_MULLO_NEON]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_NEON], algorithms[SHOUP_B1_SCALE_NEON]}, 2);
#elif AVX2
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_AVX2], algorithms[UNROLLING_SHOUP_SCALE_AVX2]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_AVX2], algorithms[SHOUP_SCALE_MULLO_AVX2]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_MULLO_AVX2], algorithms[SHOUP_SCALE_MULLO_V2_AVX2]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_AVX2], algorithms[SHOUP_B1_SCALE_AVX2]}, 2);
#endif
#if AVX512
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_AVX512], algorithms[UNROLLING_SHOUP_SCALE_AVX512]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_AVX512], algorithms[SHOUP_SCALE_MULLO_AVX512]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_MULLO_AVX512], algorithms[SHOUP_SCALE_MULLO_V2_AVX512]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[SHOUP_SCALE_AVX512], algorithms[SHOUP_B1_SCALE_AVX512]}, 2);
#endif
    return ret ? ERR_GEN_GRAPHS : 0;
}

int main(int argc, char const *argv[])
{
    rand_init();
    int ret;
    Options options = {.flags = 0, .scale = 1, .points = 100};
    Parameters param;
    if ((ret = set_options(argc, argv, &options)))
        goto end;
    if ((ret = init_param(&options, &param)))
        goto end;
    ret = generate_graphs(options, param);
end:
    rand_clear();
    if (ret)
        PRINT_ERROR(ret);
    return ret;
}
