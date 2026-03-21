/**
 * @file graph.h
 * @brief Generates graph.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "algo_registry.h"
#include "utils.h"

extern const char *colors[9];

int generate_graph(int scale, ulong nb_points, Parameters param, Algorithm algorithms[], int nb_algo);

#endif