#include "support/configCosmos.h"
#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"

int32_t parse_control(string args);
int32_t request_get_sat_json(string &request, string &response, Agent *agent);
int32_t request_get_node_json(string &request, string &response, Agent *agent);
int32_t request_get_pieces_json(string &request, string &response, Agent *agent);
int32_t request_get_devspec_json(string &request, string &response, Agent *agent);
int32_t request_get_devgen_json(string &request, string &response, Agent *agent);
int32_t request_get_location_node(string &request, string &response, Agent *agent);
int32_t request_get_offsetutc(string &request, string &response, Agent *agent);
int32_t request_set_thrust(string &request, string &response, Agent *agent);
int32_t request_set_torque(string &request, string &response, Agent *agent);
int32_t request_dump_node(string &request, string &response, Agent *agent);
Physics::Simulator *sim;
Agent *agent;
double simdt = 1.;
uint16_t realtime=1;
uint16_t printevent=0;
uint16_t postevent=0;
double initialutc = 60107.01;
double endutc = 0.;
double deltautc = 0.;
double fov_override = 0.;
double ifov_override = 0.;
string realmname = "propagate";
//string orbitfile = "/home/user/cosmos/source/projects/cosmos_sttr2020/cosmos/realms/sttr/orbit.dat";
//string satfile = "/home/user/cosmos/source/projects/cosmos_sttr2020/cosmos/realms/sttr/sats.dat";
//string targetfile = "/home/user/cosmos/source/projects/cosmos_sttr2020/cosmos/realms/sttr/targets.dat";
string orbitfile = "orbit.dat";
string satfile = "sats.dat";
string targetfile = "targets.dat";
uint32_t runcount = 1000;
socket_bus data_channel_out;
DeviceCpu deviceCpu;
DeviceDisk deviceDisk;

vector<vector<cosmosstruc>> results;

// output sat positions for visualization (ECI)
void create_sat_eci_position_files()	{
    for (size_t sat_num=0; sat_num<results[0].size(); ++sat_num)	{
        ofstream out;
        // replace with your path
        out.open("/home/user/cosmos/source/core/python/plot_orbit/sttr/sat_"+std::to_string(sat_num)+".eci");
        if(out.is_open())   {
            // start at time = 1 so file lines = runcount
            for (size_t t=1; t<results.size(); ++t) {
                out << std::fixed << std::setprecision(6);
                out
                    <<results[t][sat_num].node.loc.pos.eci.s.col[0]<<","
                    <<results[t][sat_num].node.loc.pos.eci.s.col[1]<<","
                    <<results[t][sat_num].node.loc.pos.eci.s.col[2]<<","
                    <<results[t][sat_num].node.loc.pos.eci.utc
                    <<endl;
            }
            out.close();
        }
    }
    return;
}

// output sat positions for visualization (GEOC)
void create_sat_geoc_position_files()	{
    for (size_t sat_num=0; sat_num<results[0].size(); ++sat_num)	{
        ofstream out;
        // replace with your path
        out.open("/home/user/cosmos/source/core/python/plot_orbit/sttr/sat_"+std::to_string(sat_num)+".geoc");
        if(out.is_open())   {
            // start at time = 1 so file lines = runcount
            for (size_t t=1; t<results.size(); ++t) {
                out << std::fixed << std::setprecision(6);
                out
                    <<results[t][sat_num].node.loc.pos.geoc.s.col[0]<<","
                    <<results[t][sat_num].node.loc.pos.geoc.s.col[1]<<","
                    <<results[t][sat_num].node.loc.pos.geoc.s.col[2]<<","
                    <<results[t][sat_num].node.loc.pos.geoc.utc
                    <<endl;
            }
            out.close();
        }
    }
    return;
}

// output target positions for visualization (ECI)
void create_target_eci_position_files()	{
    // just using target for mothership for now
    size_t s = 0;
    for (size_t targ=0; targ<results[0][s].target.size(); ++targ)   {
        ofstream out;
        // replace with your path
        out.open("/home/user/cosmos/source/core/python/plot_orbit/sttr/target_"+std::to_string(targ)+".eci");
        if(out.is_open())   {
            // start at time = 1 so file lines = runcount (also, NOTE: target eci / utc not set for time = 0)
            for (size_t t=1; t<results.size(); ++t) {
                // output position of targets
                out << std::fixed << std::setprecision(6);
                out
                    <<results[t][s].target[targ].loc.pos.eci.s.col[0]<<","
                    <<results[t][s].target[targ].loc.pos.eci.s.col[1]<<","
                    <<results[t][s].target[targ].loc.pos.eci.s.col[2]<<","
                    <<results[t][s].target[targ].loc.pos.eci.utc
                    <<endl;
            }
            out.close();
        }
    }
    return;
}

