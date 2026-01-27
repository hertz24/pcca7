/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef SHOUP_H
#define SHOUP_H

#include <stdint.h>
#include <math.h>
#include <assert.h>

#include "calculation.h"

uint64_t shoup_algorithm(uint32_t a, uint32_t b, uint64_t b_bis, uint64_t p);

Vector shoup_scalar_algorithm(Parameters param, Vector vector);

#endif