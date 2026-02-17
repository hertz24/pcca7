#include <flint/flint.h>
#include <flint/profiler.h>
#include <flint/ulong_extras.h>
#include "../src/shoup.c"

#define NB_ITER 1000

void sample_mulmod_ulong(void * arg, ulong count)
{
    nn_ptr array = (nn_ptr) flint_malloc(NB_ITER*sizeof(ulong));
    FLINT_TEST_INIT(state);

    for (ulong i = 0; i < count; i++)
    {
        ulong bits = n_randint(state, FLINT_BITS - 1) + 1;  // 1...63
        ulong d = n_randbits(state, bits);  // 0 < d < 2**(FLINT_BITS-1) required by FLINT's mulmod_shoup
        ulong a = n_randint(state, d);  // a must be < d

        for (ulong j = 0; j < NB_ITER; j++)
            array[j] = n_randlimb(state);  // array[j] is arbitrary

        const ulong a_pr = n_mulmod_precomp_shoup(a, d);

        prof_start();
        for (ulong j = 0; j < NB_ITER; j++)
            array[j] = n_mulmod_shoup(a, array[j], a_pr, d);
        prof_stop();
    }

    flint_free(array);
    FLINT_TEST_CLEAR(state);
}

void sample_mulmod_ulong(void * arg, ulong count)
{
    nn_ptr array = (nn_ptr) flint_malloc(NB_ITER*sizeof(ulong));
    FLINT_TEST_INIT(state);

    for (ulong i = 0; i < count; i++)
    {
        ulong bits = n_randint(state, FLINT_BITS - 1) + 1;  // 1...63
        ulong d = n_randbits(state, bits);  // 0 < d < 2**(FLINT_BITS-1) required by FLINT's mulmod_shoup
        ulong a = n_randint(state, d);  // a must be < d

        for (ulong j = 0; j < NB_ITER; j++)
            array[j] = n_randlimb(state);  // array[j] is arbitrary

        const ulong a_pr = n_mulmod_precomp_shoup(a, d);

        prof_start();
        for (ulong j = 0; j < NB_ITER; j++)
            array[j] = n_mulmod_shoup(a, array[j], a_pr, d);
        prof_stop();
    }

    flint_free(array);
    FLINT_TEST_CLEAR(state);
}


int main(void)
{
    double min, max;

    flint_printf("mulmod (time in microseconds):\n");

    prof_repeat(&min, &max, sample_mulmod_ulong, NULL);
    flint_printf("%.3f\n", min/NB_ITER);

    return 0;
}
