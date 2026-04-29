/**
 * @file parameters.h
 * @brief Parameter management for modular operations.
 * @author Henry Zheng
 * @author Duc Vinh Nguyen
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "flint/ulong_extras.h"

/** @brief Random state for FLINT */
extern flint_rand_t state;

/**
 * @brief Initializes @c state for the random generations.
 */
void rand_init(void);

/**
 * @brief Clears @c state at the end of the program.
 */
void rand_clear(void);

/**
 * @brief Returns a random prime number.
 *
 * @param bits The number of bits of the prime number. If @p bits < 2, then the number of bits of @c p is random in [2, 31].
 *
 * @return The prime number
 */
ulong rand_prime(ulong bits);

/**
 * @brief Returns the largest n-bits prime number.
 *
 * @param bits The number of bits of the prime number
 *
 * @retval The largest prime number with @p bits bits
 * @retval 0 if @p bits < 2
 */
ulong max_prime_bits(ulong bits);

#define DECLARE_PARAM(BIT, CAST)                                                                                                           \
    /**                                                                                                                                    \
     * @brief Contains the data to be calculated.                                                                                          \
     */                                                                                                                                    \
    typedef struct                                                                                                                         \
    {                                                                                                                                      \
        /**                                                                                                                                \
         * @brief Constant multiplier #b.                                                                                                  \
         *                                                                                                                                 \
         * It is the constant scalar value to multiply each element of #tab_a.                                                             \
         * #b must be in [0, p[.                                                                                                           \
         */                                                                                                                                \
        uint##BIT##_t b;                                                                                                                   \
        uint##BIT##_t b_precomp; /**< Precomputed value for Shoup's algorithm */                                                           \
        uint##BIT##_t p;         /**< The prime modulus */                                                                                 \
    } param##BIT##_t;                                                                                                                      \
                                                                                                                                           \
    /**                                                                                                                                    \
     * @brief Initializes the parameters with given @p b and @p p                                                                          \
     *                                                                                                                                     \
     * @param b The multiplier                                                                                                             \
     * @param p The prime modulus                                                                                                          \
     *                                                                                                                                     \
     * @return The parameters                                                                                                              \
     */                                                                                                                                    \
    static inline param##BIT##_t init_parameters##BIT(uint##BIT##_t b, uint##BIT##_t p)                                                    \
    {                                                                                                                                      \
        return (param##BIT##_t){b, (CAST)(BIT) / p, p};                                                                                    \
    }                                                                                                                                      \
                                                                                                                                           \
    /**                                                                                                                                    \
     * @brief Returns a random parameters.                                                                                                 \
     *                                                                                                                                     \
     * @param b_bits The number of bits of @c b. If @p b_bits > 31, then the number of bits of @c b is random in [0, 31].                  \
     * @param p_bits The number of bits of @c p. If @p p_bits < 2 or @p p_bits > 31, then the number of bits of @c p is random in [2, 31]. \
     *                                                                                                                                     \
     * @return The parameters                                                                                                              \
     */                                                                                                                                    \
    uint##BIT##_t rand_parameters##BIT(ulong b_bits, ulong p_bits);                                                                        \
                                                                                                                                           \
    /**                                                                                                                                    \
     * @brief Prints the data of the parameters.                                                                                           \
     *                                                                                                                                     \
     * @param out The output                                                                                                               \
     * @param param The parameters to print                                                                                                \
     */                                                                                                                                    \
    void print_param##BIT(FILE *out, param##BIT##_t param);

DECLARE_PARAM(16, uint32_t)
DECLARE_PARAM(32, uint64_t)

#endif
