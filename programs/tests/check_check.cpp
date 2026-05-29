#include "support/configCosmos.h"
#include "support/check.h"
#include "support/jsonclass.h"

int main(int argc, char **argv)
{
    const string HYTI2VERSION = "1.02";
    const string USAGE = "check_check [filename]";
    if (argc == 2)
    {
        if (!strcmp(argv[1], "--version"))
        {
            printf("Version: %s\n", HYTI2VERSION.c_str());
            exit (1);
        }
        if (!strcmp(argv[1], "--help"))
        {
            printf("Usage: %s\n", USAGE.c_str());
            exit (1);
        }
    }

    Log::Checker *check;
    json11::Json input;
    std::istream *dis;
    ifstream ifs;

    if (argc > 1)
    {
        // If a command-line argument is provided, treat it as a filename
        ifs.open(argv[1]);
        if (ifs.is_open())
        {
            dis = &ifs;
        }
        else
        {
            printf("Unable to open %s\n", argv[1]);
            exit(1);
        }
    }
    else
    {
        // If no argument, use stdin
        dis = &std::cin;
    }

    check = new Log::Checker();
    string iline;
    while(std::getline(*dis, iline))
    {
        check->Test(iline);
    }

    if (argc > 1 && ifs.is_open())
    {
        ifs.close();
    }

    exit(100*(float(check->failcount)/check->maxid));
}
