#include "../include/utils.h"

static void test_algorithm(void *arg, ulong count)
{
    void **data = (void **)arg;
    Vector (*algorithm)(Parameters, Vector) = *data;
    Parameters param = *((Parameters *)*(data + 1));
    Vector v = *((Vector *)*(data + 2));
    for (ulong i = 0; i < count; i++)
    {
        prof_start();
        free_vector(algorithm(param, v));
        prof_stop();
    }
}

double time_algorithm(Vector (*algorithm)(Parameters, Vector), Parameters param, Vector v)
{
    double min, max;
    void **data = malloc(3 * sizeof(void *));
    if (data == NULL)
    {
        perror("time_algorithm data malloc");
        return -1;
    }
    *data = algorithm;
    *(data + 1) = &param;
    *(data + 2) = &v;
    prof_repeat(&min, &max, test_algorithm, data);
    free(data);
    return min;
}

static int write_fd(int fd, char buffer[])
{
    int size = strlen(buffer);
    int total = 0;
    int bytes_written;
    while (total < size)
    {
        if ((bytes_written = write(fd, buffer + total, size - total)) == -1)
        {
            perror("write_fd write");
            return -1;
        }
        total += bytes_written;
    }
    return total;
}

int generate_curve(int scale, ulong nb_points, Parameters param)
{
    int fd = open("graph.gp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("generate_curve fd open");
        return 1;
    }
    write_fd(fd, "set terminal pngcairo enhanced font 'arial,10'\n");
    write_fd(fd, "set datafile separator ','\n");
    write_fd(fd, "set key outside\n");
    char buffer[MAX_BUFFER];
    snprintf(buffer, MAX_BUFFER - 1, "set title 'Execution time for b = %u and p = %u'\n", param.b, param.p);
    write_fd(fd, buffer);
    write_fd(fd, "set output 'graph.png'\n");
    write_fd(fd, "set xlabel 'Size of the vector'\n");
    write_fd(fd, "set ylabel 'Time in milliseconds'\n");
    write_fd(fd, "set logscale y\n");
    char instruction[5] = "\0";
#if NEON
    snprintf(instruction, 5, "NEON");
#else
    snprintf(instruction, 4, "AVX");
#endif
    snprintf(buffer, MAX_BUFFER - 1, "plot '-' title 'Naive scale' with points pt 7 ps 0.25 linecolor 'red', '-' title 'Shoup scale (reference)' with points pt 7 ps 0.25 linecolor 'green', '-' title 'Shoup scale (%s)' with points pt 7 ps 0.25 linecolor 'blue', '-' title 'Shoup scale with multiply low (%s)' with points pt 7 ps 0.25 linecolor 'purple'\n", instruction, instruction);
    write_fd(fd, buffer);
    Vector *vectors = malloc(nb_points * sizeof(Vector));
    if (vectors == NULL)
    {
        perror("generate_curve vectors malloc");
        close(fd);
        return 1;
    }
    Vector (*algorithms[4])(Parameters, Vector) = {naive_scale, shoup_scale_ref, shoup_scale, shoup_scale_mullo};
    for (ulong i = 0; i < nb_points; i++)
        *(vectors + i) = rand_vector((i + 1) * scale);
    for (int i = 0; i < 4; i++)
    {
        for (ulong j = 0; j < nb_points; j++)
        {
            snprintf(buffer, MAX_BUFFER - 1, "%ld,%.20f\n", (j + 1) * scale, time_algorithm(algorithms[i], param, *(vectors + j)));
            write_fd(fd, buffer);
        }
        write_fd(fd, "e\n");
    }
    for (ulong i = 0; i < nb_points; i++)
        free_vector(*(vectors + i));
    free(vectors);
    close(fd);
    if (fork() == 0)
    {
        execlp("gnuplot", "gnuplot", "graph.gp", (char *)NULL);
        perror("generate_curve execlp");
        exit(1);
    }
    int ret = 0;
    wait(&ret);
    return ret;
}