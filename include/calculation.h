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

/**
 * @struct Parameters
 * @brief Contains the data to be calculated
 */
typedef struct
{
    /**
     * @brief Array of values a_i of size #isze
     *
     * Each value a_i must be in [0, p[.
     * This array represents the vector A = (a_1, a_2, ..., a_n).
     */
    uint32_t *tab_a;
    int size; /**< @brief The size of #tab_a */

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

int is_prime(uint64_t p);

Parameters init_parameters(int size, uint32_t b, uint64_t p);

void free_parameters(Parameters param);

Vector init_vector(int size);

void free_vector(Vector vector);

void print_vector(Vector vector);

#endif