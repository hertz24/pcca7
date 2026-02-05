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
#include <arm_neon.h>

#include "calculation.h"

uint32_t shoup_algorithm(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p);

Vector shoup_scalar_neon(Parameters param, Vector vector);

#endif