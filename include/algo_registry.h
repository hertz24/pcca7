/**
 * @file algo_registry.h
 * @brief List of available algorithms.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef ALGO_REGISTRY_H
#define ALGO_REGISTRY_H

#include "instruction.h"
#include "utils.h"
#include "shoup.h"

/**
 * @enum AlgorithmID
 * @brief Each enum value corresponds to an algorithm.
 */
typedef enum
{
    NAIVE_SCALE,
    SHOUP_SCALE_REF,
    SHOUP_SCALE_FLINT
#if NEON
    ,
    SHOUP_SCALE_NEON,
    UNROLLING_SHOUP_SCALE_NEON,
    SHOUP_SCALE_MULLO_NEON,
    SHOUP_B1_SCALE_NEON
#elif AVX2
    ,
    SHOUP_SCALE_AVX2,
    UNROLLING_SHOUP_SCALE_AVX2,
    SHOUP_SCALE_MULLO_AVX2,
    SHOUP_SCALE_MULLO_V2_AVX2,
    SHOUP_B1_SCALE_AVX2
#endif
#if AVX512
    ,
    SHOUP_SCALE_AVX512,
    UNROLLING_SHOUP_SCALE_AVX512,
    SHOUP_SCALE_MULLO_AVX512,
    SHOUP_SCALE_MULLO_V2_AVX512,
    SHOUP_B1_SCALE_AVX512
#endif
} AlgorithmID;

/**
 * @struct Algorithm
 * @brief Contains the name and the function of the algorithm.
 */
typedef struct
{
    const char *name;
    Vector (*const address)(Parameters, Vector);
} Algorithm;

extern const Algorithm algorithms[];

extern const int NB_ALGO;

#endif