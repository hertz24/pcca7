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
#include <flint/flint.h>

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

__attribute__((optimize("no-tree-vectorize"))) static inline uint32_t normal_shoup(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p)
{
    /* FLINT ASSERT: disabled until specified at configure */
    /* --> this avoids disturbing benchmarks with unwanted operations */
    FLINT_ASSERT(n_is_prime(p) && a < p && b < p);

    // a * b_bis / 2^32
    uint32_t q = ((uint64_t)a * b_bis) >> 32;

    // (a * b - q * p): no need % 2^32 since all variables are uint32
    uint32_t c = a * b - q * p;

    if (c >= p)
        c -= p;
    return c;
}

/**
 * @brief Exact same function of @ref normal_shoup, but with auto-vectorization allowed
 */
static inline uint32_t vectorized_shoup(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p)
{
    FLINT_ASSERT(n_is_prime(p) && a < p && b < p);
    uint32_t q = ((uint64_t)a * b_bis) >> 32;
    uint32_t c = a * b - q * p;
    if (c >= p)
        c -= p;
    return c;
}

Vector shoup_scalar(Parameters param, Vector v);

#endif
