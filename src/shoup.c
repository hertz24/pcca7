#include "../include/shoup.h"

__attribute__((optimize("O0"))) Vector shoup_scale_ref(Parameters param, Vector v)
{
    Vector res = init_vector(v.size);
    for (ulong i = 0; i < v.size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

#if NEON
static Vector _shoup_scale_neon(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    ulong n = size - (size % 4);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_precomp = vdup_n_u32(param.b_precomp);
    ulong i = 0;
    for (; i + 3 < n; i += 4)
    {
        uint32x2_t va_0 = vld1_u32(v.elements + i);
        uint32x2_t va_1 = vld1_u32(v.elements + i + 2);

        // Calculates a * b_precomp
        uint64x2_t ab_precomp_0 = vmull_u32(va_0, vb_precomp);
        uint64x2_t ab_precomp_1 = vmull_u32(va_1, vb_precomp);

        // q = (a * b_precomp) >> 32
        uint64x2_t q_0 = vshrq_n_u64(ab_precomp_0, 32);
        uint64x2_t q_1 = vshrq_n_u64(ab_precomp_1, 32);

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
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}
#endif

#if AVX2
static inline __m256i _shoup_core_avx2(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp, __m256i mask32)
{
    // 1. q = (a * b_precomp) >> 32:
    __m256i q = _mm256_mul_epu32(va, vb_precomp);
    q = _mm256_srli_epi64(q, 32);

    // 2. ab:
    __m256i ab = _mm256_mul_epu32(va, vb);

    // 3. qp:
    __m256i qp = _mm256_mul_epu32(q, vp);

    // 4. c = (ab - qp) & mask:
    __m256i c = _mm256_sub_epi64(ab, qp);
    c = _mm256_and_si256(c, mask32);

    // 5. if (c >= p) c -= p
    __m256i compare = _mm256_cmpgt_epi64(vp, c);
    __m256i sub_mask = _mm256_andnot_si256(compare, vp);

    return _mm256_sub_epi64(c, sub_mask);
}

static Vector _shoup_scale_avx256(Parameters param, Vector v)
{
    int size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_precomp = _mm256_set1_epi64x(param.b_precomp);
    __m256i vp = _mm256_set1_epi64x(param.p);
    __m256i mask_32 = _mm256_set1_epi64x(0xFFFFFFFF);

    int i = 0;
    for (; i + 31 < size; i += 32)
    {
        // 2. Load 8 elements [v7 v6 v5 v4 v3 v2 v1 v0]:
        __m256i va_input_1 = _mm256_loadu_si256((__m256i const *)(v.elements + i));
        __m256i va_input_2 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 8));
        __m256i va_input_3 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 16));
        __m256i va_input_4 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 24));

        // 3. Split 8 elements into two sets of 4 : va_input and va_odd
        __m256i va_odd_1 = _mm256_srli_epi64(va_input_1, 32); // Elements [null, 7, null, 5, null , 3, null, 1]
        __m256i va_odd_2 = _mm256_srli_epi64(va_input_2, 32);
        __m256i va_odd_3 = _mm256_srli_epi64(va_input_3, 32);
        __m256i va_odd_4 = _mm256_srli_epi64(va_input_4, 32);

        // 4. Process Even Lanes:
        __m256i c_even_1 = _shoup_core_avx2(va_input_1, vb, vb_precomp, vp, mask_32);
        __m256i c_even_2 = _shoup_core_avx2(va_input_2, vb, vb_precomp, vp, mask_32);
        __m256i c_even_3 = _shoup_core_avx2(va_input_3, vb, vb_precomp, vp, mask_32);
        __m256i c_even_4 = _shoup_core_avx2(va_input_4, vb, vb_precomp, vp, mask_32);

        // 5. Process Odd Lanes:
        __m256i c_odd_1 = _shoup_core_avx2(va_odd_1, vb, vb_precomp, vp, mask_32);
        __m256i c_odd_2 = _shoup_core_avx2(va_odd_2, vb, vb_precomp, vp, mask_32);
        __m256i c_odd_3 = _shoup_core_avx2(va_odd_3, vb, vb_precomp, vp, mask_32);
        __m256i c_odd_4 = _shoup_core_avx2(va_odd_4, vb, vb_precomp, vp, mask_32);

        // 6. Shift odd by 32 bits to the left results back and merge:
        __m256i res1_odd_shifted = _mm256_slli_epi64(c_odd_1, 32); // [7, null, 5, null, 3, null, 1, null]
        __m256i res2_odd_shifted = _mm256_slli_epi64(c_odd_2, 32);
        __m256i res3_odd_shifted = _mm256_slli_epi64(c_odd_3, 32);
        __m256i res4_odd_shifted = _mm256_slli_epi64(c_odd_4, 32);
        __m256i res1_combined = _mm256_or_si256(c_even_1, res1_odd_shifted); // [7, null, 5, null, 3, null, 1, null] or [null, 6, null, 4, null, 2, null, 0]
        __m256i res2_combined = _mm256_or_si256(c_even_2, res2_odd_shifted);
        __m256i res3_combined = _mm256_or_si256(c_even_3, res3_odd_shifted);
        __m256i res4_combined = _mm256_or_si256(c_even_4, res4_odd_shifted);

        // 7. Store:
        _mm256_storeu_si256((__m256i *)(res.elements + i), res1_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 8), res2_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 16), res3_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 24), res4_combined);
    }
    for (; i < size; i++)
        res.elements[i] = shoup(v.elements[i], param.b, param.b_precomp, param.p);
    return res;
}

