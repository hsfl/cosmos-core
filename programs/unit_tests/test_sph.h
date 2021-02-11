#ifndef __TEST_SPH
#define __TEST_SPH

#include <math.h>

#include "../simulation/sph.h"
#include "gtest/gtest.h"

// Kernel function
TEST(sph_sim, kernel) {
	int type1 = 1;
	int type2 = 2;
	// type = 1, 0 <= s <= 1
	EXPECT_DOUBLE_EQ(kernel(0,0,type1), 0);	// nan
	EXPECT_DOUBLE_EQ(kernel(1,2,type1), 23/(256*M_PI));
	EXPECT_DOUBLE_EQ(kernel(1,1,type1), 1/(4*M_PI));
	// type = 1, 1 < s <= 2
	EXPECT_DOUBLE_EQ(kernel(3,2,type1), 1/(256*M_PI));
	// type = 1, s > 2
	EXPECT_DOUBLE_EQ(kernel(3,1,type1), 0);

	// type = 2, s <= 2
	EXPECT_DOUBLE_EQ(kernel(0,0,type2), 0);	// nan
	EXPECT_DOUBLE_EQ(kernel(1,1,type2), 15/(64*M_PI));
	EXPECT_DOUBLE_EQ(kernel(3,2,type2), 15/(2048*M_PI));
	EXPECT_DOUBLE_EQ(kernel(2,1,type2), 0);
	// type = 2, s > 2
	EXPECT_DOUBLE_EQ(kernel(3,1,type2), 0);
}

// Kernel gradient function
TEST(sph_sim, kernel_grad) {
	int type1 = 1;
	int type2 = 2;
	// type = 1, 0 <= s <= 1
	EXPECT_DOUBLE_EQ(kernel_grad(0,0,type1), 0);	// nan
	EXPECT_DOUBLE_EQ(kernel_grad(1,2,type1), -15/(256*M_PI));
	EXPECT_DOUBLE_EQ(kernel_grad(1,1,type1), -3/(4*M_PI));
	// type = 1, 1 < s <= 2
	EXPECT_DOUBLE_EQ(kernel_grad(3,2,type1), -3/(256*M_PI));
	// type = 1, s > 2
	EXPECT_DOUBLE_EQ(kernel_grad(3,1,type1), 0);

	// type = 2, s <= 2
	EXPECT_DOUBLE_EQ(kernel_grad(0,0,type2), 0);	// nan
	EXPECT_DOUBLE_EQ(kernel_grad(1,1,type2), -15/(32*M_PI));
	EXPECT_DOUBLE_EQ(kernel_grad(3,2,type2), -15/(1024*M_PI));
	EXPECT_DOUBLE_EQ(kernel_grad(2,1,type2), 0);
	// type = 2, s > 2
	EXPECT_DOUBLE_EQ(kernel_grad(3,1,type2), 0);
}

// SPH update state vector
TEST(sph_sim, update_state) {
	sph_sim SPH;

	// Update sph state with new state matrix, 2 vehicles = 2
	// Currently can't change the number of vehicles, since states is only created once at initialization.
	// To change, would have to change when init2d() is run in init(), among other things and considerations.
	vector<statestruct> newstate;
	newstate.resize(9);
	for(int i = 0; i < 9; ++i) {
		newstate[i].x_pos = i;
		newstate[i].y_pos = i;
	}
	json11::Json json_newstate = json11::Json::object { {"state", newstate } };
	int agent_id = 2;
	SPH.sph_update_state(json_newstate.dump(), agent_id);
	auto state = SPH.get_states();
	EXPECT_EQ(state.rows(),9 + SPH.get_nobs() + SPH.get_nrd());
	for(int i = 0; i < 9; ++i) {
		if(i == agent_id-1) {
			EXPECT_DOUBLE_EQ(state(i,0), -1.3333333309643272);
			EXPECT_DOUBLE_EQ(state(i,1), 2.3094010663786895);
		} else {
			EXPECT_DOUBLE_EQ(state(i,0), i);
			EXPECT_DOUBLE_EQ(state(i,1), i);
		}
	}



}


#endif