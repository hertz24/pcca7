/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

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

#if AVX512
#define NB_ALGO 6
#elif NEON || AVX2
#define NB_ALGO 5
#else
#define NB_ALGO 3
#endif

#endif