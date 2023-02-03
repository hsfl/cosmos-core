#include "bytelib.h"

//! Determine local byte order
/*! Investigate a locally stored number to determine the byte order of the local machine.
    \return Order as provided in ::ByteOrder.
*/

//std::uint8_t local_byte_order()
ByteOrder local_byte_order()
{
    uint16_t test = 1;
    uint8_t *check;

    check = (uint8_t *)&test;

    if (check[0] == 0)
        return (ByteOrder::BIGENDIAN);
    else
        return (ByteOrder::LITTLEENDIAN);
}

//! Memory to generic unsigned integer
/*! Return the generic unsigned integer equivalent of a location in memory, reversed based on the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 8 bit unsigned integer
*/

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
uint64_t uintswap(uint8_t bits, uint64_t value, ByteOrder order)
{
    uint64_t rb = value;
    uint64_t rev = 0;
#else
size_t uintswap(uint8_t bits, size_t value, ByteOrder order)
{
    size_t rb = value;
    size_t rev = 0;
#endif
    if (local_byte_order() == order)
    {
        return rb;
    }
    else
    {
#if ((SIZE_WIDTH) == (UINT64_WIDTH))
        for (uint64_t ic=0; ic<bits; ++ic)
#else
        for (size_t ic=0; ic<bits; ++ic)
#endif
        {
            rev <<= 1;
            if ((rb & 1) == 1)
            {
                rev ^= 1;
            }
            rb >>= 1;
        }
        return rev;
    }
}

//! Memory to 8 bit unsigned integer
/*! Return the 8 bit unsigned integer equivalent of a location in memory, reversed based on the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 8 bit unsigned integer
*/

uint8_t uint8from(uint8_t *pointer, ByteOrder order)
{
    uint8_t rb = *pointer;
    uint8_t rev = 0;
    if (local_byte_order() == order)
    {
        return rb;
    }
    else
    {
        for (uint8_t ic=0; ic<8; ++ic)
        {
            rev <<= 1;
            if ((rb & 1) == 1)
            {
                rev ^= 1;
            }
            rb >>= 1;
        }
        return rev;
    }
}

uint8_t uint8to(uint8_t *pointer, ByteOrder order)
{
    uint8_t rb = *pointer;
    uint8_t rev = 0;
    if (local_byte_order() == order)
    {
        return rb;
    }
    else
    {
        for (uint8_t ic=0; ic<8; ++ic)
        {
            rev <<= 1;
            if ((rb & 1) == 1)
            {
                rev ^= 1;
            }
            rb >>= 1;
        }
        return rev;
    }
}

void uint8from(vector<uint8_t> src, vector<uint8_t> &dst, ByteOrder order)
{
    dst.resize(src.size());
    for (size_t i=0; i<src.size(); ++i)
    {
        dst[i] = uint8from(&src[i], order);
    }
}

void uint8to(vector<uint8_t> src, vector<uint8_t> &dst, ByteOrder order)
{
    dst.resize(src.size());
    for (size_t i=0; i<src.size(); ++i)
    {
        dst[i] = uint8from(&src[i], order);
    }
}


//! Memory to 16 bit unsigned integer
/*! Return the 16 bit unsigned integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 16 bit unsigned integer
*/

uint16_t uint16from(uint8_t *pointer, ByteOrder order)
{
    uint16_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint16_t *)rb;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,2);
    }
    else
    {
        rb[1] = pointer[0];
        rb[0] = pointer[1];
    }

    return (*result);
}

//! Memory to 16 bit signed integer
/*! Return the 16 bit signed integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 16 bit signed integer
*/

int16_t int16from(uint8_t *pointer, ByteOrder order)
{
    int16_t *result;
    uint16_t rb;

    result = (int16_t *)&rb;
    rb = uint16from(pointer,order);

    return (*result);
}

//! Memory to 32 bit unsigned integer
/*! Return the 32 bit unsigned integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 32 bit unsigned integer
*/

uint32_t uint32from(const uint8_t *pointer, ByteOrder order)
{
    uint32_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint32_t *)rb;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,4);
    }
    else
    {
        rb[3] = pointer[0];
        rb[2] = pointer[1];
        rb[1] = pointer[2];
        rb[0] = pointer[3];
    }

    return (*result);
}

