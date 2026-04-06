#include "../include/shoup.h"

__attribute__((optimize("no-tree-vectorize"))) Vector shoup_scale_ref(Parameters param, Vector v)
{
    Vector res = init_vector(v.size);
    for (ulong i = 0; i < v.size; i++)
        *(res.elements + i) = shoup_ref(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

Vector shoup_scale_flint(Parameters param, Vector v)
{
    Vector res = init_vector(v.size);
    /*
     * NOTE: double precomputation
     */
    ulong b_precomp = n_mulmod_precomp_shoup(param.b, param.p);
    for (ulong i = 0; i < v.size; i++)
        *(res.elements + i) = n_mulmod_shoup(param.b, *(v.elements + i), b_precomp, param.p);
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

Vector shoup_scale_neon(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    ulong n = size - (size % 4);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_precomp = vdup_n_u32(param.b_precomp);
    ulong i = 0;
    for (; i + 1 < n; i += 2)
    {
        uint32x2_t va = vld1_u32(v.elements + i);

        // Stocks the value
        vst1_u32(res.elements + i, _shoup_neon(va, vb, vb_precomp, vp));
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

Vector unrolling_shoup_scale_neon(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    ulong n = size - (size % 4);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_precomp = vdup_n_u32(param.b_precomp);
    ulong i = 0;
    for (; i + 7 < n; i += 8)
    {
        uint32x2_t va_0 = vld1_u32(v.elements + i);
        uint32x2_t va_1 = vld1_u32(v.elements + i + 2);
        uint32x2_t va_2 = vld1_u32(v.elements + i + 4);
        uint32x2_t va_3 = vld1_u32(v.elements + i + 6);

        // Stocks the value
        vst1_u32(res.elements + i, _shoup_neon(va_0, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 2, _shoup_neon(va_1, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 4, _shoup_neon(va_2, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 6, _shoup_neon(va_3, vb, vb_precomp, vp));
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline uint32x2_t _shoup_mullo_neon(uint32x2_t va, uint32x2_t vb, uint32x2_t vb_precomp, uint32x2_t vp)
{
    uint64x2_t vab_precomp = vmull_u32(va, vb_precomp);
    uint32x2_t vq = vmovn_u64(vshrq_n_u64(vab_precomp, 32));
    uint32x2_t vab = vmul_u32(va, vb);
    uint32x2_t vqp = vmul_u32(vq, vp);
    uint32x2_t vc = vsub_u32(vab, vqp);
    uint32x2_t cmp = vcge_u32(vc, vp);
    return vsub_u32(vc, vand_u32(cmp, vp));
}

Vector shoup_scale_mullo_neon(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    ulong n = size - (size % 4);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_precomp = vdup_n_u32(param.b_precomp);
    ulong i = 0;
    for (; i + 7 < n; i += 8)
    {
        uint32x2_t va_0 = vld1_u32(v.elements + i);
        uint32x2_t va_1 = vld1_u32(v.elements + i + 2);
        uint32x2_t va_2 = vld1_u32(v.elements + i + 4);
        uint32x2_t va_3 = vld1_u32(v.elements + i + 6);
        vst1_u32(res.elements + i, _shoup_mullo_neon(va_0, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 2, _shoup_mullo_neon(va_1, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 4, _shoup_mullo_neon(va_2, vb, vb_precomp, vp));
        vst1_u32(res.elements + i + 6, _shoup_mullo_neon(va_3, vb, vb_precomp, vp));
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline uint32x2_t _shoup_b1_neon(uint32x2_t va, uint32x2_t vb_precomp, uint32x2_t vp)
{
    uint64x2_t vab_precomp = vmull_u32(va, vb_precomp);
    uint32x2_t vq = vmovn_u64(vshrq_n_u64(vab_precomp, 32));
    uint64x2_t vqp = vmull_u32(vq, vp);
    uint32x2_t vc = vmovn_u64(vsubq_u64(vmovl_u32(va), vqp));
    uint32x2_t cmp = vcge_u32(vc, vp);
    return vsub_u32(vc, vand_u32(cmp, vp));
}

Vector shoup_b1_scale_neon(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    ulong n = size - (size % 4);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_precomp = vdup_n_u32(param.b_precomp);
    ulong i = 0;
    for (; i + 7 < n; i += 8)
    {
        uint32x2_t va_0 = vld1_u32(v.elements + i);
        uint32x2_t va_1 = vld1_u32(v.elements + i + 2);
        uint32x2_t va_2 = vld1_u32(v.elements + i + 4);
        uint32x2_t va_3 = vld1_u32(v.elements + i + 6);

        // Stocks the value
        vst1_u32(res.elements + i, _shoup_b1_neon(va_0, vb_precomp, vp));
        vst1_u32(res.elements + i + 2, _shoup_b1_neon(va_1, vb_precomp, vp));
        vst1_u32(res.elements + i + 4, _shoup_b1_neon(va_2, vb_precomp, vp));
        vst1_u32(res.elements + i + 6, _shoup_b1_neon(va_3, vb_precomp, vp));
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}
#elif AVX2
static inline __m256i 2_shoup_avx2(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp)
{
    // 1. q = (a * b_precomp) >> 32:
    __m256i vq = _mm256_mul_epu32(va, vb_precomp);
    vq = _mm256_srli_epi64(vq, 32);

    // 2. ab:
    __m256i vab = _mm256_mul_epu32(va, vb);

    // 3. qp:
    __m256i vqp = _mm256_mul_epu32(vq, vp);

    // 4. c = (ab - qp):
    __m256i vc = _mm256_sub_epi64(vab, vqp);

    // 5. if (c >= p) c -= p
    __m256i cmp = _mm256_cmpgt_epi64(vp, vc);
    __m256i sub_mask = _mm256_andnot_si256(cmp, vp);

    return _mm256_sub_epi64(vc, sub_mask);
}

Vector shoup_scale_avx2(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_precomp = _mm256_set1_epi64x(param.b_precomp);
    __m256i vp = _mm256_set1_epi64x(param.p);
    ulong i = 0;
    for (; i + 7 < size; i += 8)
    {
        // 2. Load 8 elements [v7 v6 v5 v4 v3 v2 v1 v0]:
        __m256i even_va = _mm256_loadu_si256((__m256i const *)(v.elements + i));

        // 3. Split 8 elements into two sets of 4 : even_va and odd_va
        __m256i odd_va = _mm256_srli_epi64(even_va, 32); // Elements [null, 7, null, 5, null , 3, null, 1]

        // 4. Process Even Lanes:
        __m256i even_vc = _shoup_avx2(even_va, vb, vb_precomp, vp);

        // 5. Process Odd Lanes:
        __m256i odd_vc = _shoup_avx2(odd_va, vb, vb_precomp, vp);

        // 6. Shift odd by 32 bits to the left results back and merge:
        __m256i odd_shifted = _mm256_slli_epi64(odd_vc, 32);   // [7, null, 5, null, 3, null, 1, null]
        __m256i merge = _mm256_or_si256(even_vc, odd_shifted); // [7, null, 5, null, 3, null, 1, null] or [null, 6, null, 4, null, 2, null, 0]

        // 7. Store:
        _mm256_storeu_si256((__m256i *)(res.elements + i), merge);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

Vector unrolling_shoup_scale_avx2(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_precomp = _mm256_set1_epi64x(param.b_precomp);
    __m256i vp = _mm256_set1_epi64x(param.p);
    ulong i = 0;
    for (; i + 31 < size; i += 32)
    {
        __m256i even_va_0 = _mm256_loadu_si256((__m256i const *)(v.elements + i));
        __m256i even_va_1 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 8));
        __m256i even_va_2 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 16));
        __m256i even_va_3 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 24));

        __m256i odd_va_0 = _mm256_srli_epi64(even_va_0, 32);
        __m256i odd_va_1 = _mm256_srli_epi64(even_va_1, 32);
        __m256i odd_va_2 = _mm256_srli_epi64(even_va_2, 32);
        __m256i odd_va_3 = _mm256_srli_epi64(even_va_3, 32);

        __m256i even_vc_0 = _shoup_avx2(even_va_0, vb, vb_precomp, vp);
        __m256i even_vc_1 = _shoup_avx2(even_va_1, vb, vb_precomp, vp);
        __m256i even_vc_2 = _shoup_avx2(even_va_2, vb, vb_precomp, vp);
        __m256i even_vc_3 = _shoup_avx2(even_va_3, vb, vb_precomp, vp);

        __m256i odd_vc_0 = _shoup_avx2(odd_va_0, vb, vb_precomp, vp);
        __m256i odd_vc_1 = _shoup_avx2(odd_va_1, vb, vb_precomp, vp);
        __m256i odd_vc_2 = _shoup_avx2(odd_va_2, vb, vb_precomp, vp);
        __m256i odd_vc_3 = _shoup_avx2(odd_va_3, vb, vb_precomp, vp);

        __m256i odd_shifted_0 = _mm256_slli_epi64(odd_vc_0, 32);
        __m256i odd_shifted_1 = _mm256_slli_epi64(odd_vc_1, 32);
        __m256i odd_shifted_2 = _mm256_slli_epi64(odd_vc_2, 32);
        __m256i odd_shifted_3 = _mm256_slli_epi64(odd_vc_3, 32);

        __m256i merge_0 = _mm256_or_si256(even_vc_0, odd_shifted_0);
        __m256i merge_1 = _mm256_or_si256(even_vc_1, odd_shifted_1);
        __m256i merge_2 = _mm256_or_si256(even_vc_2, odd_shifted_2);
        __m256i merge_3 = _mm256_or_si256(even_vc_3, odd_shifted_3);

        _mm256_storeu_si256((__m256i *)(res.elements + i), merge_0);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 8), merge_1);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 16), merge_2);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 24), merge_3);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline __m256i _shoup_mullo_avx2(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp)
{
    __m256i vq = _mm256_mul_epu32(va, vb_precomp);
    vq = _mm256_srli_epi64(vq, 32);
    __m256i vab = _mm256_mullo_epi32(va, vb);
    __m256i vqp = _mm256_mullo_epi32(vq, vp);
    __m256i vc = _mm256_sub_epi32(vab, vqp);
    __m256i cmp = _mm256_cmpgt_epi64(vp, vc);
    __m256i sub_mask = _mm256_andnot_si256(cmp, vp);
    return _mm256_sub_epi64(vc, sub_mask);
}

Vector shoup_scale_mullo_avx2(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_precomp = _mm256_set1_epi64x(param.b_precomp);
    __m256i vp = _mm256_set1_epi64x(param.p);
    ulong i = 0;
    for (; i + 31 < size; i += 32)
    {
        __m256i even_va_0 = _mm256_loadu_si256((__m256i const *)(v.elements + i));
        __m256i even_va_1 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 8));
        __m256i even_va_2 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 16));
        __m256i even_va_3 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 24));

        __m256i odd_va_0 = _mm256_srli_epi64(even_va_0, 32);
        __m256i odd_va_1 = _mm256_srli_epi64(even_va_1, 32);
        __m256i odd_va_2 = _mm256_srli_epi64(even_va_2, 32);
        __m256i odd_va_3 = _mm256_srli_epi64(even_va_3, 32);

        __m256i even_vc_0 = _shoup_mullo_avx2(even_va_0, vb, vb_precomp, vp);
        __m256i even_vc_1 = _shoup_mullo_avx2(even_va_1, vb, vb_precomp, vp);
        __m256i even_vc_2 = _shoup_mullo_avx2(even_va_2, vb, vb_precomp, vp);
        __m256i even_vc_3 = _shoup_mullo_avx2(even_va_3, vb, vb_precomp, vp);

        __m256i odd_vc_0 = _shoup_mullo_avx2(odd_va_0, vb, vb_precomp, vp);
        __m256i odd_vc_1 = _shoup_mullo_avx2(odd_va_1, vb, vb_precomp, vp);
        __m256i odd_vc_2 = _shoup_mullo_avx2(odd_va_2, vb, vb_precomp, vp);
        __m256i odd_vc_3 = _shoup_mullo_avx2(odd_va_3, vb, vb_precomp, vp);

        __m256i odd_shifted_0 = _mm256_slli_epi64(odd_vc_0, 32);
        __m256i odd_shifted_1 = _mm256_slli_epi64(odd_vc_1, 32);
        __m256i odd_shifted_2 = _mm256_slli_epi64(odd_vc_2, 32);
        __m256i odd_shifted_3 = _mm256_slli_epi64(odd_vc_3, 32);

        __m256i merge_0 = _mm256_or_si256(even_vc_0, odd_shifted_0);
        __m256i merge_1 = _mm256_or_si256(even_vc_1, odd_shifted_1);
        __m256i merge_2 = _mm256_or_si256(even_vc_2, odd_shifted_2);
        __m256i merge_3 = _mm256_or_si256(even_vc_3, odd_shifted_3);

        _mm256_storeu_si256((__m256i *)(res.elements + i), merge_0);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 8), merge_1);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 16), merge_2);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 24), merge_3);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline __m256i _fake_mulhi(__m256i va, __m256i vb)
{
    __m256i res_even = _mm256_mul_epu32(va, vb);
    __m256i res_odd = _mm256_mul_epu32(_mm256_srli_epi64(va, 32), vb);

    __m256i hi_even = _mm256_srli_epi64(res_even, 32);
    __m256i hi_odd = _mm256_and_si256(res_odd, _mm256_set1_epi64x(0xFFFFFFFF00000000ULL));

    return _mm256_or_si256(hi_even, hi_odd);
}

