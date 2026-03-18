#include "../include/curve.h"

static int benchmark(int fd, int scale, ulong nb_points, Parameters param)
{
    Vector *vectors = malloc(nb_points * sizeof(Vector));
    if (vectors == NULL)
    {
        perror("benchmark vectors malloc");
        return 1;
    }
    for (ulong i = 0; i < nb_points; i++)
        *(vectors + i) = rand_vector((i + 1) * scale);
    for (int i = 0; i < NB_ALGO; i++)
    {
        for (ulong j = 0; j < nb_points; j++)
            dprintf(fd, "%ld,%.20f\n", (j + 1) * scale, time_algorithm(algorithms[i], param, *(vectors + j)));
        dprintf(fd, "e\n");
    }
    for (ulong i = 0; i < nb_points; i++)
        free_vector(*(vectors + i));
    free(vectors);
    return 0;
}

int generate_curve(int scale, ulong nb_points, Parameters param)
{
    int fd = open("graph.gp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("generate_curve fd open");
        return 1;
    }
    int ret = 0;
    char instruction[5] =
#if NEON
        "NEON";
#elif AVX2
        "AVX2";
#else
        "\0";
#endif
    dprintf(fd, "set terminal pngcairo size 1200,800 enhanced font 'arial,10'\n"
                "set datafile separator ','\n"
                "set key outside\n"
                "set title 'Execution time for b = %u and p = %u'\n"
                "set output 'graph.png'\n"
                "set xlabel 'Size of the vector'\n"
                "set ylabel 'Time in milliseconds'\n"
                "set logscale y\n"
                "plot '-' title 'Naive scale' with points pt 7 ps 0.25 linecolor 'red',"
                "'-' title 'Shoup scale (reference)' with points pt 7 ps 0.25 linecolor 'green',"
                "'-' title 'Shoup scale (FLINT)' with points pt 7 ps 0.25 linecolor 'orange'",
            param.b, param.p);
#if NEON || AVX2
    dprintf(fd, ",'-' title 'Shoup scale (%s)' with points pt 7 ps 0.25 linecolor 'blue',"
                "'-' title 'Shoup scale with multiply low (%s)' with points pt 7 ps 0.25 linecolor 'purple'",
            instruction, instruction);
#endif
#if AVX512
    dprintf(fd, ",'-' title 'Shoup scale (AVX512)' with points pt 7 ps 0.25 linecolor 'cyan'");
#endif
    dprintf(fd, "\n");
    if (benchmark(fd, scale, nb_points, param) == 1)
    {
        perror("generate_curve benchmark");
        ret = 1;
        goto end;
    }
    if (fork() == 0)
    {
        execlp("gnuplot", "gnuplot", "graph.gp", (char *)NULL);
        perror("generate_curve execlp");
        exit(1);
    }
    int status;
    wait(&status);
    if (WIFEXITED(status))
        ret = WEXITSTATUS(status);
end:
    close(fd);
    return ret;
}