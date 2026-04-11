/**
 * @file shoup.h
 * @brief Shoup's modular multiplication: scalar versions, NEON and SIMD.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef SHOUP_H
#define SHOUP_H

#include <stdint.h>
#include <flint/flint.h>

#include "instruction.h"
#include "vector.h"

/**
 * @defgroup Shoup Shoup's algorithm
 * @brief Shoup's modular multiplication implementation without costly division operations
 *
 * Shoup's algorithm efficiently computes a * b mod p using a precomputed value depending on the multiplier b and the prime modulus p.
 *
 * Principle:
 *  - Precompute b_precomp = (b * 2^32) / p using 32-bit modular arithmetic
 *  - Compute the estimated quotient q = (a * b_precomp) / 2^32
 *  - Compute the intermediate remainder c = a * b - q * p (no need modulus 2^32 since all variables are uint32_t)
 *  - If c >= p, subtract c by p
 *
 * The algorithms are supposed that p is a prime number such that p < 2^31 and b < p.
 *
 * @see parameters.h
 *
 * @note @c _ref versions use @c __attribute__((optimize("no-tree-vectorize"))) to disable auto-vectorization and provide a scalar reference for comparison.
 *
 * @note @c _b1 versions are optimized for b = 1, which saves one operation.
 */
/** @{ */
__attribute__((optimize("no-tree-vectorize"))) static inline uint32_t shoup_ref(uint32_t a, uint32_t b, uint32_t b_precomp, uint32_t p)
{
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;
    uint32_t c = a * b - q * p;
    if (c >= p)
        c -= p;
    return c;
}

__attribute__((optimize("no-tree-vectorize"))) static inline uint32_t shoup_b1_ref(uint32_t a, uint32_t b_precomp, uint32_t p)
{
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;
    uint32_t c = a - q * p;
    if (c >= p)
        c -= p;
    return c;
}

static inline uint32_t shoup(uint32_t a, uint32_t b, uint32_t b_precomp, uint32_t p)
{
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;
    uint32_t c = a * b - q * p;
    if (c >= p)
        c -= p;
    return c;
}

static inline uint32_t shoup_b1(uint32_t a, uint32_t b_precomp, uint32_t p)
{
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;
    uint32_t c = a - q * p;
    if (c >= p)
        c -= p;
    return c;
}
/** @} */

/**
 * @defgroup ShoupScale Vector scaling by Shoup's modular multiplication
 * @brief Functions to multiply each element of a vector by a scalar modulo p, using Shoup's algorithm.
 *
 * There are several versions:
 *  - @c shoup_scale_ref : reference non-vectorized implementation.
 *  - @c shoup_scale_flint : implementation based on FLINT library.
 *  - @c _neon , @c _avx2 , @c _avx512 : vectorized versions using appropriate SIMD instructions depending on the architecture.
 *  - @c _mullo : versions that use the multiply low instruction.
 *  - @c _b1_ : versions optimized for b = 1, saving one operation per element.
 *  - @c unrolling_ : versions using loop unrolling.
 *
 * @note Vectorized versions are conditionally compiled based on the target architecture.
 */
/** @{ */
Vector shoup_scale_ref(Parameters param, Vector v);

Vector shoup_scale_flint(Parameters param, Vector v);

#if NEON
Vector shoup_scale_neon(Parameters param, Vector v);

Vector unrolling_shoup_scale_neon(Parameters param, Vector v);

Vector shoup_scale_mullo_neon(Parameters param, Vector v);

Vector shoup_b1_scale_neon(Parameters param, Vector v);
#endif

#if AVX2
Vector shoup_scale_avx2(Parameters param, Vector v);

Vector unrolling_shoup_scale_avx2(Parameters param, Vector v);

Vector shoup_scale_mullo_avx2(Parameters param, Vector v);

Vector shoup_scale_mullo_v2_avx2(Parameters param, Vector v);

Vector shoup_b1_scale_avx2(Parameters param, Vector v);
#endif

#if AVX512
Vector shoup_scale_avx512(Parameters param, Vector v);

Vector unrolling_shoup_scale_avx512(Parameters param, Vector v);

Vector shoup_scale_mullo_avx512(Parameters param, Vector v);

Vector shoup_b1_scale_avx512(Parameters param, Vector v);
#endif
/** @} */

#endif