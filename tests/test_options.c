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
            uint32_t p = n_randbits(state, p_bits);
            char p_str[BUFFER_SIZE];
            uint32_to_str(p, p_str);
            char const *argv_2[] = {"./pcca7", "-p", p_str};
            ret = test_options(3, argv_2, OPT_P, (n_is_prime(p)) ? 0 : ERR_PRIME, &err);
            if (ret == 1)
                goto end;
            char p_bits_str[BUFFER_SIZE];
            uint32_to_str((uint32_t)p_bits, p_bits_str);
            char const *argv_3[] = {"./pcca7", "-p_bits", p_bits_str};
            ret = test_options(3, argv_3, OPT_P_BITS, 0, &err);
            if (ret == 1)
                goto end;
            for (ulong b_bits = 0; b_bits < 32 && ret == 0; b_bits++)
            {
                uint32_t b = n_randbits(state, b_bits);
                char b_str[BUFFER_SIZE];
                uint32_to_str(b, b_str);
                char const *argv_4[] = {"./pcca7", "-b", b_str};
                ret = test_options(3, argv_4, OPT_B, 0, &err);
                if (ret == 1)
                    goto end;
                char b_bits_str[BUFFER_SIZE];
                uint32_to_str((uint32_t)b_bits, b_bits_str);
                char const *argv_5[] = {"./pcca7", "-b_bits", b_bits_str};
                ret = test_options(3, argv_5, OPT_B_BITS, 0, &err);
                if (ret == 1)
                    goto end;
                char const *argv_6[] = {"./pcca7", "-p", p_str, "-b", b_str};
                ret = test_options(5, argv_6, OPT_P | OPT_B, (n_is_prime(p)) ? 0 : ERR_PRIME, &err);
                if (ret == 1)
                    goto end;
                char const *argv_7[] = {"./pcca7", "-p_bits", p_bits_str, "-b", b_str};
                ret = test_options(5, argv_7, OPT_P_BITS | OPT_B, 0, &err);
                if (ret == 1)
                    goto end;
                char const *argv_8[] = {"./pcca7", "-p", p_str, "-b_bits", b_bits_str};
                ret = test_options(5, argv_8, OPT_P | OPT_B_BITS, (n_is_prime(p)) ? 0 : ERR_PRIME, &err);
                if (ret == 1)
                    goto end;
                char const *argv_9[] = {"./pcca7", "-p_bits", p_bits_str, "-b_bits", b_bits_str};
                ret = test_options(5, argv_9, OPT_P_BITS | OPT_B_BITS, 0, &err);
                if (ret == 1)
                    goto end;
                char const *argv_10[] = {"./pcca7", "-p_bits", p_bits_str, "-p", p_str, "-b", b_str};
                ret = test_options(7, argv_10, OPT_P_BITS | OPT_B, 0, &err);
                if (ret == 1)
                    goto end;
                char const *argv_11[] = {"./pcca7", "-p_bits", p_bits_str, "-not_exist", p_str, "-b", b_str};
                ret = test_options(7, argv_11, OPT_P_BITS | OPT_B, ERR_UNRECOGNIZED, &err);
                if (ret == 1)
                    goto end;
            }
        }
    }
    SUCCESS("test_options", &out);
end:
    close(out);
    close(err);
    return ret;
}
