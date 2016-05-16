#ifndef SERIAL_H
#define SERIAL_H

#include "configCosmos.h"
#include <iostream>
#include <cstdint>

#if defined(COSMOS_WIN_OS)
#include "windows.h"
#endif

using std::string;

class Serial
{
public:

    string port;
    uint32_t baudrate;

#if defined(COSMOS_WIN_OS)
    HANDLE handle;
#endif
    Serial();
    Serial(string port, uint32_t baudrate);
    void open();
};

#endif // SERIAL_H