static inline __m256i _shoup_mullo_avx2_v2(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp)
{
    __m256i vq = _fake_mulhi(va, vb_precomp);
    __m256i vab = _mm256_mullo_epi32(va, vb);
    __m256i vqp = _mm256_mullo_epi32(vq, vp);
    __m256i vc = _mm256_sub_epi32(vab, vqp);
    __m256i cmp = _mm256_cmpgt_epi64(vp, vc);
    __m256i sub_mask = _mm256_andnot_si256(cmp, vp);
    return _mm256_sub_epi64(vc, sub_mask);
}

Vector shoup_scale_mullo_avx2_v2(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_precomp = _mm256_set1_epi64x(param.b_precomp);
    __m256i vp = _mm256_set1_epi64x(param.p);
    ulong i = 0;
    for (; i + 31 < size; i += 32)
    {
        __m256i va = _mm256_loadu_si256((__m256i const *)(v.elements + i));

        __m256i vc = _shoup_mullo_avx2_v2(va, vb, vb_precomp, vp);

        _mm256_storeu_si256((__m256i *)(res.elements + i), vc);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline __m256i _shoup_b1_avx2(__m256i va, __m256i vb_precomp, __m256i vp)
{
    /*
     * NOTE: doesn't work without mask
     */
    __m256i mask_32 = _mm256_set1_epi64x(0xFFFFFFFF);
    __m256i vq = _mm256_mul_epu32(va, vb_precomp);
    vq = _mm256_srli_epi64(vq, 32);
    __m256i vqp = _mm256_mul_epu32(vq, vp);
    __m256i vc = _mm256_sub_epi64(va, vqp);
    vc = _mm256_and_si256(vc, mask_32);
    __m256i cmp = _mm256_cmpgt_epi64(vp, vc);
    __m256i sub_mask = _mm256_andnot_si256(cmp, vp);
    return _mm256_sub_epi64(vc, sub_mask);
}

Vector shoup_b1_scale_avx2(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m256i vb_precomp = _mm256_set1_epi64x(param.b_precomp);
    __m256i vp = _mm256_set1_epi64x(param.p);
    ulong i = 0;
    for (; i + 31 < size; i += 32)
    {
        __m256i even_va_0 = _mm256_loadu_si256((__m256i const *)(v.elements + i));
        __m256i even_va_1 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 8));
        __m256i even_va_2 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 16));
        __m256i even_va_3 = _mm256_loadu_si256((__m256i const *)(v.elements + i + 24));

        __m256i odd_va_0 = _mm256_srli_epi64(even_va_0, 32);
        __m256i odd_va_1 = _mm256_srli_epi64(even_va_1, 32);
        __m256i odd_va_2 = _mm256_srli_epi64(even_va_2, 32);
        __m256i odd_va_3 = _mm256_srli_epi64(even_va_3, 32);

        __m256i even_vc_0 = _shoup_b1_avx2(even_va_0, vb_precomp, vp);
        __m256i even_vc_1 = _shoup_b1_avx2(even_va_1, vb_precomp, vp);
        __m256i even_vc_2 = _shoup_b1_avx2(even_va_2, vb_precomp, vp);
        __m256i even_vc_3 = _shoup_b1_avx2(even_va_3, vb_precomp, vp);

        __m256i odd_vc_0 = _shoup_b1_avx2(odd_va_0, vb_precomp, vp);
        __m256i odd_vc_1 = _shoup_b1_avx2(odd_va_1, vb_precomp, vp);
        __m256i odd_vc_2 = _shoup_b1_avx2(odd_va_2, vb_precomp, vp);
        __m256i odd_vc_3 = _shoup_b1_avx2(odd_va_3, vb_precomp, vp);

        __m256i odd_shifted_0 = _mm256_slli_epi64(odd_vc_0, 32);
        __m256i odd_shifted_1 = _mm256_slli_epi64(odd_vc_1, 32);
        __m256i odd_shifted_2 = _mm256_slli_epi64(odd_vc_2, 32);
        __m256i odd_shifted_3 = _mm256_slli_epi64(odd_vc_3, 32);

        __m256i merge_0 = _mm256_or_si256(even_vc_0, odd_shifted_0);
        __m256i merge_1 = _mm256_or_si256(even_vc_1, odd_shifted_1);
        __m256i merge_2 = _mm256_or_si256(even_vc_2, odd_shifted_2);
        __m256i merge_3 = _mm256_or_si256(even_vc_3, odd_shifted_3);

        _mm256_storeu_si256((__m256i *)(res.elements + i), merge_0);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 8), merge_1);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 16), merge_2);
        _mm256_storeu_si256((__m256i *)(res.elements + i + 24), merge_3);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}
