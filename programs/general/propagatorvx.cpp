#include "support/configCosmos.h"
#include "physics/simulatorclass.h"
//#include "agent/agentclass.h"
#include "support/jsonclass.h"

//int32_t request_get_sat_json(string &request, string &response, Agent *agent);
//int32_t request_get_node_json(string &request, string &response, Agent *agent);
//int32_t request_get_pieces_json(string &request, string &response, Agent *agent);
//int32_t request_get_devspec_json(string &request, string &response, Agent *agent);
//int32_t request_get_devgen_json(string &request, string &response, Agent *agent);
//int32_t request_get_location(string &request, string &response, Agent *agent);
//int32_t request_set_thrust(string &request, string &response, Agent *agent);
//int32_t request_set_torque(string &request, string &response, Agent *agent);

#define DEBUG 1

// GLOBAL PROPAGATOR VARIABLES

int32_t parse_control(string args);
Physics::Simulator *sim;
//Agent *agent;
//double speed=1.;
//change to 0 to test fastest mode
double speed=1.;

double simdt = 1.;				//sim->Init
double initialutc = 60107.01;	//sim->Init
string realmname = "propagate";	//sim->Init

double endutc = 0.;				// does this get used anywhere?
double deltautc = 0.;			// does this get used anywhere?
string orbitfile = "orbit.dat";	// does this get used anywhere?
string satfile = "sats.dat";
string targetfile = "targets.dat";
string tlefile = "tle.dat";		// does this get used anywhere?
int32_t runcount = 1500;
vector <cartpos> lvlhoffset;

int main(int argc, char *argv[])
{
	int32_t iretn;

	cout<<"Testing Propagator V3 (x)"<<endl;

// play around with time
	cout<<"initialutc (mjd2iso) = "<<initialutc<<" --> "<<mjd2iso8601(initialutc)<<endl;
	cout<<"initialutc (utc2iso) = "<<initialutc<<" --> "<<utc2iso8601(initialutc)<< " --> " << iso86012utc(utc2iso8601(initialutc)) <<endl;

	calstruc caltime = mjd2cal(initialutc);
	double new_initialutc = cal2mjd(caltime);
	cout<<"Year = "<<caltime.year<<endl;
	cout<<"Month = "<<caltime.month<<endl;
	cout<<"Day = "<<caltime.dom<<endl;
	cout<<"Hour = "<<caltime.hour<<endl;
	cout<<"Minute = "<<caltime.minute<<endl;
	cout<<"Second = "<<caltime.second<<endl;
	cout<<"Error in inversion (mjd -> calstruc -> mjd) = "<<initialutc - new_initialutc<<" = "<<86400*(initialutc - new_initialutc)<<" seconds"<<endl;;
	/// test error in iso and back
	new_initialutc = iso86012utc(mjd2iso8601(initialutc));
	cout<<"Error in inversion (mjd -> iso8601 -> mjd) = "<<initialutc - new_initialutc<<" = "<<86400*(initialutc - new_initialutc)<<" seconds"<<endl;;

	if (argc > 1)
	{
		iretn = parse_control(argv[1]);
		if(iretn < 0)	{
			cerr<<"Unable to parse all control arguments.  Exiting."<<endl;
			exit(iretn);
		}
	}

	// check the values that may have been set by parse_control before creating simulator

/*
	agent = new Agent(realmname, "", "propagate", 0.);
	agent->set_debug_level(0);
	agent->add_request("get_sat_json", request_get_sat_json, "nodename", "Get JSON description of satellite for Node nodename");
	agent->add_request("get_node_json", request_get_node_json, "nodename", "Get JSON description of node for Node nodename");
	agent->add_request("get_pieces_json", request_get_pieces_json, "nodename", "Get JSON description of pieces for Node nodename");
	agent->add_request("get_devgen_json", request_get_devgen_json, "nodename", "Get JSON description of general devices for Node nodename");
	agent->add_request("get_devspec_json", request_get_devspec_json, "nodename", "Get JSON description of specific for Node nodename");
	agent->add_request("get_location", request_get_location, "nodename", "Get JSON of position and attitude for Node nodename");
	agent->add_request("set_thrust", request_set_thrust, "nodename {thrust}", "Set JSON Vector of thrust for Node nodename");
	agent->add_request("set_torque", request_set_torque, "nodename {torque}", "Set JSON Vector of torque for Node nodename");
*/

	cout<<"Creating new physics simulator...";
	sim = new Physics::Simulator();
	cout<<"done."<<endl;

	iretn = sim->GetError();
	if (iretn <0) {
		exit(iretn);
	}
	sim->Init(simdt, realmname, initialutc);
	sim->ParseOrbitFile();
	sim->ParseSatFile();
	lvlhoffset.resize(sim->cnodes.size());
	sim->ParseTargetFile();
	if (speed == 1.)
	{
		//agent->cinfo->agent0.aprd = simdt;
		//agent->start_active_loop();
	}
	int run = 0;
	while (run++ < runcount)
	{
		cout<<"Propagating...";
//		if (speed == 1.)
//		{
//			for (uint16_t i=1; i<sim->cnodes.size(); ++i)
//			{
//				locstruc target;
//				target.pos.eci = sim->cnodes[0]->currentinfo.node.loc.pos.eci;
//				target.pos.lvlh = lvlhoffset[i];
//				pos_lvlh2origin(target);
//				sim->UpdatePush(sim->cnodes[i]->currentinfo.node.name, Physics::ControlThrust(sim->cnodes[i]->currentinfo.node.loc.pos.eci, target.pos.eci, sim->cnodes[i]->currentinfo.mass, sim->cnodes[i]->currentinfo.devspec.thst[0].maxthrust/sim->cnodes[i]->currentinfo.mass, simdt));
//			}
//		}
		sim->Propagate();
		cout<<"done."<<endl;
		for (auto &state : sim->cnodes)
		{
			if (speed > 1.0 && state->currentinfo.event.size())
			{
				for (eventstruc event : state->currentinfo.event)
				{
					json11::Json jobj = json11::Json::object({
						{"event_utc", event.utc},
						{"event_name", event.name},
						{"event_type", static_cast<int>(event.type)},
						{"event_flag", static_cast<int>(event.flag)},
						{"event_el", event.el},
						{"event_az", event.az},
						{"geodpos", state->currentinfo.node.loc.pos.geod.s}
					});
					printf("%s\n", jobj.dump().c_str());
				}
			}
		}
		if (speed == 1.)
		{
			//agent->finish_active_loop();
		}
	}
}

