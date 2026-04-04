/**
 * @file graph.h
 * @brief Generates graphs.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "algo_registry.h"
#include "utils.h"

/**
 * @brief Generates graphs for given algorithms using given parameters.
 *
 * @param scale Scaling factor for the number of points in graphs
 * @param nb_points Number of measurement points
 * @param param The parameters
 * @param algorithms The list of algorithms to test
 * @param nb_algo The number of algorithms
 *
 * @retval 0 on success
 * @retval 1 on fail
 */
int generate_graph(int scale, ulong nb_points, Parameters param, Algorithm algorithms[], int nb_algo);

#endif
