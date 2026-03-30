/**
 * @file error.h
 * @brief Macros for error handling in the main
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#define ERR_INPUT 1
#define ERR_PRIME 2
#define ERR_UNRECOGNIZED 3
#define ERR_B_GE_P 4
#define ERR_BBITS_GE_PBITS 5
#define ERR_NO_PRIME_FOR_BITS 6
#define ERR_GEN_GRAPHS 7

#define PRINT_ERROR(ERR)                                                                                        \
    switch (ERR)                                                                                                \
    {                                                                                                           \
    case ERR_INPUT:                                                                                             \
        fprintf(stderr, "Error input arguments.\n");                                                            \
        break;                                                                                                  \
    case ERR_PRIME:                                                                                             \
        fprintf(stderr, "p must be a prime number.\n");                                                         \
        break;                                                                                                  \
    case ERR_UNRECOGNIZED:                                                                                      \
        fprintf(stderr, "Option unrecognized.\n");                                                              \
        break;                                                                                                  \
    case ERR_B_GE_P:                                                                                            \
        fprintf(stderr, "b must be less than p.\n");                                                            \
        break;                                                                                                  \
    case ERR_BBITS_GE_PBITS:                                                                                    \
        fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n"); \
        break;                                                                                                  \
    case ERR_NO_PRIME_FOR_BITS:                                                                                 \
        fprintf(stderr, "For the number of bits given for p, there is no prime p such that p > b.\n");          \
        break;                                                                                                  \
    }

#endif
