#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"
#include <immintrin.h>
#include "flint/nmod.h"
#include "flint/ulong_extras.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(812112989, 97);
    Vector rand_v = rand_vector(100);
    print_vector(rand_v);
    Vector v = shoup_scalar_algorithm(param1, rand_v);
    printf("%f s\n", time_algorithm(shoup_scalar_algorithm, param1, v));
    print_vector(v);

    printf("--------FLINT-------------\n");
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
    }

    printf("\n-------------Vectorization of multiplication scalar vector-----------\n");
    int size = rand_v.size;
    Vector res_vec = init_vector(size);

    int i = 0;
    for (; i < size ; i+= 4){

        //Load
        __m256i va = _mm256_loadu_si256((__m256i const*)&rand_v.elements[i]);
        __m256i vb = _mm256_set1_epi64x(param1.b);
        __m256i vb_bis = _mm256_set1_epi64x(param1.b_bis);
        __m256i vp = _mm256_set1_epi64x(param1.p);


        //Compute
        //1. a * b_bis / 2^32 >> 32
        __m256i q = _mm256_mul_epu32(va, vb_bis); 
        q = _mm256_srli_epi64(q, 32);

        //2. (a * b - q * p) % 2^32
        //2.1 a * b
        __m256i ab = _mm256_mul_epu32(va, vb);
        //2.2 q * p
        __m256i qp = _mm256_mul_epu32(q, vp);
        //2.3 ab - qp
        __m256i c = _mm256_sub_epi64(ab, qp);
        //2.4 
        __m256i mask_32 = _mm256_set1_epi64x(0xFFFFFFFF);
        __m256i c_32 = _mm256_and_si256(c, mask_32);
/*         __m256i mask_cmp = _mm256_cmpgt_epi64(c_32, vp);
        __m256i to_subtract = _mm256_and_si256(mask_cmp, vp);
        c = _mm256_sub_epi64(c, to_subtract);  */
        
       _mm256_storeu_si256((__m256i*)&res_vec.elements[i], c_32);
    }
    print_vector(res_vec);

    return 0;
}
