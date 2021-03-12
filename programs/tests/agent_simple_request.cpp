#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "support/timeutils.h"
#include "agent/agentclass.h"
#include "support/cosmos-errno.h"

#include <iostream>
#include <string>

Cosmos::Support::Error e;
static Agent *agent;
string example_request(vector<string> &args, int32_t &error){

    if(args.size() < 1) { // incorrect arg count
        error = AGENT_ERROR_REQUEST;    // update error
        return "";
    }

    string response = "";

    // reading argument in as int
    int r = stoi(args[0]);
    // use the argument to do something
    for(int i = 0; i < r; i++) response+="HelloWorld";

    // return the response string
    return response;
}

string no_arg_request(int32_t &error){

    string response ="HelloWorld";

    return response;
}

int main(int argc, char **argv)
{
    int32_t error = 0;
    agent = new Agent("","test_simple_request");

    agent->add_request("test_req", example_request, "count", "an example usage of simple requests");
    agent->add_request("test_noarg", no_arg_request, "", "an example usage of simple requests");
    agent->add_device("sun1", DeviceType::SSEN, error);
    agent->add_device("sun2", DeviceType::SSEN, error);
    agent->add_device("ant1", DeviceType::ANT, error);
    string sun2 = agent->get_soh_name("sun2", "utc", error);
    if(error < 0){
        cout << e.ErrorString(error) << endl;
    }
    cout << sun2 << endl;
    string azi = agent->get_soh_name("ant1", "azim", error);
    if(error < 0){
        cout << e.ErrorString(error) << endl;
    }
    cout << azi  << endl;
    agent->set_value(azi, 45);
    //json_parse("{\"node_utc\":20,}", agent->cinfo);
    double utc = json_get_double(azi, agent->cinfo);
    cout << utc;


}
