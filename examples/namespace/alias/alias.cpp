#include <iostream>     // std::cout, std::left
#include <iomanip>      // std::setprecision
using namespace std;

// cosmos includes
#include "agentlib.h"
#include "jsonlib.h"

cosmosstruc *cdata; // to access the cosmos data, will change later

int main()
{
    cout << "Example for COSMOS Namespace (elements) Aliases" << endl;


    string nodename = "cubesat1";

    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_client(AGENT_TYPE_UDP,
                                     nodename, 10000)))
    {
        cout << "agent_setup_client failed (error <" << AGENT_ERROR_JSON_CREATE << ">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        cout << "agent setup client ... OK" << endl;
    }

    jsonhandle eqhandle;
    string equation = "(\"device_imu_temp_000\" - 273.15)";
    json_equation_map(equation,cdata,&eqhandle);
    json_addentry("tempCelcius", equation, cdata);

    while(1) {
        // set temp in Kelvin
        cdata->devspec.imu[0]->gen.temp = 0;

        double tempCelcius = json_equation_handle(&eqhandle, cdata);
        cout << tempCelcius << endl;

        double temp = json_get_double("tempCelcius", cdata);
        cout << temp << endl;
    };

    return 0;

}

