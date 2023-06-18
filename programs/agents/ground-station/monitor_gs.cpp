#include "support/configCosmos.h"
#include "agent/agentclass.h"

static Agent *agent;
static vector<beatstruc> radiobeat;
static beatstruc controlbeat;
static string control_state;

int32_t query_control();
int32_t query_radios();

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    string node = "";

    switch (argc)
    {
    case 2:
        node = argv[1];
        break;
    }

    agent = new Agent();
    if ((iretn = agent->wait()) < 0)
    {
        agent->debug_log.Printf("%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }

    while (1)
    {
        iretn = query_control();
        if (iretn >= 0)
        {
            printf("%s\r", control_state.c_str());
	    fflush(stdout);
        }
        secondsleep(2.);
    }
}

int32_t query_radios() { return 0; }

int32_t query_control()
{
    // Locate agent_control if not present
    if (controlbeat.utc == 0.)
    {
        controlbeat = agent->find_agent("any", "control", 5.);
        if (controlbeat.utc == 0.)
        {
            return AGENT_ERROR_DISCOVERY;
        }
    }

    // Send request
    string response;
    agent->send_request(controlbeat, "get_state", response);
    control_state = response;
    for (uint16_t i=response.size(); i<200; ++i)
    {
        control_state.push_back(' ');
    }

    return 0;
}
