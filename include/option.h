/**
 * @file option.h
 * @brief Managing command-line options and settings for graph generation.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#include <stdint.h>

#include "error.h"
#include "graph.h"

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
 * @param argv The array containing the command-line
 * @param[in, out] options The pointer to the @c Options structure which will be filled based on the command-line
 *
 * @retval 0 success
 * @retval diffent value depending on the error
 *
 * @see error.h
 */
int set_options(int argc, char const *argv[], Options *options);

/**
 * @brief Completes the options by generating the missing parameter.
 *
 * This function automatically generates either @c p or @c b depending on which one is already set in the options structure.
 *
 * @param[in, out] options The pointer to the @c Options structure
 *
 * @retval 0 success
 * @retval diffent value depending on the error
 *
 * @see error.h
 */
int init_param(Options *options, Parameters *param);

int generate_graphs(Options options, Parameters param);
