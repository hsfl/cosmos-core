#include "support/configCosmos.h"
#include "device/serial/serialclass.h"
#include "support/elapsedtime.h"
#include "support/timelib.h"

int main(int argc, char *argv[])
{
    bool xonxoff = false;
    bool rtscts = false;
    string name = "/dev/ttyUSB0";
    int32_t baud = 115200;
    string parity = "none";
    size_t bits = 8;
    size_t stop = 1;
    uint16_t endcount = .5 * baud;
    double duration = 1.;

    switch (argc)
    {
    case 8:
        if (!strcmp(argv[7], "rtscts"))
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

    endcount = duration * baud / (bits + (parity=="none"?0:1) + stop + 1.);
    Serial *port = new Serial(name, baud, bits, parity, stop);
    port->set_flowcontrol(rtscts, xonxoff);

    size_t writecount = 0;
    size_t twritecount = 0;
    size_t errorcount = 0;
    size_t timeoutcount = 0;
    int32_t result;
    ElapsedTime et;
    ElapsedTime tet;
    for (uint8_t i=1; i<255; ++i)
    {
        vector <uint8_t> data;
        for (size_t j=0; j<endcount; ++j)
        {
            data.push_back(i);
        }
        writecount = 0;
        do
        {
            result = port->put_data(&data[writecount], data.size() - writecount);
            if (result > 0)
            {
                writecount += result;
                twritecount += result;
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
                secondsleep(.001);
            }
        } while (result < 0 || writecount < data.size());
        printf("%lf %hhu: %lu/%lu Writes @ %.0lf BPS Errors: %lu Timeouts: %lu\n", tet.split(), i, writecount, twritecount, writecount/tet.lap(), errorcount, timeoutcount);
        tet.reset();
    }
    result = port->put_char(4);

    printf("\n%lf Writes: %lu @ %lf BPS Errors: %lu Timeouts: %lu\n", et.split(), twritecount, twritecount/et.split(), errorcount, timeoutcount);

}
