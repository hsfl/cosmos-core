#ifndef SERIAL_H
#define SERIAL_H

#include <iostream>
#include <cstdint>

#if defined(_WIN32)
#include "windows.h"
#endif

using std::string;

class Serial
{
public:

    string port;
    uint32_t baudrate;

#if defined(_WIN32)
    HANDLE handle;
#endif
    Serial();
    Serial(string port, uint32_t baudrate);
    void open();
};

#endif // SERIAL_H