#endif

#if AVX512
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
    for (; i + 15 < size; i += 16)
    {
        __m512i even_va = _mm512_loadu_si512((__m512i const *)(v.elements + i));

        __m512i odd_va = _mm512_srli_epi64(even_va, 32);

        __m512i even_vc = _shoup_avx512(even_va, vb, vb_precomp, vp);

        __m512i odd_vc = _shoup_avx512(odd_va, vb, vb_precomp, vp);

        __m512i odd_shifted = _mm512_slli_epi64(odd_vc, 32);

        __m512i merge = _mm512_or_si512(even_vc, odd_shifted);

        _mm512_storeu_si512((__m512 *)(res.elements + i), merge);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

Vector unrolling_shoup_scale_avx512(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m512i vb = _mm512_set1_epi64(param.b);
    __m512i vb_precomp = _mm512_set1_epi64(param.b_precomp);
    __m512i vp = _mm512_set1_epi64(param.p);
    ulong i = 0;
    for (; i + 63 < size; i += 64)
    {
        __m512i even_va_0 = _mm512_loadu_si512((__m512i const *)(v.elements + i));
        __m512i even_va_1 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 16));
        __m512i even_va_2 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 32));
        __m512i even_va_3 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 48));

        __m512i odd_va_0 = _mm512_srli_epi64(even_va_0, 32);
        __m512i odd_va_1 = _mm512_srli_epi64(even_va_1, 32);
        __m512i odd_va_2 = _mm512_srli_epi64(even_va_2, 32);
        __m512i odd_va_3 = _mm512_srli_epi64(even_va_3, 32);

        __m512i even_vc_0 = _shoup_avx512(even_va_0, vb, vb_precomp, vp);
        __m512i even_vc_1 = _shoup_avx512(even_va_1, vb, vb_precomp, vp);
        __m512i even_vc_2 = _shoup_avx512(even_va_2, vb, vb_precomp, vp);
        __m512i even_vc_3 = _shoup_avx512(even_va_3, vb, vb_precomp, vp);

        __m512i odd_vc_0 = _shoup_avx512(odd_va_0, vb, vb_precomp, vp);
        __m512i odd_vc_1 = _shoup_avx512(odd_va_1, vb, vb_precomp, vp);
        __m512i odd_vc_2 = _shoup_avx512(odd_va_2, vb, vb_precomp, vp);
        __m512i odd_vc_3 = _shoup_avx512(odd_va_3, vb, vb_precomp, vp);

        __m512i odd_shifted_0 = _mm512_slli_epi64(odd_vc_0, 32);
        __m512i odd_shifted_1 = _mm512_slli_epi64(odd_vc_1, 32);
        __m512i odd_shifted_2 = _mm512_slli_epi64(odd_vc_2, 32);
        __m512i odd_shifted_3 = _mm512_slli_epi64(odd_vc_3, 32);

        __m512i merge_0 = _mm512_or_si512(even_vc_0, odd_shifted_0);
        __m512i merge_1 = _mm512_or_si512(even_vc_1, odd_shifted_1);
        __m512i merge_2 = _mm512_or_si512(even_vc_2, odd_shifted_2);
        __m512i merge_3 = _mm512_or_si512(even_vc_3, odd_shifted_3);

        _mm512_storeu_si512((__m512 *)(res.elements + i), merge_0);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 16), merge_1);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 32), merge_2);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 48), merge_3);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline __m512i _shoup_mullo_avx512(__m512i va, __m512i vb, __m512i vb_precomp, __m512i vp)
{
    __m512i vq = _mm512_mul_epu32(va, vb_precomp);
    vq = _mm512_srli_epi64(vq, 32);
    __m512i vab = _mm512_mullo_epi32(va, vb);
    __m512i vqp = _mm512_mullo_epi32(vq, vp);
    __m512i vc = _mm512_sub_epi32(vab, vqp);
    __mmask8 cmp = _mm512_cmple_epu64_mask(vp, vc);
    return _mm512_mask_sub_epi64(vc, cmp, vc, vp);
}

