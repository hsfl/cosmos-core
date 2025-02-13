#include "support/configCosmos.h"
#include "support/stringlib.h"

int main(int argc, char *argv[])
{
    string test = "0123456789";
    double value;

    if (argc ==2)
    {
        test = argv[1];
    }
    value = stod(test);

    for (int16_t i=-1; i<3; ++i)
    {
        printf("precision: %d\n", i);
        printf("floatany(double): %s\n", to_floatany((double)value, i).c_str());
        printf("floatany(float): %s\n", to_floatany((float)value, i).c_str());

        printf("floatexp(double): %s\n", to_floatexp((double)value, i).c_str());
        printf("floatexp(float): %s\n", to_floatexp((float)value, i).c_str());

        printf("floating(double): %s\n", to_floating((double)value, i).c_str());
        printf("floating(float): %s\n", to_floating((float)value, i).c_str());
    }

    printf("unsigned(native): %s\n", to_unsigned(value,3 , true).c_str());
    printf("unsigned(uint64): %s\n", to_unsigned((size_t)value,3 , true).c_str());
    printf("unsigned(uint32): %s\n", to_unsigned((uint32_t)value,3 , true).c_str());
    printf("unsigned(uint16): %s\n", to_unsigned((uint16_t)value,3 , true).c_str());
    printf("unsigned(uint8): %s\n", to_unsigned((uint8_t)value,3 , true).c_str());

    printf("signed(native): %s\n", to_signed(value,3 , true).c_str());
    printf("signed(int64): %s\n", to_signed((ptrdiff_t)value,3 , true).c_str());
    printf("signed(int32): %s\n", to_signed((int32_t)value,3 , true).c_str());
    printf("signed(int16): %s\n", to_signed((int16_t)value,3 , true).c_str());
    printf("signed(int8): %s\n", to_signed((int8_t)value,3 , true).c_str());

    printf("%s\n", to_label("unsigned(native)", value, 3).c_str());
#if ((SIZE_WIDTH) == (UINT64_WIDTH))
    printf("%s\n", to_label("unsigned(uint64)", (uint64_t)value, 3).c_str());
#endif
    printf("%s\n", to_label("unsigned(uint32)", (uint32_t)value, 3).c_str());
    printf("%s\n", to_label("unsigned(uint16)", (uint16_t)value, 3).c_str());
    printf("%s\n", to_label("unsigned(uint8)", (uint8_t)value, 3).c_str());
#if ((PTRDIFF_WIDTH) == (INT64_WIDTH))
    printf("%s\n", to_label("signed(int64)", (int64_t)value, 3).c_str());
#endif
    printf("%s\n", to_label("signed(int32)", (int32_t)value, 3).c_str());
    printf("%s\n", to_label("signed(int16)", (int16_t)value, 3).c_str());
    printf("%s\n", to_label("signed(int8)", (int8_t)value, 3).c_str());
}
