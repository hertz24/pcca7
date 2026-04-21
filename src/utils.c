#include "../include/utils.h"

static void test_algorithm(void *arg, ulong count)
{
    void **data = (void **)arg;
    Vector (*algorithm)(Parameters, Vector) = *data;
    Parameters param = *((Parameters *)*(data + 1));
    Vector v = *((Vector *)*(data + 2));
    for (ulong i = 0; i < count; i++)
    {
        prof_start();
        /*
         * NOTE: the free doesn't affect the measurement of time.
         */
        free_vector(algorithm(param, v));
        prof_stop();
    }
}

#if NEON
void prof_repeat(double *min, double *max, profile_target_t target, void *arg)
{
    const ulong FIXED_TRIALS = 300000;
    double min_time = DBL_MAX, max_time = DBL_MIN;
    init_clock(0);

    // Executes the algorithm 300,000 times
    target(arg, FIXED_TRIALS);

    double total = get_clock(0);
    double per_trial = total / FIXED_TRIALS;
    if (per_trial > max_time)
        max_time = per_trial;
    if (per_trial < min_time)
        min_time = per_trial;
    if (min)
        *min = min_time;
    if (max)
        *max = max_time;
}
#endif

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector v)
{
    double min, max;
    prof_repeat(&min, &max, test_algorithm, (void *[]){algorithm, &param, &v});
    return min;
}
