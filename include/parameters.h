/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef CALCULATION_H
#define CALCULATION_H

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "flint/ulong_extras.h"

/**
 * @struct Parameters
 * @brief Contains the data to be calculated
 */
typedef struct
{
    /**
     * @brief Constant multiplier #b
     *
     * It is the constant scalar value to multiply each element of #tab_a.
     * #b must be in [0, p[.
     */
    uint32_t b;
    uint32_t b_precomp; /**< @brief Precomputed value for Shoup's algorithm */
    uint32_t p;         /**< @brief The prime modulus */
} Parameters;

uint32_t rand_prime(void);

static inline uint32_t closest_prime(uint32_t b)
{
    return n_nextprime(b, 1);
}

static inline Parameters init_parameters(uint32_t b, uint32_t p)
{
    return (Parameters){b, ((uint64_t)b << 32) / p, p};
}

/**
 * @brief Returns a random paramaters
 *
 * @param bits The number of bits of b
 * @return The parameters
 */
Parameters rand_parameters_b(ulong bits);

/**
 * @brief Returns a random paramaters
 *
 * @param bits The number of bits of p. If @p bits <= 1, then the number of bits of p is random
 * @return The parameters
 */
Parameters rand_parameters(ulong bits);

void print_param(Parameters param);

#endif