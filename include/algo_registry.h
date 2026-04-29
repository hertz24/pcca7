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
 * @brief Each enum value corresponds to an algorithm: the value corresponds to the index of the algorithm in the @c algorithms array.
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

extern const int NB_ALGO;

#define DECLARE_ALGO_REGISTRY(BIT)                                         \
    /**                                                                    \
     * @brief Contains the name and the function of the algorithm.         \
     */                                                                    \
    typedef struct                                                         \
    {                                                                      \
        const char *name;                                                  \
        vector##BIT##_t (*const address)(param##BIT##_t, vector##BIT##_t); \
    } algorithm##BIT##_t;                                                  \
                                                                           \
    extern const algorithm##BIT##_t algorithms##BIT[];

DECLARE_ALGO_REGISTRY(16)
DECLARE_ALGO_REGISTRY(32)

#endif