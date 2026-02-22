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
    uint32_t b_bis; /**< @brief Precomputed value for Shoup's algorithm */
    uint32_t p;     /**< @brief The prime modulus */
} Parameters;

typedef struct
{
    uint32_t *elements;
    ulong size;
} Vector;

uint32_t rand_prime();

Parameters init_parameters(uint32_t b, uint32_t p);

Parameters rand_parameters();

void print_param(Parameters param);

Vector init_vector(ulong size);

Vector rand_vector(ulong size, uint32_t p);

void free_vector(Vector v);

void print_vector(Vector v);

Vector naive_scalar_product(Parameters param, Vector v);

#endif