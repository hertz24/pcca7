/**
 * @file main.c
 * @brief Main benchmark program for modular algorithms.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#include "../include/option.h"

int main(int argc, char const *argv[])
{
    rand_init();
    int ret;
    Options options = {0, 0, 0, 1, 100, 0, 0};
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
