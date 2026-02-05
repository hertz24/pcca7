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
    int size = v.size;
    Vector res = init_vector(size);
    int n = size - (size % 2);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_bis = vdup_n_u32(param.b_bis);
    for (int i = 0; i < n; i += 2)
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
    for (int i = n; i < size; i++)
        *(res.elements + i) = shoup_algorithm(*(v.elements + i), param.b, param.b_bis, param.p);
    return res;
}
#endif

#if AVX2
static Vector shoup_scalar_avx(Parameters param, Vector v)
{
    int size = v.size;
    int n = size - (size % 4);
    Vector res = init_vector(size);
    __m256i vb = _mm256_set1_epi64x(param.b);
    __m256i vb_bis = _mm256_set1_epi64x(param.b_bis);
    __m256i vp = _mm256_set1_epi64x(param.p);
    __m256i mask_32 = _mm256_set1_epi64x(0xFFFFFFFF);

    for (int i = 0; i < n; i += 4)
    {
        // Load 4 elements in 32 bits
        __m128i va128 = _mm_loadu_si128((__m128i const *)(v.elements + i));

        // Convert to 64 bits
        __m256i va = _mm256_cvtepu32_epi64(va128);

        // Calculates a * b_bis
        __m256i ab_bis = _mm256_mul_epu32(va, vb_bis);

        // Compute q
        __m256i q = _mm256_srli_epi64(ab_bis, 32);

        // Compute c
        __m256i ab = _mm256_mul_epu32(va, vb);
        __m256i qp = _mm256_mul_epu32(q, vp);
        __m256i c = _mm256_sub_epi64(ab, qp);

        // CRITICAL: Apply the modulo/mask here to match your scalar code
        c = _mm256_and_si256(c, mask_32);

        __m256i p_gt_c = _mm256_cmpgt_epi64(vp, c);
        __m256i sub_mask = _mm256_andnot_si256(p_gt_c, vp);
        c = _mm256_sub_epi64(c, sub_mask);

        // Convert to 32 bits
        __m128i c_tmp1 = _mm256_castsi256_si128(c);
        __m128i c_tmp2 = _mm256_extracti128_si256(c, 1);

        __m128i tmp = _mm_setr_epi32((int)_mm_extract_epi64(c_tmp1, 0), (int)_mm_extract_epi64(c_tmp1, 1), (int)_mm_extract_epi64(c_tmp2, 0), (int)_mm_extract_epi64(c_tmp2, 1));

        // Store results
        _mm_storeu_si128((__m128i *)(res.elements + i), tmp);
    }
    for (int i = n; i < size; i++)
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