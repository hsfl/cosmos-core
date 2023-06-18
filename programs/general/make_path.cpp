#include "support/configCosmos.h"
#include "support/datalib.h"

int main(int argc, char *argv[])
{
    string node = "";
    string agent = "log";
    string location = "outgoing";
    string extra = "";
    string type = "txt";
    double mjd = currentmjd();

    if (argc == 2 && argv[1][0] == '-')
    {
        printf("Usage: make_path [ location [ agent [ type [ extra [ node [ mjd ] ] ] ] ] ]\n");
        exit (1);
    }

    switch (argc)
    {
    case 7:
        mjd = atof(argv[6]);
    case 6:
        node = argv[5];
    case 5:
        extra = argv[4];
    case 4:
        type = argv[3];
    case 3:
        agent = argv[2];
    case 2:
        location = argv[1];
    default:
        break;
    }

    if (node.empty())
    {
        char hostname[60];
        gethostname(hostname, sizeof (hostname));
        node = hostname;

    }

    string name = data_name(mjd, type, node, agent, extra);
    name = data_base_path(node, location, agent, name);

    if (name.size())
    {
        printf("%s\n", name.c_str());
    }
}
