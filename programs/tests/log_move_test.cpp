#include "support/configCosmos.h"
#include "support/datalib.h"
#include "support/elapsedtime.h"

int main(int argc, char *argv[])
{
    ElapsedTime et;
    int32_t iretn;
    int32_t lretn = -1;

    do
    {
        iretn = log_move_file(argv[1], argv[2], true, 120);
        if (lretn != iretn)
        {
            printf("\n");
            lretn = iretn;
        }
        if (iretn < 0)
        {
            printf("%5.1f %s\r", et.split(), cosmos_error_string(iretn).c_str());
            fflush(stdout);
        }
    } while (iretn < 0);

}