int32_t parse_control(string args)
{
	uint16_t argcount = 0;
	string estring;
	json11::Json jargs = json11::Json::parse(args, estring);

	if (!estring.empty()) {
		cerr << "Error parsing JSON: " << estring << endl;
		return -1; // or handle the error as needed
	}

    if (!jargs["runcount"].is_null()) {
        ++argcount;
        // Check runcount is integer
        double runcount_arg = jargs["runcount"].number_value();
        if (runcount_arg != static_cast<int32_t>(runcount_arg)) {
            cerr << "runcount must be a positive integer" << endl;
            return -1; // or handle the error as needed
        }
        // Check runcount is positive
        if (runcount_arg < 1) {
            cerr << "runcount must be greater than 0" << endl;
            return -1; // or handle the error as needed
        }
        runcount = static_cast<int32_t>(runcount_arg);
        if (DEBUG > 0) cerr << "runcount set to " << runcount << endl;
    }

	if (!jargs["speed"].is_null())
	{
		++argcount;
		speed = jargs["speed"].number_value();
		if(DEBUG>0)	cerr<<"speed set to "<<speed<<endl;
	}
	if (!jargs["initialutc"].is_null())
	{
		++argcount;
		// check if time is in the right century, etc
		initialutc = jargs["initialutc"].number_value();
		if(DEBUG>0)	cerr<<"initialutc set to "<<initialutc<<endl;
	}
	if (!jargs["endutc"].is_null())
	{
		++argcount;
		// check if time is in the right century, etc
		endutc = jargs["endutc"].number_value();
		if(DEBUG>0)	cerr<<"endutc set to "<<endutc<<endl;
	}
	if (!jargs["deltautc"].is_null())
	{
		++argcount;
		deltautc = jargs["deltautc"].number_value();
		if(DEBUG>0)	cerr<<"deltautc set to "<<deltautc<<endl;
	}
	if (!jargs["simdt"].is_null())
	{
		++argcount;
        // Check simdt is positive
        if (jargs["simdt"].number_value() <= 0.) {
            cerr << "simdt must be greater than 0" << endl;
            return -1; // or handle the error as needed
        }
		simdt = jargs["simdt"].number_value();
		if(DEBUG>0)	cerr<<"simdt set to "<<simdt<<endl;
	}
	//	if (!jargs["minaccelratio"].is_null())
	//	{
	//		++argcount;
	//		minaccelratio = jargs["minaccelratio"].number_value();
	//	}
	if (!jargs["satfile"].is_null())
	{
		++argcount;
		satfile = jargs["satfile"].string_value();
		// JIMNOTE: check if it is a file and readable?
		if(DEBUG>0)	cerr<<"satfile set to "<<satfile<<endl;
	}
	if (!jargs["orbitfile"].is_null())
	{
		++argcount;
		orbitfile = jargs["orbitfile"].string_value();
		// JIMNOTE: check if it is a file and readable?
		if(DEBUG>0)	cerr<<"orbitfile set to "<<orbitfile<<endl;
	}
	if (!jargs["targetfile"].is_null())
	{
		++argcount;
		targetfile = jargs["targetfile"].string_value();
		// JIMNOTE: check if it is a file and readable?
		if(DEBUG>0)	cerr<<"targetfile set to "<<targetfile<<endl;
	}
	if (!jargs["realmname"].is_null())
	{
		++argcount;
		realmname = jargs["realmname"].string_value();
		if(DEBUG>0)	cerr<<"realmname set to "<<realmname<<endl;
	}

	return argcount;
}

