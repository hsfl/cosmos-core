#include "serial.h"
#if defined(COSMOS_WIN_OS)
#include "Windows.h"
#endif

using std::cout;
using std::endl;

Serial::Serial()
{

}

Serial::Serial(string port, uint32_t baudrate)
{
    this->port = port;
    this->baudrate = baudrate;
}

void Serial::open(){
    //    fd = ::open (port_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

#if defined(COSMOS_WIN_OS)
    handle = CreateFileA(port.c_str(),
                         GENERIC_READ | GENERIC_WRITE,
                         0,                 // no share
                         NULL,              // no security
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,  //
                         NULL);                 // no templates

    if (handle == INVALID_HANDLE_VALUE) {
        cout << "error" << endl;
    } else {
        cout << "pass" << endl;
    }
#endif
}