Vector shoup_scale_mullo_avx512(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m512i vb = _mm512_set1_epi64(param.b);
    __m512i vb_precomp = _mm512_set1_epi64(param.b_precomp);
    __m512i vp = _mm512_set1_epi64(param.p);
    ulong i = 0;
    for (; i + 63 < size; i += 64)
    {
        __m512i even_va_0 = _mm512_loadu_si512((__m512i const *)(v.elements + i));
        __m512i even_va_1 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 16));
        __m512i even_va_2 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 32));
        __m512i even_va_3 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 48));

        __m512i odd_va_0 = _mm512_srli_epi64(even_va_0, 32);
        __m512i odd_va_1 = _mm512_srli_epi64(even_va_1, 32);
        __m512i odd_va_2 = _mm512_srli_epi64(even_va_2, 32);
        __m512i odd_va_3 = _mm512_srli_epi64(even_va_3, 32);

        __m512i even_vc_0 = _shoup_mullo_avx512(even_va_0, vb, vb_precomp, vp);
        __m512i even_vc_1 = _shoup_mullo_avx512(even_va_1, vb, vb_precomp, vp);
        __m512i even_vc_2 = _shoup_mullo_avx512(even_va_2, vb, vb_precomp, vp);
        __m512i even_vc_3 = _shoup_mullo_avx512(even_va_3, vb, vb_precomp, vp);

        __m512i odd_vc_0 = _shoup_mullo_avx512(odd_va_0, vb, vb_precomp, vp);
        __m512i odd_vc_1 = _shoup_mullo_avx512(odd_va_1, vb, vb_precomp, vp);
        __m512i odd_vc_2 = _shoup_mullo_avx512(odd_va_2, vb, vb_precomp, vp);
        __m512i odd_vc_3 = _shoup_mullo_avx512(odd_va_3, vb, vb_precomp, vp);

        __m512i odd_shifted_0 = _mm512_slli_epi64(odd_vc_0, 32);
        __m512i odd_shifted_1 = _mm512_slli_epi64(odd_vc_1, 32);
        __m512i odd_shifted_2 = _mm512_slli_epi64(odd_vc_2, 32);
        __m512i odd_shifted_3 = _mm512_slli_epi64(odd_vc_3, 32);

        __m512i merge_0 = _mm512_or_si512(even_vc_0, odd_shifted_0);
        __m512i merge_1 = _mm512_or_si512(even_vc_1, odd_shifted_1);
        __m512i merge_2 = _mm512_or_si512(even_vc_2, odd_shifted_2);
        __m512i merge_3 = _mm512_or_si512(even_vc_3, odd_shifted_3);

        _mm512_storeu_si512((__m512 *)(res.elements + i), merge_0);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 16), merge_1);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 32), merge_2);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 48), merge_3);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}


