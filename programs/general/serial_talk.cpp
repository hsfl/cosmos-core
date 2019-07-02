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

    switch (argc)
    {
    case 8:
        if (!strcmp(argv[6], "rtscts"))
        {
           rtscts = true;
        }
        if (!strcmp(argv[6], "xonxoff"))
        {
            xonxoff = true;
        }
    case 7:
        stop = atoi(argv[5]);
    case 6:
        parity = atoi(argv[4]);
    case 5:
        bits = atoi(argv[3]);
    case 4:
        baud = atol(argv[2]);
    case 3:
        endcount = atof(argv[2]) * baud / 10.;
    case 2:
        name = argv[1];
    }

    Serial *port = new Serial(name, baud, bits, parity, stop);
    port->set_flowcontrol(rtscts, xonxoff);
    port->set_timeout(1, 0);

    size_t writecount = 0;
    size_t errorcount = 0;
    size_t timeoutcount = 0;
    int32_t result;
    double sum = 0;
    do
    {
        ElapsedTime et;
        result = port->put_char('a');
        if (result > 0)
        {
            sum += et.lap();
            printf("%c", (char)result);
            fflush(stdout);
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
                ++errorcount;
            }
        }
    } while (writecount < endcount);
    result = port->put_char(4);

    printf("\nWrites: %lu @ %lf BPS Errors: %lu Timeouts: %lu\n", writecount, sum/writecount, errorcount, timeoutcount);

}
