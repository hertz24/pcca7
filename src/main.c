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
    Algorithm graph1[] = {algorithms[0], algorithms[1], algorithms[2]
#if NEON || AVX2
                          ,
                          algorithms[3]
#endif
#if AVX512
                          ,
                          algorithms[8]
#endif
    };
    int ret = generate_graph(scale, points, param, graph1, TAB_SIZE(graph1));
#if NEON || AVX2
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[4]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[5]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[5], algorithms[6]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[7]}, 2);
#endif
#if AVX512
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[8], algorithms[9]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[8], algorithms[10]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[10], algorithms[11]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[8], algorithms[12]}, 2);
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
