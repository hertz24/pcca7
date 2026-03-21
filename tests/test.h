/**
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define VECTOR_SIZE 100

#define BOLD "\e[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define END "\033[0m"

#define SUCCESS(test) \
    printf("%s...\t" BOLD GREEN "no error" END "\n", test)

#define ERROR(test) \
    fprintf(stderr, "%s...\t" BOLD RED "error" END ": ", test)

#endif