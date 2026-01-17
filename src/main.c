#include "../include/shoup.h"
#include <stdio.h>

int main(void)
{
    printf("%d\n", shoup_algorithm(12129133, 12912313, 13));
    return 0;
}
