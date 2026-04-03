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
            fprintf(stderr, "Incorrect flag returned.\n");
        return 1;
    }
    return 0;
}

static void uint32_to_str(uint32_t n, char buffer[])
{
    snprintf(buffer, BUFFER_SIZE - 1, "%u", n);
}

static int test_set_options(uint32_t p, uint32_t p_bits, uint32_t b, uint32_t b_bits, int *err)
{
    char p_str[BUFFER_SIZE], b_str[BUFFER_SIZE];
    char p_bits_str[BUFFER_SIZE], b_bits_str[BUFFER_SIZE];
    uint32_to_str(p, p_str);
    uint32_to_str(b, b_str);
    uint32_to_str(p_bits, p_bits_str);
    uint32_to_str(b_bits, b_bits_str);
    int verify_prime = (n_is_prime(p)) ? 0 : ERR_PRIME;
    struct
    {
        int argc;
        char const *argv[10];
        char expected_flags;
        int expected_ret;
    } tests[] = {
        {1, {"./pcca7"}, 0, 0},
        {3, {"./pcca7", "-p", p_str}, OPT_P, verify_prime},
        {3, {"./pcca7", "-p_bits", p_bits_str}, OPT_P_BITS, 0},
        {3, {"./pcca7", "-b", b_str}, OPT_B, 0},
        {3, {"./pcca7", "-b_bits", b_bits_str}, OPT_B_BITS, 0},
        {5, {"./pcca7", "-p", p_str, "-b", b_str}, OPT_P | OPT_B, verify_prime},
        {5, {"./pcca7", "-p_bits", p_bits_str, "-b", b_str}, OPT_P_BITS | OPT_B, 0},
        {5, {"./pcca7", "-p", p_str, "-b_bits", b_bits_str}, OPT_P | OPT_B_BITS, verify_prime},
        {5, {"./pcca7", "-p_bits", p_bits_str, "-b_bits", b_bits_str}, OPT_P_BITS | OPT_B_BITS, 0},
        {7, {"./pcca7", "-p_bits", p_bits_str, "-p", p_str, "-b", b_str}, OPT_P_BITS | OPT_B, 0},
        {7, {"./pcca7", "-p_bits", p_bits_str, "-not_exist", p_str, "-b", b_str}, OPT_P_BITS | OPT_B, ERR_UNRECOGNIZED},
    };
    for (size_t i = 0; i < TAB_SIZE(tests); i++)
        if (test_options(tests[i].argc, tests[i].argv, tests[i].expected_flags, tests[i].expected_ret, err))
            return 1;
    return 0;
}

int main(void)
{
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_options: the print statements in the functions under test are not silenced.\n");
    rand_init();
    int ret = 0;
    for (int i = 0; i < NB_TESTS; i++)
        for (uint32_t p_bits = 0; p_bits < 32; p_bits++)
        {
            uint32_t p = n_randbits(state, p_bits);
            for (uint32_t b_bits = 0; b_bits < 32; b_bits++)
            {
                uint32_t b = n_randbits(state, b_bits);
                if ((ret = test_set_options(p, p_bits, b, b_bits, &err)) == 1)
                    goto end;
            }
        }
    SUCCESS("test_options", &out);
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}