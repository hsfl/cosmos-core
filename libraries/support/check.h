#ifndef CHECK_H
#define CHECK_H

#include "support/configCosmos.h"

namespace Cosmos {
namespace Support {
namespace Log {
class Checker
{
public:
    Checker(string base_name="test", string date_time="");
    ~Checker();
    int32_t Report(string name, string description, string result, bool pass);
    int32_t Save();
    int32_t Test(string nrecord);

    uint32_t maxid = 0;
    uint32_t failcount = 0;
    struct record
    {
        uint32_t id = 0;
        string name;
        string description;
        string result;
        bool pass = false;

        json11::Json to_json() const
        {
            return json11::Json::object
                {
                 { "id"   , id },
                 { "name"  , name },
                 { "description"  , description },
                 { "result"  , result },
                 { "pass" , pass},
                 };
        }

        int32_t from_json(const string& s)
        {
            string error;
            json11::Json p = json11::Json::parse(s,error);
            if(error.empty())
            {
                uint16_t count = 0;
                if(!p["id"].is_null())
                {
                    id = p["id"].number_value();
                    ++count;
                }
                if(!p["name"].is_null())
                {
                    name = p["name"].string_value();
                    ++count;
                }
                if(!p["description"].is_null())
                {
                    description = p["description"].string_value();
                    ++count;
                }
                if(!p["result"].is_null())
                {
                    result = p["result"].string_value();
                    ++count;
                }
                if(!p["pass"].is_null())
                {
                    pass = p["pass"].bool_value();
                    ++count;
                }
                return  count;
            }
            else
            {
                return GENERAL_ERROR_ARGS;
            }
        }

    };

    vector <record> records;

    json11::Json to_json() const
    {
        return json11::Json::object
            {
                { "maxid"   , maxid },
                { "failcount"   , failcount },
                { "records" , records}
            };
    }

    int32_t from_json(const string& s)
    {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty())
        {
            uint16_t count = 0;
            if(!p["maxid"].is_null())
            {
                maxid = p["maxid"].number_value();
                ++count;
            }
            if(!p["failcount"].is_null())
            {
                failcount = p["failcount"].number_value();
                ++count;
            }
            records.resize(p["records"].array_items().size());
            for (uint16_t i=0; i<records.size(); ++i)
            {
                if (!p["records"][i].is_null())
                {
                    count += records[i].from_json(p["records"][i].dump());
                }
            }
            return  count;
        }
        else
        {
            return GENERAL_ERROR_ARGS;
        }
    }


private:
    string file_name;
    FILE *fp = nullptr;
};
}
}
}

#endif // CHECK_H
