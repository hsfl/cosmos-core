#include "support/configCosmos.h"
#include "math/mathlib.h"

int main(int argc, char *argv[])
{
    bool lsb = true;
    string input = argv[1];
    if (argc == 3)
    {
        if (argv[2][0] == 'l')
        {
            lsb = true;
        }
        else
        {
            lsb = false;
        }
    }

    uint16_t lcrc = calc_crc16ccitt(reinterpret_cast<uint8_t *>(&input[0]), input.size(), true);
    uint16_t mcrc = calc_crc16ccitt(reinterpret_cast<uint8_t *>(&input[0]), input.size(), false);
    printf("LSB: %04x MSB: %04x\n", lcrc, mcrc);
}
