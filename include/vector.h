/**
 * @file vector.h
 * @brief 32-bit integer vectors for modular calculations.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "parameters.h"

#define DECLARE_VECTOR(BIT)                                                                    \
    /**                                                                                        \
     * @brief Vector of integers for modular arithmetic.                                       \
     */                                                                                        \
    typedef struct                                                                             \
    {                                                                                          \
        uint##BIT##_t *elements; /**< Pointer to the array of elements */                      \
        ulong size;              /**< Length of the vector */                                  \
    } vector##BIT##_t;                                                                         \
                                                                                               \
    vector##BIT##_t init_vector##BIT(ulong size);                                              \
                                                                                               \
    /**                                                                                        \
     * @brief Generates a random vector.                                                       \
     *                                                                                         \
     * @param size The size of the vector                                                      \
     *                                                                                         \
     * @return The random vector                                                               \
     */                                                                                        \
    vector##BIT##_t rand_vector##BIT(ulong size);                                              \
                                                                                               \
    /**                                                                                        \
     * @brief Frees the pointer to the array of elements of @p v                               \
     *                                                                                         \
     * @param v The vector to free                                                             \
     */                                                                                        \
    void free_vector##BIT(vector##BIT##_t v);                                                  \
                                                                                               \
    /**                                                                                        \
     * @brief Prints the elements of the vector.                                               \
     *                                                                                         \
     * @param out The output                                                                   \
     * @param v The vector to print                                                            \
     */                                                                                        \
    void print_vector##BIT(FILE *out, vector##BIT##_t v);                                      \
                                                                                               \
    /**                                                                                        \
     * @brief Compare the elements of two vectors.                                             \
     *                                                                                         \
     * @param v1 The first vector                                                              \
     * @param v2 The second vector                                                             \
     *                                                                                         \
     * @retval 1 if the two vectors are the same                                               \
     * @retval 0 otherwise                                                                     \
     */                                                                                        \
    int compare_vectors##BIT(vector##BIT##_t v1, vector##BIT##_t v2);                          \
                                                                                               \
    /**                                                                                        \
     * @brief Naive modular multiplication of a vector by a constant (a * b mod p) in @p param \
     *                                                                                         \
     * @param param The parameters                                                             \
     * @param v The vector                                                                     \
     *                                                                                         \
     * @return A new vector after the calculation                                              \
     */                                                                                        \
    vector##BIT##_t naive_scale##BIT(param##BIT##_t param, vector##BIT##_t v);

DECLARE_VECTOR(16)
DECLARE_VECTOR(32)

#endif
