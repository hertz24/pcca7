/**
 * @file test.h
 * @brief All macros and functions for the tests.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/shoup.h"
#include "../include/utils.h"
#include "../include/parameters.h"
#include "../include/algo_registry.h"
#include "../include/options.h"

#define NB_TESTS 50
#define VECTOR_SIZE 100
#define BUFFER_SIZE 16

#define BOLD "\e[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define END "\033[0m"

#define SUCCESS(test, out)     \
    fflush(stdout);            \
    dup2(*out, STDOUT_FILENO); \
    printf("%s...\t" BOLD GREEN "no error" END "\n", test)

#define FAIL(test, err)        \
    fflush(stderr);            \
    dup2(*err, STDERR_FILENO); \
    fprintf(stderr, "%s...\t" BOLD RED "error" END ": ", test)

static inline int begin(int *out, int *err)
{
    *out = dup(STDOUT_FILENO);
    *err = dup(STDERR_FILENO);
    int null = open("/dev/null", O_WRONLY);
    if (null == -1)
        return 1;
    dup2(null, STDOUT_FILENO);
    dup2(null, STDERR_FILENO);
    close(null);
    return 0;
}

#endif
