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

#include "utils.h"

typedef struct
{
    uint32_t *tab_a;
    int size;
    uint32_t b;
    uint64_t b_bis;
    uint64_t p;
} Parameters;

typedef struct
{
    uint64_t *elements;
    int size;
} Vector;

Parameters init_parameters(int size, uint32_t b, uint64_t p);

void free_parameters(Parameters param);

Vector init_vector(int size);

void free_vector(Vector vector);

void print_vector(Vector vector);

#endif