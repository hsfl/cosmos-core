#include "support/configCosmos.h"
#include "support/jsonclass.h"
#include "support/jsonobject.h"
#include "support/stringlib.h"

string skip_undesired(string input);

int main(int argc, char *argv[])
{
    char buf[1000];
    FILE *fpi = nullptr;
    FILE *fpo = nullptr;

    if (argc != 3)
    {
        printf("Usage: tab2json [-|filename.tab] [-|filename.json]\n");
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
        printf("Usage: tab2json [-|filename.tab] [-|filename.json]\n");
        exit(GENERAL_ERROR_ARGS);
    }

    if (fgets(buf, 1000, fpi) == nullptr)
    {
        exit(-errno);
    }
    vector <string> fields = string_split(skip_undesired(buf), "\t");

    while (fgets(buf, 1000, fpi) != nullptr)
    {
        vector <string> values = string_split(skip_undesired(buf), "\t");
        JSONObject jobject;
        for (uint16_t i=0; i<fields.size(); ++i)
        {
            if (fields[i].find("event_utc") == 0)
            {
                jobject.addElement(fields[i], stof(values[i]));
            }
            else if (fields[i].find("event_name") == 0)
            {
                jobject.addElement(fields[i], values[i]);
            }
            else if (fields[i].find("event_type") == 0)
            {
                jobject.addElement(fields[i], stoi(values[i]));
            }
            else if (fields[i].find("event_flag") == 0)
            {
                jobject.addElement(fields[i], stoi(values[i]));
            }
            else if (fields[i].find("event_data") == 0)
            {
                jobject.addElement(fields[i], values[i]);
            }
            else if (fields[i].find("event_condition") == 0)
            {
                if (values[i][0] == '"')
                {
                    jobject.addElement(fields[i], values[i].substr(1,values[i].size()-2));
                }
                else
                {
                    jobject.addElement(fields[i], values[i]);
                }
            }
        }
        fprintf(fpo, "%s\n", jobject.to_json_string().c_str());
    }

}

string skip_undesired(string input)
{
    string output;
    for (uint16_t i = 0; i<input.length()-1; ++i)
    {
        if (input[i] == '"' && input[i+1] == '"')
        {
            continue;
        }
        if (input[i] == '\r' || input[i] == '\n')
        {
            continue;
        }
        output.push_back(input[i]);
    }
    if (input.back() != '\r' && input.back() != '\n')
    {
        output.push_back(input.back());
    }
    return output;
}
