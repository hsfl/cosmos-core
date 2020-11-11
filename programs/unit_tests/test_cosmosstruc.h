#ifndef __TEST_COSMOSSTRUC
#define __TEST_COSMOSSTRUC

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "gtest/gtest.h"

TEST(cosmosstruc, allocation)   {
	//cosmosstruc* c = new cosmosstruc();	
	cosmosstruc* c = json_init();	
	cout<<"\t\t\t\t\tcinfo = <"<<c<<">"<<endl;
	EXPECT_NE(c, nullptr);
}


// check every initialization value

// double timestamp
TEST(cosmosstruc, json_init_timestamp)	{

	cosmosstruc* c = json_init();
	double timestamp = c->timestamp;
	double current = currentmjd();
	cout<<"\t\t\t\t\ttimestamp = "<<timestamp<<"\tcurrent = "<<current<<endl;
	stringstream tmp;
	tmp << setprecision(6) << fixed << timestamp;
	timestamp = stod(tmp.str());
	tmp.str(string());
	tmp << setprecision(6) << fixed << current;
	current = stod(tmp.str());
	EXPECT_DOUBLE_EQ(timestamp, current);
}

// uint16_t jmapped
TEST(cosmosstruc, json_init_jmapped)	{
	cosmosstruc* c = json_init();
	// should equal total number of objects in the map
	EXPECT_EQ(c->jmapped, 0);
}

// skip jmap
// skip emap

// vector<vector<unitstruc> unit
TEST(cosmosstruc, json_init_unit)	{
	cosmosstruc* c = json_init();

	// check size of unit vector (i.e. number of basic units)
	EXPECT_EQ(c->unit.size(), JSON_UNIT_COUNT);

	// check there are 4 units of time (sec, min, hour, day, mjd)
	EXPECT_EQ(c->unit[JSON_UNIT_TIME].size(), 5);

	// check the settings of the day time element




	// check a random element
	

	EXPECT_DOUBLE_EQ(c->node.loc.utc, 0.0);
}








// check all get_value methods
TEST(cosmosstruc, get_value)	{
	cosmosstruc* c = json_init();
	c->add_name("UTC", &c->node.loc.utc);
	EXPECT_DOUBLE_EQ(c->get_value<double>("UTC"), 0.0);
}



// check all set_value methods
TEST(cosmosstruc, set_value)	{
	cosmosstruc* c = json_init();
	c->add_name("UTC", &c->node.loc.utc);
	c->set_value<double>("UTC", 5.432);
	EXPECT_DOUBLE_EQ(c->get_value<double>("UTC"), 5.432);
}

#endif
