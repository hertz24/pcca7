#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"
#include "flint/fmpz.h"

int main(void)
{
    srand(time(NULL));
    Parameters param1 = init_parameters(100, 812112989, 97);
    Vector v = shoup_scalar_algorithm(param1);
    printf("%f s\n", time_algorithm(shoup_scalar_algorithm, param1));
    print_vector(v);

    printf("--------FLINT-------------\n");
    fmpz_t n;
    fmpz_init(n);
    
    // Calculate 100!
    fmpz_fac_ui(n, 100);
    
    printf("100! = ");
    fmpz_print(n);
    printf("\n");
    
    fmpz_clear(n);
    return 0;
}