// output target positions for visualization (GEOC)
void create_target_geoc_position_files()	{
    // just using target for mothership for now
    size_t s = 0;
    for (size_t targ=0; targ<results[0][s].target.size(); ++targ)   {
        ofstream out;
        // replace with your path
        out.open("/home/user/cosmos/source/core/python/plot_orbit/sttr/target_"+std::to_string(targ)+".geoc");
        if(out.is_open())   {
            // start at time = 1 so file lines = runcount (also, NOTE: target geoc / utc not set for time = 0)
            for (size_t t=1; t<results.size(); ++t) {
                // output position of targets
                out << std::fixed << std::setprecision(6);
                out
                    <<results[t][s].target[targ].loc.pos.geoc.s.col[0]<<","
                    <<results[t][s].target[targ].loc.pos.geoc.s.col[1]<<","
                    <<results[t][s].target[targ].loc.pos.geoc.s.col[2]<<","
                    <<results[t][s].target[targ].loc.pos.geoc.utc
                    <<endl;
            }
            out.close();
        }
    }
    return;
}

// output attitude vectors for visualization (ECI)
void create_attitude_vector_files()	{

	// print out quaternions
	// for each sat
    for (size_t sat_num=0; sat_num<results[0].size(); ++sat_num)    {
        ofstream out;
        // replace with your path
        out.open("/home/user/cosmos/source/core/python/plot_orbit/sttr/sat_"+std::to_string(sat_num)+".att");
        if(out.is_open())   {
            // for each time step // start at time = 1 so file lines = runcount
            for (size_t t=1; t<results.size(); ++t)   {
                Quaternion Q(
                    results[t][sat_num].node.loc.att.icrf.s.d.x,
                    results[t][sat_num].node.loc.att.icrf.s.d.y,
                    results[t][sat_num].node.loc.att.icrf.s.d.z,
                    results[t][sat_num].node.loc.att.icrf.s.w
                );
				// this is the vector pointing toward the earth in the body frame
                Vector V(0,-1,0,0);
                Vector R = Q.irotate(V);

                out << std::fixed << std::setprecision(6);
                out
                    << R.x << ", "
                    << R.y << ", "
                    << R.z << ", "
                    << 100 // utc
                    << endl;
            }
            out.close();
        }
    }
    return;
}

vector<size_t> indices_of_targets_observed(
	vector<vector<cosmosstruc>>& results,
	vector<size_t>& sat_nums,
	vector<size_t>& target_nums,
	size_t start_timestep=0,
	size_t stop_timestep=runcount-1
)
{
	vector<bool>	target_observed(results[0][0].target.size(), false);
	vector<size_t>	indices_of_targets_observed;

	// for each sat
	for (size_t sat_num : sat_nums)	{
		// for each time step
		for (size_t t=start_timestep; t<=stop_timestep; ++t)	{
			// for each target
			for (size_t target_num : target_nums)	{
				// if observed
                if(results[t][sat_num].target[target_num].cover[0].area != 0.) {
					// note that it has been observed
					target_observed[target_num] = true;
				}
			}
		}
	}
	// record indices of all targets that have been observed
	for(size_t target_num : target_nums)	{
		if(target_observed[target_num])	{
			indices_of_targets_observed.push_back(target_num);
		}
	}
	return indices_of_targets_observed;
}

size_t number_of_targets_observed(
	vector<vector<cosmosstruc>>& results,
	vector<size_t>& sat_nums,
	vector<size_t>& target_nums,
	size_t start_timestep=0,
	size_t stop_timestep=runcount-1
)
{
	return indices_of_targets_observed(results, sat_nums, target_nums, start_timestep, stop_timestep).size();
}

double extract_results_min_resolution(
	vector<vector<cosmosstruc>>& results,
	vector<size_t>& sat_nums,
	vector<size_t>& target_nums,
	size_t start_timestep=0,
	size_t stop_timestep=runcount-1
)
{

	cout<<std::fixed<<std::setprecision(7);
	double min_resolution = std::numeric_limits<double>::max();
	// for each sat
	for (size_t sat_num : sat_nums)	{
		// for each time step
		for (size_t t=start_timestep; t<=stop_timestep; ++t)	{
			// for each target
			for (size_t target_num : target_nums)	{
                // if target was observed
                if(results[t][sat_num].target[target_num].cover[0].area != 0.) {
                    // record minimum resolution
					if(results[t][sat_num].target[target_num].cover[0].resolution<min_resolution)	{
						min_resolution=results[t][sat_num].target[target_num].cover[0].resolution;
					}
				}
			}
		}
	}
	return min_resolution;
}

