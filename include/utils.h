/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <time.h>

#include "calculation.h"

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector vector);

/**
 * @brief Compare the elements of two vectors
 *
 * @param v1 The first vector
 * @param v2 The second vector
 *
 * @return 0 if there is not error, otherwise 1
 */
int compare_vectors(Vector v1, Vector v2);

#endif