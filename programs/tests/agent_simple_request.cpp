#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "support/timeutils.h"
#include "agent/agentclass.h"

#include <iostream>
#include <string>


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
    agent = new Agent("","test_simple_request");

    agent->add_request("test_req", example_request, "count", "an example usage of simple requests");
    agent->add_request("test_noarg", no_arg_request, "", "an example usage of simple requests");
    while(agent->running()){
        COSMOS_SLEEP(1.);
    }

}
