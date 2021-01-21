#ifndef __TEST_SPH_SIM
#define __TEST_SPH_SIM

#include <math.h>

#include "../simulation/sph_sim.h"
#include "gtest/gtest.h"

// Kernel function
TEST(sph_sim, kernel) {
	int type1 = 1;
	int type2 = 2;
	EXPECT_DOUBLE_EQ(kernel(1,2,type1),23/(256*M_PI));
	EXPECT_DOUBLE_EQ(kernel(1,1,type1),1/(4*M_PI));

}


#endif