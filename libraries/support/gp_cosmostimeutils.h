#ifndef GP_COSMOSTIMEUTILS_H
#define GP_COSMOSTIMEUTILS_H

// COSMOS Core Libraries
#include "configCosmos.h"
#include "agentlib.h"           // COMSOS Agent Library

int32_t sanitize_hec_message_data_for_printing(void *vdata, uint16_t size);

// Time Macros
#define MJD_TO_SECONDS 86400
#define CURRENT_TIME_us currentmjd(0.0)*MJD_TO_SECONDS*1000*1000
#define INFO_SIZE 32
#define SECONDS_TO_DAYS(x)              ((double)(x) / 86400.)
#define DAYS_TO_SECONDS(x)              ((double)(x) * 86400.)
#define TIME_SECS_TO_DAYS(x)            ((double)(x) / 86400.)
#define TIME_DAYS_TO_SECS(x)            ((double)(x) * 86400.)

#define MJD_UNIX_OFFSET 40587.  // 40587.5 stated by others, 40587. is used by COSMOS as of 7/8/2015

#define MJD_ABS_CONSTANT                57210.24306     // Epoch: 0h Nov 17, 1858 (Per Wikipedia 7/8/2015)
#define TIME_SECS_SINCE_MJD(x)          (  ( (currentmjd(0.)-(x)) ) * 86400.   )
#define TIME_UNIXs_TO_MJD(unixSecs)     (  ( (unixSecs) / 86400.0 ) + MJD_UNIX_OFFSET  )            // From http://stackoverflow.com/questions/466321/convert-unix-timestamp-to-julian
#define TIME_MJD_TO_UNIXs(mjd)          (((mjd) - MJD_UNIX_OFFSET) * 86400.0)                       // Inverse operation of above
#define TIME_UNIX_TV_TO_DOUBLE_SECS(x)  ( ((double)(x.tv_sec)) + ((double)(x.tv_usec)  / 1000000. ) )

#define IF_ERROR(x)  if ( 0 > (iretn = (x))  )
#define EXIT_WITH_ERROR(string) { printf(string); exit(1); }

extern double mjd_start_time;
#define UPTIME (86400.*(currentmjd(0.)-mjd_start_time))


extern "C"
{
    void print_timestamp();
    size_t cosmos_timeutils_gen_timestr(time_t unixtime, char* timestring, uint16_t max_len);
}


#endif // GP_COSMOSTIMEUTILS_H

