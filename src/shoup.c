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
    // vab_precomp = [ a_0 * b_precomp_0 = ab_precomp_0, a_1 * b_precomp_1 = ab_precomp_1 ]
    uint64x2_t vab_precomp = vmull_u32(va, vb_precomp);

    // vq = [ q_0, q_1 ]
    uint32x2_t vq = vshrn_n_u64(vab_precomp, 32);

    // vab = [ a_0 * b_0 = ab_0, a_1 * b_1 = ab_1 ]
    uint64x2_t vab = vmull_u32(va, vb);

    // vqp = [ q_0 * p_0 = qp_0, q_1 * p_1 = qp_1 ]
    uint64x2_t vqp = vmull_u32(vq, vp);

    // vc = [ ab_0 - qp_0 = c_0, ab_1 - qp_1 = c_1 ]
    uint32x2_t vc = vmovn_u64(vsubq_u64(vab, vqp));

    // if (c >= p) c -= p
    return vmin_u32(vc, vsub_u32(vc, vp));
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
        // Load [ a_0, a_1 ]
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
    // vab_precomp = [ a_0 * b_precomp_0 = ab_precomp_0, a_1 * b_precomp_1 = ab_precomp_1 ]
    uint64x2_t vab_precomp = vmull_u32(va, vb_precomp);

    // vq = [ q_0, q_1 ]
    uint32x2_t vq = vshrn_n_u64(vab_precomp, 32);

    // vab = [ a_0 * b_0 mod 2^32 = ab_0, a_1 * b_1 mod 2^32 = ab_1 ]
    uint32x2_t vab = vmul_u32(va, vb);

    // vqp = [ q_0 * p_0 mod 2^32 = qp_0, q_1 * p_1 mod 2^32 = qp_1 ]
    uint32x2_t vqp = vmul_u32(vq, vp);

    // vc = [ ab_0 - qp_0 = c_0, ab_1 - qp_1 = c_1]
    uint32x2_t vc = vsub_u32(vab, vqp);
    return vmin_u32(vc, vsub_u32(vc, vp));
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
    uint32x2_t vq = vshrn_n_u64(vab_precomp, 32);
    uint64x2_t vqp = vmull_u32(vq, vp);
    uint32x2_t vc = vmovn_u64(vsubq_u64(vmovl_u32(va), vqp));
    return vmin_u32(vc, vsub_u32(vc, vp));
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
static inline __m256i _shoup_avx2(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp)
{
    // vq = [ (a_0 * b_precomp_0 = q_0q_1), (a_2 * b_precomp_2 = q_2q_3), (a_4 * b_precomp_4 = q_4q_5), (a_6 * b_precomp_6 = q_6q_7) ]
    __m256i vq = _mm256_mul_epu32(va, vb_precomp);

    // vq = [ (0, q_0), (0, q_2), (0, q_4), (0, q_6) ]
    vq = _mm256_srli_epi64(vq, 32);

    // vab = [ (a_0 * b_0 = ab_0ab_1), (a_2 * b_2 = ab_2ab_3), (a_4 * b_4 = ab_4ab_5), (a_6 * b_6 = ab_6ab_7) ]
    __m256i vab = _mm256_mul_epu32(va, vb);

    // vqp = [ (q_0 * p_0 = qp_0qp_1), (q_2 * p_2 = qp_2qp_3), (q_4 * p_4 = qp_4qp_5), (q_6 * p_6 = qp_6qp_7) ]
    __m256i vqp = _mm256_mul_epu32(vq, vp);

    // vc = [ (ab_0 - qp_0 = c_0c_1), (ab_2 - qp_2 = c_2c_3), (ab_4 - qp_4 = c_4c_5), (ab_6 - qp_6 = c_6c_7) ]
    __m256i vc = _mm256_sub_epi64(vab, vqp);

    // if (c >= p) c -= p
    return _mm256_min_epu32(vc, _mm256_sub_epi32(vc, vp));
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
        // Load [ a_0, a_1, a_2, a_3, a_4, a_5, a_6, a_7 ] = [ (a_1, a_0), (a_3, a_2), (a_5, a_4), (a_7, a_6) ] but only even index will be directly handled
        __m256i even_va = _mm256_loadu_si256((__m256i const *)(v.elements + i));

        // Need to move odd index to even index to be handled: [ (0, a_1), (0, a_3), (0, a_5), (0, a_7) ]
        __m256i odd_va = _mm256_srli_epi64(even_va, 32);

        // Process Even Lanes
        __m256i even_vc = _shoup_avx2(even_va, vb, vb_precomp, vp);

        // Process Odd Lanes:
        __m256i odd_vc = _shoup_avx2(odd_va, vb, vb_precomp, vp);

        // Shift odd by 32 bits to the left results back and merge: [ (c_1, 0), (c_3, 0), (c_5, 0), (c_7, 0) ]
        __m256i odd_shifted = _mm256_slli_epi64(odd_vc, 32);

        // Merge even and odd: [ c_0, c_1, c_2, c_3, c_4, c_5, c_6, c_7 ]
        __m256i merge = _mm256_or_si256(even_vc, odd_shifted);

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
    // vq = [ (a_0 * b_precomp_0 = q_0q_1), (a_2 * b_precomp_2 = q_2q_3), (a_4 * b_precomp_4 = q_4q_5), (a_6 * b_precomp_6 = q_6q_7) ]
    __m256i vq = _mm256_mul_epu32(va, vb_precomp);

    // vq = [ (0, q_0), (0, q_2), (0, q_4), (0, q_6) ]
    vq = _mm256_srli_epi64(vq, 32);

    // Preserves only the least significant bit: vab = [ (a_0 * b_0 mod 2^32 = ab_0), (a_2 * b_2 mod 2^32 = ab_2), (a_4 * b_4 mod 2^32 = ab_4), (a_6 * b_6 mod 2^32 = ab_6) ]
    __m256i vab = _mm256_mullo_epi32(va, vb);

    // vqp = [ [q_0 * p_0 mod 2^32 = qp_0], [q_2 * p_2 mod 2^32 = qp_2], [q_4 * p_4 mod 2^32 = qp_4], [q_6 * p_6 mod 2^32 = qp_6] ]
    __m256i vqp = _mm256_mullo_epi32(vq, vp);

    // vc = [ (0, ab_0 - qp_0), (0, ab_2 - qp_2), (0, ab_4 - qp_4), (0, ab_6 - qp_6) ]
    __m256i vc = _mm256_sub_epi32(vab, vqp);

    // if (c >= p) c -= p
    return _mm256_min_epu32(vc, _mm256_sub_epi32(vc, vp));
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

static inline __m256i _mulhi_emul_avx2(__m256i va, __m256i vb)
{
    // res_even = [ (a_0 * b_0), (a_2 * b_2), (a_4 * b_4), (a_6 * b_6) ]
    __m256i res_even = _mm256_mul_epu32(va, vb);

    // res_odd = [ (a_1 * b_0), (a_3 * b_2), (a_5 * b_4), (a_7 * b_6) ]
    __m256i res_odd = _mm256_mul_epu32(_mm256_srli_epi64(va, 32), vb);

    // hi_even = [ (0, hi(a_0 * b_0)), (0, hi(a_2 * b_2)), (0, hi(a_4 * b_4)), (0, hi(a_6*b_6)) ]
    __m256i hi_even = _mm256_srli_epi64(res_even, 32);

    // Removes the least significant bits: hi_odd = [ (hi(a_1 * b_0), 0), (hi(a_3 * b_2), 0), (hi(a_5 * b_4), 0), (hi(a_7 * b_6), 0) ]
    __m256i hi_odd = _mm256_and_si256(res_odd, _mm256_set1_epi64x(0xFFFFFFFF00000000ULL));

    // [ (hi(a_1 * b_0), hi(a_0 * b_0)), (hi(a_3 * b_2), hi(a_2 * b_2)), (hi(a_5 * b_4), hi(a_4 * b_4)), (hi(a_7 * b_6), hi(a_6 * b_6)) ]
    return _mm256_or_si256(hi_even, hi_odd);
}

static inline __m256i _shoup_mullo_v2_avx2(__m256i va, __m256i vb, __m256i vb_precomp, __m256i vp)
{
    __m256i vq = _mulhi_emul_avx2(va, vb_precomp);
    __m256i vab = _mm256_mullo_epi32(va, vb);
    __m256i vqp = _mm256_mullo_epi32(vq, vp);
    __m256i vc = _mm256_sub_epi32(vab, vqp);
    return _mm256_min_epu32(vc, _mm256_sub_epi32(vc, vp));
}

Vector shoup_scale_mullo_v2_avx2(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi32(param.b);
    __m256i vb_precomp = _mm256_set1_epi32(param.b_precomp);
    __m256i vp = _mm256_set1_epi32(param.p);
    ulong i = 0;
    for (; i + 7 < size; i += 8)
    {
        __m256i va = _mm256_loadu_si256((__m256i const *)(v.elements + i));

        __m256i vc = _shoup_mullo_v2_avx2(va, vb, vb_precomp, vp);
        _mm256_storeu_si256((__m256i *)(res.elements + i), vc);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
}

static inline __m256i _shoup_b1_avx2(__m256i va, __m256i vb_precomp, __m256i vp)
{
    // Clears the most significant bits
    va = _mm256_and_si256(va, _mm256_set1_epi64x(0xFFFFFFFF));

    // vq = [ (a_0 * b_precomp_0 = q_0q_1), (a_2 * b_precomp_2 = q_2q_3), (a_4 * b_precomp_4 = q_4q_5), (a_6 * b_precomp_6 = q_6q_7) ]
    __m256i vq = _mm256_mul_epu32(va, vb_precomp);

    // vq = [ (0, q_0), (0, q_2), (0, q_4), (0, q_6) ]
    vq = _mm256_srli_epi64(vq, 32);

    // vqp = [ (q_0 * p_0 = qp_0qp_1), (q_2 * p_2 = qp_2qp_3), (q_4 * p_4 = qp_4qp_5), (q_6 * p_6 = qp_6qp_7) ]
    __m256i vqp = _mm256_mul_epu32(vq, vp);

    // vc = [ (ab_0 - qp_0 = c_0c_1), (ab_2 - qp_2 = c_2c_3), (ab_4 - qp_4 = c_4c_5), (ab_6 - qp_6 = c_6c_7) ]
    __m256i vc = _mm256_sub_epi64(va, vqp);

    // if (c >= p) c -= p
    return _mm256_min_epu32(vc, _mm256_sub_epi32(vc, vp));
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
    return _mm512_min_epu32(vc, _mm512_sub_epi32(vc, vp));
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
    return _mm512_min_epu32(vc, _mm512_sub_epi32(vc, vp));
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
    va = _mm512_and_si512(va, _mm512_set1_epi64(0xFFFFFFFF));
    __m512i vq = _mm512_mul_epu32(va, vb_precomp);
    vq = _mm512_srli_epi64(vq, 32);
    __m512i vqp = _mm512_mul_epu32(vq, vp);
    __m512i vc = _mm512_sub_epi64(va, vqp);
    return _mm512_min_epu32(vc, _mm512_sub_epi32(vc, vp));
}

static inline __m512i _mulhi_emul_avx512(__m512i va, __m512i vb)
{
    __m512i res_even = _mm512_mul_epu32(va, vb);
    __m512i res_odd = _mm512_mul_epu32(_mm512_srli_epi64(va, 32), vb);
    __m512i hi_even = _mm512_srli_epi64(res_even, 32);
    __m512i hi_odd = _mm512_and_si512(res_odd, _mm512_set1_epi64(0xFFFFFFFF00000000ULL));
    return _mm512_or_si512(hi_even, hi_odd);
}

static inline __m512i _shoup_mullo_v2_avx512(__m512i va, __m512i vb, __m512i vb_precomp, __m512i vp)
{
    __m512i vq = _mulhi_emul_avx512(va, vb_precomp);
    __m512i vab = _mm512_mullo_epi32(va, vb);
    __m512i vqp = _mm512_mullo_epi32(vq, vp);
    __m512i vc = _mm512_sub_epi32(vab, vqp);
    return _mm512_min_epu32(vc, _mm512_sub_epi32(vc, vp));
}

Vector shoup_scale_mullo_v2_avx512(Parameters param, Vector v)
{
    ulong size = v.size;
    Vector res = init_vector(size);
    __m512i vb = _mm512_set1_epi32(param.b);
    __m512i vb_precomp = _mm512_set1_epi32(param.b_precomp);
    __m512i vp = _mm512_set1_epi32(param.p);
    ulong i = 0;
    for (; i + 15 < size; i += 16)
    {
        __m512i va = _mm512_loadu_si512((__m512i const *)(v.elements + i));

        __m512i vc = _shoup_mullo_v2_avx512(va, vb, vb_precomp, vp);
        _mm512_storeu_si512((__m512i *)(res.elements + i), vc);
    }
    for (; i < size; i++)
        *(res.elements + i) = shoup(*(v.elements + i), param.b, param.b_precomp, param.p);
    return res;
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