//! Memory to 32 bit signed integer
/*! Return the 32 bit signed integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 32 bit signed integer
*/

int32_t int32from(uint8_t *pointer, ByteOrder order)
{
    int32_t *result;
    uint32_t rb;

    result = (int32_t *)&rb;
    rb = uint32from(pointer,order);

    return (*result);
}

//! Memory to 32 bit float
/*! Return the 32 bit float equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 32 bit float
*/

float floatfrom(uint8_t *pointer, ByteOrder order)
{
    float result;
    uint8_t *rb;

    rb = (uint8_t *)&result;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,4);
    }
    else
    {
        rb[3] = pointer[0];
        rb[2] = pointer[1];
        rb[1] = pointer[2];
        rb[0] = pointer[3];
    }

    return (result);
}

//! Memory to 64 bit float
/*! Return the 64 bit float equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::ByteOrder.
    \return 64 bit float
*/

double doublefrom(uint8_t *pointer, ByteOrder order)
{
    double result;
    uint8_t *rb;

    rb = (uint8_t *)&result;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,8);
    }
    else
    {
        rb[7] = pointer[0];
        rb[6] = pointer[1];
        rb[5] = pointer[2];
        rb[4] = pointer[3];
        rb[3] = pointer[4];
        rb[2] = pointer[5];
        rb[1] = pointer[6];
        rb[0] = pointer[7];
    }

    return (result);
}

//! 32 bit unsigned integer to memory
/*! Cast a 32 bit unsigned integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::ByteOrder.
*/

void uint32to(uint32_t value, uint8_t *pointer, ByteOrder order)
{
    uint32_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint32_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,4);
    }
    else
    {
        pointer[0] = rb[3];
        pointer[1] = rb[2];
        pointer[2] = rb[1];
        pointer[3] = rb[0];
    }

}

//! 32 bit signed integer to memory
/*! Cast a 32 bit signed integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::ByteOrder.
*/

void int32to(int32_t value, uint8_t *pointer, ByteOrder order)
{
    int32_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (int32_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,4);
    }
    else
    {
        pointer[0] = rb[3];
        pointer[1] = rb[2];
        pointer[2] = rb[1];
        pointer[3] = rb[0];
    }

}

//! 16 bit unsigned integer to memory
/*! Cast a 16 bit unsigned integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::ByteOrder.
*/

void uint16to(uint16_t value, uint8_t *pointer, ByteOrder order)
{
    uint16_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint16_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,2);
    }
    else
    {
        pointer[0] = rb[1];
        pointer[1] = rb[0];
    }

}

//! 16 bit signed integer to memory
/*! Cast a 16 bit signed integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::ByteOrder.
*/

void int16to(int16_t value, uint8_t *pointer, ByteOrder order)
{
    int16_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (int16_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,2);
    }
    else
    {
        pointer[0] = rb[1];
        pointer[1] = rb[0];
    }

}

//! 32 bit floating point to memory
/*! Cast a 32 bit floating point equivalent into a location in memory, corrected for the local byte order.
    \param value float to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::ByteOrder.
*/

void floatto(float value, uint8_t *pointer, ByteOrder order)
{
    float *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (float *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,4);
    }
    else
    {
        pointer[0] = rb[3];
        pointer[1] = rb[2];
        pointer[2] = rb[1];
        pointer[3] = rb[0];
    }

}

//! 64 bit floating point to memory
/*! Cast a 64 bit floating point equivalent into a location in memory, corrected for the local byte order.
    \param value float to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::ByteOrder.
*/

void doubleto(double value, uint8_t *pointer, ByteOrder order)
{
    double *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (double *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,8);
    }
    else
    {
        pointer[0] = rb[7];
        pointer[1] = rb[6];
        pointer[2] = rb[5];
        pointer[3] = rb[4];
        pointer[4] = rb[3];
        pointer[5] = rb[2];
        pointer[6] = rb[1];
        pointer[7] = rb[0];
    }
}
