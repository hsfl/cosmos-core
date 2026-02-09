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
    trecord.id = records.size() + 1;
    if (trecord.id > maxid)
    {
        maxid = trecord.id;
    }
    if (records.size() < trecord.id)
    {
        records.resize(trecord.id);
    }
    if (!trecord.pass)
    {
        ++failcount;
    }
    records[trecord.id-1] = trecord;

    return maxid;
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
        return maxid;
    }
    else
    {
        failcount = maxid;
        return iretn;
    }
}
