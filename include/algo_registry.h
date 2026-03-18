/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef ALGO_REGISTRY_H
#define ALGO_REGISTRY_H

#include "instruction.h"
#include "utils.h"
#include "shoup.h"

typedef struct
{
    const char *name;
    Vector (*const algorithm)(Parameters, Vector);
} Algorithms;

extern const Algorithms algorithms[];

extern const int NB_ALGO;

#endif