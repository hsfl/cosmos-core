// Example of an agent making a request to another agent
// agent 001 makes request to get upon activation

#include "configCosmos.h"
#include "elapsedtime.hpp"
#include "timeutils.hpp"
#include "agentlib.h"

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

int agentloop();
int generic_node_test(int type_number, string type);
int set_zeros(int type_number, string type);

string agentname        = "agent_generic_device_test";
string nodename         = "telem";
string agent_neighbor   = "agent_generic_device_neighbor"; //name of the agent that the request is directed to

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

beatstruc beat_agent_neighbour;
cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

int main(int argc, char *argv[])
{
    //setEnvCosmos("C:/COSMOS/");

    int iretn;

    cout << "Agent Name: " << agentname << endl;
    cout << "Agent Setup Server ... " ;
    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_server(AGENT_TYPE_UDP,
                                     nodename.c_str(),
                                     agentname.c_str(),
                                     1.0,
                                     0,
                                     AGENTMAXBUFFER)))
    {
        cout << "agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        cout << "OK" << endl;
    }

    beat_agent_neighbour = agent_find_server(cdata, nodename, agent_neighbor, 5.);

    if (beat_agent_neighbour.utc > 0){
        cout << "beat agent_neighbour node: " << beat_agent_neighbour.node << endl;
    } else {
        cout << "could not find " << agent_neighbor << endl;
        exit (0);
    }


    // Start agent thread
    iretn = agentloop();

    return 0;
}

int agentloop()
{
    cout << "agent " << agentname <<  " ...online " << endl;


    set_zeros(0, "vuint8");
    set_zeros(1, "vint8");
    set_zeros(2, "vuint16");
    set_zeros(3, "vint16");
    set_zeros(4, "vuint32");
    set_zeros(5, "vint32");
    set_zeros(6, "vfloat");
    set_zeros(7, "vdouble");
    //set_zeros(8, "vstring");

    // Start executing the agent
    while(agent_running(cdata))
    {


        cout << "-------------------" << endl;

        generic_node_test(0,"vuint8");
        generic_node_test(1,"vint8");
        generic_node_test(2,"vuint16");
        generic_node_test(3,"vint16");
        generic_node_test(4,"vuint32");
        generic_node_test(5,"vint32");
        generic_node_test(6,"vfloat");
        generic_node_test(7,"vdouble");
        generic_node_test(8,"vstring");


        COSMOS_SLEEP(1.0);

    }
    return (0);

}



