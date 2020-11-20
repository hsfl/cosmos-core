/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

/*! \file agent_calc.cpp
* \brief Demonstration calculator Agent
*/

//! \ingroup agents
//! \defgroup agent_calc Calculation Agent
//! This program accepts requests to perform simple math operations and returns the result.
//! With it you can:
//! - add
//! - subtract
//! - multiply
//! - divide

// code from testbed/blank_agent.cpp

#include "support/configCosmos.h"

#include <stdio.h>
#include <iostream>

using namespace std;

#include "agent/agentclass.h"
#include "physics/physicslib.h"
#include "support/jsonlib.h"

int myagent();

static char nodename[COSMOS_MAX_NAME + 1] = "otb";
static char agentname[COSMOS_MAX_NAME + 1] = "calc";

int32_t request_add(string &request, string &response, Agent *agent);
int32_t request_sub(string &request, string &response, Agent *agent);
int32_t request_mul(string &request, string &response, Agent *agent);
int32_t request_div(string &request, string &response, Agent *agent);

int32_t request_change_node_name(string &request, string &response, Agent *agent);

#define MAXBUFFERSIZE 100000 // comm buffer for agents

static Agent *agent; // to access the cosmos data, will change later
/*
void replace(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

vector<size_t> find_newlines(const string& sample) {
    vector<size_t> characterLocations;
    for(size_t i =0; i < sample.size(); i++) if(sample[i] == '\n') characterLocations.push_back(i);
    return characterLocations;
}

void pretty_form(string& js)	{

	replace(js, ", ", ",\n");
	replace(js, "{", "{\n");
	replace(js, "[", "[\n");
	replace(js, "}", "\n}");
	replace(js, "]", "\n]");

	// create vector[char position] = # of indents
	int indent = 0;
	vector<size_t> indents;
	for(size_t i = 0; i < js.size(); ++i)	{
		if(js[i]=='['){	++indent;}
		if(js[i]=='{'){	++indent;}
		if(js[i]==']'){	--indent; indents[i-1]--;}
		if(js[i]=='}'){	--indent; indents[i-1]--;}
		indents.push_back(indent);
	}

	// find position of all '\n' characters
	vector<size_t> newlines = find_newlines(js);

	// insert the appropriate # of indents after the '\n' char
    for(size_t i = newlines.size(); --i!=0; ) {
		string indent_string;
		for(size_t j = 0; j < indents[newlines[i]]; ++j)	indent_string += "  ";
		js.insert(newlines[i]+1, indent_string);
	}
	return;
}
*/

