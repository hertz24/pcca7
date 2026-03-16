/**
 * @authors
 * - Henry Zheng
 * - Duc Vinh Nguyen
 */

#ifndef CURVE_H
#define CURVE_H

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "utils.h"

#if AVX512
#define NB_ALGO 5
#else
#define NB_ALGO 4
#endif

int generate_curve(int scale, ulong nb_points, Parameters param);

#endif