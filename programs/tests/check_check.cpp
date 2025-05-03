#include "support/configCosmos.h"
#include "support/check.h"
#include "support/jsonclass.h"

int main(int argc, char **argv)
{
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
            dis = &std::cin;
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
