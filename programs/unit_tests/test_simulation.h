#ifndef __TEST_SIMULATION
#define __TEST_SIMULATION

#include <math.h>

#include "gtest/gtest.h"
#include "agent/agentclass.h"
static Agent* agent;

// (for a single agent)
// Make a request for the agent states
TEST(simulation, request_agent_states) {
	agent = new Agent("world","controller",1.);
}

// HCL happens here
TEST(simulation, hcl) {
	
}

// State is properly fed into SPH. Conversions, etc.
TEST(simulation, sph_interface) {
	
}

// SPH outputs correct values
TEST(simulation, sph_output) {
	
}


// (for the simulation)
// Start agents at not really random substeps of dt
TEST(simulation, start_agents) {
	
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