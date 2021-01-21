#ifndef __TEST_SPH_SIM
#define __TEST_SPH_SIM

#include "../simulation/sph_sim.h"
#include "gtest/gtest.h"

TEST(sph_sim, ObviouslyTrue)   { EXPECT_EQ(0.0, 0.0); }

// Kernel function
TEST(sph_sim, kernel) {
	double r = 1;
	double h = 1;
	int type = 1;
	EXPECT_DOUBLE_EQ(kernel(r,h,type),1.0);
}


#endif