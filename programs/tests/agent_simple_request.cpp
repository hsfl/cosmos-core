#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "support/timeutils.h"
#include "agent/agentclass.h"
#include "support/cosmos-errno.h"

#include <iostream>
#include <string>

Cosmos::Support::Error e;
static Agent *agent;
string request_add(vector<string> &args, int32_t &error){

    if(args.size() < 2) { // incorrect arg count
        error = AGENT_ERROR_REQUEST;    // update error
        return "";
    }

    string response = "";

    // reading argument in as int
    int num1 = stoi(args[0]);
    int num2 = stoi(args[1]);
    // use the argument to do something
    int sum = num1+ num2;
    // return the response string
    return to_string(sum);
}

string request_hello(int32_t &error){

    string response ="HelloWorld";

    return response;
}

int main(int argc, char **argv)
{
    int32_t error = 0;
    agent = new Agent("","test_simple_request");

    agent->add_request("add", request_add, "int1 int2", "returns the sum of 2 integers");
    agent->add_request("hello", request_hello, "", "replies with: \"HelloWorld\"");
    devicestruc* sun1 = agent->add_device("sun1", DeviceType::SSEN, error);
    devicestruc* sun2 =agent->add_device("sun2", DeviceType::SSEN, error);
    devicestruc* ant1 =agent->add_device("ant1", DeviceType::ANT, error);
    string sun2_utc = agent->get_soh_name("sun2", "utc", error);
    if(error < 0){
        cout << e.ErrorString(error) << endl;
    }
    cout << sun2_utc << endl;
    string azi = agent->get_soh_name("sun2", "azimuth", error);
    if(error < 0){
        cout << e.ErrorString(error) << endl;
    }
    cout << azi  << endl;
    agent->set_value(azi, 45);


//    while(agent->running()){

//        COSMOS_SLEEP(2.);
//    }

}
