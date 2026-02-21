/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <flint/profiler.h>

#include "calculation.h"

/**
 * @brief Compare the elements of two vectors
 *
 * @param v1 The first vector
 * @param v2 The second vector
 *
 * @return 0 if there is not error, otherwise 1
 */
int compare_vectors(Vector v1, Vector v2);

/**
 * @brief Measure the time of an algorithm in milliseconds
 *
 * @param algorithm The algorithm to measure
 * @param param
 * @param vector The vector to calculate
 *
 * @return The time taken by the algorithm in milliseconds
 */
double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector vector);

#endif