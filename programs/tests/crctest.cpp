#include "support/configCosmos.h"
#include "math/crclib.h"

int main(int argc, char *argv[])
{
    //bool lsb = true;
    string input = "123456789";
    if (argc == 2)
    {
        input = argv[1];
    }

    uint16_t crc;

    CRC16 calc_crc;
    for (auto type : calc_crc.types)
    {
        calc_crc.set(type.first);
        crc = calc_crc.calc(input);
        printf("%s[%04x]: %04x\n", type.first.c_str(), type.second.test, crc);
    }

//    uint16_t lcrc = calc_crc16ccitt(reinterpret_cast<uint8_t *>(&input[0]), input.size(), true);
//    uint16_t mcrc = calc_crc16ccitt(reinterpret_cast<uint8_t *>(&input[0]), input.size(), false);
//    printf("LSB: %04x MSB: %04x\n", lcrc, mcrc);
}
