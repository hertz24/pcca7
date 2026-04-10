/**
 * @file shoup.h
 * @brief Shoup's modular multiplication: scalar versions and SIMD.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef SHOUP_H
#define SHOUP_H

#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <flint/flint.h>
#include <string.h>
#include <immintrin.h>

#include "instruction.h"
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

__attribute__((optimize("no-tree-vectorize"))) static inline uint32_t shoup_b1_ref(uint32_t a, uint32_t b_precomp, uint32_t p)
{
    /* FLINT ASSERT: disabled until specified at configure */
    /* --> this avoids disturbing benchmarks with unwanted operations */
    FLINT_ASSERT(n_is_prime(p) && a < p);

    // a * b_precomp / 2^32
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;

    // c = a * b - q * p: no need % 2^32 since all variables are uint32_t
    uint32_t c = a - q * p;

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

static inline uint32_t shoup_b1(uint32_t a, uint32_t b_precomp, uint32_t p)
{
    FLINT_ASSERT(n_is_prime(p) && a < p && b < p);
    uint32_t q = ((uint64_t)a * b_precomp) >> 32;
    uint32_t c = a - q * p;
    if (c >= p)
        c -= p;
    return c;
}

static inline void print_m256i(__m256i v)
{
    // A 256-bit register holds 8 integers of 32 bits each
    uint32_t val[8];

    // Store the 256-bit vector into the local array
    _mm256_storeu_si256((__m256i *)val, v);

    printf("Valeurs : [%u, %u, %u, %u, %u, %u, %u, %u]\n",
           val[0], val[1], val[2], val[3],
           val[4], val[5], val[6], val[7]);
}

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

Vector shoup_scale_mullo_avx2_v2(Parameters param, Vector v);

Vector shoup_b1_scale_avx2(Parameters param, Vector v);
#endif

#if AVX512
Vector shoup_scale_avx512(Parameters param, Vector v);

Vector unrolling_shoup_scale_avx512(Parameters param, Vector v);

Vector shoup_scale_mullo_avx512(Parameters param, Vector v);

Vector shoup_b1_scale_avx512(Parameters param, Vector v);
#endif

#endif