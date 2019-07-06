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
    uint16_t endcount = .5 * baud;
    double duration = 1.;

    switch (argc)
    {
    case 8:
        if (!strcmp(argv[8], "rtscts"))
        {
            rtscts = true;
        }
        if (!strcmp(argv[7], "xonxoff"))
        {
            xonxoff = true;
        }
    case 7:
        stop = atoi(argv[6]);
    case 6:
        parity = atoi(argv[5]);
    case 5:
        bits = atoi(argv[4]);
    case 4:
        duration = atof(argv[3]);
    case 3:
        baud = atol(argv[2]);
    case 2:
        name = argv[1];
    }

    endcount = duration * baud / (bits + parity + stop + 1.);
    Serial *port = new Serial(name, baud, bits, parity, stop);
    port->set_flowcontrol(rtscts, xonxoff);
    port->set_timeout(4.);

    int32_t result;
    ElapsedTime et;
    for (uint8_t i=65; i<125; ++i)
    {
        size_t writecount = 0;
        size_t errorcount = 0;
        size_t timeoutcount = 0;
        for (size_t j=0; j<endcount; ++j)
        {
            result = port->put_char(i);
            if (result >= 0)
            {
                ++writecount;
            }
            else
            {
                if (result == SERIAL_ERROR_TIMEOUT)
                {
                    ++timeoutcount;
                }
                else
                {
                    printf("%s\n", cosmos_error_string(result).c_str());
                    ++errorcount;
                }
            }
        }
        double lap = et.lap();
        printf("%f %f: Wrote %lu %c @ %lf BPS: %lu Errors: %lu Timeouts\n", et.split(), lap, writecount, i, writecount/lap, errorcount, timeoutcount);
    }
    result = port->put_char(4);
}
