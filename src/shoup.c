#include "../include/shoup.h"

uint32_t shoup_algorithm(uint32_t a, uint32_t b, uint32_t b_bis, uint32_t p)
{
    assert(n_is_prime(p) && a < p && b < p);

    // a * b_bis / 2^32
    uint32_t q = ((uint64_t)a * b_bis) >> 32;

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
    ulong n = size - (size % 4);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_bis = vdup_n_u32(param.b_bis);
    ulong i = 0;
    for (; i + 3 < n; i += 4)
    {
        uint32x2_t va_0 = vld1_u32(v.elements + i);
        uint32x2_t va_1 = vld1_u32(v.elements + i + 2);

        // Calculates a * b_bis
        uint64x2_t ab_bis_0 = vmull_u32(va_0, vb_bis);
        uint64x2_t ab_bis_1 = vmull_u32(va_1, vb_bis);

        // q = (a * b_bis) >> 32
        uint64x2_t q_0 = vshrq_n_u64(ab_bis_0, 32);
        uint64x2_t q_1 = vshrq_n_u64(ab_bis_1, 32);

        // Convert uint64x2_t to uint32x2_t
        uint32x2_t q32_0 = vmovn_u64(q_0);
        uint32x2_t q32_1 = vmovn_u64(q_1);

        // c = (a * b - q * p) % 2^32 = (a * b - q * p) & (2^32 - 1)
        uint64x2_t vab_0 = vmull_u32(va_0, vb);
        uint64x2_t vab_1 = vmull_u32(va_1, vb);
        uint64x2_t vqp_0 = vmull_u32(q32_0, vp);
        uint64x2_t vqp_1 = vmull_u32(q32_1, vp);
        uint64x2_t c_0 = vandq_u64(vsubq_u64(vab_0, vqp_0), vdupq_n_u64((1UL << 32) - 1));
        uint64x2_t c_1 = vandq_u64(vsubq_u64(vab_1, vqp_1), vdupq_n_u64((1UL << 32) - 1));
        uint32x2_t c32_0 = vmovn_u64(c_0);
        uint32x2_t c32_1 = vmovn_u64(c_1);

        // Compare c >= p
        uint32x2_t cmp_0 = vcge_u32(c32_0, vp);
        uint32x2_t cmp_1 = vcge_u32(c32_1, vp);
        uint32x2_t p_to_sub_0 = vand_u32(cmp_0, vp);
        uint32x2_t p_to_sub_1 = vand_u32(cmp_1, vp);
        c32_0 = vsub_u32(c32_0, p_to_sub_0);
        c32_1 = vsub_u32(c32_1, p_to_sub_1);

        // Stocks the value
        vst1_u32(res.elements + i, c32_0);
        vst1_u32(res.elements + i + 2, c32_1);
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

    int i = 0;
    for (; i + 7 < size; i += 8)
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
        __m128i c_tmp1 = _mm256_castsi256_si128(c_0);
        __m128i c_tmp2 = _mm256_extracti128_si256(c_1, 1);

        __m128i tmp = _mm_setr_epi32((int)_mm_extract_epi64(c_tmp1, 0), (int)_mm_extract_epi64(c_tmp1, 1), (int)_mm_extract_epi64(c_tmp2, 0), (int)_mm_extract_epi64(c_tmp2, 1));

        // Store results
        _mm_storeu_si128((__m128i *)(res.elements + i), tmp);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup_algorithm(*(v.elements + i), param.b, param.b_bis, param.p);
    return res;
}

static inline __m256i shoup_core_avx2(__m256i va, __m256i vb, __m256i vb_bis, __m256i vp, __m256i mask32) {
    // 1. q = (a * b_bis) >> 32:
    __m256i q = _mm256_mul_epu32(va, vb_bis);
    q = _mm256_srli_epi64(q, 32);

    // 2. ab:
    __m256i ab = _mm256_mul_epu32(va, vb);

    // 3. qp:
    __m256i qp = _mm256_mul_epu32(q, vp);

    // 4. c = (ab - qp) & mask:
    __m256i c = _mm256_sub_epi64(ab, qp);
    c = _mm256_and_si256(c, mask32);

    // 5. if (c >= p) c -= p
    __m256i p_gt_c = _mm256_cmpgt_epi64(vp, c);
    __m256i sub_mask = _mm256_andnot_si256(p_gt_c, vp);
    
    return _mm256_sub_epi64(c, sub_mask);
}

static Vector shoup_scalar_avx_256(Parameters param, Vector v)
{
    int size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_bis = _mm256_set1_epi64x(param.b_bis);
    __m256i vp = _mm256_set1_epi64x(param.p);
    __m256i mask_32 = _mm256_set1_epi64x(0xFFFFFFFF);

    int i = 0;
    for (; i + 31 < size; i += 32) {
        // 2. Load 8 elements [v7 v6 v5 v4 v3 v2 v1 v0]:
        __m256i v1_input = _mm256_loadu_si256((__m256i const *)(v.elements + i));
        __m256i v2_input = _mm256_loadu_si256((__m256i const *)(v.elements + 8));
        __m256i v3_input = _mm256_loadu_si256((__m256i const *)(v.elements + 16));
        __m256i v4_input = _mm256_loadu_si256((__m256i const *)(v.elements + 24));

        // 3. Split 8 elements into two sets of 4 :
        __m256i va_even_1 = _mm256_and_si256(v1_input, mask_32); // Elements [null, 6, null, 4, null, 2, null, 0]
        __m256i va_even_2 = _mm256_and_si256(v2_input, mask_32); 
        __m256i va_even_3 = _mm256_and_si256(v3_input, mask_32); 
        __m256i va_even_4 = _mm256_and_si256(v4_input, mask_32); 
        __m256i va_odd_1  = _mm256_srli_epi64(v1_input, 32);    // Elements [null, 7, null, 5, null , 3, null, 1]
        __m256i va_odd_2  = _mm256_srli_epi64(v2_input, 32);    
        __m256i va_odd_3  = _mm256_srli_epi64(v3_input, 32);    
        __m256i va_odd_4  = _mm256_srli_epi64(v4_input, 32);    

        // 4. Process Even Lanes:
        __m256i c_even_1 = shoup_core_avx2(va_even_1, vb, vb_bis, vp, mask_32);
        __m256i c_even_2 = shoup_core_avx2(va_even_2, vb, vb_bis, vp, mask_32);
        __m256i c_even_3 = shoup_core_avx2(va_even_3, vb, vb_bis, vp, mask_32);
        __m256i c_even_4 = shoup_core_avx2(va_even_4, vb, vb_bis, vp, mask_32);

        // 5. Process Odd Lanes:
        __m256i c_odd_1  = shoup_core_avx2(va_odd_1,  vb, vb_bis, vp, mask_32);
        __m256i c_odd_2  = shoup_core_avx2(va_odd_2,  vb, vb_bis, vp, mask_32);
        __m256i c_odd_3  = shoup_core_avx2(va_odd_3,  vb, vb_bis, vp, mask_32);
        __m256i c_odd_4  = shoup_core_avx2(va_odd_4,  vb, vb_bis, vp, mask_32);

        // 6. Shift odd by 32 bits to the left results back and merge:
        __m256i res1_odd_shifted = _mm256_slli_epi64(c_odd_1, 32); // [7, null, 5, null, 3, null, 1, null]
        __m256i res2_odd_shifted = _mm256_slli_epi64(c_odd_2, 32); 
        __m256i res3_odd_shifted = _mm256_slli_epi64(c_odd_3, 32); 
        __m256i res4_odd_shifted = _mm256_slli_epi64(c_odd_4, 32); 
        __m256i res1_combined    = _mm256_or_si256(c_even_1, res1_odd_shifted); // [7, null, 5, null, 3, null, 1, null] or [null, 6, null, 4, null, 2, null, 0]
        __m256i res2_combined    = _mm256_or_si256(c_even_2, res2_odd_shifted); 
        __m256i res3_combined    = _mm256_or_si256(c_even_3, res3_odd_shifted); 
        __m256i res4_combined    = _mm256_or_si256(c_even_4, res4_odd_shifted); 

        // 7. Store:
        _mm256_storeu_si256((__m256i *)(res.elements + i), res1_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + 8), res2_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + 16), res3_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + 24), res4_combined);
    }

    for (; i < size; i++) {
        res.elements[i] = shoup_algorithm(v.elements[i], param.b, param.b_bis, param.p);
    }

    return res;
}
#endif

Vector shoup_scalar(Parameters param, Vector v)
{
#if NEON
    return shoup_scalar_neon(param, v);
#else
    return shoup_scalar_avx_256(param, v);
#endif
}
