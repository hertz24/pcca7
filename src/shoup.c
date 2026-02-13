#include "../include/shoup.h"

uint32_t shoup_algorithm(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p)
{
    assert(n_is_prime(p) && a < p && b < p);

    // a * b_bis / 2^32
    uint32_t q = (uint64_t)(a * b_bis) >> 32;

    // (a * b - q * p) % 2^32
    uint32_t c = (a * b - q * p) % (1UL << 32);

    if (c >= p)
        c -= p;
    return c;
}

#if NEON
static Vector shoup_scalar_neon(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    ulong n = size - (size % 2);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_bis = vdup_n_u32(param.b_bis);
    ulong i = 0;
    for (; i < n; i += 2)
    {
        uint32x2_t va = vld1_u32(v.elements + i);

        // Calculates a * b_bis
        uint64x2_t ab_bis = vmull_u32(va, vb_bis);

        // q = (a * b_bis) >> 32
        uint64x2_t q = vshrq_n_u64(ab_bis, 32);

        // Convert uint64x2_t to uint32x2_t
        uint32x2_t q32 = vmovn_u64(q);

        // c = (a * b - q * p) % 2^32 = (a * b - q * p) & (2^32 - 1)
        uint64x2_t vab = vmull_u32(va, vb);
        uint64x2_t vqp = vmull_u32(q32, vp);
        uint64x2_t c = vandq_u64(vsubq_u64(vab, vqp), vdupq_n_u64((1UL << 32) - 1));
        uint32x2_t c32 = vmovn_u64(c);

        // Compare c >= p
        uint32x2_t cmp = vcge_u32(c32, vp);
        uint32x2_t p_to_sub = vand_u32(cmp, vp);
        c32 = vsub_u32(c32, p_to_sub);

        // Stocks the value
        vst1_u32(res.elements + i, c32);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup_algorithm(*(v.elements + i), param.b, param.b_bis, param.p);
    return res;
}
#endif

#if AVX2
static Vector shoup_scalar_avx(Parameters param, Vector v)
{
    int size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_bis = _mm256_set1_epi64x(param.b_bis);
    __m256i vp = _mm256_set1_epi64x(param.p);
    __m256i mask_32 = _mm256_set1_epi64x(0xFFFFFFFF);

    /* example unrolling + loadu_si256:
     * https://github.com/vneiger/pml/blob/main/flint-extras/src/nmod32_vec/dot_split.c#L62
     * */

    ulong i = 0;
    for (; i + 7 < n; i += 8)
    {
        // Load 4 elements in 32 bits
        __m128i va128_0 = _mm_loadu_si128((__m128i const *)(v.elements + i));
        __m128i va128_1 = _mm_loadu_si128((__m128i const *)(v.elements + i + 4));
        /* dans une autre version: on pourrait charger 256,
         * [v0 v1 v2 v3 v4 v5 v6 v7]
         * 1/ extraire v0 v2 v4 v6, faire le calcul avec
         * 2/ extraire v1 v3 v5 v7, faire le calcul avec
         */

        // Convert to 64 bits
        __m256i va_0 = _mm256_cvtepu32_epi64(va128_0);
        __m256i va_1 = _mm256_cvtepu32_epi64(va128_1);

        // Calculates a * b_bis
        __m256i ab_bis_0 = _mm256_mul_epu32(va_0, vb_bis);
        __m256i ab_bis_1 = _mm256_mul_epu32(va_1, vb_bis);

        // Compute q
        __m256i q_0 = _mm256_srli_epi64(ab_bis_0, 32);
        __m256i q_1 = _mm256_srli_epi64(ab_bis_1, 32);

        // Compute c
        __m256i ab_0 = _mm256_mul_epu32(va_0, vb);
        __m256i ab_1 = _mm256_mul_epu32(va_1, vb);
        __m256i qp_0 = _mm256_mul_epu32(q_0, vp);
        __m256i qp_1 = _mm256_mul_epu32(q_1, vp);
        __m256i c_0 = _mm256_sub_epi64(ab_0, qp_0);
        __m256i c_1 = _mm256_sub_epi64(ab_1, qp_1);

        c_0 = _mm256_and_si256(c_0, mask_32);
        c_1 = _mm256_and_si256(c_1, mask_32);

        __m256i p_gt_c_0 = _mm256_cmpgt_epi32(vp, c_0);
        __m256i p_gt_c_1 = _mm256_cmpgt_epi32(vp, c_1);
        __m256i sub_mask_0 = _mm256_andnot_si256(p_gt_c_0, vp);
        __m256i sub_mask_1 = _mm256_andnot_si256(p_gt_c_1, vp);
        c_0 = _mm256_sub_epi64(c_0, sub_mask_0);
        c_1 = _mm256_sub_epi64(c_1, sub_mask_1);

        // Convert to 32 bits
        // FIXME lignes a ameliorer pour "store" la bonne partie de c
        __m128i c_tmp1 = _mm256_castsi256_si128(c);
        __m128i c_tmp2 = _mm256_extracti128_si256(c, 1);

        __m128i tmp = _mm_setr_epi32((int)_mm_extract_epi64(c_tmp1, 0), (int)_mm_extract_epi64(c_tmp1, 1), (int)_mm_extract_epi64(c_tmp2, 0), (int)_mm_extract_epi64(c_tmp2, 1));

        // Store results
        _mm_storeu_si128((__m128i *)(res.elements + i), tmp);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup_algorithm(*(v.elements + i), param.b, param.b_bis, param.p);
    return res;
}
#endif

Vector shoup_scalar(Parameters param, Vector v)
{
#if NEON
    return shoup_scalar_neon(param, v);
#else
    return shoup_scalar_avx(param, v);
#endif
}