int main(int argc, char *argv[])
{
    int iretn;

    // Process arguments if present

    // Make node_name = 1st argument
	if (argc == 2)
        strcpy(nodename, argv[1]);
    // Make agent_proc = 2st argument
	if (argc == 3)	{
        strcpy(agentname, argv[2]);
        strcpy(nodename, argv[1]);
	}

    // Initialize agents. Set nodename if provided through command line args
	if (argc > 1)
    {
        // Initialize agent instance with specified node and agent names
        agent = new Agent(nodename, agentname);

        // Check if agent was successfully constructed.
        if ((iretn = agent->wait()) < 0)
        {
            fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
            exit(iretn);
        }
        else
        {
            fprintf(agent->get_debug_fd(), "%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
        }


        cout << "Hello, I am an agent. My name is [" << nodename << ":" << agentname << "]" << endl << endl;
	}
	else
    {
        // Initialize agent instance with unspecified node name but specified agent name
        agent = new Agent("", agentname);

        // Check if agent was successfully constructed.
        if ((iretn = agent->wait()) < 0)
        {
            fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
            exit(iretn);
        }
        else
        {
            fprintf(agent->get_debug_fd(), "%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
        }


        cout << "Hello, I am an agent. My name is [null:" << agentname << "]" << endl << endl;
	}

    for (uint16_t i=0; i<agent->cinfo->jmap.size(); ++i)
	{
        if (agent->cinfo->jmap[i].size())
		{
            cout << "jmap[" << i << "]:" << agent->cinfo->jmap[i][0].name << endl;
		}
	}

    cout << agent->cinfo->node.name << endl;

	string jsp;
    json_out(jsp, "node_name", agent->cinfo);
    cout << "<" << jsp << ">" << endl;
	jsp.clear();
    json_out(jsp, "event_name", agent->cinfo);
    cout << "before <" << jsp << ">" << endl;
	
	//set_json_value("event_name", "EVENT NAME");
	strcpy(agent->cinfo->event[0].name, "NEW NAME");
	jsp.clear();
    json_out(jsp, "event_name", agent->cinfo);

	cout << " after <" << jsp << ">" << endl;

	agent->cinfo->equation.push_back(equationstruc{ "string1", "string2" });

// see if we can print out entire cosmosstruc in JSON

	agent->cinfo->add_name("Entire COSMOSSTRUC", agent->cinfo);
	cout<<"Output:\n\t<"<<agent->cinfo->get_json<cosmosstruc>("Entire COSMOSSTRUC")<<">"<<endl;





string entire = R"(
{
  "Entire COSMOSSTRUC": {
	"node": {
		"name": "HAL12001",
		"downtime" : 123
	},
	"jmapped": 181,
	"unit": [
		[
			{
				"name": "",
				"p0": 0,
				"p1": 0,
				"p2": 0,
				"type": 0
			}
		],
		[{"name": "m", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "km", "p0": 0, "p1": 0.0010000000474974513, "p2": 0, "type": 1}, {"name": "cm", "p0": 0, "p1": 100, "p2": 0, "type": 1}, {"name": "ft", "p0": 0, "p1": 3.2808330059051514, "p2": 0, "type": 1}], [{"name": "kg", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "g", "p0": 0, "p1": 1000, "p2": 0, "type": 1}], [{"name": "kgm2", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "s", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "min", "p0": 0, "p1": 0.01666666753590107, "p2": 0, "type": 1}, {"name": "hr", "p0": 0, "p1": 0.00027777778450399637, "p2": 0, "type": 1}, {"name": "day", "p0": 0, "p1": 1.1574074051168282e-05, "p2": 0, "type": 1}, {"name": "MJD", "p0": 0, "p1": 1.1574074051168282e-05, "p2": 0, "type": 1}], [{"name": "MJD", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "min", "p0": 0, "p1": 1440, "p2": 0, "type": 1}, {"name": "hr", "p0": 0, "p1": 24, "p2": 0, "type": 1}, {"name": "day", "p0": 0, "p1": 1, "p2": 0, "type": 1}], [{"name": "A", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "K", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "�C", "p0": -273.14999389648438, "p1": 1, "p2": 0, "type": 1}], [{"name": "Ca", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "m2", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "ft2", "p0": 0, "p1": 10.763910293579102, "p2": 0, "type": 1}], [{"name": "m3", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "m/s", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "km/s", "p0": 0, "p1": 0.0010000000474974513, "p2": 0, "type": 1}, {"name": "cm/s", "p0": 0, "p1": 100, "p2": 0, "type": 1}], [{"name": "m/s2", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "g", "p0": 0, "p1": 0.10197161883115768, "p2": 0, "type": 1}], [{"name": "kg/m3", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Cd/m2", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "rad", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "deg", "p0": 0, "p1": 57.295780181884766, "p2": 0, "type": 1}], [{"name": "rad/s", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "deg/s", "p0": 0, "p1": 57.295780181884766, "p2": 0, "type": 1}], [{"name": "", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "sr", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Hz", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "N", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "dyn", "p0": 0, "p1": 100000, "p2": 0, "type": 1}, {"name": "lb", "p0": 0, "p1": 0.22481000423431396, "p2": 0, "type": 1}], [{"name": "Nm", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Pa", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "j", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "erg", "p0": 0, "p1": 10000000, "p2": 0, "type": 1}], [{"name": "watt", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "C", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "Ahr", "p0": 0, "p1": 3600, "p2": 0, "type": 1}], [{"name": "V", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "F", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "ohm", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "A/m", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Wb", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "T", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "uT", "p0": 0, "p1": 1000000, "p2": 0, "type": 1}, {"name": "nT", "p0": 0, "p1": 1000000000, "p2": 0, "type": 1}], [{"name": "Am2", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "s", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "GiB", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "KiB", "p0": 0, "p1": 0.0009765625, "p2": 0, "type": 1}], [{"name": "", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "Please work", "p0": 0, "p2": 0, "type": 13}]],
"equation": [
	{"name": "string3", "value": "string4"}
]
}})";


//string entire = R"({"Entire COSMOSSTRUC": {"jmapped": 181, "timestamp": 59166.746294288205, "unit": [[{"name": "", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "m", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "km", "p0": 0, "p1": 0.0010000000474974513, "p2": 0, "type": 1}, {"name": "cm", "p0": 0, "p1": 100, "p2": 0, "type": 1}, {"name": "ft", "p0": 0, "p1": 3.2808330059051514, "p2": 0, "type": 1}], [{"name": "kg", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "g", "p0": 0, "p1": 1000, "p2": 0, "type": 1}], [{"name": "kgm2", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "s", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "min", "p0": 0, "p1": 0.01666666753590107, "p2": 0, "type": 1}, {"name": "hr", "p0": 0, "p1": 0.00027777778450399637, "p2": 0, "type": 1}, {"name": "day", "p0": 0, "p1": 1.1574074051168282e-05, "p2": 0, "type": 1}, {"name": "MJD", "p0": 0, "p1": 1.1574074051168282e-05, "p2": 0, "type": 1}], [{"name": "MJD", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "min", "p0": 0, "p1": 1440, "p2": 0, "type": 1}, {"name": "hr", "p0": 0, "p1": 24, "p2": 0, "type": 1}, {"name": "day", "p0": 0, "p1": 1, "p2": 0, "type": 1}], [{"name": "A", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "K", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "�C", "p0": -273.14999389648438, "p1": 1, "p2": 0, "type": 1}], [{"name": "Ca", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "m2", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "ft2", "p0": 0, "p1": 10.763910293579102, "p2": 0, "type": 1}], [{"name": "m3", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "m/s", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "km/s", "p0": 0, "p1": 0.0010000000474974513, "p2": 0, "type": 1}, {"name": "cm/s", "p0": 0, "p1": 100, "p2": 0, "type": 1}], [{"name": "m/s2", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "g", "p0": 0, "p1": 0.10197161883115768, "p2": 0, "type": 1}], [{"name": "kg/m3", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Cd/m2", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "rad", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "deg", "p0": 0, "p1": 57.295780181884766, "p2": 0, "type": 1}], [{"name": "rad/s", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "deg/s", "p0": 0, "p1": 57.295780181884766, "p2": 0, "type": 1}], [{"name": "", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "sr", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Hz", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "N", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "dyn", "p0": 0, "p1": 100000, "p2": 0, "type": 1}, {"name": "lb", "p0": 0, "p1": 0.22481000423431396, "p2": 0, "type": 1}], [{"name": "Nm", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Pa", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "j", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "erg", "p0": 0, "p1": 10000000, "p2": 0, "type": 1}], [{"name": "watt", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "C", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "Ahr", "p0": 0, "p1": 3600, "p2": 0, "type": 1}], [{"name": "V", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "F", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "ohm", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "A/m", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "Wb", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "T", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "uT", "p0": 0, "p1": 1000000, "p2": 0, "type": 1}, {"name": "nT", "p0": 0, "p1": 1000000000, "p2": 0, "type": 1}], [{"name": "Am2", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "s", "p0": 0, "p1": 0, "p2": 0, "type": 0}], [{"name": "GiB", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "KiB", "p0": 0, "p1": 0.0009765625, "p2": 0, "type": 1}], [{"name": "", "p0": 0, "p1": 0, "p2": 0, "type": 0}, {"name": "Please work", "p0": 0, "p1": 100, "p2": 0, "type": 13}]]}})";

agent->cinfo->set_json_value<cosmosstruc>("Entire COSMOSSTRUC", entire);

	//cout<<"Output:\n\t<"<<agent->cinfo->get_json<cosmosstruc>("Entire COSMOSSTRUC")<<">"<<endl;
	//string js = agent->cinfo->get_json<cosmosstruc>("Entire COSMOSSTRUC");	
	//pretty_form(js);
	string js = agent->cinfo->get_json<cosmosstruc>("Entire COSMOSSTRUC");	
	cout<<"Output:<\n"<<js<<"\n>"<<endl;

	devicestruc d;
	agent->cinfo->device.push_back(d);

	js = agent->cinfo->get_json_pretty<cosmosstruc>("Entire COSMOSSTRUC");	
	cout<<"Output:<\n"<<js<<"\n>"<<endl;

	agent->cinfo->add_name("Entire COSMOSSTRUC Equations", &agent->cinfo->equation);
	cout<<"Output:\n\t<\n"<<agent->cinfo->get_json<vector<equationstruc>>("Entire COSMOSSTRUC Equations")<<">"<<endl;
	cout<<"Output:\n\t<\n"<<agent->cinfo->get_json_pretty<vector<equationstruc>>("Entire COSMOSSTRUC Equations")<<">"<<endl;


/*
	agent->cinfo->add_name("Entire COSMOSSTRUC Unit", &agent->cinfo->unit);
	cout<<"Output:\n\t<"<<agent->cinfo->get_json<vector<vector<unitstruc>>>("Entire COSMOSSTRUC Unit")<<">"<<endl;

	equationstruc eq = { "Test Name", "Test Value" };
	vector<equationstruc> eqs;
	eqs.push_back(eq);
	agent->cinfo->set_value<vector<equationstruc>>("Entire COSMOSSTRUC Equations", eqs);
	cout<<"Output:\n\t<"<<agent->cinfo->get_json<vector<equationstruc>>("Entire COSMOSSTRUC Equations")<<">"<<endl;

*/

    // Define the requests that we need for this agent
    if ((iretn=agent->add_request("add",request_add)))
        exit (iretn);

    if ((iretn=agent->add_request("sub",request_sub)))
        exit (iretn);

    if ((iretn=agent->add_request("mul",request_mul)))
        exit (iretn);

    if ((iretn=agent->add_request("div",request_div)))
        exit (iretn);

    if ((iretn=agent->add_request("node",request_change_node_name)))
        exit (iretn);

	// Start our own thread
    iretn = myagent();
}

int myagent()
{
	// Start performing the body of the agent
    while(agent->running())
	{
		COSMOS_SLEEP(0.1); // no support in win
	}

	return 0;
}

// the name of this fn will always be changed
int32_t request_add(string &request, string &response, Agent *agent)
{
	float a,b;

	sscanf(request.c_str(),"%*s %f %f",&a,&b);
    response = std::to_string(a + b);

	return 0;
}

// the name of this fn will always be changed
int32_t request_sub(string &request, string &response, Agent *agent)
{
	float a,b;

	sscanf(request.c_str(),"%*s %f %f",&a,&b);
    response = std::to_string(a - b);

	return 0;
}
// the name of this fn will always be changed
int32_t request_mul(string &request, string &response, Agent *agent)
{
	float a,b;

    sscanf(request.c_str(),"%*s %f %f", &a, &b);
    response = std::to_string(a * b);

	return 0;
}
// the name of this fn will always be changed
int32_t request_div(string &request, string &response, Agent *agent)
{
	float a,b;

    sscanf(request.c_str(),"%*s %f %f", &a, &b);
    response = std::to_string(a / b);

	return 0;
}

int32_t request_change_node_name(string &request, string &response, Agent *agent)
{
	char new_name[41];
	sscanf(request.c_str(),"%*s %40s", new_name);

    strcpy(agent->cinfo->node.name, new_name);
    cout << "The new node name is <" << agent->cinfo->node.name << ">" << endl;

	return 0;
}
