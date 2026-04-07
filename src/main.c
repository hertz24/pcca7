/**
 * @file main.c
 * @brief Main benchmark program for modular algorithms.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#include "../include/options.h"
#include "../include/shoup.h"

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
                          algorithms[7]
#endif
    };
    int ret = 0;
#if NEON || AVX2
    // ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[4]}, 2);
    // ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[5]}, 2);
    ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[5], algorithms[6]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[3], algorithms[6]}, 2);
#endif
#if AVX512
    // ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[7], algorithms[8]}, 2);
    // ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[7], algorithms[9]}, 2);
    if (options.b == 1)
        ret |= generate_graph(scale, points, param, (Algorithm[]){algorithms[7], algorithms[10]}, 2);
#endif
    return ret ? ERR_GEN_GRAPHS : 0;
}

int main(int argc, char const *argv[])
{
    rand_init();
    int ret;
    Options options = {.flags = 0, .scale = 1, .points = 10000};
    Parameters param;
    Vector v = rand_vector(100000);
    print_vector(v);
    if ((ret = set_options(argc, argv, &options)))
        goto end;
    if ((ret = init_param(&options, &param)))
        goto end;
    Vector ref = naive_scale(param, v);
    Vector res = shoup_scale_mullo_avx2_v2(param, v);
    printf("----V2---\n");
    shoup_scale_mullo_avx2(param,v);
    print_vector(ref);
    print_vector(res);
    fprintf(stdout, "%s\n", algorithms[6].name);
    // ret = generate_graphs(options, param);

end:
    rand_clear();
    if (ret)
        PRINT_ERROR(ret);
    return ret;
}
