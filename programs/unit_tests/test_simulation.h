#ifndef __TEST_SIMULATION
#define __TEST_SIMULATION

#include <math.h>
#include <vector>

#include "gtest/gtest.h"
#include "agent/agentclass.h"
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
// Start agents at not really random substeps of dt
TEST(simulation, start_agents) {
	agent = new Agent("world","controller",1.);
	simulation sim(agent);
	sim.init_sim_agents();
	std::vector<string> response = sim.send_req_to_all_agents("get_initial_time");
	std::vector<double> initial_times(response.size());
	std::transform(response.begin(), response.end(), initial_times.begin(), [](const std::string& val) {
    	return std::stod(val);
	});
	for(size_t i = 0; i < initial_times.size(); ++i) {
		ASSERT_DOUBLE_EQ(initial_times[i], 0);
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