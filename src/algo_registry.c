#include "../include/algo_registry.h"

const Algorithm algorithms[] = {{"Naive scale", naive_scale}, {"Shoup scale (reference)", shoup_scale_ref}, {"Shoup scale (FLINT)", shoup_scale_flint}
#if NEON
                                ,
                                {"Shoup scale (NEON)", shoup_scale_neon},
                                {"Unorlling Shoup scale (NEON)", unrolling_shoup_scale_neon},
                                {"Shoup scale with multiply low (NEON)", shoup_scale_mullo_neon},
                                {"Shoup scale (NEON) for b = 1", shoup_b1_scale_neon}
#elif AVX2
                                ,
                                {"Shoup scale (AVX2)", shoup_scale_avx2},
                                {"Unrolling Shoup scale (AVX2)", unrolling_shoup_scale_avx2},
                                {"Shoup scale with multiply low (AVX2)", shoup_scale_mullo_avx2},
                                {"Shoup scale with multiply low version 2 (AVX2)", shoup_scale_mullo_v2_avx2},
                                {"Shoup scale (AVX2) for b = 1", shoup_b1_scale_avx2}
#endif
#if AVX512
                                ,
                                {"Shoup scale (AVX512)", shoup_scale_avx512},
                                {"Unrolling Shoup scale (AVX512)", unrolling_shoup_scale_avx512},
                                {"Shoup scale with multiply low (AVX512)", shoup_scale_mullo_avx512},
                                {"Shoup scale with multiply low version 2 (AVX512)", shoup_scale_mullo_v2_avx512},
                                {"Shoup scale (AVX512) for b = 1", shoup_b1_scale_avx512}
#endif
};

const int NB_ALGO = TAB_SIZE(algorithms);