static inline __m256i _shoup_core_avx2_mullo(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp)
{
    // 1. q = (a * b_precomp) >> 32:
    __m256i q = _mm256_mul_epu32(va, vb_precomp);
    q = _mm256_srli_epi64(q, 32);

    // 2. ab:
    __m256i ab = _mm256_mullo_epi32(va, vb);

    // 3. qp
    __m256i qp = _mm256_mullo_epi32(q, vp);

    // 4. c = ab - qp
    __m256i c = _mm256_sub_epi32(ab, qp);

    // 5. if (c >= p) c -= p
    __m256i compare = _mm256_cmpgt_epi64(vp, c);
    __m256i sub_mask = _mm256_andnot_si256(compare, vp);

    return _mm256_sub_epi64(c, sub_mask);
}

static Vector _shoup_scale_avx256_mullo(Parameters param, Vector v)
{
    int size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_precomp = _mm256_set1_epi64x(param.b_precomp);
    __m256i vp = _mm256_set1_epi64x(param.p);

    int i = 0;
    for (; i + 31 < size; i += 32)
    {
        // 2. Load 8 elements [v7 v6 v5 v4 v3 v2 v1 v0]:
        __m256i va_input_1 = _mm256_loadu_si256((__m256i const *)(v.elements + i));
        __m256i va_input_2 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 8));
        __m256i va_input_3 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 16));
        __m256i va_input_4 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 24));

        // 3. Split 8 elements into two sets of 4 : va_input and va_odd
        __m256i va_odd_1 = _mm256_srli_epi64(va_input_1, 32); // Elements [null, 7, null, 5, null , 3, null, 1]
        __m256i va_odd_2 = _mm256_srli_epi64(va_input_2, 32);
        __m256i va_odd_3 = _mm256_srli_epi64(va_input_3, 32);
        __m256i va_odd_4 = _mm256_srli_epi64(va_input_4, 32);

        // 4. Process Even Lanes:
        __m256i c_even_1 = _shoup_core_avx2_mullo(va_input_1, vb, vb_precomp, vp);
        __m256i c_even_2 = _shoup_core_avx2_mullo(va_input_2, vb, vb_precomp, vp);
        __m256i c_even_3 = _shoup_core_avx2_mullo(va_input_3, vb, vb_precomp, vp);
        __m256i c_even_4 = _shoup_core_avx2_mullo(va_input_4, vb, vb_precomp, vp);

        // 5. Process Odd Lanes:
        __m256i c_odd_1 = _shoup_core_avx2_mullo(va_odd_1, vb, vb_precomp, vp);
        __m256i c_odd_2 = _shoup_core_avx2_mullo(va_odd_2, vb, vb_precomp, vp);
        __m256i c_odd_3 = _shoup_core_avx2_mullo(va_odd_3, vb, vb_precomp, vp);
        __m256i c_odd_4 = _shoup_core_avx2_mullo(va_odd_4, vb, vb_precomp, vp);

        // 6. Shift odd by 32 bits to the left results back and merge:
        __m256i res1_odd_shifted = _mm256_slli_epi64(c_odd_1, 32); // [7, null, 5, null, 3, null, 1, null]
        __m256i res2_odd_shifted = _mm256_slli_epi64(c_odd_2, 32);
        __m256i res3_odd_shifted = _mm256_slli_epi64(c_odd_3, 32);
        __m256i res4_odd_shifted = _mm256_slli_epi64(c_odd_4, 32);
        __m256i res1_combined = _mm256_or_si256(c_even_1, res1_odd_shifted); // [7, null, 5, null, 3, null, 1, null] or [null, 6, null, 4, null, 2, null, 0]
        __m256i res2_combined = _mm256_or_si256(c_even_2, res2_odd_shifted);
        __m256i res3_combined = _mm256_or_si256(c_even_3, res3_odd_shifted);
        __m256i res4_combined = _mm256_or_si256(c_even_4, res4_odd_shifted);

        // 7. Store:
        _mm256_storeu_si256((__m256i *)(res.elements + i), res1_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 8), res2_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 16), res3_combined);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 24), res4_combined);
    }
    for (; i < size; i++)
        res.elements[i] = shoup(v.elements[i], param.b, param.b_precomp, param.p);
    return res;
}
#endif

Vector shoup_scale(Parameters param, Vector v)
{
#if NEON
    return _shoup_scale_neon(param, v);
#else
    return _shoup_scale_avx256_mullo(param, v);
#endif
}
