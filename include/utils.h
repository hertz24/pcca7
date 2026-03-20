/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef UTILS_H
#define UTILS_H

#include <flint/profiler.h>

#include "vector.h"

#define TAB_SIZE(tab) \
    (sizeof(tab) / sizeof(tab[0]))

/**
 * @brief Measure the time of an algorithm in milliseconds
 *
 * @param algorithm The algorithm to measure
 * @param param
 * @param v The vector to calculate
 *
 * @return The time taken by the algorithm in milliseconds
 */
double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector v);

ulong nb_bits(ulong x);

#endif