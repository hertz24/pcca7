#include "../include/utils.h"
#include "../include/shoup.h"

int compare_vectors(Vector v1, Vector v2)
{
    if (v1.size != v2.size)
    {
        fprintf(stderr, "The size of vectors aren't the same.\n");
        return 0;
    }
    for (ulong i = 0; i < v1.size; i++)
        if (*(v1.elements + i) != *(v2.elements + i))
        {
            printf("v1: %d and v2: %d at index %ld\n", *(v1.elements + i), *(v2.elements + i), i);
            return 1;
        }
    return 0;
}

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

int generate_curve(int scale, ulong nb_points, uint32_t b, uint32_t p)
{
    int fd = open("graph.gp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("generate_curve fd open");
        return 1;
    }
    Parameters param = init_parameters(b, p);
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
    write_fd(fd, "plot '-' title 'Naive scale' with points pt 7 ps 0.25 linecolor 'red', '-' title 'Shoup scale (reference)' with points pt 7 ps 0.25 linecolor 'green', '-' title 'Shoup scale (");
#if NEON
    snprintf(buffer, MAX_BUFFER - 1, "NEON");
#else
    snprintf(buffer, MAX_BUFFER - 1, "AVX");
#endif
    write_fd(fd, buffer);
    write_fd(fd, ")' with points pt 7 ps 0.25 linecolor 'blue'\n");
    Vector *vectors = malloc(nb_points * sizeof(Vector));
    if (vectors == NULL)
    {
        perror("generate_curve vectors malloc");
        close(fd);
        return 1;
    }
    for (ulong i = 1; i <= nb_points; i++)
    {
        *(vectors + i - 1) = rand_vector(i * scale, param.p);
        snprintf(buffer, MAX_BUFFER - 1, "%ld,%.20f\n", i * scale, time_algorithm(naive_scale, param, *(vectors + i - 1)));
        write_fd(fd, buffer);
    }
    write_fd(fd, "e\n");
    for (ulong i = 0; i < nb_points; i++)
    {
        snprintf(buffer, MAX_BUFFER - 1, "%ld,%.20f\n", (i + 1) * scale, time_algorithm(shoup_scale_ref, param, *(vectors + i)));
        write_fd(fd, buffer);
    }
    write_fd(fd, "e\n");
    for (ulong i = 0; i < nb_points; i++)
    {
        snprintf(buffer, MAX_BUFFER - 1, "%ld,%.20f\n", (i + 1) * scale, time_algorithm(shoup_scale, param, *(vectors + i)));
        free_vector(*(vectors + i));
        write_fd(fd, buffer);
    }
    free(vectors);
    write_fd(fd, "e\n");
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