#include "../include/shoup.h"

uint64_t shoup_algorithm(uint32_t a, uint32_t b, uint64_t b_bis, uint64_t p)
{
    assert(n_is_prime(p));

    // a * b_bis / 2^32
    uint64_t q = (a * b_bis) >> 32;

    // (a * b - q * p) % 2^32
    uint64_t c = (a * b - q * p) % (1UL << 32);

    if (c >= p)
        c -= p;
    return c;
}

Vector shoup_scalar_algorithm(Parameters param, Vector vector)
{
    int size = vector.size;
    Vector res = init_vector(size);
    int n = size - (size % 2);
    uint32_t b_bis_hi = (param.b_bis >> 32);
    uint32_t b_bis_lo = (param.b_bis);
    uint32x2_t vb = vdup_n_u32(param.b);
    uint32x2_t vp = vdup_n_u32(param.p);
    uint32x2_t vb_bis_hi = vdup_n_u32(b_bis_hi);
    uint32x2_t vb_bis_lo = vdup_n_u32(b_bis_lo);
    for (int i = 0; i < n; i += 2)
    {
        uint32x2_t va = vld1_u32(vector.elements + i);

        // calculate a * b_bis_lo and a * b_bis_hi
        uint64x2_t a_mul_b_bis_lo = vmull_u32(va, vb_bis_lo);
        uint64x2_t a_mul_b_bis_hi = vmull_u32(va, vb_bis_hi);

        // q = (a * b_bis_hi) + ((a * b_bis_lo) >> 32)
        uint64x2_t q = vaddq_u64(a_mul_b_bis_hi, vshrq_n_u64(a_mul_b_bis_lo, 32));
        uint32x2_t q32 = vmovn_u64(q);

        // (a * b - q * p) % 2^32 = (a * b - q * p) & (2^32 - 1)
        uint64x2_t vab = vmull_u32(va, vb);
        uint64x2_t vqp = vmull_u32(q32, vp);
        uint64x2_t c = vandq_u64(vsubq_u64(vab, vqp), vdupq_n_u64((1UL << 32) - 1));
        uint64x2_t p64 = vmovl_u32(vp);

        // compare c >= p
        uint64x2_t cmp = vcgeq_u64(c, p64);
        uint64x2_t p_to_sub = vandq_u64(cmp, p64);
        c = vsubq_u64(c, p_to_sub);

        // stocks the value
        vst1_u32(res.elements + i, vmovn_u64(c));
    }
    for (int i = n; i < size; i++)
        *(res.elements + i) = shoup_algorithm(*(vector.elements + i), param.b, param.b_bis, param.p);
    return res;
}