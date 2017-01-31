#include "support/configCosmos.h"
#include "device/serial/serialclass.h"

int main(int argc, char *argv[])
{
    bool xonxoff = false;
    bool rtscts = false;
    string name = "/dev/ttyUSB0";
    int32_t baud = 115200;
    size_t parity = 0;
    size_t bits = 8;
    size_t stop = 1;

    switch (argc)
    {
    case 7:
        if (!strcmp(argv[6], "rtscts"))
        {
           rtscts = true;
        }
        if (!strcmp(argv[6], "xonxoff"))
        {
            xonxoff = true;
        }
    case 6:
        stop = atoi(argv[5]);
    case 5:
        parity = atoi(argv[4]);
    case 4:
        bits = atoi(argv[3]);
    case 3:
        baud = atol(argv[2]);
    case 2:
        name = argv[1];
    }

    Serial *port = new Serial(name, baud, bits, parity, stop);
    port->set_flowcontrol(rtscts, xonxoff);
    port->set_timeout(1, 0);

    size_t readcount = 0;
    size_t errorcount = 0;
    size_t timeoutcount = 0;
    int32_t result;
    do
    {
        result = port->get_char();
        if (result > 0)
        {
            printf("%c", (char)result);
            fflush(stdout);
            ++readcount;
        }
        else
        {
            if (result == SERIAL_ERROR_TIMEOUT)
            {
                ++timeoutcount;
            }
            else
            {
                ++errorcount;
            }
        }
    } while (result != 4);

    printf("\nReads: %lu Errors: %lu Timeouts: %lu\n", readcount, errorcount, timeoutcount);

}
