#include "support/configCosmos.h"
#include "agent/agentclass.h"

static Agent *agent;
static beatstruc antbeat;
static string antbase = "";
static string ant_state;

int32_t query_antenna();

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    string node = "";

    switch (argc)
    {
    case 3:
        node = argv[2];
    case 2:
        antbase = argv[1];
        break;
    default:
        printf("Usage: agent_antenna antenna node");
        exit (1);
    }

    agent = new Agent();
    if ((iretn = agent->wait()) < 0)
    {
        agent->debug_error.Printf("%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }

    while (1)
    {
        iretn = query_antenna();
        if (iretn >= 0)
        {
            printf("%s\r", ant_state.c_str());
	    fflush(stdout);
        }
        secondsleep(2.);
    }
}

int32_t query_radios() { return 0; }

int32_t query_antenna()
{
    // Locate agent_ant if not present
    if (antbeat.utc == 0.)
    {
        antbeat = agent->find_agent("any", antbase, 5.);
        if (antbeat.utc == 0.)
        {
            return AGENT_ERROR_DISCOVERY;
        }
    }

    // Send request
    string response;
    agent->send_request(antbeat, "get_state", response);
    ant_state = response;
    for (uint16_t i=response.size(); i<200; ++i)
    {
        ant_state.push_back(' ');
    }

    return 0;
}
