/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "parameters.h"

typedef struct
{
    uint32_t *elements;
    ulong size;
} Vector;

Vector init_vector(ulong size);

/**
 * @brief Generates a random vector
 *
 * @param size The size of the vector
 *
 * @return The random vector
 */
Vector rand_vector(ulong size);

void free_vector(Vector v);

void print_vector(Vector v);

/**
 * @brief Compare the elements of two vectors
 *
 * @param v1 The first vector
 * @param v2 The second vector
 *
 * @return -1 if the two vectors haven't the same size, the size of the vectors if there isn't error, otherwise the index of the error
 */
int compare_vectors(Vector v1, Vector v2);

Vector naive_scale(Parameters param, Vector v);

#endif