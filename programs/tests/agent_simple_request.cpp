#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "support/timeutils.h"
#include "agent/agentclass.h"
#include "support/cosmos-errno.h"

#include <iostream>
#include <string>

Cosmos::Support::Error e;
static Agent *agent;
string request_add(vector<string> &args, int32_t &status){

    if(args.size() < 2) { // incorrect arg count
        status = AGENT_ERROR_REQUEST;    // update error
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

string request_hello(int32_t &status){

    string response ="HelloWorld";

    return response;
}

int main(int argc, char **argv)
{
    int32_t status = 0;
    agent = new Agent("","test_simple_request");

    status = agent->add_request("add", request_add, "int1 int2", "returns the sum of 2 integers");
    status = agent->add_request("hello", request_hello, "", "replies with: \"HelloWorld\"");

    //adding a device
    devicestruc *sun1 = nullptr;
    status = agent->add_device("sun1", DeviceType::SSEN, &sun1);
    if(status < 0){
        cout << e.ErrorString(status) << endl;
    }
    sun1->ssen.azimuth = 50.;
    cout << sun1->ssen.azimuth << endl;

    string sun2_utc;
    status = agent->device_property_name("sun2", "utc", sun2_utc);
    if(status < 0){
        cout << e.ErrorString(status) << endl;
    }
    cout << sun2_utc << endl;
    string azi;
    status = agent->device_property_name("sun2", "azimuth", azi);
    if(status < 0){
        cout << e.ErrorString(status) << endl;
    }
    cout << azi  << endl;





}
