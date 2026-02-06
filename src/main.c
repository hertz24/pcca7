#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"
#include "flint/nmod.h"
#include "flint/ulong_extras.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(96, rand_prime(1000000));
    print_param(param1);
    Vector rand_v = rand_vector(100000, param1.p);
    // print_vector(rand_v);
    Vector v = shoup_scalar(param1, rand_v);
    printf("%.20f s\n", time_algorithm(shoup_scalar, param1, v));
    //  print_vector(v);
    printf("\n");
    Vector v1 = naive_scalar_product(param1, rand_v);
    printf("%.20f s\n", time_algorithm(shoup_scalar, param1, v));
    // print_vector(v1);
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
    return 0;
}
