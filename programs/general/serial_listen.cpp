#include "support/configCosmos.h"
#include "device/serial/serialclass.h"
#include "support/elapsedtime.h"

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
    port->set_timeout(4.);

    size_t readcount = 0;
    size_t errorcount = 0;
    size_t timeoutcount = 0;
    int32_t lastresult = 0;
    int32_t result;
    ElapsedTime et;
    do
    {
        result = port->get_char();
        if (result >= 0)
        {
            ++readcount;
            if (result != lastresult)
            {
                lastresult = result;
                double lap = et.lap();
                printf("%f %f: Read %lu %c(%0x) @ %f BPS: %lu Errors: %lu Timeouts\n", et.split(), lap, readcount, result, result, readcount/lap, errorcount, timeoutcount);
                readcount = 0;
                errorcount = 0;
                timeoutcount = 0;
            }
        }
        else
        {
            if (result == SERIAL_ERROR_TIMEOUT)
            {
                ++timeoutcount;
            }
            else
            {
                printf("%f: %s\n", et.split(), cosmos_error_string(result).c_str());
                ++errorcount;
            }
        }
    } while (1);

}
