#include "test.h"

int main(void)
{
    int ret = 0;
    int out, err;
    if (begin(&out, &err))
        fprintf(stderr, "test_param: the print statements in the functions under test are not silenced.\n");
    rand_init();
    for (int i = 0; i < NB_TESTS; i++)
        for (ulong j = 0; j <= 32; j++)
            for (ulong k = 0; k <= 32; k++)
            {
                Parameters param = rand_parameters(j, k);
                if (j > k && j <= 31 && k >= 2 && k <= 31 && param.p != 0)
                {
                    FAIL("test_param", &err);
                    fprintf(stderr, "param should be {0}.\n");
                    ret = 1;
                    goto end;
                }
                else if (param.p == 0)
                    continue;
                if (param.b >= param.p)
                {
                    FAIL("test_param", &err);
                    fprintf(stderr, "param.b == %u >= param.p == %u\n", param.b, param.p);
                    ret = 1;
                    goto end;
                }
            }
    SUCCESS("test_param", &out);
end:
    close(out);
    close(err);
    rand_clear();
    return ret;
}
