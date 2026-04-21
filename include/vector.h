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

/**
 * @struct Vector
 * @brief Vector of 32-bit integers for modular arithmetic.
 */
typedef struct
{
    uint32_t *elements; /**< Pointer to the array of elements */
    ulong size;         /**< Length of the vector */
} Vector;

Vector init_vector(ulong size);

/**
 * @brief Generates a random vector.
 *
 * @param size The size of the vector
 *
 * @return The random vector
 */
Vector rand_vector(ulong size);

/**
 * @brief Frees the pointer to the array of elements of @p v
 *
 * @param v The vector to free
 */
void free_vector(Vector v);

/**
 * @brief Prints the elements of the vector.
 *
 * @param out The output
 * @param v The vector to print
 */
void print_vector(FILE *out, Vector v);

/**
 * @brief Compare the elements of two vectors.
 *
 * @param v1 The first vector
 * @param v2 The second vector
 *
 * @retval 1 if the two vectors are the same
 * @retval 0 otherwise
 */
int compare_vectors(Vector v1, Vector v2);

/**
 * @brief Naive modular multiplication of a vector by a constant (a * b mod p) in @p param
 *
 * @param param The parameters
 * @param v The vector
 *
 * @return A new vector after the calculation
 */
Vector naive_scale(Parameters param, Vector v);

#endif
