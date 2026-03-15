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
#include <string.h>

#if defined(__ARM_NEON)
#define NEON 1
#include <arm_neon.h>
#else
#define NEON 0
#endif

#if defined(__AVX2__)
#define AVX2 1
#else
#define AVX2 0
#endif

#if defined(__AVX512F__)
#define AVX512 1
#else
#define AVX512 0
#endif

#if AVX2 || AVX512
#include <immintrin.h>
#endif

#include "vector.h"

__attribute__((optimize("no-tree-vectorize"))) static inline uint32_t shoup_ref(uint32_t a, uint32_t b, uint32_t b_precomp, uint32_t p)
{
    /* FLINT ASSERT: disabled until specified at configure */
    /* --> this avoids disturbing benchmarks with unwanted operations */
    FLINT_ASSERT(n_is_prime(p) && a < p && b < p);

    // a * b_precomp / 2^32
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;

    // c = a * b - q * p: no need % 2^32 since all variables are uint32_t
    uint32_t c = a * b - q * p;

    if (c >= p)
        c -= p;
    return c;
}

static inline uint32_t shoup(uint32_t a, uint32_t b, uint32_t b_precomp, uint32_t p)
{
    FLINT_ASSERT(n_is_prime(p) && a < p && b < p);
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;
    uint32_t c = a * b - q * p;
    if (c >= p)
        c -= p;
    return c;
}

Vector shoup_scale_ref(Parameters param, Vector v);

Vector shoup_scale(Parameters param, Vector v);

Vector shoup_scale_mullo(Parameters param, Vector v);

#if AVX512
Vector shoup_scale_avx512(Parameters param, Vector v);
#endif

#endif
