#include "support/configCosmos.h"
#include "support/jsonclass.h"
#include "support/jsonobject.h"

int main(int argc, char *argv[])
{
    char buf[1000];
    FILE *fpi = nullptr;
    FILE *fpo = nullptr;

    if (argc != 3)
    {
        printf("Usage: json2tab [-|filename.json] [-|filename.tab]\n");
        exit(GENERAL_ERROR_ARGS);
    }

    if (argv[1][0] == '-')
    {
        fpi = stdin;
    }
    else
    {
        fpi = fopen(argv[1], "r");
    }

    if (argv[2][0] == '-')
    {
        fpo = stdout;
    }
    else
    {
        fpo = fopen(argv[2], "w");
    }

    if (fpi == nullptr || fpo == nullptr)
    {
        printf("Usage: json2tab [-|filename.json] [-|filename.tab]\n");
        exit(GENERAL_ERROR_ARGS);
    }

    if (fgets(buf, 1000, fpi) == nullptr)
    {
        exit(-errno);
    }
    Json jobject(buf);
    for (const auto& member : jobject.ObjectContents)
    {
        if (member.first.find("event_") == 0)
        {
            fprintf(fpo, "%s\t", member.first.c_str());
        }
    }
//    for (uint16_t i=0; i<jobject.Members.size(); ++i)
//    {
//        if (jobject.Members[i].value.name.find("event_") == 0)
//        {
//            fprintf(fpo, "%s\t", jobject.Members[i].value.name.c_str());
//        }
//    }
    fprintf(fpo, "\n");
    for (const auto& member : jobject.ObjectContents)
//    for (uint16_t i=0; i<jobject.Members.size(); ++i)
    {
        if (member.first.find("event_utc") == 0)
        {
            fprintf(fpo, "%f\t", member.second.nvalue);
        }
        else if (member.first.find("event_name") == 0)
        {
            fprintf(fpo, "%s\t", member.second.svalue.c_str());
        }
        else if (member.first.find("event_type") == 0)
        {
            fprintf(fpo, "%u\t", static_cast<uint32_t>(member.second.nvalue));
        }
        else if (member.first.find("event_flag") == 0)
        {
            fprintf(fpo, "%u\t", static_cast<uint32_t>(member.second.nvalue));
        }
        else if (member.first.find("event_data") == 0)
        {
            fprintf(fpo, "%s\t", member.second.svalue.c_str());
        }
        else if (member.first.find("event_condition") == 0)
        {
            fprintf(fpo, "%s\t", member.second.svalue.c_str());
        }
    }
    fprintf(fpo, "\n");

    while (fgets(buf, 1000, fpi) != nullptr)
    {
        Json jobject(buf);
        for (const auto& member : jobject.ObjectContents)
//        for (uint16_t i=0; i<jobject.Members.size(); ++i)
        {
            if (member.first.find("event_utc") == 0)
            {
                fprintf(fpo, "%f\t", member.second.nvalue);
            }
            else if (member.first.find("event_name") == 0)
            {
                fprintf(fpo, "%s\t", member.second.svalue.c_str());
            }
            else if (member.first.find("event_type") == 0)
            {
                fprintf(fpo, "%u\t", static_cast<uint32_t>(member.second.nvalue));
            }
            else if (member.first.find("event_flag") == 0)
            {
                fprintf(fpo, "%u\t", static_cast<uint32_t>(member.second.nvalue));
            }
            else if (member.first.find("event_data") == 0)
            {
                fprintf(fpo, "%s\t", member.second.svalue.c_str());
            }
            else if (member.first.find("event_condition") == 0)
            {
                fprintf(fpo, "%s\t", member.second.svalue.c_str());
            }
        }
        fprintf(fpo, "\n");
    }
}
