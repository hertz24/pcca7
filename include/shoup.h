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

#if defined(__ARM_NEON)
#define NEON 1
#include <arm_neon.h>
#else
#define NEON 0
#endif

#if defined(__AVX2__)
#define AVX2 1
#include <immintrin.h>
#else
#define AVX2 0
#endif

#include "calculation.h"

uint32_t shoup_algorithm(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p);

Vector shoup_scalar(Parameters param, Vector v);

#endif