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
 * @brief Constants representing specific error conditions.
 * @{
 */

/** Invalid command-line arguments. */
#define ERR_INPUT 1

/** The provided p is not a prime number. */
#define ERR_PRIME 2

/** Unrecognized option. */
#define ERR_UNRECOGNIZED 3

/** b must be less than p. */
#define ERR_B_GE_P 4

/** Number of bits of p must be greater than or equal to number of bits of b. */
#define ERR_BBITS_GE_PBITS 5

/** No prime p exists with the given number of bits such that p > b. */
#define ERR_NO_PRIME_FOR_BITS 6

/** Error while generating graphs */
#define ERR_GEN_GRAPHS 7

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
    case ERR_BBITS_GE_PBITS:                                                                                    \
        fprintf(stderr, "The number of bits of p must be greater than or equal to the number of bits of b.\n"); \
        break;                                                                                                  \
    case ERR_NO_PRIME_FOR_BITS:                                                                                 \
        fprintf(stderr, "For the number of bits given for p, there is no prime p such that p > b.\n");          \
        break;                                                                                                  \
    case ERR_GEN_GRAPHS:                                                                                        \
        fprintf(stderr, "Error occurred while generating the graphs.\n");                                       \
        break;                                                                                                  \
    }

#endif
