#include "../include/algo_registry.h"

const Algorithm algorithms[] = {{"Naive scale", naive_scale}, {"Shoup scale (reference)", shoup_scale_ref}, {"Shoup scale (FLINT)", shoup_scale_flint}
#if NEON
                                ,
                                {"Shoup scale (NEON)", shoup_scale_neon},
                                {"Shoup scale with multiply low (NEON)", shoup_scale_mullo_neon}
#elif AVX2
                                ,
                                {"Shoup scale (AVX2)", shoup_scale_avx2},
                                {"Shoup scale with multiply low (AVX2)", shoup_scale_mullo_avx2}
#endif
#if AVX512
                                ,
                                {"Shoup scale (AVX512)", shoup_scale_avx512}
#endif
};

const int NB_ALGO = TAB_SIZE(algorithms);
