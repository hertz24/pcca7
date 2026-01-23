#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"
#include "flint/nmod.h"
#include "flint/ulong_extras.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(100, 812112989, 97);
    Vector v = shoup_scalar_algorithm(param1);
    printf("%f s\n", time_algorithm(shoup_scalar_algorithm, param1));
    print_vector(v);

    printf("--------FLINT-------------\n");
    mp_limb_t p = 17; 
    nmod_t mod;
    nmod_init(&mod, p);

    mp_limb_t a = 10;
    mp_limb_t b = 12;
    mp_limb_t res;

    printf("Calculs modulo p = %lu\n", (unsigned long) p);
    printf("a = %lu, b = %lu\n\n", (unsigned long) a, (unsigned long) b);

    // 2. Somme : (a + b) mod p
    res = nmod_add(a, b, mod);
    printf("Somme (a + b): %lu\n", (unsigned long) res);

    // 3. Produit : (a * b) mod p
    res = nmod_mul(a, b, mod);
    printf("Produit (a * b): %lu\n", (unsigned long) res);

    // 4. Inversion : a^-1 mod p
    res = nmod_inv(a, mod);
    if (res != 0) {
        printf("Inverse de a = 10: %lu  (Verification: %lu * %lu mod %lu = %lu)\n", 
                (unsigned long) res, 
                (unsigned long) a, 
                (unsigned long) res, 
                (unsigned long) p,
                (unsigned long) nmod_mul(a, res, mod));
    } else {
        printf("a = 10 n'est pas inversible modulo p\n");
    }
    return 0;
}
