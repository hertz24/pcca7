#include "../include/utils.h"

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

#define DEFINE_UTILS(BIT)                                                                                                               \
    static void test_algorithm_##BIT(void *arg, ulong count)                                                                            \
    {                                                                                                                                   \
        void **data = (void **)arg;                                                                                                     \
        vector##BIT##_t (*algorithm)(param##BIT##_t, vector##BIT##_t) = *data;                                                          \
        param##BIT##_t param = *((param##BIT##_t *)*(data + 1));                                                                        \
        vector##BIT##_t v = *((vector##BIT##_t *)*(data + 2));                                                                          \
        for (ulong i = 0; i < count; i++)                                                                                               \
        {                                                                                                                               \
            prof_start();                                                                                                               \
            /*                                                                                                                          \
             * NOTE: the free doesn't affect the measurement of time.                                                                   \
             */                                                                                                                         \
            free_vector_##BIT(algorithm(param, v));                                                                                     \
            prof_stop();                                                                                                                \
        }                                                                                                                               \
    }                                                                                                                                   \
                                                                                                                                        \
    double time_algorithm_##BIT(vector##BIT##_t (*algorithm)(param##BIT##_t, vector##BIT##_t), param##BIT##_t param, vector##BIT##_t v) \
    {                                                                                                                                   \
        double min, max;                                                                                                                \
        prof_repeat(&min, &max, test_algorithm_##BIT, (void *[]){algorithm, &param, &v});                                               \
        return min;                                                                                                                     \
    }

DEFINE_UTILS(16)
DEFINE_UTILS(32)
