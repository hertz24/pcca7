#include "../include/utils.h"

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    if (argc % 2 == 0)
    {
        fprintf(stderr, "Error input arguments.\n");
        return 1;
    }
    uint32_t p = rand_prime();
    uint32_t b;
    int opt_b = 0;
    int scale = 1;
    ulong points = 1000;
    for (int i = 1; i < argc; i += 2)
        if (strcmp("-p", argv[i]) == 0)
            p = atoi(argv[i + 1]);
        else if (strcmp("-b", argv[i]) == 0)
        {
            b = atoi(argv[i + 1]);
            opt_b = 1;
        }
        else if (strcmp("-scale", argv[i]) == 0)
            scale = atoi(argv[i + 1]);
        else if (strcmp("-pts", argv[i]) == 0)
            points = strtoul(argv[i + 1], NULL, 10);
        else
        {
            fprintf(stderr, "Option %s unrecognized.\n", argv[i]);
            return 1;
        }
    if (!opt_b)
        b = rand() % p;
    if (b >= p)
    {
        fprintf(stderr, "b must be less than p.\n");
        return 1;
    }
    return generate_curve(scale, points, b, p);
}
