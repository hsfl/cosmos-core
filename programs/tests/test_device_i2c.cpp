#include "device/i2c/i2c.h"

using Cosmos::I2C;

int main(int argc, const char* argv[])
{
    cout << "testing I2C library" << endl;
    // 0x08 - motor controller board
    // 0x57 - CubeADCS
    I2C i2c("/dev/i2c-2", 0x08);
    i2c.send("dsdsa");

}

