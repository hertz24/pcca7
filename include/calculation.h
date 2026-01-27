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
    uint64_t b_bis; /**< @brief Precomputed value for Shoup's algorithm */
    uint64_t p;     /**< @brief The prime modulus */
} Parameters;

typedef struct
{
    uint64_t *elements;
    int size;
} Vector;

Parameters init_parameters(uint32_t b, uint64_t p);

Vector init_vector(int size);

Vector rand_vector(int size);

void free_vector(Vector vector);

void print_vector(Vector vector);

#endif