int generic_node_test(int type_number, string type){
    string requestString;
    string response;
    char response_c_str[300];
    int iretn;

    // convert device number to string: 2 -> "002"
    ostringstream dev_number_string;
    dev_number_string << setw(3) << setfill('0') << type_number;

    // command ex.: getvalue {\"device_telem_vuint16_002\"}
    requestString = "getvalue {\"device_telem_" + type + "_" + dev_number_string.str() + "\"}";

    // makes a request to agent get
    iretn = agent_send_request(cdata,
                               beat_agent_neighbour,
                               requestString.c_str(),
                               response_c_str, // convert to C++ string
                               512,
                               2);

    //cout << "return:" << iretn << endl;

    response = string(response_c_str);
    json_parse(response,cdata);

    string value_str;

    if (type == "vuint8"){
        uint8_t value= cdata->devspec.telem[type_number]->vuint8;
        value_str = to_string((int)value);
    }

    if (type == "vint8"){
        int8_t value= cdata->devspec.telem[type_number]->vint8;
        value_str = to_string((int)value);
    }

    if (type == "vuint16"){
        uint16_t value= cdata->devspec.telem[type_number]->vuint16;
        value_str = to_string(value);
    }

    if (type == "vint16"){
        int16_t value= cdata->devspec.telem[type_number]->vint16;
        value_str = to_string(value);
    }

    if (type == "vuint32"){
        uint32_t value= cdata->devspec.telem[type_number]->vuint32;
        value_str = to_string(value);
    }

    if (type == "vint32"){
        int32_t value= cdata->devspec.telem[type_number]->vint32;
        value_str = to_string(value);
    }

    if (type == "vfloat"){
        float value= cdata->devspec.telem[type_number]->vfloat;
        value_str = to_string(value);
    }

    if (type == "vdouble"){
        double value= cdata->devspec.telem[type_number]->vdouble;
        value_str = to_string(value);
    }

    if (type == "vstring"){
        value_str = string(cdata->devspec.telem[type_number]->vstring);
    }

    if ( response.size() > 1){

        // print status
        cout << "received from device #" << type_number << " " << type << " : " <<  value_str << endl;

        // just increment the value by 1

        if (type == "vuint8"){
            cdata->devspec.telem[type_number]->vuint8 ++;
            value_str = to_string(cdata->devspec.telem[type_number]->vuint8);
        }

        if (type == "vint8"){
            cdata->devspec.telem[type_number]->vint8 ++;
            value_str = to_string(cdata->devspec.telem[type_number]->vint8);
        }

        if (type == "vuint16"){
            cdata->devspec.telem[type_number]->vuint16 ++;
            value_str = to_string(cdata->devspec.telem[type_number]->vuint16);
        }

        if (type == "vint16"){
            cdata->devspec.telem[type_number]->vint16 ++;
            value_str = to_string(cdata->devspec.telem[type_number]->vint16);
        }

        if (type == "vuint32"){
            cdata->devspec.telem[type_number]->vuint32 ++;
            value_str = to_string(cdata->devspec.telem[type_number]->vuint32);
        }

        if (type == "vint32"){
            cdata->devspec.telem[type_number]->vint32 ++;
            value_str = to_string(cdata->devspec.telem[type_number]->vint32);
        }

        if (type == "vfloat"){
            cdata->devspec.telem[type_number]->vfloat += 0.1;
            value_str = to_string(cdata->devspec.telem[type_number]->vfloat);
        }

        if (type == "vdouble"){
            cdata->devspec.telem[type_number]->vdouble += 0.1;
            value_str = to_string(cdata->devspec.telem[type_number]->vdouble);
        }

        if (type == "vstring"){
            //cdata->devspec.telem[type_number]->vstring = "asd";
            //value_str = to_string(cdata->devspec.telem[type_number]->vuint8);
			uint32_t itemp = atol(value_str.c_str());
			++itemp;
			sprintf(cdata->devspec.telem[type_number]->vstring, "%lu", itemp);
		}


        // now send the new value to the neighbour agent
        // command ex.: setvalue {\"device_telem_vuint16_002\"}
        requestString = "setvalue {\"device_telem_"+type+"_"
                + dev_number_string.str() + "\":"
                + value_str
                + "}";

        // makes a request to agent get
        iretn = agent_send_request(cdata,
                                   beat_agent_neighbour,
                                   requestString.c_str(),
                                   response_c_str, // convert to C++ string
                                   512,
                                   2);


        //cout << "Received from agent_neighbour: " << response.size() << " bytes : " << response << endl;
        response = "";
    }

    return iretn;
}



int set_zeros(int type_number, string type){
    string requestString;
    char response_c_str[300];
    int iretn;

    // convert device number to string: 2 -> "002"
    ostringstream dev_number_string;
    dev_number_string << setw(3) << setfill('0') << type_number;


    // now send the new value to the neighbour agent
    // command ex.: setvalue {\"device_telem_vuint16_002\"}
    requestString = "setvalue {\"device_telem_"+type+"_"
            + dev_number_string.str() + "\":"
            + to_string(0)
            + "}";

    // makes a request to agent get
    iretn = agent_send_request(cdata,
                               beat_agent_neighbour,
                               requestString.c_str(),
                               response_c_str, // convert to C++ string
                               512,
                               2);


    return iretn;
}
