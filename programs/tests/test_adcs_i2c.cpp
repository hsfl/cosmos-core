#include "support/configCosmos.h"
#include "adcs/adcs.h"

int main(int, char *argv[])
{
    ADCS *adcs = new ADCS();

    string bus = argv[1];
    uint8_t address = atoi(argv[2]);
    adcs->init(bus, address);

    double mjd1 = currentmjd();

    int32_t status = adcs->set_current_unix_time(mjd1-10);

    cout << "Unix Time Status = " << status << endl;

    COSMOS_SLEEP(5);

    double mjd2;

    status = adcs->get_current_unix_time(mjd2);

    cout << "Unix Time Status = " << status << endl;

}
