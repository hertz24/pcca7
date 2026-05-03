#include "../include/algo_registry.h"

const algorithm32_t algorithms_32[] = {{"Naive scale", naive_scale_32},
                                       {"Shoup scale (reference)", shoup_scale_ref_32},
                                       {"Shoup scale (FLINT)", shoup_scale_flint_32}
#if NEON
                                       ,
                                       {"Shoup scale (NEON)", shoup_scale_neon},
                                       {"Unrolling Shoup scale (NEON)", unrolling_shoup_scale_neon},
                                       {"Shoup scale with multiply low (NEON)", shoup_scale_mullo_neon},
                                       {"Shoup scale (NEON) for b = 1", shoup_b1_scale_neon}
#elif AVX2
                                       ,
                                       {"Shoup scale (AVX2)", shoup_scale_avx2_32},
                                       {"Unrolling Shoup scale (AVX2)", unrolling_shoup_scale_avx2_32},
                                       {"Shoup scale with multiply low (AVX2)", shoup_scale_mullo_avx2_32},
                                       {"Shoup scale with multiply low version 2 (AVX2)", shoup_scale_mullo_v2_avx2_32},
                                       {"Shoup scale (AVX2) for b = 1", shoup_b1_scale_avx2_32}
#endif
#if AVX512
                                       ,
                                       {"Shoup scale (AVX-512)", shoup_scale_avx512_32},
                                       {"Unrolling Shoup scale (AVX-512)", unrolling_shoup_scale_avx512_32},
                                       {"Shoup scale with multiply low (AVX-512)", shoup_scale_mullo_avx512_32},
                                       {"Shoup scale with multiply low version 2 (AVX-512)", shoup_scale_mullo_v2_avx512_32},
                                       {"Shoup scale (AVX-512) for b = 1", shoup_b1_scale_avx512_32}
#endif
};

const int NB_ALGO = TAB_SIZE(algorithms_32);
