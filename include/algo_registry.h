#ifndef ALGO_REGISTRY_H
#define ALGO_REGISTRY_H

#include "instruction.h"
#include "vector.h"
#include "shoup.h"

extern const char *algo_names[];

extern Vector (*const algorithms[])(Parameters, Vector);

extern const int NB_ALGO;

#endif