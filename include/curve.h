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

#include "algo_registry.h"
#include "utils.h"

extern const char *colors[9];

int generate_curve(int scale, ulong nb_points, Parameters param, Algorithm algorithms[], int nb_algo);

#endif