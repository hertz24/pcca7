/**
 * @file utils.h
 * @brief Various utility functions.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef UTILS_H
#define UTILS_H

#include <flint/profiler.h>
#include <float.h>
#include <math.h>

#include "instruction.h"
#include "vector.h"

#define TAB_SIZE(tab) \
    (sizeof(tab) / sizeof(tab[0]))

#if NEON
void prof_repeat(double *min, double *max, profile_target_t target, void *arg);
#endif

/**
 * @brief Measure the time of an algorithm in milliseconds.
 *
 * @param algorithm The algorithm to measure
 * @param param The parameters containing the data
 * @param v The vector to calculate
 *
 * @return The time taken by the algorithm in milliseconds
 */
double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector v);

#endif
