#include "../include/shoup.h"
#include "../include/calculation.h"
#include "../include/utils.h"

int main(void)
{
    srand(time(NULL));
    return generate_curve(1, 100);
}
