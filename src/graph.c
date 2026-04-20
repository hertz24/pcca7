#include "../include/graph.h"

static const char *colors[9] = {"red", "dark-green", "orange", "blue", "purple", "dark-cyan", "grey", "brown", "dark-pink"};

double **data_tab = NULL;

int init_data_tab(void)
{
    if (data_tab == NULL)
    {
        data_tab = calloc(NB_ALGO, sizeof(double *));
        if (data_tab == NULL)
        {
            perror("init_data_tab data_tab calloc");
            return 1;
        }
    }
    return 0;
}

void free_data_tab()
{
    for (int i = 0; i < NB_ALGO; i++)
        free(*(data_tab + i));
    free(data_tab);
}

static int benchmark(int fd, int scale, ulong nb_points, Parameters param, AlgorithmID algorithm_ids[], int nb_algo)
{
    Vector *vectors = malloc(nb_points * sizeof(Vector));
    if (vectors == NULL)
    {
        perror("benchmark vectors malloc");
        return 1;
    }
    int ret = 0;
    for (ulong i = 0; i < nb_points; i++)
        *(vectors + i) = rand_vector((i + 1) * scale);
    for (int i = 0; i < nb_algo; i++)
    {
        int id = algorithm_ids[i];
        if (*(data_tab + id) == NULL)
        {
            *(data_tab + id) = malloc(nb_points * sizeof(double));
            if (*(data_tab + id) == NULL)
            {
                perror("benchmark *(data_tab + id) malloc");
                ret = 1;
                goto end;
            }
            for (ulong j = 0; j < nb_points; j++)
                *(*(data_tab + id) + j) = time_algorithm(algorithms[id].address, param, *(vectors + j));
        }
        for (ulong j = 0; j < nb_points; j++)
            dprintf(fd, "%ld,%.20f\n", (j + 1) * scale, *(*(data_tab + id) + j));
        dprintf(fd, "e\n");
    }
end:
    for (ulong i = 0; i < nb_points; i++)
        free_vector(*(vectors + i));
    free(vectors);
    return ret;
}

int generate_graph(int scale, ulong nb_points, Parameters param, AlgorithmID algorithm_ids[], int nb_algo)
{
    static int count = 1;
    mkdir("graphs", 0755);
    char file_name[32];
    snprintf(file_name, sizeof(file_name) - 1, "graphs/graph%d.gp", count);
    int fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("generate_curve fd open");
        return 1;
    }
    dprintf(fd, "set terminal pngcairo size 1200,800 enhanced font 'arial,10'\n"
                "set datafile separator ','\n"
                "set key outside\n"
                "set title 'Execution time for b = %u and p = %u'\n"
                "set output 'graphs/graph%d.png'\n"
                "set xlabel 'Size of the vector'\n"
                "set ylabel 'Time in milliseconds'\n"
                "plot ",
            param.b, param.p, count++);
    for (int i = 0; i < nb_algo; i++)
    {
        dprintf(fd, "'-' title '%s' with points pt 7 ps 0.25 linecolor '%s'", algorithms[algorithm_ids[i]].name, colors[i]);
        if (i < nb_algo - 1)
            dprintf(fd, ", ");
    }
    dprintf(fd, "\n");
    int ret = 0;
    if (benchmark(fd, scale, nb_points, param, algorithm_ids, nb_algo) == 1)
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