double extract_results_average_percent(
    vector<vector<cosmosstruc>>& results,
    vector<size_t>& sat_nums,
    vector<size_t>& target_nums,
    size_t start_timestep = 0,
    size_t stop_timestep = runcount - 1
) {
    cout << std::fixed << std::setprecision(7);
    double average_percent = 0.0;  // Start with 0 for running average
    size_t count = 0;              // Number of valid percent values

    // Iterate over satellites
    for (size_t sat_num : sat_nums) {
        // Iterate over timesteps
        for (size_t t = start_timestep; t <= stop_timestep; ++t) {
            // Iterate over targets
            for (size_t target_num : target_nums) {
                // If the target was observed
                if(results[t][sat_num].target[target_num].cover[0].area != 0.) {
                    // Update the running average
                    double percent = results[t][sat_num].target[target_num].cover[0].percent;
                    count++;
                    average_percent += (percent - average_percent) / count;
                }
            }
        }
    }

    // Return the computed average percent
    return average_percent;
}

double extract_results_total_area(
    vector<vector<cosmosstruc>>& results,
    vector<size_t>& sat_nums,
    vector<size_t>& target_nums,
    size_t start_timestep = 0,
    size_t stop_timestep = runcount - 1
) {
    cout << std::fixed << std::setprecision(7);
    double total_area = 0.0;  // Initialize total area to zero

    // Iterate over satellites
    for (size_t sat_num : sat_nums) {
        // Iterate over timesteps
        for (size_t t = start_timestep; t <= stop_timestep; ++t) {
            // Iterate over targets
            for (size_t target_num : target_nums) {
                // If the target was observed
                if(results[t][sat_num].target[target_num].cover[0].area != 0.) {
                    // Accumulate the area
                    total_area += results[t][sat_num].target[target_num].cover[0].area;
                }
            }
        }
    }

    // Return the total area
    return total_area;
}

// extract simulation results from results
void extract_results(
	vector<vector<cosmosstruc>>& results,
	vector<size_t>& sat_nums,
	vector<size_t>& target_nums,
	string filename="results.dat",
	size_t start_timestep=0,
	size_t stop_timestep=runcount-1
)
{

	if(sat_nums.empty())	return;

	// check the ranges of the sat indices
	// check the ranges of the target indices

	ofstream out;
	// try to open file
	out.open(filename);
	if(out.is_open())   {
		out<<std::fixed<<std::setprecision(7);
		// for each sat
		for (size_t sat_num : sat_nums)	{
			// for each time step
			for (size_t t=start_timestep; t<=stop_timestep; ++t)	{
				// for each target
                for (size_t target_num : target_nums)	{
                    if(results[t][sat_num].target[target_num].cover[0].area != 0.) {
                            out<<"cover[0].resolution == "<<results[t][sat_num].target[target_num].cover[0].resolution<<endl;
					}
				}
			}
		}
		out.close();
	}
	return;
}


// For cosmos web
socket_channel cosmos_web_telegraf_channel, cosmos_web_api_channel;
const string TELEGRAF_ADDR = "";
string cosmos_web_addr = "";
const int TELEGRAF_PORT = 10096;
const int API_PORT = 10097;
bool sockets_initialized = false;
int32_t open_cosmos_web_sockets(string cosmos_web_addr);
void reset_db(Physics::Simulator *sim);
int32_t send_telem_to_cosmos_web(cosmosstruc* cinfo);

