#ifndef __TEST_SIMULATION
#define __TEST_SIMULATION

#include <math.h>
#include <vector>

#include "gtest/gtest.h"
#include "agent/agentclass.h"
#include "support/json11.hpp"
#include "../simulation/simulation.h"

static Agent* agent;

TEST(simulation, create_new_agent) {
	agent = new Agent("world","controller",1.);
}
/*
// (for a single agent)
// Make a request for the agent states
TEST(simulation, request_agent_states) {
	simulation sim(agent);
	sim.init_sim_agents();
	string response;
	agent->send_request(agent->find_agent("world", "controller", 2.), "get_state_vectors", response, 2.);
	std::vector<double> x = {-5,0,5, -5,0,5, -5,0,5};
	std::vector<double> y = { 5,5,5,  0,0,0, -5,-5,-5};
	for(size_t i = 0; i < response.size(); ++i) {
		// state json object
		string error;
		json11::Json parsed = json11::Json::parse(response,error);
		if(error.empty()) {
			if(!parsed["x_position"].is_null()) { EXPECT_DOUBLE_EQ(parsed["x_position"].number_value(), x[i]); }
			if(!parsed["y_position"].is_null()) { EXPECT_DOUBLE_EQ(parsed["y_position"].number_value(), y[i]); }
			if(!parsed["z_position"].is_null()) { EXPECT_DOUBLE_EQ(parsed["z_position"].number_value(), 0); }
			if(!parsed["x_velocity"].is_null()) { EXPECT_DOUBLE_EQ(parsed["x_velocity"].number_value(), 0); }
			if(!parsed["y_velocity"].is_null()) { EXPECT_DOUBLE_EQ(parsed["y_velocity"].number_value(), 0); }
			if(!parsed["z_velocity"].is_null()) { EXPECT_DOUBLE_EQ(parsed["z_velocity"].number_value(), 0); }
			if(!parsed["timestamp"].is_null()) { EXPECT_DOUBLE_EQ(parsed["timestamp"].number_value(), sim.get_initial_time()+i*0.01); }
		} else {
			std::cerr << "State vector json object from agent " << to_string(i) << "was empty."  << endl;
		}
	}
}

// HCL happens here
// ie: time coordinating the state vectors
TEST(simulation, hcl) {
	simulation sim(agent);
	sim.init_sim_agents();
}

// State is properly fed into SPH. Conversions, etc.
TEST(simulation, sph_interface) {
	
}

// Send sph results back to world controller
TEST(simulation, send_world_new_state) {
	simulation sim(agent);
	sim.init_sim_agents();
	vector<statestruct> newstate;
	int agent_id = 1;
	newstate.resize(9);
	newstate[agent_id].x_pos = 98;
	newstate[agent_id].y_pos = 99;
	newstate[agent_id].agent_id = agent_id;
	json11::Json json_newstate = json11::Json::object { {"state", newstate } };
	string request = "send_world_new_state " + json_newstate.dump();
	string response = "";
	send_world_new_state(request, response, agent);
	EXPECT_DOUBLE_EQ(agent->cinfo->get_value<double>("state[" + to_string(agent_id) + "].x_position"), 98);
	EXPECT_DOUBLE_EQ(agent->cinfo->get_value<double>("state[" + to_string(agent_id) + "].y_position"), 99);
	EXPECT_EQ(agent->cinfo->get_value<int>("state[" + to_string(agent_id) + "].agent_id"), agent_id);
}

// SPH outputs correct values
TEST(simulation, sph_output) {
	
}


// (for the simulation)
// Sim agent role call
TEST(simulation, all_sim_agents_running) {
	simulation sim(agent);
	EXPECT_TRUE(sim.all_sim_agents_running());
}

*/

// Initialize agents, for their initial time and initial states.
TEST(simulation, init_agents) {
	simulation sim(agent);
	sim.init_sim_agents();

	// vector of states
	std::vector<string> response = sim.send_req_to_all_agents("get_state_vector");
	std::vector<double> x = {-5,0,5, -5,0,5, -5,0,5};
	std::vector<double> y = { 5,5,5,  0,0,0, -5,-5,-5};
	for(size_t i = 0; i < response.size(); ++i) {
		// state json object
		string error;
		json11::Json parsed = json11::Json::parse(response[i],error);
		if(error.empty()) {
			if(!parsed["state"][i]["x_position"].is_null()) { EXPECT_DOUBLE_EQ(parsed["state"][i]["x_position"].number_value(), x[i]); }
			if(!parsed["state"][i]["y_position"].is_null()) { EXPECT_DOUBLE_EQ(parsed["state"][i]["y_position"].number_value(), y[i]); }
			if(!parsed["state"][i]["z_position"].is_null()) { EXPECT_DOUBLE_EQ(parsed["state"][i]["z_position"].number_value(), 0); }
			if(!parsed["state"][i]["x_velocity"].is_null()) { EXPECT_DOUBLE_EQ(parsed["state"][i]["x_velocity"].number_value(), 0); }
			if(!parsed["state"][i]["y_velocity"].is_null()) { EXPECT_DOUBLE_EQ(parsed["state"][i]["y_velocity"].number_value(), 0); }
			if(!parsed["state"][i]["z_velocity"].is_null()) { EXPECT_DOUBLE_EQ(parsed["state"][i]["z_velocity"].number_value(), 0); }
			if(!parsed["state"][i]["timestamp"].is_null()) { EXPECT_DOUBLE_EQ(parsed["state"][i]["timestamp"].number_value(), sim.get_initial_time()+i*0.01); }
			if(!parsed["state"][i]["agent_id"].is_null()) { EXPECT_EQ(parsed["state"][i]["agent_id"].int_value(), i); }
			cout << parsed.dump() << " " << i << endl;
		} else {
			std::cerr << "State vector json object from agent " << to_string(i) << " was empty."  << endl;
		}
	}
}

// Populate and calculate world history
TEST(simulation, pseudo_hcl) {
	
}

// Update states via simulation when not computing with SPH
TEST(simulation, update_world_state) {
	
}

// Plot at each substep of dt
TEST(simulation, plot_simulation) {
	
}

TEST(simulation, shutdown_agent) {
	agent->shutdown();
}


#endif

// Note to self:
// Due to the way agents work, test_simulation.h has to be the first one included in test_all.cpp. Otherwise happy segfault surprise.