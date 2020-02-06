#include "support/configCosmos.h"
#include "device/serial/serialclass.h"
#include "support/elapsedtime.h"

int main(int argc, char *argv[])
{
//    bool xonxoff = false;
//    bool rtscts = false;
    string name = "/dev/ttyUSB0";
    int32_t baud = 115200;
    size_t parity = 0;
    size_t bits = 8;
    size_t stop = 1;
    bool dtr_state = false;
    double wait_time = 0.;

    switch (argc)
    {
    case 4:
        wait_time = atof(argv[3]);
    case 3:
        if (!strcmp(argv[2], "on"))
        {
            dtr_state = true;
        }
        else
        {
            dtr_state = false;
        }
    case 2:
        name = argv[1];
    }

    Serial *port = new Serial(name, baud, bits, parity, stop);
    port->set_dtr(dtr_state);

    COSMOS_SLEEP(wait_time);
}
