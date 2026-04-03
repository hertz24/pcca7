#include "test.h"

static int test_options(int argc, char const *argv[], char expected_flag, int expected_ret, int *err)
{
    Options options = {.flags = 0, .scale = 1, .points = 100};
    int ret = set_options(argc, argv, &options);
    if (ret != expected_ret || (options.flags != expected_flag && expected_ret == 0))
    {
        FAIL("test_options", err);
        if (ret != expected_ret)
            fprintf(stderr, "Incorrect value returned.\n");
        else
            fprintf(stderr, "Incorrect flag returned.%d\n", options.flags);
        return 1;
    }
    return 0;
}

static void uint32_to_str(uint32_t n, char buffer[])
{
    snprintf(buffer, BUFFER_SIZE - 1, "%u", n);
}

int main(void)
{
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_options: the print statements in the functions under test are not silenced.\n");
    int ret = 0;
    for (int i = 0; i < NB_TESTS; i++)
    {
        char const *argv_1[] = {"./pcca7"};
        ret = test_options(1, argv_1, 0, 0, &err);
        if (ret == 1)
            goto end;
        for (ulong p_bits = 0; p_bits < 32; p_bits++)
        {
            uint32_t p = rand_prime(i);
            char p_str[BUFFER_SIZE];
            uint32_to_str(p, p_str);
            char const *argv_2[] = {"./pcca7", "-p", p_str};
            ret = test_options(3, argv_2, OPT_P, (n_is_prime(p)) ? 0 : ERR_PRIME, &err);
            for (ulong b_bits = 0; b_bits < 32 && !ret; b_bits++)
            {
                uint32_t b = _n_randint(state, b_bits);
                char b_str[BUFFER_SIZE];
                uint32_to_str(b, b_str);
                char const *argv_3[] = {"./pcca7", "-b", b_str};
                ret = test_options(3, argv_3, OPT_B, 0, &err);
                if (ret == 1)
                    break;
                char const *argv_4[] = {"./pcca7", "-p", p_str, "-b", b_str};
                ret = test_options(5, argv_4, OPT_P | OPT_B, (n_is_prime(p)) ? 0 : ERR_PRIME, &err);
            }
            if (ret == 1)
                goto end;
        }
    }
    SUCCESS("test_options", &out);
end:
    close(out);
    close(err);
    return ret;
}
