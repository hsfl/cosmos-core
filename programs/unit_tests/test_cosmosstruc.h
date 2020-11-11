#ifndef __TEST_COSMOSSTRUC
#define __TEST_COSMOSSTRUC

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "gtest/gtest.h"

//int run_all_tests(int argc, char** argv) {
    //testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
//}

//TEST(SanityCheck, ObviouslyTrue)   { EXPECT_EQ(0.0, 0.0); }

TEST(cosmosstruc, allocation)   {
	//cosmosstruc* c = new cosmosstruc();	
	cosmosstruc* c = json_init();	
	cout<<"\tcinfo = <"<<c<<">"<<endl;
	EXPECT_NE(c, nullptr);
}

TEST(cosmosstruc, init_utc)	{
	cosmosstruc* c = json_init();
	EXPECT_DOUBLE_EQ(c->node.loc.utc, 0.0);
}

TEST(cosmosstruc, get_value)	{
	cosmosstruc* c = json_init();
	c->add_name("UTC", &c->node.loc.utc);
	EXPECT_DOUBLE_EQ(c->get_value<double>("UTC"), 0.0);
}

TEST(cosmosstruc, set_value)	{
	cosmosstruc* c = json_init();
	c->add_name("UTC", &c->node.loc.utc);
	c->set_value<double>("UTC", 5.432);
	EXPECT_DOUBLE_EQ(c->get_value<double>("UTC"), 5.432);
}

#endif
