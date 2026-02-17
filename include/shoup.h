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

/* NOTE this was moved from C file, and is now a static inline function */
/* forbid auto-vectorization: */
/* __attribute__((optimize("no-tree-vectorize"))) */
static inline uint32_t n32_mulmod_shoup_noavx(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p)
{
    /* FLINT ASSERT: disabled until specified at configure */
    /* --> this avoids disturbing benchmarks with unwanted operations */
    FLINT_ASSERT(n_is_prime(p) && a < p && b < p);

    // a * b_bis / 2^32
    uint32_t q = ((uint64_t)a * b_bis) >> 32;

    // (a * b - q * p) % 2^32
    /* uint32_t c = (a * b - q * p) % (1UL << 32); */
    // NOTE the "% .." is useless since all variables are uint32
    uint32_t c = a * b - q * p;

    if (c >= p)
        c -= p;
    return c;
}

/* exact same function, but with auto-vectorization allowed */
static inline uint32_t n32_mulmod_shoup_autoavx(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p)
{
    /* FLINT ASSERT: disabled until specified at configure */
    /* --> this avoids disturbing benchmarks with unwanted operations */
    FLINT_ASSERT(n_is_prime(p) && a < p && b < p);

    // a * b_bis / 2^32
    uint32_t q = ((uint64_t)a * b_bis) >> 32;

    // (a * b - q * p) % 2^32
    /* uint32_t c = (a * b - q * p) % (1UL << 32); */
    // NOTE the "% .." is useless since all variables are uint32
    uint32_t c = a * b - q * p;

    if (c >= p)
        c -= p;
    return c;
}




Vector shoup_scalar(Parameters param, Vector v);

#endif
