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