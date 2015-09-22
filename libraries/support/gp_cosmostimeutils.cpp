#include "gp_cosmostimeutils.h"
#include <cstdint>

double mjd_start_time;

int32_t sanitize_hec_message_data_for_printing(void *vdata, uint16_t size)
{
    int i;
    char *data = (char*)vdata;

    // Scan through message and eliminate all non-printable characters
    for(i = 0; i<size; i++)
    {
        // Replace all non-printable characters with spaces
        if (!isprint(data[i]) && data[i] != 0) data[i] = ' ';
    }

    // Return number of items checked
    return i+1;
}


void print_timestamp()
{
    printf("[%11.3lf] ", UPTIME);
}


size_t cosmos_timeutils_gen_timestr(time_t unixtime, char* timestring, uint16_t max_len)
{
    struct tm * timeinfo;
    timeinfo = localtime(&unixtime);    // Convert UNIX Epoch timestamp to struct tm for custom print
    return strftime(timestring, max_len, "%c %Z", timeinfo);
}


