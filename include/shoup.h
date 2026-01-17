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

#include "utils.h"

uint32_t shoup_algorithm(uint32_t a, uint32_t b, uint64_t p);

#endif