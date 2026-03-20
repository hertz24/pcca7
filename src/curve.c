#include "../include/curve.h"

const char *colors[9] = {"red", "dark-green", "orange", "blue", "purple", "dark-cyan", "grey", "brown", "dark-pink"};

static int benchmark(int fd, int scale, ulong nb_points, Parameters param, Algorithm algorithms[], int nb_algo)
{
    Vector *vectors = malloc(nb_points * sizeof(Vector));
    if (vectors == NULL)
    {
        perror("benchmark vectors malloc");
        return 1;
    }
    for (ulong i = 0; i < nb_points; i++)
        *(vectors + i) = rand_vector((i + 1) * scale);
    for (int i = 0; i < nb_algo; i++)
    {
        for (ulong j = 0; j < nb_points; j++)
            dprintf(fd, "%ld,%.20f\n", (j + 1) * scale, time_algorithm(algorithms[i].address, param, *(vectors + j)));
        dprintf(fd, "e\n");
    }
    for (ulong i = 0; i < nb_points; i++)
        free_vector(*(vectors + i));
    free(vectors);
    return 0;
}

int generate_curve(int scale, ulong nb_points, Parameters param, Algorithm algorithms[], int nb_algo)
{
    static int count = 1;
    char file_name[16];
    snprintf(file_name, 15, "graph%d.gp", count);
    int fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("generate_curve fd open");
        return 1;
    }
    int ret = 0;
    dprintf(fd, "set terminal pngcairo size 1200,800 enhanced font 'arial,10'\n"
                "set datafile separator ','\n"
                "set key outside\n"
                "set title 'Execution time for b = %u and p = %u'\n"
                "set output 'graph%d.png'\n"
                "set xlabel 'Size of the vector'\n"
                "set ylabel 'Time in milliseconds'\n"
                "set logscale y\nplot ",
            param.b, param.p, count++);
    for (int i = 0; i < nb_algo; i++)
    {
        dprintf(fd, "'-' title '%s' with points pt 7 ps 0.25 linecolor '%s'", algorithms[i].name, colors[i]);
        if (i < nb_algo - 1)
            dprintf(fd, ", ");
    }
    dprintf(fd, "\n");
    if (benchmark(fd, scale, nb_points, param, algorithms, nb_algo) == 1)
    {
        perror("generate_curve benchmark");
        ret = 1;
        goto end;
    }
    if (fork() == 0)
    {
        execlp("gnuplot", "gnuplot", file_name, (char *)NULL);
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