int main(int argc, char *argv[])
{
    int32_t iretn;
    uint32_t elapsed = 0;

    std::cout << std::fixed << std::setprecision(7);

    if (argc > 1)
    {
        parse_control(argv[1]);
		//debug
		//cout<<"fov_override = "<<fov_override<<endl;
		//cout<<"ifov_override = "<<ifov_override<<endl;
    }

    agent = new Agent(realmname, "", "propagate", 0.);
    agent->set_debug_level(0);
    agent->add_request("get_sat_json", request_get_sat_json, "nodename", "Get JSON description of satellite for Node nodename");
    agent->add_request("get_node_json", request_get_node_json, "nodename", "Get JSON description of node for Node nodename");
    agent->add_request("get_pieces_json", request_get_pieces_json, "nodename", "Get JSON description of pieces for Node nodename");
    agent->add_request("get_devgen_json", request_get_devgen_json, "nodename", "Get JSON description of general devices for Node nodename");
    agent->add_request("get_devspec_json", request_get_devspec_json, "nodename", "Get JSON description of specific for Node nodename");
    agent->add_request("get_location_node", request_get_location_node, "nodename", "Get JSON of position and attitude for Node nodename");
    agent->add_request("get_offsetutc", request_get_offsetutc, "", "Get simulator offset of UTC in julian days");
    agent->add_request("set_thrust", request_set_thrust, "nodename {thrust}", "Set JSON Vector of thrust for Node nodename");
    agent->add_request("set_torque", request_set_torque, "nodename {torque}", "Set JSON Vector of torque for Node nodename");
    agent->add_request("dump_node", request_dump_node, "nodename", "Dump description for Node nodename");

    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_log.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    // todo: check return value
    iretn=sim->Init(simdt, realmname, initialutc);
    if(iretn<0)	{	cout<<"unable to initialize simulation"<<endl; exit(-1); }

    // parse orbit file and do not proceed if no orbits are parsed
    iretn=sim->ParseOrbitFile(orbitfile);
    if(iretn<0)	{	cout<<"unable to parse orbitfile = <"<<orbitfile<<">"<<endl; exit(-1); }
    //cout<<"currentutc = "<<sim->currentutc<<endl; // debug
    //cout<<"initialutc = "<<sim->initialutc<<endl; // debug

    // parse satellite file and do not proceed if no satellites are parsed
    iretn=sim->ParseSatFile(satfile);
    if(iretn<=0)	{	cout<<"unable to parse satfile = <"<<satfile<<">"<<endl; exit(-1); }
    //cout<<"sim->cnodes.size() = "<<sim->cnodes.size()<<endl; // debug

// override fov and ifov of all detectors
	//cout<<"number of detectors: "<<sim->detectors.size()<<endl;
/*
	for(const auto& pair : sim->detectors)	{
		cout<<"detector name = "<<pair.first<<endl;
		cout<<"detector fov  = "<<pair.second.fov<<endl;
		cout<<"detector ifov = "<<pair.second.ifov<<endl;
	}
	// old values
	// new values
*/

    // parse target file and do not proceed if no targets are parsed
    iretn=sim->ParseTargetFile(targetfile);
    if(iretn<=0)	{	cout<<"unable to parse targetfile = <"<<targetfile<<">"<<endl; exit(-1); }
    //cout<<"sim->targets.size() = "<<sim->targets.size()<<endl; // debug

    // debug print targets
    for(size_t i = 0; i < sim->cnodes.size(); ++i)	{
        //cout<<"sim->cnodes["<<i<<"]->currentinfo.target.size() = "<<sim->cnodes[i]->currentinfo.target.size()<<endl;
        //for(size_t j = 0; j < sim->cnodes[i]->currentinfo.target.size(); ++j)	{
        //cout<<"sim->cnodes["<<i<<"]->currentinfo.target["<<j<<"].loc.pos.geod.s = ("<<RAD2DEG(sim->cnodes[i]->currentinfo.target[j].loc.pos.geod.s.lat)<<", "<<RAD2DEG(sim->cnodes[i]->currentinfo.target[j].loc.pos.geod.s.lon)<<", "<<sim->cnodes[i]->currentinfo.target[j].loc.pos.geod.s.h<<")"<<endl;
        //}
    }

    if (realtime)
    {
        agent->cinfo->agent0.aprd = simdt;
        agent->start_active_loop();
    }

    sim->Propagate(results, runcount);
    sim->Target(results);
    sim->Metric(results);

	//cout<<"number of detectors: "<<sim->detectors.size()<<endl; // why does this equal 1 ?

    // debug results
    //cout<<"runcount == "<<runcount<<endl;
    //cout<<"results.size() == "<<results.size()<<endl;
    //cout<<"results[0].size() == "<<results[0].size()<<endl;
    //cout<<"results["<<runcount-1<<"].size() == "<<results[runcount-1].size()<<endl;

    while (elapsed < runcount)
    {
        for (uint16_t i=0; i<results[elapsed].size(); ++i)
        {
            if (results[elapsed][i].event.size())
            {
                for (eventstruc event : results[elapsed][i].event)
                {
                    if (printevent)
                    {
                        string output = "type: event";
                        output += to_label("\tnode", results[elapsed][i].node.name);
                        output += to_label("\tutc", to_mjd(results[elapsed][i].node.utc));
                        output += to_label("\tname", event.name);
                        output += to_label("\ttype", to_unsigned(static_cast<int>(event.type)));
                        output += to_label("\tflag", to_unsigned(static_cast<int>(event.flag)));
                        output += to_label("\tel", to_floating(event.el, 4));
                        output += to_label("\taz", to_floating(event.az, 4));
                        output += to_label("\tdtime", to_floating(86400.*event.dtime, 1));
                        output += to_label("\tlon", to_floating(results[elapsed][i].node.loc.pos.geod.s.lon, 5));
                        output += to_label("\tlat", to_floating(results[elapsed][i].node.loc.pos.geod.s.lat, 5));
                        output += to_label("\talt", to_floating(results[elapsed][i].node.loc.pos.geod.s.h, 1));
                        output += to_label("\tpowerin", to_floating(results[elapsed][i].node.phys.powgen, 2));
                        output += to_label("\tland", (event.flag & EVENT_FLAG_LAND) != 0);
                        output += to_label("\tumbra", (event.flag & EVENT_FLAG_UMBRA) != 0);
                        output += to_label("\tgs", (event.flag & EVENT_FLAG_GS) != 0);
                        printf("%s\n", output.c_str());
                    }
                    if (postevent)
                    {
                        json11::Json jobj = json11::Json::object({
                            {"mtype", "event"},
                            {"node_name", results[elapsed][i].node.name},
                            {"event_utc", event.utc},
                            {"event_name", event.name},
                            {"event_type", static_cast<int>(event.type)},
                            {"event_flag", static_cast<int>(event.flag)},
                            {"event_el", event.el},
                            {"event_az", event.az},
                            {"geodpos", results[elapsed][i].node.loc.pos.geod.s}
                        });
                        string output = jobj.dump();
                        iretn = socket_post(data_channel_out, output.c_str());

                        // Post SOH
                        string jstring;
                        agent->post(Agent::AgentMessage::EVENT, jobj.dump());
                    }
                }
            }
            if (printevent)
            {
                string output = "type: node";
                output += to_label("\tnode", results[elapsed][i].node.name);
                output += to_label("\tutc", to_mjd(results[elapsed][i].node.utc));
                output += to_label("\tecix", to_floating(results[elapsed][i].node.loc.pos.eci.s.col[0], 1));
                output += to_label("\teciy", to_floating(results[elapsed][i].node.loc.pos.eci.s.col[1], 1));
                output += to_label("\teciz", to_floating(results[elapsed][i].node.loc.pos.eci.s.col[2], 1));
                output += to_label("\tecivx", to_floating(results[elapsed][i].node.loc.pos.eci.v.col[0], 2));
                output += to_label("\tecivy", to_floating(results[elapsed][i].node.loc.pos.eci.v.col[1], 2));
                output += to_label("\tecivz", to_floating(results[elapsed][i].node.loc.pos.eci.v.col[2], 2));
                output += to_label("\teciax", to_floating(results[elapsed][i].node.loc.pos.eci.a.col[0], 3));
                output += to_label("\teciay", to_floating(results[elapsed][i].node.loc.pos.eci.a.col[1], 3));
                output += to_label("\teciaz", to_floating(results[elapsed][i].node.loc.pos.eci.a.col[2], 3));
                //                output += to_label("\tthetax", to_floating(results[elapsed][i].node.loc.att.icrf.s.d.x, 4));
                //                output += to_label("\tthetay", to_floating(results[elapsed][i].node.loc.att.icrf.s.d.y, 4));
                //                output += to_label("\tthetaz", to_floating(results[elapsed][i].node.loc.att.icrf.s.d.z, 4));
                //                output += to_label("\tthetaw", to_floating(results[elapsed][i].node.loc.att.icrf.s.w, 4));
                //                output += to_label("\tomegax", to_floating(results[elapsed][i].node.loc.att.icrf.v.col[0], 1));
                //                output += to_label("\tomegay", to_floating(results[elapsed][i].node.loc.att.icrf.v.col[1], 1));
                //                output += to_label("\tomegaz", to_floating(results[elapsed][i].node.loc.att.icrf.v.col[2], 1));
                //                output += to_label("\talphax", to_floating(results[elapsed][i].node.loc.att.icrf.a.col[0], 1));
                //                output += to_label("\talphay", to_floating(results[elapsed][i].node.loc.att.icrf.a.col[1], 1));
                //                output += to_label("\talphaz", to_floating(results[elapsed][i].node.loc.att.icrf.a.col[2], 1));
                output += to_label("\tpowerin", to_floating(results[elapsed][i].node.phys.powgen, 2));
                output += to_label("\tpowerout", to_floating(results[elapsed][i].node.phys.powuse, 2));
                results[elapsed][i].devspec.cpu[0].load = static_cast <float>(deviceCpu.getLoad());
                results[elapsed][i].devspec.cpu[0].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
                results[elapsed][i].devspec.cpu[0].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
                results[elapsed][i].devspec.cpu[0].maxload = deviceCpu.getCpuCount();
                output += to_label("\tload", to_floating(results[elapsed][i].devspec.cpu[0].load / results[elapsed][i].devspec.cpu[0].maxload, 3));
                output += to_label("\tmemory", to_floating(results[elapsed][i].devspec.cpu[0].gib / results[elapsed][i].devspec.cpu[0].maxgib, 3));
                output += to_label("\tstorage", to_floating(results[elapsed][i].devspec.cpu[0].storage, 4));
                cartpos delta = eci2lvlh(results[elapsed][0].node.loc.pos.eci, results[elapsed][i].node.loc.pos.eci);
                //                rvector ds = results[elapsed][i].node.loc.pos.eci.s - sim->cnodes[0]->currentinfo.node.loc.pos.eci.s;
                //                rvector dv = results[elapsed][i].node.loc.pos.eci.v - sim->cnodes[0]->currentinfo.node.loc.pos.eci.v;
                output += to_label("\tdeltax", to_floating(delta.s.col[0], 2));
                output += to_label("\tdeltay", to_floating(delta.s.col[1], 2));
                output += to_label("\tdeltaz", to_floating(delta.s.col[2], 2));
                output += to_label("\tdeltavx", to_floating(delta.v.col[0], 3));
                output += to_label("\tdeltavy", to_floating(delta.v.col[1], 3));
                output += to_label("\tdeltavz", to_floating(delta.v.col[2], 3));
                printf("%s\n", output.c_str());
            }
        }
        //        if (printevent)
        //        {
        //            printf("\n");
        //            fflush(stdout);
        //        }
        ++elapsed;
    }


    // create position files for visualization
    create_sat_eci_position_files();
    create_sat_geoc_position_files();
    create_target_eci_position_files();
    create_target_geoc_position_files();

    // todo:  create attitude vector files (eci) for visualization
    create_attitude_vector_files();


    // analyze results

    // look at coverage metrics

    // see if any are non-zero
/*
    // for each time step
    for (size_t t=0; t<results.size(); ++t)	{
    	// for each sat
    	for (size_t sat_num=0; sat_num<results[0].size(); ++sat_num)	{
			// for each target
        	for (size_t targ=0; targ<results[0][sat_num].target.size(); ++targ)   {
                if (results[t][sat_num].target[targ].cover.size())	{
                    cout<<
                        "sat #"<<sat_num<<
                        ", target #"<<targ<<
                        ", time = "<<t<<
                        ",percent = "<<results[t][sat_num].target[targ].cover[0].percent<<
                        ",area = "<<results[t][sat_num].target[targ].cover[0].area<<
                        ",resolution = "<<results[t][sat_num].target[targ].cover[0].resolution<<
                        ",specmin = "<<results[t][sat_num].target[targ].cover[0].specmin<<
                        ",specmax = "<<results[t][sat_num].target[targ].cover[0].specmax<<
                        ",azimuth = "<<results[t][sat_num].target[targ].cover[0].azimuth<<
                        ",elevation = "<<results[t][sat_num].target[targ].cover[0].elevation<<
                        endl;
				}
			}
		}
	}
*/
	vector<size_t> sat_nums;
	//sat_nums.push_back(0);
	sat_nums.push_back(1);
	sat_nums.push_back(2);
	sat_nums.push_back(3);
	sat_nums.push_back(4);

	vector<size_t> target_nums;
	for(size_t target_num = 0; target_num < 35; ++target_num)	{
		target_nums.push_back(target_num);
	}

// RESULTS
	// Extract the substring after the last slash
	size_t last_slash_pos = satfile.find_last_of('/');
	std::string filename = (last_slash_pos == std::string::npos) ? satfile : satfile.substr(last_slash_pos + 1);
	cout<<filename<<", ";
	cout<<extract_results_total_area(results, sat_nums, target_nums)<<", ";
	cout<<extract_results_average_percent(results, sat_nums, target_nums)<<", ";
	cout<<extract_results_min_resolution(results, sat_nums, target_nums)<<endl;

	extract_results(results, sat_nums, target_nums, "/home/user/cosmos/source/core/python/plot_orbit/sttr/result_dat.csv");
	//cout<<"number of targets observed = "<<number_of_targets_observed(results, sat_nums, target_nums)<<endl;
	vector<size_t> target_indices = indices_of_targets_observed(results, sat_nums, target_nums);
	for(size_t target_num : target_indices)	{
		vector<size_t> single_target(1, target_num);
		//cout<<"target #"<<target_num<<" observed."<<endl;
	}
	for(size_t s : sat_nums)	{
		vector<size_t> single_sat(1, s);
		//cout<<"sat #"<<s<<" observed "<<number_of_targets_observed(results, single_sat, target_nums)<<" targets."<<endl;
	}
	return 0;
}