/*
int32_t request_get_sat_json(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			string temp;
			response = json_pieces(temp, &(*sit)->currentinfo);
			json_join(response, json_devices_general(temp, &(*sit)->currentinfo));
			json_join(response, json_devices_specific(temp, &(*sit)->currentinfo));
		}
	}
	return response.length();
}

int32_t request_get_node_json(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			json_node(response, &(*sit)->currentinfo);
		}
	}
	return response.length();
}

int32_t request_get_pieces_json(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			json_pieces(response, &(*sit)->currentinfo);
		}
	}
	return response.length();
}

int32_t request_get_devgen_json(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			json_devices_general(response, &(*sit)->currentinfo);
		}
	}
	return response.length();
}

int32_t request_get_devspec_json(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			json_devices_specific(response, &(*sit)->currentinfo);
		}
	}
	return response.length();
}

int32_t request_get_location(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			json11::Json jobj = json11::Json::object({
				{"node", (*sit)->currentinfo.node.name},
				{"utcoffset", (*sit)->currentinfo.node.utcoffset},
				{"pos", (*sit)->currentinfo.node.loc.pos.eci},
				{"att", (*sit)->currentinfo.node.loc.att.icrf}
			});
			response = jobj.dump();
		}
	}
	return response.length();
}

int32_t request_set_thrust(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			string estring;
			json11::Json jargs = json11::Json::parse(args[2], estring);
			if (estring.empty())
			{
				(*sit)->currentinfo.node.phys.fpush.from_json(args[2]);
				response = (*sit)->currentinfo.node.phys.fpush.to_json().dump();
			}
		}
	}
	return response.length();
}

int32_t request_set_torque(string &request, string &response, Agent *agent)
{
	vector<string> args = string_split(request);
	response.clear();

	if (args.size() > 1)
	{
		Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
		if (sit != sim->cnodes.end())
		{
			string estring;
			json11::Json jargs = json11::Json::parse(args[2], estring);
			if (estring.empty())
			{
				(*sit)->currentinfo.node.phys.ftorque.from_json(args[2]);
				response = (*sit)->currentinfo.node.phys.ftorque.to_json().dump();
			}
		}
	}
	return response.length();
}
*/
