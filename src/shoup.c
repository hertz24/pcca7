#include "../include/shoup.h"

__attribute__((optimize("no-tree-vectorize"))) Vector shoup_scale_ref(Parameters param, Vector v)
{
    Vector res = init_vector(v.size);
    for (ulong i = 0; i < v.size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

#if NEON
static inline uint32x2_t _shoup_neon(uint32x2_t va, uint32x2_t vb, uint32x2_t vb_precomp, uint32x2_t vp)
{
    // Calculates a * b_precomp
    uint64x2_t vab_precomp = vmull_u32(va, vb_precomp);

    // q = (a * b_precomp) >> 32
    // vmovn_u64 converts into 32 bits
    uint32x2_t vq = vmovn_u64(vshrq_n_u64(vab_precomp, 32));

    // c = a * b - q * p
    uint64x2_t vab = vmull_u32(va, vb);
    uint64x2_t vqp = vmull_u32(vq, vp);
    uint32x2_t vc = vmovn_u64(vsubq_u64(vab, vqp));

    // Compare c >= p
    uint32x2_t cmp = vcge_u32(vc, vp);
    return vsub_u32(vc, vand_u32(cmp, vp));
}

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
        uint32x2_t va0 = vld1_u32(v.elements + i);
        uint32x2_t va1 = vld1_u32(v.elements + i + 2);

        // Stocks the value
        vst1_u32(res.elements + i, _shoup_neon(va0, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 2, _shoup_neon(va1, vb, vb_precomp, vp));
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline uint32x2_t _shoup_neon_mullo(uint32x2_t va, uint32x2_t vb, uint32x2_t vb_precomp, uint32x2_t vp)
{
    uint64x2_t vab_precomp = vmull_u32(va, vb_precomp);
    uint32x2_t vq = vmovn_u64(vshrq_n_u64(vab_precomp, 32));
    uint32x2_t ab = vmul_u32(va, vb);
    uint32x2_t qp = vmul_u32(vq, vp);
    uint32x2_t c = vsub_u32(ab, qp);
    uint32x2_t cmp = vcge_u32(c, vp);
    return vsub_u32(c, vand_u32(cmp, vp));
}

static Vector _shoup_scale_neon_mullo(Parameters param, Vector v)
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
        uint32x2_t va0 = vld1_u32(v.elements + i);
        uint32x2_t va1 = vld1_u32(v.elements + i + 2);
        vst1_u32(res.elements + i, _shoup_neon_mullo(va0, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 2, _shoup_neon_mullo(va1, vb, vb_precomp, vp));
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}
#endif

#if AVX2
static inline __m256i _shoup_avx(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp, __m256i mask32)
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

static Vector _shoup_scale_avx(Parameters param, Vector v)
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
        __m256i c_even_1 = _shoup_avx(va_input_1, vb, vb_precomp, vp, mask_32);
        __m256i c_even_2 = _shoup_avx(va_input_2, vb, vb_precomp, vp, mask_32);
        __m256i c_even_3 = _shoup_avx(va_input_3, vb, vb_precomp, vp, mask_32);
        __m256i c_even_4 = _shoup_avx(va_input_4, vb, vb_precomp, vp, mask_32);

        // 5. Process Odd Lanes:
        __m256i c_odd_1 = _shoup_avx(va_odd_1, vb, vb_precomp, vp, mask_32);
        __m256i c_odd_2 = _shoup_avx(va_odd_2, vb, vb_precomp, vp, mask_32);
        __m256i c_odd_3 = _shoup_avx(va_odd_3, vb, vb_precomp, vp, mask_32);
        __m256i c_odd_4 = _shoup_avx(va_odd_4, vb, vb_precomp, vp, mask_32);

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

static inline __m256i _shoup_avx_mullo(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp)
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

static Vector _shoup_scale_avx_mullo(Parameters param, Vector v)
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
        __m256i c_even_1 = _shoup_avx_mullo(va_input_1, vb, vb_precomp, vp);
        __m256i c_even_2 = _shoup_avx_mullo(va_input_2, vb, vb_precomp, vp);
        __m256i c_even_3 = _shoup_avx_mullo(va_input_3, vb, vb_precomp, vp);
        __m256i c_even_4 = _shoup_avx_mullo(va_input_4, vb, vb_precomp, vp);

        // 5. Process Odd Lanes:
        __m256i c_odd_1 = _shoup_avx_mullo(va_odd_1, vb, vb_precomp, vp);
        __m256i c_odd_2 = _shoup_avx_mullo(va_odd_2, vb, vb_precomp, vp);
        __m256i c_odd_3 = _shoup_avx_mullo(va_odd_3, vb, vb_precomp, vp);
        __m256i c_odd_4 = _shoup_avx_mullo(va_odd_4, vb, vb_precomp, vp);

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

#ifdef AVX512
static inline __m512i _shoup_avx512(__m512i va, __m512i vb, __m512i vb_precomp, __m512i vp)
{
    __m512i vq = _mm512_mul_epu32(va, vb_precomp);
    vq = _mm512_srli_epi64(vq, 32);
    __m512i vab = _mm512_mul_epu32(va, vb);
    __m512i vqp = _mm512_mul_epu32(vq, vp);
    __m512i vc = _mm512_sub_epi64(vab, vqp);
    __mmask8 cmp = _mm512_cmple_epu64_mask(vp, vc);
    return _mm512_mask_sub_epi64(vc, cmp, vc, vp);
}

Vector shoup_scale_avx512(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m512i vb = _mm512_set1_epi64(param.b);
    __m512i vb_precomp = _mm512_set1_epi64(param.b_precomp);
    __m512i vp = _mm512_set1_epi64(param.p);
    ulong i = 0;
    for (; i + 63 < size; i += 64)
    {
        __m512i even_va1 = _mm512_loadu_si512((__m512i const *)(v.elements + i));
        __m512i even_va2 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 16));
        __m512i even_va3 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 32));
        __m512i even_va4 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 48));

        __m512i odd_va1 = _mm512_srli_epi64(even_va1, 32);
        __m512i odd_va2 = _mm512_srli_epi64(even_va2, 32);
        __m512i odd_va3 = _mm512_srli_epi64(even_va3, 32);
        __m512i odd_va4 = _mm512_srli_epi64(even_va4, 32);

        __m512i even_vc_1 = _shoup_avx512(even_va1, vb, vb_precomp, vp);
        __m512i even_vc_2 = _shoup_avx512(even_va2, vb, vb_precomp, vp);
        __m512i even_vc_3 = _shoup_avx512(even_va3, vb, vb_precomp, vp);
        __m512i even_vc_4 = _shoup_avx512(even_va4, vb, vb_precomp, vp);

        __m512i odd_vc_1 = _shoup_avx512(odd_va1, vb, vb_precomp, vp);
        __m512i odd_vc_2 = _shoup_avx512(odd_va2, vb, vb_precomp, vp);
        __m512i odd_vc_3 = _shoup_avx512(odd_va3, vb, vb_precomp, vp);
        __m512i odd_vc_4 = _shoup_avx512(odd_va4, vb, vb_precomp, vp);

        __m512i shift_1 = _mm512_slli_epi64(odd_vc_1, 32);
        __m512i shift_2 = _mm512_slli_epi64(odd_vc_2, 32);
        __m512i shift_3 = _mm512_slli_epi64(odd_vc_3, 32);
        __m512i shift_4 = _mm512_slli_epi64(odd_vc_4, 32);

        __m512i merge_1 = _mm512_or_si512(even_vc_1, shift_1);
        __m512i merge_2 = _mm512_or_si512(even_vc_2, shift_2);
        __m512i merge_3 = _mm512_or_si512(even_vc_3, shift_3);
        __m512i merge_4 = _mm512_or_si512(even_vc_4, shift_4);

        _mm512_storeu_si512((__m512 *)(res.elements + i), merge_1);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 16), merge_2);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 32), merge_3);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 48), merge_4);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}
#endif

Vector shoup_scale(Parameters param, Vector v)
{
#if NEON
    return _shoup_scale_neon(param, v);
#else
    return _shoup_scale_avx(param, v);
#endif
}

Vector shoup_scale_mullo(Parameters param, Vector v)
{
#if NEON
    return _shoup_scale_neon_mullo(param, v);
#else
    return _shoup_scale_avx_mullo(param, v);
#endif
}