int32_t parse_control(string args)
{
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
    //    if (!jargs["thrustctl"].is_null())
    //    {
    //        ++argcount;
    //        thrustctl = jargs["thrustctl"].number_value();
    //    }
    if (!jargs["printevent"].is_null())
    {
        ++argcount;
        printevent = jargs["printevent"].number_value();
    }
    if (!jargs["postevent"].is_null())
    {
        ++argcount;
        postevent = jargs["postevent"].number_value();
    }
    if (!jargs["runcount"].is_null())
    {
        ++argcount;
        runcount = jargs["runcount"].number_value();
    }
    if (!jargs["realtime"].is_null())
    {
        ++argcount;
        realtime = jargs["realtime"].number_value();
    }
    if (!jargs["initialutc"].is_null())
    {
        ++argcount;
        initialutc = jargs["initialutc"].number_value();
    }
    else
    {
        initialutc = currentmjd();
    }
    if (!jargs["endutc"].is_null())
    {
        ++argcount;
        endutc = jargs["endutc"].number_value();
    }
    if (!jargs["deltautc"].is_null())
    {
        ++argcount;
        deltautc = jargs["deltautc"].number_value();
    }
    if (!jargs["simdt"].is_null())
    {
        ++argcount;
        simdt = jargs["simdt"].number_value();
    }
    //    if (!jargs["minaccelratio"].is_null())
    //    {
    //        ++argcount;
    //        minaccelratio = jargs["minaccelratio"].number_value();
    //    }
    if (!jargs["satfile"].is_null())
    {
        ++argcount;
        satfile = jargs["satfile"].string_value();
    }
    if (!jargs["orbitfile"].is_null())
    {
        ++argcount;
        orbitfile = jargs["orbitfile"].string_value();
    }
    if (!jargs["targetfile"].is_null())
    {
        ++argcount;
        targetfile = jargs["targetfile"].string_value();
    }
    if (!jargs["realmname"].is_null())
    {
        ++argcount;
        realmname = jargs["realmname"].string_value();
    }
    if (!jargs["cosmos_web_addr"].is_null())
    {
        ++argcount;
        cosmos_web_addr = jargs["cosmos_web_addr"].string_value();
    }
    if (!jargs["fov_override"].is_null())
    {
        ++argcount;
        fov_override = jargs["fov_override"].number_value();
    }
    if (!jargs["ifov_override"].is_null())
    {
        ++argcount;
        ifov_override = jargs["ifov_override"].number_value();
    }
    return argcount;
}

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

