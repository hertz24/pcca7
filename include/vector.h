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

Vector rand_vector(ulong size);

void free_vector(Vector v);

void print_vector(Vector v);

/**
 * @brief Compare the elements of two vectors
 *
 * @param v1 The first vector
 * @param v2 The second vector
 *
 * @return 0 if there is not error, otherwise 1
 */
int compare_vectors(Vector v1, Vector v2);

Vector naive_scale(Parameters param, Vector v);

#endif