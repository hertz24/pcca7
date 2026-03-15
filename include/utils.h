/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <flint/profiler.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "vector.h"
#include "shoup.h"

#define MAX_BUFFER 1024

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

int generate_curve(int scale, ulong nb_points, Parameters param);

#endif