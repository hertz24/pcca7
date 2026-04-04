/**
 * @file options.h
 * @brief Managing command-line options and settings for graph generation.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#include <stdint.h>
#include <string.h>

#include "error.h"
#include "graph.h"

#define OPT_P 1
#define OPT_B 2
#define OPT_P_BITS 4
#define OPT_B_BITS 8

/**
 * @struct Options
 * @brief Contains the entered data.
 */
typedef struct
{
    /**
     * @brief Union allowing to specify the modulus p either directly or by its bit length.
     */
    union
    {
        uint32_t p;   /**< @brief Direct value of the prime modulus p */
        ulong p_bits; /**< @brief Number of bits for generating a random prime p */
    };

    /**
     * @brief Union allowing to specify the multiplier b either directly or by its bit length.
     */
    union
    {
        uint32_t b;   /**< Direct value of the multiplier b */
        ulong b_bits; /**< Number of bits for generating a random multiplier b */
    };
    unsigned char flags; /**< Bitmask indicating which options were provided */
    int scale;           /**< Scaling factor for the number of points in graphs */
    ulong points;        /**< Number of measurement points */
} Options;

/**
 * @brief Initializes the struct of @p options
 *
 * @param argc The number of elements in @p argv
 * @param argv The array containing the command-line
 * @param[in, out] options The pointer to the @c Options structure which will be filled based on the command-line
 *
 * @retval 0 on success
 * @retval Different value depending on the error
 *
 * @see error.h
 */
int set_options(int argc, char const *argv[], Options *options);

/**
 * @brief Completes the options by generating the missing parameter.
 *
 * This function automatically generates either @c p or @c b depending on which one is already set in the options structure.
 *
 * @param[in] options The pointer to the @c Options structure
 * @param[out] param The parameters to initialize
 *
 * @retval 0 on success
 * @retval Non-zero on error
 *
 * @see error.h
 */
int init_param(Options *options, Parameters *param);
