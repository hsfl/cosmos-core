#include "support/configCosmos.h"
#include "support/check.h"
#include "support/timelib.h"

Log::Checker::Checker(string name, string date_time)
{
    if (date_time.empty())
    {
        date_time = utc2datetime(currentmjd());
    }
    file_name = name + "_" + date_time + ".chk";
}

Log::Checker::~Checker()
{
    if (fp != nullptr)
    {
        fclose(fp);
    }
}

int32_t Log::Checker::Report(string name, string description, string result, bool pass)
{
    if (fp == nullptr)
    {
        fp = fopen(file_name.c_str(), "a");
        if (fp == nullptr)
        {
            return -errno;
        }
    }

    fprintf(fp, "{\"sequencenumber\":%u,\"name\":\"%s\",\"description\":\"%s\",\"results\":\"%s\",\"pass\":\"%s\"}\n", ++sequence_num, name.c_str(), description.c_str(), result.c_str(), (pass?"true":"false"));

    return sequence_num;
}
