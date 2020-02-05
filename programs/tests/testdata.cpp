#include "support/configCosmos.h"

int main(int argc, char *argv[])
{
    FILE *op = stdout;

    uint32_t littlecount = 1;
    uint32_t bigcount = 1;
    uint32_t value = 1;
    uint32_t step = 0;
    uint8_t size = 1;

    switch (argc)
    {
    case 4:
        op = fopen(argv[3], "w");
    case 3:
        {
            string targ = argv[2];
            size_t tloc = targ.find(":");
            if (tloc != string::npos)
            {
                step = atoll(targ.substr(tloc+1, targ.size()-tloc+1).c_str());
                value = atol(targ.substr(0, tloc).c_str());
            }
            else
            {
                value = atoll(argv[1]);
            }
            if (value < 256)
            {
                size = 1;
            }
            else if (value < 65536)
            {
                size = 2;
            }
            else if (value < 1677216)
            {
                size = 3;
            }
            else
            {
                size = 4;
            }
        }
    case 2:
        {
            string targ = argv[1];
            size_t tloc = targ.find(":");
            if (tloc != string::npos)
            {
                littlecount = atol(targ.substr(tloc+1, targ.size()-tloc+1).c_str());
                bigcount = atol(targ.substr(0, tloc).c_str());
            }
            else
            {
                bigcount = atol(argv[1]);
            }
        }
        break;
    default:
        printf("Usage: testdata big_steps[:small_steps] initial_value[:value_step] file\n");
    }

    for (uint32_t bigstep=0; bigstep<bigcount; ++bigstep)
    {
        for (uint32_t smallstep=0; smallstep<littlecount; ++smallstep)
        {
            uint32_t tvalue = value;
            uint8_t outbyte = tvalue%256;
            fwrite(&outbyte, 1, 1, op);
            for (uint16_t ministep=1; ministep<size; ++ministep)
            {
                tvalue /= 256;
                outbyte = tvalue%256;
                fwrite(&outbyte, 1, 1, op);
            }
        }
        value += step;
    }

    if (op != stdout)
    {
        fclose(op);
    }
}
