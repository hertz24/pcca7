/**
 * @file error.h
 * @brief Macros for error handling in the main.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

/**
 * @defgroup ErrorCodes Error codes returned by functions
 * @brief Constants representing specific error conditions. Error 1 corresponds to an initialization issue with @c data_tab
 * @{
 */

/** Invalid command-line arguments. */
#define ERR_INPUT 2

/** The provided p is not a prime number. */
#define ERR_PRIME 3

/** Unrecognized option. */
#define ERR_UNRECOGNIZED 4

/** b must be less than p. */
#define ERR_B_GE_P 5

/** Number of bits of p must be greater than or equal to number of bits of b. */
#define ERR_B_BITS_GE_P_BITS 6

/** No prime p exists with the given number of bits such that p > b. */
#define ERR_NO_PRIME_FOR_BITS 7

/** Error while generating graphs */
#define ERR_GEN_GRAPHS 8

#define ERR_NEGATIVE_NUMBER 9

#define ERR_NO_B_FOR_P 10

#define INVALID_P_BITS 11

#define INVALID_B_BITS 12

#define INVALID_NUMBER_PTS 13

/** @} */

/**
 * @def PRINT_ERROR(ERR)
 * @brief Prints an error message corresponding to the error code.
 * @param ERR One of the error constants.
 */
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
    case ERR_B_BITS_GE_P_BITS:                                                                                  \
        fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n"); \
        break;                                                                                                  \
    case ERR_NO_PRIME_FOR_BITS:                                                                                 \
        fprintf(stderr, "For the number of bits given for p, there is no prime p such that p > b.\n");          \
        break;                                                                                                  \
    case ERR_GEN_GRAPHS:                                                                                        \
        fprintf(stderr, "Error occurred while generating the graphs.\n");                                       \
        break;                                                                                                  \
    case ERR_NEGATIVE_NUMBER:                                                                                   \
        fprintf(stderr, "Negative number entered.\n");                                                          \
        break;                                                                                                  \
    case ERR_NO_B_FOR_P:                                                                                        \
        fprintf(stderr, "For the p given, there is no b such that b < p.\n");                                   \
        break;                                                                                                  \
    case INVALID_P_BITS:                                                                                        \
        fprintf(stderr, "The number of bits given for prime p is invalid.\n");                                  \
        break;                                                                                                  \
    case INVALID_B_BITS:                                                                                        \
        fprintf(stderr, "The number of bits given for b is invalid.\n");                                        \
        break;                                                                                                  \
    case INVALID_NUMBER_PTS:                                                                                    \
        fprintf(stderr, "The number of points given is invalid.\n");                                            \
        break;                                                                                                  \
    }

#endif
