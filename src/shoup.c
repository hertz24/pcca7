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

Vector shoup_scalar_neon(Parameters param, Vector vector)
{
    int size = vector.size;
    Vector res = init_vector(size);
    int n = size - (size % 2);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_bis = vdup_n_u32(param.b_bis);
    for (int i = 0; i < n; i += 2)
    {
        uint32x2_t va = vld1_u32(vector.elements + i);

        // calculate a * b_bis
        uint64x2_t ab_bis = vmull_u32(va, vb_bis);

        // q = (a * b_bis) >> 32
        uint64x2_t q = vshrq_n_u64(ab_bis, 32);

        // convert uint64x2_t to uint32x2_t
        uint32x2_t q32 = vmovn_u64(q);

        // c = (a * b - q * p) % 2^32 = (a * b - q * p) & (2^32 - 1)
        uint64x2_t vab = vmull_u32(va, vb);
        uint64x2_t vqp = vmull_u32(q32, vp);
        uint64x2_t c = vandq_u64(vsubq_u64(vab, vqp), vdupq_n_u64((1UL << 32) - 1));
        uint32x2_t c32 = vmovn_u64(c);

        // compare c >= p
        uint32x2_t cmp = vcge_u32(c32, vp);
        uint32x2_t p_to_sub = vand_u32(cmp, vp);
        c32 = vsub_u32(c32, p_to_sub);

        // stocks the value
        vst1_u32(res.elements + i, c32);
    }
    for (int i = n; i < size; i++)
        *(res.elements + i) = shoup_algorithm(*(vector.elements + i), param.b, param.b_bis, param.p);
    return res;
}