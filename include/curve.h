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

int generate_curve(int scale, ulong nb_points, Parameters param);

#endif