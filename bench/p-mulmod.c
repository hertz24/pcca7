#include <flint/flint.h>
#include <flint/profiler.h>
#include <flint/ulong_extras.h>
#include "../include/shoup.h"

/* KEEP 1 for the moment */
#define NB_ITER 1

typedef struct
{
    flint_bitcnt_t bits;
    ulong len;
} info_t;

void sample_mulmod_uint64(void *arg, ulong count)
{
    info_t *info = (info_t *)arg;

    nn_ptr array = (nn_ptr)flint_malloc(info->len * sizeof(ulong));
    FLINT_TEST_INIT(state);

    for (ulong i = 0; i < count; i++)
    {
        ulong p = n_randbits(state, info->bits);
        ulong b = n_randint(state, p); // b must be < p

        for (ulong j = 0; j < info->len; j++)
            array[j] = n_randlimb(state); // array[j] is arbitrary

        const ulong b_pr = n_mulmod_precomp_shoup(b, p);

        prof_start();
        for (ulong rep = 0; rep < NB_ITER; rep++)
        {
            for (ulong j = 0; j < info->len; j++)
                array[j] = n_mulmod_shoup(b, array[j], b_pr, p);
        }
        prof_stop();
    }

    flint_free(array);
    FLINT_TEST_CLEAR(state);
}

void sample_mulmod_uint32_noavx(void *arg, ulong count)
{
    info_t *info = (info_t *)arg;

    uint32_t *array = (uint32_t *)flint_malloc(info->len * sizeof(uint32_t));
    FLINT_TEST_INIT(state);

    for (ulong i = 0; i < count; i++)
    {
        ulong bits = n_randint(state, FLINT_BITS / 2 - 1) + 1; // 1...31
        uint32_t p = n_randbits(state, bits);                  // 0 < p < 2**31
        uint32_t b = n_randint(state, p);                      // a must be < p

        for (ulong j = 0; j < info->len; j++)
            array[j] = n_randlimb(state); // array[j] is arbitrary

        const uint32_t b_pr = ((uint64_t)b << 32) / p;

        prof_start();
        for (ulong rep = 0; rep < NB_ITER; rep++)
        {
            for (ulong j = 0; j < info->len; j++)
                array[j] = normal_shoup(b, array[j], b_pr, p);
        }
        prof_stop();
    }

    flint_free(array);
    FLINT_TEST_CLEAR(state);
}

void sample_mulmod_uint32_autoavx(void *arg, ulong count)
{
    info_t *info = (info_t *)arg;

    uint32_t *array = (uint32_t *)flint_malloc(info->len * sizeof(uint32_t));
    FLINT_TEST_INIT(state);

    for (ulong i = 0; i < count; i++)
    {
        ulong bits = n_randint(state, FLINT_BITS / 2 - 1) + 1; // 1...31
        uint32_t p = n_randbits(state, bits);                  // 0 < p < 2**31
        uint32_t b = n_randint(state, p);                      // a must be < p

        for (ulong j = 0; j < info->len; j++)
            array[j] = n_randlimb(state); // array[j] is arbitrary

        const uint32_t b_pr = ((uint64_t)b << 32) / p;

        prof_start();
        for (ulong rep = 0; rep < NB_ITER; rep++)
        {
            for (ulong j = 0; j < info->len; j++)
                array[j] = vectorized_shoup(b, array[j], b_pr, p);
        }
        prof_stop();
    }

    flint_free(array);
    FLINT_TEST_CLEAR(state);
}

int main(void)
{
    double min, max;

    FLINT_TEST_INIT(state);

    info_t info;

    flint_printf("*** all times in microseconds ***\n");
    flint_printf("length:    ");
    for (ulong len = 1; len < 10000; len = 2 * len)
        flint_printf("%-8d", len);
    flint_printf("\n");

    /* UINT64 */
    /* ------ */

    // recall: 0 < p < 2**(FLINT_BITS-1) required by FLINT's mulmod_shoup
    info.bits = 62;
    /* info.bits = n_randint(state, 63) + 1;  // 1...63 */
    /* flint_printf("%ld\t", info.bits); */

    /* non-vectorized mulmod */
    flint_printf("64b,noavx  ");
    for (ulong len = 1; len < 10000; len = 2 * len)
    {
        info.len = len;
        prof_repeat(&min, &max, sample_mulmod_uint64, (void *)&info);
        flint_printf("%.1e ", min / NB_ITER);
    }
    flint_printf("\n");

    /* non-vectorized mod...  (TODO) */

    /* vectorized mulmod...  (TODO) */

    /* UINT32 */
    /* ------ */

    /* info.bits = n_randint(state, 31) + 1;  // 1...31 */
    /* flint_printf("%ld\t", info.bits); */
    info.bits = 31;

    /* non-vectorized mulmod */
    flint_printf("32b,noavx  ");
    for (ulong len = 1; len < 10000; len = 2 * len)
    {
        info.len = len;
        prof_repeat(&min, &max, sample_mulmod_uint32_noavx, (void *)&info);
        flint_printf("%.1e ", min / NB_ITER);
    }
    flint_printf("\n");

    /* non-vectorized mulmod */
    flint_printf("32b,auto   ");
    for (ulong len = 1; len < 10000; len = 2 * len)
    {
        info.len = len;
        prof_repeat(&min, &max, sample_mulmod_uint32_autoavx, (void *)&info);
        flint_printf("%.1e ", min / NB_ITER);
    }
    flint_printf("\n");

    /* non-vectorized mod...  (TODO) */

    /* vectorized mulmod...  (TODO) */

    FLINT_TEST_CLEAR(state);

    return 0;
}
