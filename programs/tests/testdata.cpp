#include "support/configCosmos.h"

int main(int argc, char *argv[])
{
    FILE *op = stdout;

    uint32_t repeat = 1;
    uint32_t count = 1;
    uint8_t start_value = 1;

    switch (argc)
    {
    case 5:
        op = fopen(argv[4], "w");
    case 4:
        start_value = atol(argv[3]);
    case 3:
        count = atol(argv[2]);
    case 2:
        repeat = atol(argv[1]);
    }

    uint8_t outbyte = start_value;
    for (uint32_t bigstep=0; bigstep<count; ++bigstep)
    {
        for (uint32_t smallstep=0; smallstep<repeat; ++smallstep)
        {
            fwrite(&outbyte, 1, 1, op);
        }
        ++outbyte;
    }

    if (op != stdout)
    {
        fclose(op);
    }
}
