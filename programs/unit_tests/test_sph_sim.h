#ifndef __TEST_SPH_SIM
#define __TEST_SPH_SIM

#include <math.h>

#include "../simulation/sph_sim.h"
#include "gtest/gtest.h"

// Kernel function
TEST(sph_sim, kernel) {
	int type1 = 1;
	int type2 = 2;
	// type = 1, 0 <= s <= 1
	EXPECT_EQ(kernel(0,0,type1), 0);	// nan
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


#endif