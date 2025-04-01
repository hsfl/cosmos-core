#include "support/configCosmos.h"
#include "support/check.h"
#include "support/timelib.h"

Check::Check(string name, string date_time)
{
    if (date_time.empty())
    {
        date_time = utc2datetime(currentmjd());
    }
    file_name = name + "_" + date_time + ".chk";
}

int32_t Check::Report(string name, string description, string result, bool pass)
{
    if (fp == nullptr)
    {
        fp = fopen(file_name.c_str(), "a");
        if (fp == nullptr)
        {
            return -errno;
        }
    }

    fprintf(fp, "{\"sequencenumber\":%u,\"name\":\"%s\",\"description\":\"%s\",\"results\":\"%s\",\"pass\":\"%s\"}", sequence_num, name, description, result, (pass?"true":"false"));

    return sequence_num;
}