static inline __m512i _shoup_b1_avx512(__m512i va, __m512i vb_precomp, __m512i vp)
{
    /*
     * NOTE: doesn't work without mask
     */
    __m512i mask_32 = _mm512_set1_epi64(0xFFFFFFFF);
    __m512i vq = _mm512_mul_epu32(va, vb_precomp);
    vq = _mm512_srli_epi64(vq, 32);
    __m512i vqp = _mm512_mul_epu32(vq, vp);
    __m512i vc = _mm512_sub_epi64(va, vqp);
    vc = _mm512_and_si512(vc, mask_32);
    __mmask8 cmp = _mm512_cmple_epu64_mask(vp, vc);
    return _mm512_mask_sub_epi64(vc, cmp, vc, vp);
}

Vector shoup_b1_scale_avx512(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m512i vb_precomp = _mm512_set1_epi64(param.b_precomp);
    __m512i vp = _mm512_set1_epi64(param.p);
    ulong i = 0;
    for (; i + 63 < size; i += 64)
    {
        __m512i even_va_0 = _mm512_loadu_si512((__m512i const *)(v.elements + i));
        __m512i even_va_1 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 16));
        __m512i even_va_2 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 32));
        __m512i even_va_3 = _mm512_loadu_si512((__m512i const *)(v.elements + i + 48));

        __m512i odd_va_0 = _mm512_srli_epi64(even_va_0, 32);
        __m512i odd_va_1 = _mm512_srli_epi64(even_va_1, 32);
        __m512i odd_va_2 = _mm512_srli_epi64(even_va_2, 32);
        __m512i odd_va_3 = _mm512_srli_epi64(even_va_3, 32);

        __m512i even_vc_0 = _shoup_b1_avx512(even_va_0, vb_precomp, vp);
        __m512i even_vc_1 = _shoup_b1_avx512(even_va_1, vb_precomp, vp);
        __m512i even_vc_2 = _shoup_b1_avx512(even_va_2, vb_precomp, vp);
        __m512i even_vc_3 = _shoup_b1_avx512(even_va_3, vb_precomp, vp);

        __m512i odd_vc_0 = _shoup_b1_avx512(odd_va_0, vb_precomp, vp);
        __m512i odd_vc_1 = _shoup_b1_avx512(odd_va_1, vb_precomp, vp);
        __m512i odd_vc_2 = _shoup_b1_avx512(odd_va_2, vb_precomp, vp);
        __m512i odd_vc_3 = _shoup_b1_avx512(odd_va_3, vb_precomp, vp);

        __m512i odd_shifted_0 = _mm512_slli_epi64(odd_vc_0, 32);
        __m512i odd_shifted_1 = _mm512_slli_epi64(odd_vc_1, 32);
        __m512i odd_shifted_2 = _mm512_slli_epi64(odd_vc_2, 32);
        __m512i odd_shifted_3 = _mm512_slli_epi64(odd_vc_3, 32);

        __m512i merge_0 = _mm512_or_si512(even_vc_0, odd_shifted_0);
        __m512i merge_1 = _mm512_or_si512(even_vc_1, odd_shifted_1);
        __m512i merge_2 = _mm512_or_si512(even_vc_2, odd_shifted_2);
        __m512i merge_3 = _mm512_or_si512(even_vc_3, odd_shifted_3);

        _mm512_storeu_si512((__m512 *)(res.elements + i), merge_0);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 16), merge_1);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 32), merge_2);
        _mm512_storeu_si512((__m512 *)(res.elements + i + 48), merge_3);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}
#endif
