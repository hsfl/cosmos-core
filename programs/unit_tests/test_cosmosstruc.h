#ifndef __TEST_COSMOSSTRUC
#define __TEST_COSMOSSTRUC

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "gtest/gtest.h"

int run_all_tests(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(SanityCheck, ObviouslyTrue)   { EXPECT_EQ(0.0, 0.0); }
TEST(cosmosstruc, allocation)   {
	//cosmosstruc* c = new cosmosstruc();	
	cosmosstruc* c = json_init();	
	cout<<"cinfo = <"<<c<<">"<<endl;
	EXPECT_NE(c, nullptr);
}


#endif
