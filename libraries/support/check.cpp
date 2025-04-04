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
    record trecord;
    trecord.name = name;
    trecord.description = description;
    trecord.result = result;
    trecord.pass = pass;
    trecord.sequencenumber = records.size() + 1;
    if (trecord.sequencenumber > maxsequencenumber)
    {
        maxsequencenumber = trecord.sequencenumber;
    }
    if (records.size() < trecord.sequencenumber)
    {
        records.resize(trecord.sequencenumber);
    }
    if (!trecord.pass)
    {
        ++failcount;
    }
    records[trecord.sequencenumber-1] = trecord;

    return maxsequencenumber;
}

int32_t Log::Checker::Save()
{
    if (fp == nullptr)
    {
        fp = fopen(file_name.c_str(), "a");
        if (fp == nullptr)
        {
            return -errno;
        }
    }

    fprintf(fp, "%s\n", to_json().dump().c_str());
    return 0;
}

int32_t Log::Checker::Test(string nrecord)
{
    int32_t iretn = from_json(nrecord);

    if (iretn > 0)
    {
        return maxsequencenumber;
    }
    else
    {
        failcount = maxsequencenumber;
        return iretn;
    }
}
