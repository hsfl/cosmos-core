/*! \file
 *	\brief Example to demonstrate use of cosmos namespace aliases
 *  Follow this tutorial for more information tutorial-alias
 */

#include <iostream>     // std::cout
using namespace std;

// cosmos includes
#include "agent/agent.h"
#include "jsonlib.h"

cosmosstruc *cdata; // to access the cosmos data, will change later

/*!
 * \brief Example to demonstrate use of cosmos namespace aliases by
 * creating an alias to an equation that converts degrees Kelvin to degrees Celcius
 */
int main()
{
    cout << "Example for COSMOS Namespace (elements) Aliases" << endl;

    //! define the node name (it must be installed in <COSMOS>/nodes
    string nodename = "cubesat1";

    // Establish the agent inside the given node
    if (!(cdata = agent_setup_client(NetworkType::UDP, nodename, 10000)))
    {
        cout << "agent_setup_client failed (error <" << AGENT_ERROR_JSON_CREATE << ">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        cout << "agent setup client ... OK" << endl;
    }

    // define the equation using the COSMOS namespace
    // in this case we are setting an equation to convert the temperature
    // for imu 0 (device_imu_temp_000) from Kelvin (the standart temperature unit in COSMOS)
    // to degree Celcius
    string equation = "(\"device_imu_temp_000\" - 273.15)";

    // get the equation handle for later use
    jsonhandle eqhandle;

    // map/connect the equation to the handle
    json_equation_map(equation,cdata,&eqhandle);

    // add the equation to the system usin the alias "tempCelcius"
    // for easy retrieval of the imu temperature in celcius
    string alias = "imuTempCelcius";
    json_addentry(alias, equation, cdata);

    // set initial IMU temperature in Kelvin
    cdata->devspec.imu[0]->gen.temp = 0;

    // loop to demonstrate how to change values on the cdata structure
    // and get the values from the equation and alias mechanisms

    while(cdata->devspec.imu[0]->gen.temp <= 300) {

        cout << "temperature value set [K]           : " << cdata->devspec.imu[0]->gen.temp << endl;

        // using the equation handle get the result of the equation
        double temp = json_equation(&eqhandle, cdata);
        cout << "temperature value from handle   [C] : " << temp << endl;

        // using the alias get the result of the equation
        temp = json_get_double(alias, cdata);
        cout << "temperature value from alias    [C] : " <<  temp << endl;

        // change temperature value for demo only
        // set temp in Kelvin
        cdata->devspec.imu[0]->gen.temp += 1;

        cout << endl;
    };

    return 0;

}
