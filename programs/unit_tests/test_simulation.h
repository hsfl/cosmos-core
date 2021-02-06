#ifndef __TEST_SIMULATION
#define __TEST_SIMULATION

#include <math.h>
#include <vector>

#include "gtest/gtest.h"
#include "agent/agentclass.h"
#include "support/json11.hpp"
#include "../simulation/simulation.h"

static Agent* agent;

// (for a single agent)
// Make a request for the agent states
TEST(simulation, request_agent_states) {
	//agent = new Agent("world","controller",1.);
	
}

// HCL happens here
// ie: time coordinating the state vectors
TEST(simulation, hcl) {
	
}

// State is properly fed into SPH. Conversions, etc.
TEST(simulation, sph_interface) {
	
}

// SPH outputs correct values
TEST(simulation, sph_output) {
	
}


// (for the simulation)
// Sim agent role call
TEST(simulation, all_sim_agents_running) {
	agent = new Agent("world","controller",1.);
	simulation sim(agent);
	ASSERT_TRUE(sim.all_sim_agents_running());
	agent->shutdown();
}
// Initialize agents, for their initial time and initial states.
TEST(simulation, init_agents) {
	agent = new Agent("world","controller",1.);
	simulation sim(agent);
	sim.init_sim_agents();
	// vector of initial times
	std::vector<string> response = sim.send_req_to_all_agents("get_initial_time");
	std::vector<double> initial_times(response.size());
	std::transform(response.begin(), response.end(), initial_times.begin(), [](const std::string& val) {
    	return std::stod(val);
	});
	for(size_t i = 0; i < initial_times.size(); ++i) {
		ASSERT_DOUBLE_EQ(initial_times[i], sim.get_initial_time()+i*0.01);
		
	}

	// vector of states
	response = sim.send_req_to_all_agents("get_state_vector");
	std::vector<double> x = {-5,0,5, -5,0,5, -5,0,5};
	std::vector<double> y = { 5,5,5,  0,0,0, -5,-5,-5};
	for(size_t i = 0; i < response.size(); ++i) {
		// state json object
		string error;
		json11::Json parsed = json11::Json::parse(response[i],error);
		if(error.empty()) {
			if(!parsed["x_position"].is_null()) { ASSERT_DOUBLE_EQ(parsed["x_position"].number_value(), x[i]); }
			if(!parsed["y_position"].is_null()) { ASSERT_DOUBLE_EQ(parsed["y_position"].number_value(), y[i]); }
			if(!parsed["z_position"].is_null()) { ASSERT_DOUBLE_EQ(parsed["z_position"].number_value(), 0); }
			if(!parsed["x_velocity"].is_null()) { ASSERT_DOUBLE_EQ(parsed["x_velocity"].number_value(), 0); }
			if(!parsed["y_velocity"].is_null()) { ASSERT_DOUBLE_EQ(parsed["y_velocity"].number_value(), 0); }
			if(!parsed["z_velocity"].is_null()) { ASSERT_DOUBLE_EQ(parsed["z_velocity"].number_value(), 0); }
		} else {
			std::cerr << "State vector json object from agent " << to_string(i+1) << "was empty."  << endl;
		}
	}
	agent->shutdown();
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


#endif