int32_t request_get_location_node(string &request, string &response, Agent *agent)
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

int32_t request_get_offsetutc(string &request, string &response, Agent *agent)
{
    response = to_floating(sim->offsetutc, 8);
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
            string jstring = request.substr(request.find("{"));
            json11::Json jargs = json11::Json::parse(jstring, estring);
            if (estring.empty())
            {
                (*sit)->currentinfo.node.phys.fpush.from_json(jstring);
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

int32_t request_dump_node(string &request, string &response, Agent *agent)
{
    response.clear();
    vector<string> args = string_split(request);
    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            json_dump_node(&(*sit)->currentinfo);
            response = args[1];
        }
    }
    return response.length();
}

int32_t open_cosmos_web_sockets(string cosmos_web_addr)
{
    // No telems are to be emitted if no address was specified
    if (cosmos_web_addr.empty())
    {
        return COSMOS_GENERAL_ERROR_NOTSTARTED;
    }
    string ipv4_address = cosmos_web_addr;
    // If no . or : are found, then it may be a hostname instead, in which case convert to an ipv4 address
    if (cosmos_web_addr.find(".") == std::string::npos && cosmos_web_addr.find(":") == std::string::npos)
    {
        string response;
        int32_t iretn = hostnameToIP(cosmos_web_addr, ipv4_address, response);
        if (iretn < 0)
        {
            cout << "Encountered error in hostnameToIP: " << response << endl;
            exit(0);
        }
    }

    int32_t iretn = socket_open(&cosmos_web_telegraf_channel, NetworkType::UDP, ipv4_address.c_str(), TELEGRAF_PORT, SOCKET_TALK, SOCKET_BLOCKING, 2000000 );
    if ((iretn) < 0)
    {
        cout << "Failed to open socket cosmos_web_telegraf_channel: " << cosmos_error_string(iretn) << endl;
        exit(0);
    }
    iretn = socket_open(&cosmos_web_api_channel, NetworkType::UDP, ipv4_address.c_str(), API_PORT, SOCKET_TALK, SOCKET_BLOCKING, 2000000);
    if ((iretn) < 0)
    {
        cout << "Failed to open socket cosmos_web_api_channel: " << cosmos_error_string(iretn) << endl;
        exit(0);
    }
    sockets_initialized = true;
    return iretn;
}

