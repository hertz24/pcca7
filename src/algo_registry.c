#include "../include/algo_registry.h"

const char *algo_names[] = {"naive_scale", "shoup_scale_ref", "shoup_scale_flint"
#if NEON
                            ,
                            "shoup_scale_neon",
                            "shoup_scale_mullo_neon"
#elif AVX2
                            ,
                            "shoup_scale_avx2", "shoup_scale_mullo_avx2"
#endif
#if AVX512
                            ,
                            "shoup_scale_avx512"
#endif
};

Vector (*const algorithms[])(Parameters, Vector) = {naive_scale, shoup_scale_ref, shoup_scale_flint
#if NEON
                                                    ,
                                                    shoup_scale_neon, shoup_scale_mullo_neon
#elif AVX2
                                                    ,
                                                    shoup_scale_avx2, shoup_scale_mullo_avx2
#endif
#if AVX512
                                                    ,
                                                    shoup_scale_avx512
#endif
};

const int NB_ALGO = sizeof(algo_names) / sizeof(algo_names[0]);