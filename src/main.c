#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"
#include <immintrin.h>
#include "flint/nmod.h"
#include "flint/ulong_extras.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(96, rand_prime(1000000));
    print_param(param1);
    Vector rand_v = rand_vector(100, param1.p);
    print_vector(rand_v);
    Vector v = shoup_scalar_neon(param1, rand_v);
    printf("%f s\n", time_algorithm(shoup_scalar_neon, param1, v));
    print_vector(v);
    printf("\n");
    Vector v1 = naive_scalar_product(rand_v, param1.b, param1.p);
    print_vector(v1);
    printf("%d\n", compare_vectors(v, v1));

    /* printf("--------FLINT-------------\n");
    ulong p = 17;
    nmod_t mod;
    nmod_init(&mod, p);

    ulong a = 10;
    ulong b = 12;
    ulong res;

    printf("Calculs modulo p = %lu\n", (unsigned long)p);
    printf("a = %lu, b = %lu\n\n", (unsigned long)a, (unsigned long)b);

    // 2. Somme : (a + b) mod p
    res = nmod_add(a, b, mod);
    printf("Somme (a + b): %lu\n", (unsigned long)res);

    // 3. Produit : (a * b) mod p
    res = nmod_mul(a, b, mod);
    printf("Produit (a * b): %lu\n", (unsigned long)res);

    // 4. Inversion : a^-1 mod p
    res = nmod_inv(a, mod);
    if (res != 0)
    {
        printf("Inverse de a = 10: %lu  (Verification: %lu * %lu mod %lu = %lu)\n",
               (unsigned long)res,
               (unsigned long)a,
               (unsigned long)res,
               (unsigned long)p,
               (unsigned long)nmod_mul(a, res, mod));
    }
    else
    {
        printf("a = 10 n'est pas inversible modulo p\n");
    } */

    printf("\n-------------Vectorization of multiplication scalar vector-----------\n");
    int size = rand_v.size;
    Vector res_vec = init_vector(size);
    uint64_t *res2 = malloc(size * sizeof(uint64_t));
    // Correct Precomputation (Scalar C)
    uint64_t p = param1.p;
    uint64_t b = param1.b;

    // You MUST use 128-bit here to prevent overflow before the division
    uint64_t reduced_b = b % p; // 800 % 97 = 24
    uint64_t b_bis = ((uint64_t)reduced_b << 32) / p;

    __m256i vb = _mm256_set1_epi64x(reduced_b);
    __m256i vb_bis = _mm256_set1_epi64x(b_bis);
    __m256i vp = _mm256_set1_epi64x(p);
    __m256i mask_32 = _mm256_set1_epi64x(0xFFFFFFFF);

    for (int i = 0; i < size; i += 4)
    {
        // Correct Load: 4 unique elements
        __m256i va = _mm256_loadu_si256((__m256i const *)&rand_v.elements[i]);
        va = _mm256_and_si256(va, mask_32);
        // Compute q
        __m256i q = _mm256_mul_epu32(va, vb_bis);
        q = _mm256_srli_epi64(q, 32);

        // Compute c
        __m256i ab = _mm256_mul_epu32(va, vb);
        __m256i qp = _mm256_mul_epu32(q, vp);
        __m256i c = _mm256_sub_epi64(ab, qp);

        // CRITICAL: Apply the modulo/mask here to match your scalar code
        c = _mm256_and_si256(c, mask_32);

        __m256i p_gt_c = _mm256_cmpgt_epi64(vp, c);
        __m256i sub_mask = _mm256_andnot_si256(p_gt_c, vp);
        c = _mm256_sub_epi64(c, sub_mask);

        if (i == 0)
        {
            uint64_t va_val = _mm256_extract_epi64(va, 0);
            uint64_t q_val = _mm256_extract_epi64(q, 0);
            uint64_t ab_val = _mm256_extract_epi64(ab, 0);
            uint64_t qp_val = _mm256_extract_epi64(qp, 0);

            printf("DEBUG i=0: a=%lu, q=%lu, ab=%lu, qp=%lu, b_bis=%lu\n",
                   va_val, q_val, ab_val, qp_val, b_bis);
        }

        // Store results
        _mm256_storeu_si256((__m256i *)&res2[i], c);
    }
    for (int i = 0; i < size; i++)
    {
        // Use %llu for 64-bit unsigned integers
        printf("%lu ", res2[i]);
    }

    return 0;
}