/**
 * @brief Sends a single node's telems to cosmos web
 * 
 * @param cinfo cosmosstruc of some node to store telems for
 * @return int32_t 0 on success, negative on failure
 */
int32_t send_telem_to_cosmos_web(cosmosstruc* cinfo)
{
    if (!sockets_initialized)
    {
        return COSMOS_GENERAL_ERROR_NOTSTARTED;
    }
    // locstruc
    json11::Json jobj = json11::Json::object({
                                              {"node_name", cinfo->node.name },
                                              {"node_loc", json11::Json::object({
                                                               {"pos", json11::Json::object({
                                                                           {"eci", json11::Json::object({
                                                                                       { "utc", cinfo->node.loc.pos.eci.utc },
                                                                                       { "s", cinfo->node.loc.pos.eci.s },
                                                                                       { "v", cinfo->node.loc.pos.eci.v }
                                                                                   })}
                                                                       })},
                                                               {"att", json11::Json::object({
                                                                           {"icrf", json11::Json::object({
                                                                                        { "utc", cinfo->node.loc.att.icrf.utc },
                                                                                        { "s", cinfo->node.loc.att.icrf.s },
                                                                                        { "v", cinfo->node.loc.att.icrf.v }
                                                                                    })}
                                                                       })}
                                                           })},
                                              });
    int32_t iretn = socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    if (iretn < 0) { return iretn; }

    return 0;
}

