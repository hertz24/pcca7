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

int compare_vectors(Vector v1, Vector v2);

#endif