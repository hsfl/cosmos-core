#include "support/configCosmos.h"
#include "support/stringlib.h"

int main(int argc, char *argv[])
{
    for (uint16_t precision=0; precision<10; ++precision)
    {
        printf("%u ", precision);
        int16_t power = -9;
        for (float value=1e-10; value<1e10; value=pow(10., ++power))
        {
            printf("%s ", to_floating(value, precision).c_str());
        }
        printf("\n");
        printf("%u ", precision);
        power = -9;
        for (float value=1e-10; value<1e10; value=pow(10., ++power))
        {
            printf("%s ", to_floatexp(value, precision).c_str());
        }
        printf("\n");
        printf("%u ", precision);
        power = -9;
        for (float value=1e-10; value<1e10; value=pow(10., ++power))
        {
            printf("%s ", to_floatany(value, precision).c_str());
        }
        printf("\n");
    }
}