/**
 * @brief Reset the simulation database
 */
void reset_db(Physics::Simulator* sim)
{
    if (!sockets_initialized)
    {
        return;
    }
    socket_sendto(cosmos_web_api_channel, "{\"swchstruc\": true, \"battstruc\": true, \"bcregstruc\": true, \"cpustruc\": true, \"device\": true, \"device_type\": true, \"locstruc\": true, \"magstruc\": true, \"node\": true, \"tsenstruc\": true, \"rwstruc\": true, \"mtrstruc\": true, \"attstruc_icrf\": true, \"cosmos_event\": true, \"event_type\": true, \"gyrostruc\": true, \"locstruc_eci\": true, \"target\": true, \"cosmos_event\": true }");
    // Iterate over sats
    uint16_t node_id = 1;
    for (auto sit = sim->cnodes.begin(); sit != sim->cnodes.end(); sit++)
    {
        // Compute node id manually until it's stored somewhere in cosmosstruc
        uint16_t id = node_id;
        (*sit)->currentinfo.node.name == "mother" ? id = 0 : ++node_id;

        // Repopulate node table
        json11::Json jobj = json11::Json::object({
            {"node", json11::Json::object({
                         { "node_id", id },
                         { "node_name", (*sit)->currentinfo.node.name },
                         { "node_type", (*sit)->currentinfo.node.type },
                         { "agent_name", (*sit)->currentinfo.agent0.name },
                         { "utc", sim->currentutc },
                         { "utcstart", sim->initialutc }
                     })}
        });
        socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    }
    auto mother = sim->GetNode("mother");
    if (mother != sim->cnodes.end())
    {
        // Groundstations & targets
        json11::Json jobj = json11::Json::object({
            {"target", [mother]() -> vector<json11::Json>
                {
                    vector<json11::Json> ret;
                    uint16_t target_id = 0;
                    for (auto target : (*mother)->currentinfo.target) {
                        ret.push_back({
                            json11::Json::object({
                                { "id"  , target_id++ },
                                { "name", target.name },
                                { "type", target.type },
                                { "lat" , target.loc.pos.geod.s.lat },
                                { "lon" , target.loc.pos.geod.s.lon },
                                { "h" , target.loc.pos.geod.s.h },
                                { "area" , target.area }
                            })
                        });
                    }
                    return ret;
                }()
            }
        });
        socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    }
    // A bit silly, but reset requires some wait time at the moment 
    // cout << "Resetting db..." << endl;
    secondsleep(1.);
}
