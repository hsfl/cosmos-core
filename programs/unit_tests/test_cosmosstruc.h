#ifndef __TEST_COSMOSSTRUC
#define __TEST_COSMOSSTRUC

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "gtest/gtest.h"

TEST(cosmosstruc, allocation)   {
	//cosmosstruc* c = new cosmosstruc();	
	cosmosstruc* c = json_init();	
	cout<<"\t\t\t\t\t\t\tcinfo = <"<<c<<">"<<endl;
	EXPECT_NE(c, nullptr);
}


// check every initialization value

// double timestamp
TEST(cosmosstruc, json_init_timestamp)	{

	cosmosstruc* c = json_init();
	double timestamp = c->timestamp;
	double current = currentmjd();
	cout<<"\t\t\t\t\t\t\ttimestamp = "<<setprecision(9)<<fixed<<timestamp<<"\tcurrent = "<<current<<endl;
	stringstream tmp;
	tmp << setprecision(6) << fixed << timestamp;
	timestamp = stod(tmp.str());
	tmp.str(string());
	tmp << setprecision(6) << fixed << current;
	current = stod(tmp.str());
	EXPECT_DOUBLE_EQ(timestamp, current);
}

// skip jmapped
// skip jmap
// skip emap

// vector<vector<unitstruc> unit
TEST(cosmosstruc, json_init_unit)	{
	cosmosstruc* c = json_init();

	// check size of unit vector (i.e. number of basic units)
	EXPECT_EQ(c->unit.size(), JSON_UNIT_COUNT);

	// check a random element

	// check there are 5 units of time (sec, min, hour, day, mjd)
	EXPECT_EQ(c->unit[JSON_UNIT_TIME].size(), 5);

	// check the settings of the 'day' time element
	EXPECT_EQ(c->unit[JSON_UNIT_TIME][3].name, "day");
	EXPECT_EQ(c->unit[JSON_UNIT_TIME][3].type, JSON_UNIT_TYPE_POLY);
	EXPECT_FLOAT_EQ(c->unit[JSON_UNIT_TIME][3].p0, 0.0);
	EXPECT_FLOAT_EQ(c->unit[JSON_UNIT_TIME][3].p1, 1.f/86400.f); 
	// this is actually prolly the wrong 2nd derivative ;)
	EXPECT_FLOAT_EQ(c->unit[JSON_UNIT_TIME][3].p2, 0.0);
}


// node info
TEST(cosmosstruc, json_init_node)	{
	cosmosstruc* c = json_init();

	EXPECT_EQ(string(c->node.name), "");
	EXPECT_EQ(c->node.vertex_cnt , 0);
    EXPECT_EQ(c->node.normal_cnt , 0);
    EXPECT_EQ(c->node.face_cnt , 0);
    EXPECT_EQ(c->node.piece_cnt , 0);
    EXPECT_EQ(c->node.device_cnt , 0);
    EXPECT_EQ(c->node.port_cnt , 0);
    EXPECT_EQ(c->node.agent_cnt , 0);
    EXPECT_EQ(c->node.event_cnt , 0);
    EXPECT_EQ(c->node.target_cnt , 0);
    EXPECT_EQ(c->node.user_cnt , 0);
    EXPECT_EQ(c->node.tle_cnt , 0);
}

TEST(cosmosstruc, json_init_json)	{
	cosmosstruc* c = json_init();
	EXPECT_EQ(c->json.name, "");
	EXPECT_EQ(c->json.node, "");
	EXPECT_EQ(c->json.state, "");
	EXPECT_EQ(c->json.utcstart, "");
	EXPECT_EQ(c->json.vertexs, "");
	EXPECT_EQ(c->json.faces, "");
	EXPECT_EQ(c->json.pieces, "");
	EXPECT_EQ(c->json.devspec, "");
	EXPECT_EQ(c->json.ports, "");
	EXPECT_EQ(c->json.targets, "");
}


// check all get_value methods
TEST(cosmosstruc, get_value)	{
	cosmosstruc* c = json_init();
	c->add_name("UTC", &c->node.loc.utc, "double");
	EXPECT_DOUBLE_EQ(c->get_value<double>("UTC"), 0.0);
}



// check all set_value methods
TEST(cosmosstruc, set_value)	{
	cosmosstruc* c = json_init();
	c->add_name("UTC", &c->node.loc.utc, "double");
	c->set_value<double>("UTC", 5.432);
	EXPECT_DOUBLE_EQ(c->get_value<double>("UTC"), 5.432);
}

// check all default names were added correctly
TEST(cosmosstruc, add_default_names) {
	cosmosstruc* c = json_init();
	c->add_default_names();

	// double timestamp
	EXPECT_EQ(c->get_pointer<double>("timestamp"), &c->timestamp);

	// uint16_t jmapped
	EXPECT_EQ(c->get_pointer<uint16_t>("jmapped"), &c->jmapped);

	// vector<vector<unitstruc>> unit
	EXPECT_EQ(c->get_pointer<vector<vector<unitstruc>>>("unit"), &c->unit);
	for(size_t i = 0; i < c->unit.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<vector<unitstruc>>("unit[" + std::to_string(i) + "]"), &c->unit[i]);
		for(size_t j = 0; j < c->unit[i].capacity(); ++j) {
			EXPECT_EQ(c->get_pointer<unitstruc>("unit[" + std::to_string(i) + "][" + std::to_string(j) + "]"), &c->unit[i][j]);
			EXPECT_EQ(c->get_pointer<string>("unit[" + std::to_string(i) + "][" + std::to_string(j) + "].name"), &c->unit[i][j].name);
			EXPECT_EQ(c->get_pointer<uint16_t>("unit[" + std::to_string(i) + "][" + std::to_string(j) + "].type"), &c->unit[i][j].type);
			EXPECT_EQ(c->get_pointer<float>("unit[" + std::to_string(i) + "][" + std::to_string(j) + "].p0"), &c->unit[i][j].p0);
			EXPECT_EQ(c->get_pointer<float>("unit[" + std::to_string(i) + "][" + std::to_string(j) + "].p1"), &c->unit[i][j].p1);
			EXPECT_EQ(c->get_pointer<float>("unit[" + std::to_string(i) + "][" + std::to_string(j) + "].p2"), &c->unit[i][j].p2);
		}
	}

	// vector<equationstruc> equation
	EXPECT_EQ(c->get_pointer<vector<equationstruc>>("equation"), &c->equation);
	for(size_t i = 0; i < c->equation.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<equationstruc>("equation[" + std::to_string(i) + "]"), &c->equation[i]);
		EXPECT_EQ(c->get_pointer<string>("equation[" + std::to_string(i) + "].name"), &c->equation[i].name);
		EXPECT_EQ(c->get_pointer<string>("equation[" + std::to_string(i) + "].value"), &c->equation[i].value);
	}

	// nodestruc node
	EXPECT_EQ(c->get_pointer<nodestruc>("node"), &c->node);
	//EXPECT_EQ(c->get_pointer<char[X]>("node.name"), &c->node.name);
	//EXPECT_EQ(c->get_pointer<char[X]>("node.lastevent"), &c->node.lastevent);
	EXPECT_EQ(c->get_pointer<double>("node.lasteventutc"), &c->node.lasteventutc);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.type"), &c->node.type);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.state"), &c->node.state);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.vertex_cnt"), &c->node.vertex_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.normal_cnt"), &c->node.normal_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.face_cnt"), &c->node.face_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.piece_cnt"), &c->node.piece_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.device_cnt"), &c->node.device_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.port_cnt"), &c->node.port_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.agent_cnt"), &c->node.agent_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.event_cnt"), &c->node.event_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.target_cnt"), &c->node.target_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.user_cnt"), &c->node.user_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.tle_cnt"), &c->node.tle_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.flags"), &c->node.flags);
	EXPECT_EQ(c->get_pointer<int16_t>("node.powmode"), &c->node.powmode);
	EXPECT_EQ(c->get_pointer<uint32_t>("node.downtime"), &c->node.downtime);
	EXPECT_EQ(c->get_pointer<float>("node.azfrom"), &c->node.azfrom);
	EXPECT_EQ(c->get_pointer<float>("node.elfrom"), &c->node.elfrom);
	EXPECT_EQ(c->get_pointer<float>("node.azto"), &c->node.azto);
	EXPECT_EQ(c->get_pointer<float>("node.elto"), &c->node.elto);
	EXPECT_EQ(c->get_pointer<float>("node.range"), &c->node.range);
	EXPECT_EQ(c->get_pointer<double>("node.utcoffset"), &c->node.utcoffset);
	EXPECT_EQ(c->get_pointer<double>("node.utc"), &c->node.utc);
	EXPECT_EQ(c->get_pointer<double>("node.utcstart"), &c->node.utcstart);
	EXPECT_EQ(c->get_pointer<locstruc>("node.loc"), &c->node.loc);
	EXPECT_EQ(c->get_pointer<double>("node.loc.utc"), &c->node.loc.utc);
	EXPECT_EQ(c->get_pointer<posstruc>("node.loc.pos"), &c->node.loc.pos);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.utc"), &c->node.loc.pos.utc);
	EXPECT_EQ(c->get_pointer<cartpos>("node.loc.pos.icrf"), &c->node.loc.pos.icrf);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.icrf.utc"), &c->node.loc.pos.icrf.utc);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.icrf.s"), &c->node.loc.pos.icrf.s);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.icrf.s.col"), &c->node.loc.pos.icrf.s.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.icrf.s.col)/sizeof(c->node.loc.pos.icrf.s.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.icrf.s.col[" + std::to_string(i) + "]"), &c->node.loc.pos.icrf.s.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.icrf.v"), &c->node.loc.pos.icrf.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.icrf.v.col"), &c->node.loc.pos.icrf.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.icrf.v.col)/sizeof(c->node.loc.pos.icrf.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.icrf.v.col[" + std::to_string(i) + "]"), &c->node.loc.pos.icrf.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.icrf.a"), &c->node.loc.pos.icrf.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.icrf.a.col"), &c->node.loc.pos.icrf.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.icrf.a.col)/sizeof(c->node.loc.pos.icrf.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.icrf.a.col[" + std::to_string(i) + "]"), &c->node.loc.pos.icrf.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.icrf.pass"), &c->node.loc.pos.icrf.pass);
	EXPECT_EQ(c->get_pointer<cartpos>("node.loc.pos.eci"), &c->node.loc.pos.eci);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.eci.utc"), &c->node.loc.pos.eci.utc);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.eci.s"), &c->node.loc.pos.eci.s);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.eci.s.col"), &c->node.loc.pos.eci.s.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.eci.s.col)/sizeof(c->node.loc.pos.eci.s.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.eci.s.col[" + std::to_string(i) + "]"), &c->node.loc.pos.eci.s.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.eci.v"), &c->node.loc.pos.eci.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.eci.v.col"), &c->node.loc.pos.eci.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.eci.v.col)/sizeof(c->node.loc.pos.eci.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.eci.v.col[" + std::to_string(i) + "]"), &c->node.loc.pos.eci.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.eci.a"), &c->node.loc.pos.eci.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.eci.a.col"), &c->node.loc.pos.eci.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.eci.a.col)/sizeof(c->node.loc.pos.eci.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.eci.a.col[" + std::to_string(i) + "]"), &c->node.loc.pos.eci.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.eci.pass"), &c->node.loc.pos.eci.pass);
	EXPECT_EQ(c->get_pointer<cartpos>("node.loc.pos.sci"), &c->node.loc.pos.sci);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.sci.utc"), &c->node.loc.pos.sci.utc);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.sci.s"), &c->node.loc.pos.sci.s);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.sci.s.col"), &c->node.loc.pos.sci.s.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.sci.s.col)/sizeof(c->node.loc.pos.sci.s.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.sci.s.col[" + std::to_string(i) + "]"), &c->node.loc.pos.sci.s.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.sci.v"), &c->node.loc.pos.sci.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.sci.v.col"), &c->node.loc.pos.sci.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.sci.v.col)/sizeof(c->node.loc.pos.sci.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.sci.v.col[" + std::to_string(i) + "]"), &c->node.loc.pos.sci.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.sci.a"), &c->node.loc.pos.sci.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.sci.a.col"), &c->node.loc.pos.sci.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.sci.a.col)/sizeof(c->node.loc.pos.sci.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.sci.a.col[" + std::to_string(i) + "]"), &c->node.loc.pos.sci.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.sci.pass"), &c->node.loc.pos.sci.pass);
	EXPECT_EQ(c->get_pointer<cartpos>("node.loc.pos.geoc"), &c->node.loc.pos.geoc);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geoc.utc"), &c->node.loc.pos.geoc.utc);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.geoc.s"), &c->node.loc.pos.geoc.s);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.geoc.s.col"), &c->node.loc.pos.geoc.s.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.geoc.s.col)/sizeof(c->node.loc.pos.geoc.s.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geoc.s.col[" + std::to_string(i) + "]"), &c->node.loc.pos.geoc.s.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.geoc.v"), &c->node.loc.pos.geoc.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.geoc.v.col"), &c->node.loc.pos.geoc.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.geoc.v.col)/sizeof(c->node.loc.pos.geoc.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geoc.v.col[" + std::to_string(i) + "]"), &c->node.loc.pos.geoc.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.geoc.a"), &c->node.loc.pos.geoc.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.geoc.a.col"), &c->node.loc.pos.geoc.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.geoc.a.col)/sizeof(c->node.loc.pos.geoc.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geoc.a.col[" + std::to_string(i) + "]"), &c->node.loc.pos.geoc.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.geoc.pass"), &c->node.loc.pos.geoc.pass);
	EXPECT_EQ(c->get_pointer<cartpos>("node.loc.pos.selc"), &c->node.loc.pos.selc);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selc.utc"), &c->node.loc.pos.selc.utc);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.selc.s"), &c->node.loc.pos.selc.s);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.selc.s.col"), &c->node.loc.pos.selc.s.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.selc.s.col)/sizeof(c->node.loc.pos.selc.s.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selc.s.col[" + std::to_string(i) + "]"), &c->node.loc.pos.selc.s.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.selc.v"), &c->node.loc.pos.selc.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.selc.v.col"), &c->node.loc.pos.selc.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.selc.v.col)/sizeof(c->node.loc.pos.selc.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selc.v.col[" + std::to_string(i) + "]"), &c->node.loc.pos.selc.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.selc.a"), &c->node.loc.pos.selc.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.selc.a.col"), &c->node.loc.pos.selc.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.selc.a.col)/sizeof(c->node.loc.pos.selc.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selc.a.col[" + std::to_string(i) + "]"), &c->node.loc.pos.selc.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.selc.pass"), &c->node.loc.pos.selc.pass);
	EXPECT_EQ(c->get_pointer<geoidpos>("node.loc.pos.geod"), &c->node.loc.pos.geod);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.utc"), &c->node.loc.pos.geod.utc);
	EXPECT_EQ(c->get_pointer<gvector>("node.loc.pos.geod.s"), &c->node.loc.pos.geod.s);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.s.lat"), &c->node.loc.pos.geod.s.lat);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.s.lon"), &c->node.loc.pos.geod.s.lon);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.s.h"), &c->node.loc.pos.geod.s.h);
	EXPECT_EQ(c->get_pointer<gvector>("node.loc.pos.geod.v"), &c->node.loc.pos.geod.v);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.v.lat"), &c->node.loc.pos.geod.v.lat);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.v.lon"), &c->node.loc.pos.geod.v.lon);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.v.h"), &c->node.loc.pos.geod.v.h);
	EXPECT_EQ(c->get_pointer<gvector>("node.loc.pos.geod.a"), &c->node.loc.pos.geod.a);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.a.lat"), &c->node.loc.pos.geod.a.lat);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.a.lon"), &c->node.loc.pos.geod.a.lon);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geod.a.h"), &c->node.loc.pos.geod.a.h);
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.geod.pass"), &c->node.loc.pos.geod.pass);
	EXPECT_EQ(c->get_pointer<geoidpos>("node.loc.pos.selg"), &c->node.loc.pos.selg);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.utc"), &c->node.loc.pos.selg.utc);
	EXPECT_EQ(c->get_pointer<gvector>("node.loc.pos.selg.s"), &c->node.loc.pos.selg.s);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.s.lat"), &c->node.loc.pos.selg.s.lat);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.s.lon"), &c->node.loc.pos.selg.s.lon);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.s.h"), &c->node.loc.pos.selg.s.h);
	EXPECT_EQ(c->get_pointer<gvector>("node.loc.pos.selg.v"), &c->node.loc.pos.selg.v);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.v.lat"), &c->node.loc.pos.selg.v.lat);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.v.lon"), &c->node.loc.pos.selg.v.lon);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.v.h"), &c->node.loc.pos.selg.v.h);
	EXPECT_EQ(c->get_pointer<gvector>("node.loc.pos.selg.a"), &c->node.loc.pos.selg.a);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.a.lat"), &c->node.loc.pos.selg.a.lat);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.a.lon"), &c->node.loc.pos.selg.a.lon);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.selg.a.h"), &c->node.loc.pos.selg.a.h);
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.selg.pass"), &c->node.loc.pos.selg.pass);
	EXPECT_EQ(c->get_pointer<spherpos>("node.loc.pos.geos"), &c->node.loc.pos.geos);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.utc"), &c->node.loc.pos.geos.utc);
	EXPECT_EQ(c->get_pointer<svector>("node.loc.pos.geos.s"), &c->node.loc.pos.geos.s);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.s.phi"), &c->node.loc.pos.geos.s.phi);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.s.lambda"), &c->node.loc.pos.geos.s.lambda);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.s.r"), &c->node.loc.pos.geos.s.r);
	EXPECT_EQ(c->get_pointer<svector>("node.loc.pos.geos.v"), &c->node.loc.pos.geos.v);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.v.phi"), &c->node.loc.pos.geos.v.phi);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.v.lambda"), &c->node.loc.pos.geos.v.lambda);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.v.r"), &c->node.loc.pos.geos.v.r);
	EXPECT_EQ(c->get_pointer<svector>("node.loc.pos.geos.a"), &c->node.loc.pos.geos.a);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.a.phi"), &c->node.loc.pos.geos.a.phi);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.a.lambda"), &c->node.loc.pos.geos.a.lambda);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.geos.a.r"), &c->node.loc.pos.geos.a.r);
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.geos.pass"), &c->node.loc.pos.geos.pass);
	EXPECT_EQ(c->get_pointer<extrapos>("node.loc.pos.extra"), &c->node.loc.pos.extra);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.utc"), &c->node.loc.pos.extra.utc);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.tt"), &c->node.loc.pos.extra.tt);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.ut"), &c->node.loc.pos.extra.ut);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.tdb"), &c->node.loc.pos.extra.tdb);
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.j2e"), &c->node.loc.pos.extra.j2e);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.j2e.row"), &c->node.loc.pos.extra.j2e.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.j2e.row)/sizeof(c->node.loc.pos.extra.j2e.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.j2e.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.j2e.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.j2e.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.j2e.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.j2e.row[i].col)/sizeof(c->node.loc.pos.extra.j2e.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.j2e.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.j2e.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.dj2e"), &c->node.loc.pos.extra.dj2e);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.dj2e.row"), &c->node.loc.pos.extra.dj2e.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.dj2e.row)/sizeof(c->node.loc.pos.extra.dj2e.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.dj2e.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.dj2e.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.dj2e.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.dj2e.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.dj2e.row[i].col)/sizeof(c->node.loc.pos.extra.dj2e.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.dj2e.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.dj2e.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.ddj2e"), &c->node.loc.pos.extra.ddj2e);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.ddj2e.row"), &c->node.loc.pos.extra.ddj2e.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.ddj2e.row)/sizeof(c->node.loc.pos.extra.ddj2e.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.ddj2e.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.ddj2e.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.ddj2e.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.ddj2e.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.ddj2e.row[i].col)/sizeof(c->node.loc.pos.extra.ddj2e.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.ddj2e.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.ddj2e.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.e2j"), &c->node.loc.pos.extra.e2j);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.e2j.row"), &c->node.loc.pos.extra.e2j.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.e2j.row)/sizeof(c->node.loc.pos.extra.e2j.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.e2j.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.e2j.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.e2j.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.e2j.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.e2j.row[i].col)/sizeof(c->node.loc.pos.extra.e2j.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.e2j.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.e2j.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.de2j"), &c->node.loc.pos.extra.de2j);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.de2j.row"), &c->node.loc.pos.extra.de2j.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.de2j.row)/sizeof(c->node.loc.pos.extra.de2j.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.de2j.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.de2j.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.de2j.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.de2j.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.de2j.row[i].col)/sizeof(c->node.loc.pos.extra.de2j.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.de2j.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.de2j.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.dde2j"), &c->node.loc.pos.extra.dde2j);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.dde2j.row"), &c->node.loc.pos.extra.dde2j.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.dde2j.row)/sizeof(c->node.loc.pos.extra.dde2j.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.dde2j.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.dde2j.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.dde2j.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.dde2j.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.dde2j.row[i].col)/sizeof(c->node.loc.pos.extra.dde2j.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.dde2j.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.dde2j.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.j2t"), &c->node.loc.pos.extra.j2t);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.j2t.row"), &c->node.loc.pos.extra.j2t.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.j2t.row)/sizeof(c->node.loc.pos.extra.j2t.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.j2t.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.j2t.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.j2t.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.j2t.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.j2t.row[i].col)/sizeof(c->node.loc.pos.extra.j2t.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.j2t.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.j2t.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.j2s"), &c->node.loc.pos.extra.j2s);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.j2s.row"), &c->node.loc.pos.extra.j2s.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.j2s.row)/sizeof(c->node.loc.pos.extra.j2s.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.j2s.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.j2s.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.j2s.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.j2s.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.j2s.row[i].col)/sizeof(c->node.loc.pos.extra.j2s.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.j2s.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.j2s.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.t2j"), &c->node.loc.pos.extra.t2j);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.t2j.row"), &c->node.loc.pos.extra.t2j.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.t2j.row)/sizeof(c->node.loc.pos.extra.t2j.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.t2j.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.t2j.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.t2j.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.t2j.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.t2j.row[i].col)/sizeof(c->node.loc.pos.extra.t2j.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.t2j.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.t2j.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.s2j"), &c->node.loc.pos.extra.s2j);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.s2j.row"), &c->node.loc.pos.extra.s2j.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.s2j.row)/sizeof(c->node.loc.pos.extra.s2j.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.s2j.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.s2j.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.s2j.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.s2j.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.s2j.row[i].col)/sizeof(c->node.loc.pos.extra.s2j.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.s2j.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.s2j.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.s2t"), &c->node.loc.pos.extra.s2t);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.s2t.row"), &c->node.loc.pos.extra.s2t.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.s2t.row)/sizeof(c->node.loc.pos.extra.s2t.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.s2t.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.s2t.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.s2t.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.s2t.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.s2t.row[i].col)/sizeof(c->node.loc.pos.extra.s2t.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.s2t.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.s2t.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.ds2t"), &c->node.loc.pos.extra.ds2t);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.ds2t.row"), &c->node.loc.pos.extra.ds2t.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.ds2t.row)/sizeof(c->node.loc.pos.extra.ds2t.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.ds2t.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.ds2t.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.ds2t.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.ds2t.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.ds2t.row[i].col)/sizeof(c->node.loc.pos.extra.ds2t.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.ds2t.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.ds2t.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.t2s"), &c->node.loc.pos.extra.t2s);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.t2s.row"), &c->node.loc.pos.extra.t2s.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.t2s.row)/sizeof(c->node.loc.pos.extra.t2s.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.t2s.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.t2s.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.t2s.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.t2s.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.t2s.row[i].col)/sizeof(c->node.loc.pos.extra.t2s.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.t2s.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.t2s.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.pos.extra.dt2s"), &c->node.loc.pos.extra.dt2s);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.pos.extra.dt2s.row"), &c->node.loc.pos.extra.dt2s.row);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.dt2s.row)/sizeof(c->node.loc.pos.extra.dt2s.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.dt2s.row[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.dt2s.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.dt2s.row[" + std::to_string(i) + "].col"), &c->node.loc.pos.extra.dt2s.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.pos.extra.dt2s.row[i].col)/sizeof(c->node.loc.pos.extra.dt2s.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.dt2s.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.pos.extra.dt2s.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<cartpos>("node.loc.pos.extra.sun2earth"), &c->node.loc.pos.extra.sun2earth);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2earth.utc"), &c->node.loc.pos.extra.sun2earth.utc);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.sun2earth.s"), &c->node.loc.pos.extra.sun2earth.s);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.sun2earth.s.col"), &c->node.loc.pos.extra.sun2earth.s.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.sun2earth.s.col)/sizeof(c->node.loc.pos.extra.sun2earth.s.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2earth.s.col[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.sun2earth.s.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.sun2earth.v"), &c->node.loc.pos.extra.sun2earth.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.sun2earth.v.col"), &c->node.loc.pos.extra.sun2earth.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.sun2earth.v.col)/sizeof(c->node.loc.pos.extra.sun2earth.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2earth.v.col[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.sun2earth.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.sun2earth.a"), &c->node.loc.pos.extra.sun2earth.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.sun2earth.a.col"), &c->node.loc.pos.extra.sun2earth.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.sun2earth.a.col)/sizeof(c->node.loc.pos.extra.sun2earth.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2earth.a.col[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.sun2earth.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.extra.sun2earth.pass"), &c->node.loc.pos.extra.sun2earth.pass);
	EXPECT_EQ(c->get_pointer<cartpos>("node.loc.pos.extra.sun2moon"), &c->node.loc.pos.extra.sun2moon);
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2moon.utc"), &c->node.loc.pos.extra.sun2moon.utc);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.sun2moon.s"), &c->node.loc.pos.extra.sun2moon.s);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.sun2moon.s.col"), &c->node.loc.pos.extra.sun2moon.s.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.sun2moon.s.col)/sizeof(c->node.loc.pos.extra.sun2moon.s.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2moon.s.col[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.sun2moon.s.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.sun2moon.v"), &c->node.loc.pos.extra.sun2moon.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.sun2moon.v.col"), &c->node.loc.pos.extra.sun2moon.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.sun2moon.v.col)/sizeof(c->node.loc.pos.extra.sun2moon.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2moon.v.col[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.sun2moon.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.extra.sun2moon.a"), &c->node.loc.pos.extra.sun2moon.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.extra.sun2moon.a.col"), &c->node.loc.pos.extra.sun2moon.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.extra.sun2moon.a.col)/sizeof(c->node.loc.pos.extra.sun2moon.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.extra.sun2moon.a.col[" + std::to_string(i) + "]"), &c->node.loc.pos.extra.sun2moon.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.pos.extra.sun2moon.pass"), &c->node.loc.pos.extra.sun2moon.pass);
	EXPECT_EQ(c->get_pointer<uint16_t>("node.loc.pos.extra.closest"), &c->node.loc.pos.extra.closest);
	EXPECT_EQ(c->get_pointer<float>("node.loc.pos.earthsep"), &c->node.loc.pos.earthsep);
	EXPECT_EQ(c->get_pointer<float>("node.loc.pos.moonsep"), &c->node.loc.pos.moonsep);
	EXPECT_EQ(c->get_pointer<float>("node.loc.pos.sunsize"), &c->node.loc.pos.sunsize);
	EXPECT_EQ(c->get_pointer<float>("node.loc.pos.sunradiance"), &c->node.loc.pos.sunradiance);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.pos.bearth"), &c->node.loc.pos.bearth);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.pos.bearth.col"), &c->node.loc.pos.bearth.col);
	for(size_t i = 0; i < sizeof(c->node.loc.pos.bearth.col)/sizeof(c->node.loc.pos.bearth.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.pos.bearth.col[" + std::to_string(i) + "]"), &c->node.loc.pos.bearth.col[i]);
	}
	EXPECT_EQ(c->get_pointer<double>("node.loc.pos.orbit"), &c->node.loc.pos.orbit);
	EXPECT_EQ(c->get_pointer<attstruc>("node.loc.att"), &c->node.loc.att);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.utc"), &c->node.loc.att.utc);
	EXPECT_EQ(c->get_pointer<qatt>("node.loc.att.topo"), &c->node.loc.att.topo);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.topo.utc"), &c->node.loc.att.topo.utc);
	EXPECT_EQ(c->get_pointer<quaternion>("node.loc.att.topo.s"), &c->node.loc.att.topo.s);
	EXPECT_EQ(c->get_pointer<cvector>("node.loc.att.topo.s.d"), &c->node.loc.att.topo.s.d);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.topo.s.d.x"), &c->node.loc.att.topo.s.d.x);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.topo.s.d.y"), &c->node.loc.att.topo.s.d.y);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.topo.s.d.z"), &c->node.loc.att.topo.s.d.z);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.topo.s.w"), &c->node.loc.att.topo.s.w);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.topo.v"), &c->node.loc.att.topo.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.topo.v.col"), &c->node.loc.att.topo.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.topo.v.col)/sizeof(c->node.loc.att.topo.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.topo.v.col[" + std::to_string(i) + "]"), &c->node.loc.att.topo.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.topo.a"), &c->node.loc.att.topo.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.topo.a.col"), &c->node.loc.att.topo.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.topo.a.col)/sizeof(c->node.loc.att.topo.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.topo.a.col[" + std::to_string(i) + "]"), &c->node.loc.att.topo.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.att.topo.pass"), &c->node.loc.att.topo.pass);
	EXPECT_EQ(c->get_pointer<qatt>("node.loc.att.lvlh"), &c->node.loc.att.lvlh);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.lvlh.utc"), &c->node.loc.att.lvlh.utc);
	EXPECT_EQ(c->get_pointer<quaternion>("node.loc.att.lvlh.s"), &c->node.loc.att.lvlh.s);
	EXPECT_EQ(c->get_pointer<cvector>("node.loc.att.lvlh.s.d"), &c->node.loc.att.lvlh.s.d);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.lvlh.s.d.x"), &c->node.loc.att.lvlh.s.d.x);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.lvlh.s.d.y"), &c->node.loc.att.lvlh.s.d.y);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.lvlh.s.d.z"), &c->node.loc.att.lvlh.s.d.z);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.lvlh.s.w"), &c->node.loc.att.lvlh.s.w);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.lvlh.v"), &c->node.loc.att.lvlh.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.lvlh.v.col"), &c->node.loc.att.lvlh.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.lvlh.v.col)/sizeof(c->node.loc.att.lvlh.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.lvlh.v.col[" + std::to_string(i) + "]"), &c->node.loc.att.lvlh.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.lvlh.a"), &c->node.loc.att.lvlh.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.lvlh.a.col"), &c->node.loc.att.lvlh.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.lvlh.a.col)/sizeof(c->node.loc.att.lvlh.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.lvlh.a.col[" + std::to_string(i) + "]"), &c->node.loc.att.lvlh.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.att.lvlh.pass"), &c->node.loc.att.lvlh.pass);
	EXPECT_EQ(c->get_pointer<qatt>("node.loc.att.geoc"), &c->node.loc.att.geoc);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.geoc.utc"), &c->node.loc.att.geoc.utc);
	EXPECT_EQ(c->get_pointer<quaternion>("node.loc.att.geoc.s"), &c->node.loc.att.geoc.s);
	EXPECT_EQ(c->get_pointer<cvector>("node.loc.att.geoc.s.d"), &c->node.loc.att.geoc.s.d);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.geoc.s.d.x"), &c->node.loc.att.geoc.s.d.x);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.geoc.s.d.y"), &c->node.loc.att.geoc.s.d.y);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.geoc.s.d.z"), &c->node.loc.att.geoc.s.d.z);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.geoc.s.w"), &c->node.loc.att.geoc.s.w);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.geoc.v"), &c->node.loc.att.geoc.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.geoc.v.col"), &c->node.loc.att.geoc.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.geoc.v.col)/sizeof(c->node.loc.att.geoc.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.geoc.v.col[" + std::to_string(i) + "]"), &c->node.loc.att.geoc.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.geoc.a"), &c->node.loc.att.geoc.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.geoc.a.col"), &c->node.loc.att.geoc.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.geoc.a.col)/sizeof(c->node.loc.att.geoc.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.geoc.a.col[" + std::to_string(i) + "]"), &c->node.loc.att.geoc.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.att.geoc.pass"), &c->node.loc.att.geoc.pass);
	EXPECT_EQ(c->get_pointer<qatt>("node.loc.att.selc"), &c->node.loc.att.selc);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.selc.utc"), &c->node.loc.att.selc.utc);
	EXPECT_EQ(c->get_pointer<quaternion>("node.loc.att.selc.s"), &c->node.loc.att.selc.s);
	EXPECT_EQ(c->get_pointer<cvector>("node.loc.att.selc.s.d"), &c->node.loc.att.selc.s.d);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.selc.s.d.x"), &c->node.loc.att.selc.s.d.x);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.selc.s.d.y"), &c->node.loc.att.selc.s.d.y);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.selc.s.d.z"), &c->node.loc.att.selc.s.d.z);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.selc.s.w"), &c->node.loc.att.selc.s.w);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.selc.v"), &c->node.loc.att.selc.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.selc.v.col"), &c->node.loc.att.selc.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.selc.v.col)/sizeof(c->node.loc.att.selc.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.selc.v.col[" + std::to_string(i) + "]"), &c->node.loc.att.selc.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.selc.a"), &c->node.loc.att.selc.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.selc.a.col"), &c->node.loc.att.selc.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.selc.a.col)/sizeof(c->node.loc.att.selc.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.selc.a.col[" + std::to_string(i) + "]"), &c->node.loc.att.selc.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.att.selc.pass"), &c->node.loc.att.selc.pass);
	EXPECT_EQ(c->get_pointer<qatt>("node.loc.att.icrf"), &c->node.loc.att.icrf);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.icrf.utc"), &c->node.loc.att.icrf.utc);
	EXPECT_EQ(c->get_pointer<quaternion>("node.loc.att.icrf.s"), &c->node.loc.att.icrf.s);
	EXPECT_EQ(c->get_pointer<cvector>("node.loc.att.icrf.s.d"), &c->node.loc.att.icrf.s.d);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.icrf.s.d.x"), &c->node.loc.att.icrf.s.d.x);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.icrf.s.d.y"), &c->node.loc.att.icrf.s.d.y);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.icrf.s.d.z"), &c->node.loc.att.icrf.s.d.z);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.icrf.s.w"), &c->node.loc.att.icrf.s.w);
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.icrf.v"), &c->node.loc.att.icrf.v);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.icrf.v.col"), &c->node.loc.att.icrf.v.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.icrf.v.col)/sizeof(c->node.loc.att.icrf.v.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.icrf.v.col[" + std::to_string(i) + "]"), &c->node.loc.att.icrf.v.col[i]);
	}
	EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.icrf.a"), &c->node.loc.att.icrf.a);
	//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.icrf.a.col"), &c->node.loc.att.icrf.a.col);
	for(size_t i = 0; i < sizeof(c->node.loc.att.icrf.a.col)/sizeof(c->node.loc.att.icrf.a.col[0]); ++i) {
		EXPECT_EQ(c->get_pointer<double>("node.loc.att.icrf.a.col[" + std::to_string(i) + "]"), &c->node.loc.att.icrf.a.col[i]);
	}
	EXPECT_EQ(c->get_pointer<uint32_t>("node.loc.att.icrf.pass"), &c->node.loc.att.icrf.pass);
	EXPECT_EQ(c->get_pointer<extraatt>("node.loc.att.extra"), &c->node.loc.att.extra);
	EXPECT_EQ(c->get_pointer<double>("node.loc.att.extra.utc"), &c->node.loc.att.extra.utc);
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.att.extra.j2b"), &c->node.loc.att.extra.j2b);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.att.extra.j2b.row"), &c->node.loc.att.extra.j2b.row);
	for(size_t i = 0; i < sizeof(c->node.loc.att.extra.j2b.row)/sizeof(c->node.loc.att.extra.j2b.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.extra.j2b.row[" + std::to_string(i) + "]"), &c->node.loc.att.extra.j2b.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.extra.j2b.row[" + std::to_string(i) + "].col"), &c->node.loc.att.extra.j2b.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.att.extra.j2b.row[i].col)/sizeof(c->node.loc.att.extra.j2b.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.att.extra.j2b.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.att.extra.j2b.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<rmatrix>("node.loc.att.extra.b2j"), &c->node.loc.att.extra.b2j);
	//EXPECT_EQ(c->get_pointer<rvector[X]>("node.loc.att.extra.b2j.row"), &c->node.loc.att.extra.b2j.row);
	for(size_t i = 0; i < sizeof(c->node.loc.att.extra.b2j.row)/sizeof(c->node.loc.att.extra.b2j.row[0]); ++i) {
		EXPECT_EQ(c->get_pointer<rvector>("node.loc.att.extra.b2j.row[" + std::to_string(i) + "]"), &c->node.loc.att.extra.b2j.row[i]);
		//EXPECT_EQ(c->get_pointer<double[X]>("node.loc.att.extra.b2j.row[" + std::to_string(i) + "].col"), &c->node.loc.att.extra.b2j.row[i].col);
		for(size_t j = 0; j < sizeof(c->node.loc.att.extra.b2j.row[i].col)/sizeof(c->node.loc.att.extra.b2j.row[i].col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("node.loc.att.extra.b2j.row[" + std::to_string(i) + "].col[" + std::to_string(j) + "]"), &c->node.loc.att.extra.b2j.row[i].col[j]);
		}
	}
	EXPECT_EQ(c->get_pointer<physicsstruc>("node.phys"), &c->node.phys);
	EXPECT_EQ(c->get_pointer<double>("node.phys.dt"), &c->node.phys.dt);
	EXPECT_EQ(c->get_pointer<double>("node.phys.dtj"), &c->node.phys.dtj);
	EXPECT_EQ(c->get_pointer<double>("node.phys.utc"), &c->node.phys.utc);
	EXPECT_EQ(c->get_pointer<double>("node.phys.mjdaccel"), &c->node.phys.mjdaccel);
	EXPECT_EQ(c->get_pointer<double>("node.phys.mjddiff"), &c->node.phys.mjddiff);
	EXPECT_EQ(c->get_pointer<float>("node.phys.hcap"), &c->node.phys.hcap);
	EXPECT_EQ(c->get_pointer<float>("node.phys.mass"), &c->node.phys.mass);
	EXPECT_EQ(c->get_pointer<float>("node.phys.temp"), &c->node.phys.temp);
	EXPECT_EQ(c->get_pointer<float>("node.phys.heat"), &c->node.phys.heat);
	EXPECT_EQ(c->get_pointer<float>("node.phys.area"), &c->node.phys.area);
	EXPECT_EQ(c->get_pointer<float>("node.phys.battcap"), &c->node.phys.battcap);
	EXPECT_EQ(c->get_pointer<float>("node.phys.battlev"), &c->node.phys.battlev);
	EXPECT_EQ(c->get_pointer<float>("node.phys.powgen"), &c->node.phys.powgen);
	EXPECT_EQ(c->get_pointer<float>("node.phys.powuse"), &c->node.phys.powuse);
	EXPECT_EQ(c->get_pointer<int32_t>("node.phys.mode"), &c->node.phys.mode);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.ftorque"), &c->node.phys.ftorque);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ftorque.x"), &c->node.phys.ftorque.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ftorque.y"), &c->node.phys.ftorque.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ftorque.z"), &c->node.phys.ftorque.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ftorque.w"), &c->node.phys.ftorque.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.atorque"), &c->node.phys.atorque);
	EXPECT_EQ(c->get_pointer<double>("node.phys.atorque.x"), &c->node.phys.atorque.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.atorque.y"), &c->node.phys.atorque.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.atorque.z"), &c->node.phys.atorque.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.atorque.w"), &c->node.phys.atorque.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.rtorque"), &c->node.phys.rtorque);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rtorque.x"), &c->node.phys.rtorque.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rtorque.y"), &c->node.phys.rtorque.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rtorque.z"), &c->node.phys.rtorque.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rtorque.w"), &c->node.phys.rtorque.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.gtorque"), &c->node.phys.gtorque);
	EXPECT_EQ(c->get_pointer<double>("node.phys.gtorque.x"), &c->node.phys.gtorque.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.gtorque.y"), &c->node.phys.gtorque.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.gtorque.z"), &c->node.phys.gtorque.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.gtorque.w"), &c->node.phys.gtorque.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.htorque"), &c->node.phys.htorque);
	EXPECT_EQ(c->get_pointer<double>("node.phys.htorque.x"), &c->node.phys.htorque.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.htorque.y"), &c->node.phys.htorque.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.htorque.z"), &c->node.phys.htorque.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.htorque.w"), &c->node.phys.htorque.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.hmomentum"), &c->node.phys.hmomentum);
	EXPECT_EQ(c->get_pointer<double>("node.phys.hmomentum.x"), &c->node.phys.hmomentum.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.hmomentum.y"), &c->node.phys.hmomentum.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.hmomentum.z"), &c->node.phys.hmomentum.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.hmomentum.w"), &c->node.phys.hmomentum.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.ctorque"), &c->node.phys.ctorque);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ctorque.x"), &c->node.phys.ctorque.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ctorque.y"), &c->node.phys.ctorque.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ctorque.z"), &c->node.phys.ctorque.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.ctorque.w"), &c->node.phys.ctorque.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.fdrag"), &c->node.phys.fdrag);
	EXPECT_EQ(c->get_pointer<double>("node.phys.fdrag.x"), &c->node.phys.fdrag.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.fdrag.y"), &c->node.phys.fdrag.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.fdrag.z"), &c->node.phys.fdrag.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.fdrag.w"), &c->node.phys.fdrag.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.adrag"), &c->node.phys.adrag);
	EXPECT_EQ(c->get_pointer<double>("node.phys.adrag.x"), &c->node.phys.adrag.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.adrag.y"), &c->node.phys.adrag.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.adrag.z"), &c->node.phys.adrag.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.adrag.w"), &c->node.phys.adrag.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.rdrag"), &c->node.phys.rdrag);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rdrag.x"), &c->node.phys.rdrag.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rdrag.y"), &c->node.phys.rdrag.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rdrag.z"), &c->node.phys.rdrag.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.rdrag.w"), &c->node.phys.rdrag.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.thrust"), &c->node.phys.thrust);
	EXPECT_EQ(c->get_pointer<double>("node.phys.thrust.x"), &c->node.phys.thrust.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.thrust.y"), &c->node.phys.thrust.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.thrust.z"), &c->node.phys.thrust.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.thrust.w"), &c->node.phys.thrust.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.moi"), &c->node.phys.moi);
	EXPECT_EQ(c->get_pointer<double>("node.phys.moi.x"), &c->node.phys.moi.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.moi.y"), &c->node.phys.moi.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.moi.z"), &c->node.phys.moi.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.moi.w"), &c->node.phys.moi.w);
	EXPECT_EQ(c->get_pointer<Vector>("node.phys.com"), &c->node.phys.com);
	EXPECT_EQ(c->get_pointer<double>("node.phys.com.x"), &c->node.phys.com.x);
	EXPECT_EQ(c->get_pointer<double>("node.phys.com.y"), &c->node.phys.com.y);
	EXPECT_EQ(c->get_pointer<double>("node.phys.com.z"), &c->node.phys.com.z);
	EXPECT_EQ(c->get_pointer<double>("node.phys.com.w"), &c->node.phys.com.w);
	EXPECT_EQ(c->get_pointer<vector<Vector>>("node.phys.vertices"), &c->node.phys.vertices);
	for(size_t i = 0; i < c->node.phys.vertices.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Vector>("node.phys.vertices[" + std::to_string(i) + "]"), &c->node.phys.vertices[i]);
		EXPECT_EQ(c->get_pointer<double>("node.phys.vertices[" + std::to_string(i) + "].x"), &c->node.phys.vertices[i].x);
		EXPECT_EQ(c->get_pointer<double>("node.phys.vertices[" + std::to_string(i) + "].y"), &c->node.phys.vertices[i].y);
		EXPECT_EQ(c->get_pointer<double>("node.phys.vertices[" + std::to_string(i) + "].z"), &c->node.phys.vertices[i].z);
		EXPECT_EQ(c->get_pointer<double>("node.phys.vertices[" + std::to_string(i) + "].w"), &c->node.phys.vertices[i].w);
	}
	EXPECT_EQ(c->get_pointer<vector<trianglestruc>>("node.phys.triangles"), &c->node.phys.triangles);
	for(size_t i = 0; i < c->node.phys.triangles.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<trianglestruc>("node.phys.triangles[" + std::to_string(i) + "]"), &c->node.phys.triangles[i]);
        EXPECT_EQ(c->get_pointer<uint8_t>("node.phys.triangles[" + std::to_string(i) + "].external"), &c->node.phys.triangles[i].external);
		EXPECT_EQ(c->get_pointer<Vector>("node.phys.triangles[" + std::to_string(i) + "].com"), &c->node.phys.triangles[i].com);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].com.x"), &c->node.phys.triangles[i].com.x);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].com.y"), &c->node.phys.triangles[i].com.y);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].com.z"), &c->node.phys.triangles[i].com.z);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].com.w"), &c->node.phys.triangles[i].com.w);
		EXPECT_EQ(c->get_pointer<Vector>("node.phys.triangles[" + std::to_string(i) + "].normal"), &c->node.phys.triangles[i].normal);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].normal.x"), &c->node.phys.triangles[i].normal.x);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].normal.y"), &c->node.phys.triangles[i].normal.y);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].normal.z"), &c->node.phys.triangles[i].normal.z);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].normal.w"), &c->node.phys.triangles[i].normal.w);
		EXPECT_EQ(c->get_pointer<Vector>("node.phys.triangles[" + std::to_string(i) + "].shove"), &c->node.phys.triangles[i].shove);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].shove.x"), &c->node.phys.triangles[i].shove.x);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].shove.y"), &c->node.phys.triangles[i].shove.y);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].shove.z"), &c->node.phys.triangles[i].shove.z);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].shove.w"), &c->node.phys.triangles[i].shove.w);
		EXPECT_EQ(c->get_pointer<Vector>("node.phys.triangles[" + std::to_string(i) + "].twist"), &c->node.phys.triangles[i].twist);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].twist.x"), &c->node.phys.triangles[i].twist.x);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].twist.y"), &c->node.phys.triangles[i].twist.y);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].twist.z"), &c->node.phys.triangles[i].twist.z);
		EXPECT_EQ(c->get_pointer<double>("node.phys.triangles[" + std::to_string(i) + "].twist.w"), &c->node.phys.triangles[i].twist.w);
		EXPECT_EQ(c->get_pointer<uint16_t>("node.phys.triangles[" + std::to_string(i) + "].pidx"), &c->node.phys.triangles[i].pidx);
		//EXPECT_EQ(c->get_pointer<uint16_t[X]>("node.phys.triangles[" + std::to_string(i) + "].tidx"), &c->node.phys.triangles[i].tidx);
		for(size_t j = 0; j < sizeof(c->node.phys.triangles[i].tidx)/sizeof(c->node.phys.triangles[i].tidx[0]); ++j) {
			EXPECT_EQ(c->get_pointer<uint16_t>("node.phys.triangles[" + std::to_string(i) + "].tidx[" + std::to_string(j) + "]"), &c->node.phys.triangles[i].tidx[j]);
		}
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].heat"), &c->node.phys.triangles[i].heat);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].hcap"), &c->node.phys.triangles[i].hcap);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].emi"), &c->node.phys.triangles[i].emi);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].abs"), &c->node.phys.triangles[i].abs);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].mass"), &c->node.phys.triangles[i].mass);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].temp"), &c->node.phys.triangles[i].temp);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].area"), &c->node.phys.triangles[i].area);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].perimeter"), &c->node.phys.triangles[i].perimeter);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].irradiation"), &c->node.phys.triangles[i].irradiation);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].pcell"), &c->node.phys.triangles[i].pcell);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].ecellbase"), &c->node.phys.triangles[i].ecellbase);
		EXPECT_EQ(c->get_pointer<float>("node.phys.triangles[" + std::to_string(i) + "].ecellslope"), &c->node.phys.triangles[i].ecellslope);
		EXPECT_EQ(c->get_pointer<vector<vector<uint16_t>>>("node.phys.triangles[" + std::to_string(i) + "].triangleindex"), &c->node.phys.triangles[i].triangleindex);
		for(size_t j = 0; j < c->node.phys.triangles[i].triangleindex.capacity(); ++j) {
			EXPECT_EQ(c->get_pointer<vector<uint16_t>>("node.phys.triangles[" + std::to_string(i) + "].triangleindex[" + std::to_string(j) + "]"), &c->node.phys.triangles[i].triangleindex[j]);
			for(size_t k = 0; k < c->node.phys.triangles[i].triangleindex[j].capacity(); ++k) {
				EXPECT_EQ(c->get_pointer<uint16_t>("node.phys.triangles[" + std::to_string(i) + "].triangleindex[" + std::to_string(j) + "][" + std::to_string(k) + "]"), &c->node.phys.triangles[i].triangleindex[j][k]);
			}
		}
	}

	// vector<vertexstruc> vertexs
	EXPECT_EQ(c->get_pointer<vector<vertexstruc>>("vertexs"), &c->vertexs);
	for(size_t i = 0; i < c->vertexs.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<vertexstruc>("vertexs[" + std::to_string(i) + "]"), &c->vertexs[i]);
		EXPECT_EQ(c->get_pointer<double>("vertexs[" + std::to_string(i) + "].x"), &c->vertexs[i].x);
		EXPECT_EQ(c->get_pointer<double>("vertexs[" + std::to_string(i) + "].y"), &c->vertexs[i].y);
		EXPECT_EQ(c->get_pointer<double>("vertexs[" + std::to_string(i) + "].z"), &c->vertexs[i].z);
		EXPECT_EQ(c->get_pointer<double>("vertexs[" + std::to_string(i) + "].w"), &c->vertexs[i].w);
	}

	// vector<vertexstruc> normals
	EXPECT_EQ(c->get_pointer<vector<vertexstruc>>("normals"), &c->normals);
	for(size_t i = 0; i < c->normals.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<vertexstruc>("normals[" + std::to_string(i) + "]"), &c->normals[i]);
		EXPECT_EQ(c->get_pointer<double>("normals[" + std::to_string(i) + "].x"), &c->normals[i].x);
		EXPECT_EQ(c->get_pointer<double>("normals[" + std::to_string(i) + "].y"), &c->normals[i].y);
		EXPECT_EQ(c->get_pointer<double>("normals[" + std::to_string(i) + "].z"), &c->normals[i].z);
		EXPECT_EQ(c->get_pointer<double>("normals[" + std::to_string(i) + "].w"), &c->normals[i].w);
	}

	// vector<facestruc> faces
	EXPECT_EQ(c->get_pointer<vector<facestruc>>("faces"), &c->faces);
	for(size_t i = 0; i < c->faces.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<facestruc>("faces[" + std::to_string(i) + "]"), &c->faces[i]);
		EXPECT_EQ(c->get_pointer<uint16_t>("faces[" + std::to_string(i) + "].vertex_cnt"), &c->faces[i].vertex_cnt);
		EXPECT_EQ(c->get_pointer<vector<uint16_t>>("faces[" + std::to_string(i) + "].vertex_idx"), &c->faces[i].vertex_idx);
		for(size_t j = 0; j < c->faces[i].vertex_idx.capacity(); ++j) {
			EXPECT_EQ(c->get_pointer<uint16_t>("faces[" + std::to_string(i) + "].vertex_idx[" + std::to_string(j) + "]"), &c->faces[i].vertex_idx[j]);
		}
		EXPECT_EQ(c->get_pointer<Vector>("faces[" + std::to_string(i) + "].com"), &c->faces[i].com);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].com.x"), &c->faces[i].com.x);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].com.y"), &c->faces[i].com.y);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].com.z"), &c->faces[i].com.z);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].com.w"), &c->faces[i].com.w);
		EXPECT_EQ(c->get_pointer<Vector>("faces[" + std::to_string(i) + "].normal"), &c->faces[i].normal);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].normal.x"), &c->faces[i].normal.x);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].normal.y"), &c->faces[i].normal.y);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].normal.z"), &c->faces[i].normal.z);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].normal.w"), &c->faces[i].normal.w);
		EXPECT_EQ(c->get_pointer<double>("faces[" + std::to_string(i) + "].area"), &c->faces[i].area);
	}

	// vector<piecestruc> pieces
	EXPECT_EQ(c->get_pointer<vector<piecestruc>>("pieces"), &c->pieces);
	for(size_t i = 0; i < c->pieces.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<piecestruc>("pieces[" + std::to_string(i) + "]"), &c->pieces[i]);
		//EXPECT_EQ(c->get_pointer<char[X]>("pieces[" + std::to_string(i) + "].name"), &c->pieces[i].name);
		EXPECT_EQ(c->get_pointer<bool>("pieces[" + std::to_string(i) + "].enabled"), &c->pieces[i].enabled);
		EXPECT_EQ(c->get_pointer<uint16_t>("pieces[" + std::to_string(i) + "].cidx"), &c->pieces[i].cidx);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].density"), &c->pieces[i].density);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].mass"), &c->pieces[i].mass);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].emi"), &c->pieces[i].emi);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].abs"), &c->pieces[i].abs);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].hcap"), &c->pieces[i].hcap);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].hcon"), &c->pieces[i].hcon);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].dim"), &c->pieces[i].dim);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].area"), &c->pieces[i].area);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].volume"), &c->pieces[i].volume);
		EXPECT_EQ(c->get_pointer<uint16_t>("pieces[" + std::to_string(i) + "].face_cnt"), &c->pieces[i].face_cnt);
		EXPECT_EQ(c->get_pointer<vector<uint16_t>>("pieces[" + std::to_string(i) + "].face_idx"), &c->pieces[i].face_idx);
		for(size_t j = 0; j < c->pieces[i].face_idx.capacity(); ++j) {
			EXPECT_EQ(c->get_pointer<uint16_t>("pieces[" + std::to_string(i) + "].face_idx[" + std::to_string(j) + "]"), &c->pieces[i].face_idx[j]);
		}
		EXPECT_EQ(c->get_pointer<Vector>("pieces[" + std::to_string(i) + "].com"), &c->pieces[i].com);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].com.x"), &c->pieces[i].com.x);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].com.y"), &c->pieces[i].com.y);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].com.z"), &c->pieces[i].com.z);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].com.w"), &c->pieces[i].com.w);
		EXPECT_EQ(c->get_pointer<Vector>("pieces[" + std::to_string(i) + "].shove"), &c->pieces[i].shove);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].shove.x"), &c->pieces[i].shove.x);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].shove.y"), &c->pieces[i].shove.y);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].shove.z"), &c->pieces[i].shove.z);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].shove.w"), &c->pieces[i].shove.w);
		EXPECT_EQ(c->get_pointer<Vector>("pieces[" + std::to_string(i) + "].twist"), &c->pieces[i].twist);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].twist.x"), &c->pieces[i].twist.x);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].twist.y"), &c->pieces[i].twist.y);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].twist.z"), &c->pieces[i].twist.z);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].twist.w"), &c->pieces[i].twist.w);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].heat"), &c->pieces[i].heat);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].temp"), &c->pieces[i].temp);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].insol"), &c->pieces[i].insol);
		EXPECT_EQ(c->get_pointer<float>("pieces[" + std::to_string(i) + "].material_density"), &c->pieces[i].material_density);
		EXPECT_EQ(c->get_pointer<Vector>("pieces[" + std::to_string(i) + "].material_ambient"), &c->pieces[i].material_ambient);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_ambient.x"), &c->pieces[i].material_ambient.x);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_ambient.y"), &c->pieces[i].material_ambient.y);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_ambient.z"), &c->pieces[i].material_ambient.z);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_ambient.w"), &c->pieces[i].material_ambient.w);
		EXPECT_EQ(c->get_pointer<Vector>("pieces[" + std::to_string(i) + "].material_diffuse"), &c->pieces[i].material_diffuse);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_diffuse.x"), &c->pieces[i].material_diffuse.x);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_diffuse.y"), &c->pieces[i].material_diffuse.y);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_diffuse.z"), &c->pieces[i].material_diffuse.z);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_diffuse.w"), &c->pieces[i].material_diffuse.w);
		EXPECT_EQ(c->get_pointer<Vector>("pieces[" + std::to_string(i) + "].material_specular"), &c->pieces[i].material_specular);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_specular.x"), &c->pieces[i].material_specular.x);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_specular.y"), &c->pieces[i].material_specular.y);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_specular.z"), &c->pieces[i].material_specular.z);
		EXPECT_EQ(c->get_pointer<double>("pieces[" + std::to_string(i) + "].material_specular.w"), &c->pieces[i].material_specular.w);
	}

	// wavefront obj
	EXPECT_EQ(c->get_pointer<wavefront>("obj"), &c->obj);
	EXPECT_EQ(c->get_pointer<vector<Vector>>("obj.Vg"), &c->obj.Vg);
	for(size_t i = 0; i < c->obj.Vg.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Vector>("obj.Vg[" + std::to_string(i) + "]"), &c->obj.Vg[i]);
		EXPECT_EQ(c->get_pointer<double>("obj.Vg[" + std::to_string(i) + "].x"), &c->obj.Vg[i].x);
		EXPECT_EQ(c->get_pointer<double>("obj.Vg[" + std::to_string(i) + "].y"), &c->obj.Vg[i].y);
		EXPECT_EQ(c->get_pointer<double>("obj.Vg[" + std::to_string(i) + "].z"), &c->obj.Vg[i].z);
		EXPECT_EQ(c->get_pointer<double>("obj.Vg[" + std::to_string(i) + "].w"), &c->obj.Vg[i].w);
	}
	EXPECT_EQ(c->get_pointer<vector<Vector>>("obj.Vt"), &c->obj.Vt);
	for(size_t i = 0; i < c->obj.Vt.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Vector>("obj.Vt[" + std::to_string(i) + "]"), &c->obj.Vt[i]);
		EXPECT_EQ(c->get_pointer<double>("obj.Vt[" + std::to_string(i) + "].x"), &c->obj.Vt[i].x);
		EXPECT_EQ(c->get_pointer<double>("obj.Vt[" + std::to_string(i) + "].y"), &c->obj.Vt[i].y);
		EXPECT_EQ(c->get_pointer<double>("obj.Vt[" + std::to_string(i) + "].z"), &c->obj.Vt[i].z);
		EXPECT_EQ(c->get_pointer<double>("obj.Vt[" + std::to_string(i) + "].w"), &c->obj.Vt[i].w);
	}
	EXPECT_EQ(c->get_pointer<vector<Vector>>("obj.Vn"), &c->obj.Vn);
	for(size_t i = 0; i < c->obj.Vn.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Vector>("obj.Vn[" + std::to_string(i) + "]"), &c->obj.Vn[i]);
		EXPECT_EQ(c->get_pointer<double>("obj.Vn[" + std::to_string(i) + "].x"), &c->obj.Vn[i].x);
		EXPECT_EQ(c->get_pointer<double>("obj.Vn[" + std::to_string(i) + "].y"), &c->obj.Vn[i].y);
		EXPECT_EQ(c->get_pointer<double>("obj.Vn[" + std::to_string(i) + "].z"), &c->obj.Vn[i].z);
		EXPECT_EQ(c->get_pointer<double>("obj.Vn[" + std::to_string(i) + "].w"), &c->obj.Vn[i].w);
	}
	EXPECT_EQ(c->get_pointer<vector<Vector>>("obj.Vp"), &c->obj.Vp);
	for(size_t i = 0; i < c->obj.Vp.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Vector>("obj.Vp[" + std::to_string(i) + "]"), &c->obj.Vp[i]);
		EXPECT_EQ(c->get_pointer<double>("obj.Vp[" + std::to_string(i) + "].x"), &c->obj.Vp[i].x);
		EXPECT_EQ(c->get_pointer<double>("obj.Vp[" + std::to_string(i) + "].y"), &c->obj.Vp[i].y);
		EXPECT_EQ(c->get_pointer<double>("obj.Vp[" + std::to_string(i) + "].z"), &c->obj.Vp[i].z);
		EXPECT_EQ(c->get_pointer<double>("obj.Vp[" + std::to_string(i) + "].w"), &c->obj.Vp[i].w);
	}
	EXPECT_EQ(c->get_pointer<vector<Cosmos::wavefront::material>>("obj.Materials"), &c->obj.Materials);
	for(size_t i = 0; i < c->obj.Materials.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Cosmos::wavefront::material>("obj.Materials[" + std::to_string(i) + "]"), &c->obj.Materials[i]);
	}
	EXPECT_EQ(c->get_pointer<vector<Cosmos::wavefront::point>>("obj.Points"), &c->obj.Points);
	for(size_t i = 0; i < c->obj.Points.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Cosmos::wavefront::point>("obj.Points[" + std::to_string(i) + "]"), &c->obj.Points[i]);
	}
	EXPECT_EQ(c->get_pointer<vector<Cosmos::wavefront::line>>("obj.Lines"), &c->obj.Lines);
	for(size_t i = 0; i < c->obj.Lines.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Cosmos::wavefront::line>("obj.Lines[" + std::to_string(i) + "]"), &c->obj.Lines[i]);
	}
	EXPECT_EQ(c->get_pointer<vector<Cosmos::wavefront::face>>("obj.Faces"), &c->obj.Faces);
	for(size_t i = 0; i < c->obj.Faces.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Cosmos::wavefront::face>("obj.Faces[" + std::to_string(i) + "]"), &c->obj.Faces[i]);
	}
	EXPECT_EQ(c->get_pointer<vector<Cosmos::wavefront::group>>("obj.Groups"), &c->obj.Groups);
	for(size_t i = 0; i < c->obj.Groups.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<Cosmos::wavefront::group>("obj.Groups[" + std::to_string(i) + "]"), &c->obj.Groups[i]);
	}

	// vector<devicestruc> device
	EXPECT_EQ(c->get_pointer<vector<devicestruc>>("device"), &c->device);
	for(size_t i = 0; i < c->device.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<devicestruc>("device[" + std::to_string(i) + "]"), &c->device[i]);
		EXPECT_EQ(c->get_pointer<allstruc>("device[" + std::to_string(i) + "].all"), &c->device[i].all);
		EXPECT_EQ(c->get_pointer<bool>("device[" + std::to_string(i) + "].all.enabled"), &c->device[i].all.enabled);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.type"), &c->device[i].all.type);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.model"), &c->device[i].all.model);
		EXPECT_EQ(c->get_pointer<uint32_t>("device[" + std::to_string(i) + "].all.flag"), &c->device[i].all.flag);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.addr"), &c->device[i].all.addr);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.cidx"), &c->device[i].all.cidx);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.didx"), &c->device[i].all.didx);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.pidx"), &c->device[i].all.pidx);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.bidx"), &c->device[i].all.bidx);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all.portidx"), &c->device[i].all.portidx);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.namp"), &c->device[i].all.namp);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.nvolt"), &c->device[i].all.nvolt);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.amp"), &c->device[i].all.amp);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.volt"), &c->device[i].all.volt);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.power"), &c->device[i].all.power);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.energy"), &c->device[i].all.energy);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.drate"), &c->device[i].all.drate);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].all.temp"), &c->device[i].all.temp);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].all.utc"), &c->device[i].all.utc);
		EXPECT_EQ(c->get_pointer<antstruc>("device[" + std::to_string(i) + "].ant"), &c->device[i].ant);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].ant.align"), &c->device[i].ant.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].ant.align.d"), &c->device[i].ant.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ant.align.d.x"), &c->device[i].ant.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ant.align.d.y"), &c->device[i].ant.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ant.align.d.z"), &c->device[i].ant.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ant.align.w"), &c->device[i].ant.align.w);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ant.azim"), &c->device[i].ant.azim);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ant.elev"), &c->device[i].ant.elev);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ant.minelev"), &c->device[i].ant.minelev);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ant.maxelev"), &c->device[i].ant.maxelev);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ant.minazim"), &c->device[i].ant.minazim);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ant.maxazim"), &c->device[i].ant.maxazim);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ant.threshelev"), &c->device[i].ant.threshelev);
		EXPECT_EQ(c->get_pointer<battstruc>("device[" + std::to_string(i) + "].batt"), &c->device[i].batt);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].batt.capacity"), &c->device[i].batt.capacity);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].batt.efficiency"), &c->device[i].batt.efficiency);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].batt.charge"), &c->device[i].batt.charge);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].batt.r_in"), &c->device[i].batt.r_in);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].batt.r_out"), &c->device[i].batt.r_out);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].batt.percentage"), &c->device[i].batt.percentage);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].batt.time_remaining"), &c->device[i].batt.time_remaining);
		EXPECT_EQ(c->get_pointer<bcregstruc>("device[" + std::to_string(i) + "].bcreg"), &c->device[i].bcreg);
		EXPECT_EQ(c->get_pointer<busstruc>("device[" + std::to_string(i) + "].bus"), &c->device[i].bus);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].bus.wdt"), &c->device[i].bus.wdt);
		EXPECT_EQ(c->get_pointer<camstruc>("device[" + std::to_string(i) + "].cam"), &c->device[i].cam);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].cam.pwidth"), &c->device[i].cam.pwidth);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].cam.pheight"), &c->device[i].cam.pheight);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].cam.width"), &c->device[i].cam.width);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].cam.height"), &c->device[i].cam.height);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].cam.flength"), &c->device[i].cam.flength);
		EXPECT_EQ(c->get_pointer<cpustruc>("device[" + std::to_string(i) + "].cpu"), &c->device[i].cpu);
		EXPECT_EQ(c->get_pointer<uint32_t>("device[" + std::to_string(i) + "].cpu.uptime"), &c->device[i].cpu.uptime);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].cpu.load"), &c->device[i].cpu.load);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].cpu.maxload"), &c->device[i].cpu.maxload);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].cpu.maxgib"), &c->device[i].cpu.maxgib);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].cpu.gib"), &c->device[i].cpu.gib);
		EXPECT_EQ(c->get_pointer<uint32_t>("device[" + std::to_string(i) + "].cpu.boot_count"), &c->device[i].cpu.boot_count);
		EXPECT_EQ(c->get_pointer<diskstruc>("device[" + std::to_string(i) + "].disk"), &c->device[i].disk);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].disk.maxgib"), &c->device[i].disk.maxgib);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].disk.gib"), &c->device[i].disk.gib);
		//EXPECT_EQ(c->get_pointer<char[X]>("device[" + std::to_string(i) + "].disk.path"), &c->device[i].disk.path);
		EXPECT_EQ(c->get_pointer<gpsstruc>("device[" + std::to_string(i) + "].gps"), &c->device[i].gps);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dutc"), &c->device[i].gps.dutc);
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].gps.geocs"), &c->device[i].gps.geocs);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].gps.geocs.col"), &c->device[i].gps.geocs.col);
		for(size_t j = 0; j < sizeof(c->device[i].gps.geocs.col)/sizeof(c->device[i].gps.geocs.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geocs.col[" + std::to_string(j) + "]"), &c->device[i].gps.geocs.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].gps.geocv"), &c->device[i].gps.geocv);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].gps.geocv.col"), &c->device[i].gps.geocv.col);
		for(size_t j = 0; j < sizeof(c->device[i].gps.geocv.col)/sizeof(c->device[i].gps.geocv.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geocv.col[" + std::to_string(j) + "]"), &c->device[i].gps.geocv.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].gps.dgeocs"), &c->device[i].gps.dgeocs);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].gps.dgeocs.col"), &c->device[i].gps.dgeocs.col);
		for(size_t j = 0; j < sizeof(c->device[i].gps.dgeocs.col)/sizeof(c->device[i].gps.dgeocs.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeocs.col[" + std::to_string(j) + "]"), &c->device[i].gps.dgeocs.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].gps.dgeocv"), &c->device[i].gps.dgeocv);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].gps.dgeocv.col"), &c->device[i].gps.dgeocv.col);
		for(size_t j = 0; j < sizeof(c->device[i].gps.dgeocv.col)/sizeof(c->device[i].gps.dgeocv.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeocv.col[" + std::to_string(j) + "]"), &c->device[i].gps.dgeocv.col[j]);
		}
		EXPECT_EQ(c->get_pointer<gvector>("device[" + std::to_string(i) + "].gps.geods"), &c->device[i].gps.geods);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geods.lat"), &c->device[i].gps.geods.lat);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geods.lon"), &c->device[i].gps.geods.lon);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geods.h"), &c->device[i].gps.geods.h);
		EXPECT_EQ(c->get_pointer<gvector>("device[" + std::to_string(i) + "].gps.geodv"), &c->device[i].gps.geodv);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geodv.lat"), &c->device[i].gps.geodv.lat);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geodv.lon"), &c->device[i].gps.geodv.lon);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.geodv.h"), &c->device[i].gps.geodv.h);
		EXPECT_EQ(c->get_pointer<gvector>("device[" + std::to_string(i) + "].gps.dgeods"), &c->device[i].gps.dgeods);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeods.lat"), &c->device[i].gps.dgeods.lat);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeods.lon"), &c->device[i].gps.dgeods.lon);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeods.h"), &c->device[i].gps.dgeods.h);
		EXPECT_EQ(c->get_pointer<gvector>("device[" + std::to_string(i) + "].gps.dgeodv"), &c->device[i].gps.dgeodv);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeodv.lat"), &c->device[i].gps.dgeodv.lat);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeodv.lon"), &c->device[i].gps.dgeodv.lon);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].gps.dgeodv.h"), &c->device[i].gps.dgeodv.h);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].gps.heading"), &c->device[i].gps.heading);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].gps.sats_used"), &c->device[i].gps.sats_used);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].gps.sats_visible"), &c->device[i].gps.sats_visible);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].gps.time_status"), &c->device[i].gps.time_status);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].gps.position_type"), &c->device[i].gps.position_type);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].gps.solution_status"), &c->device[i].gps.solution_status);
		EXPECT_EQ(c->get_pointer<htrstruc>("device[" + std::to_string(i) + "].htr"), &c->device[i].htr);
		EXPECT_EQ(c->get_pointer<bool>("device[" + std::to_string(i) + "].htr.state"), &c->device[i].htr.state);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].htr.setvertex"), &c->device[i].htr.setvertex);
		EXPECT_EQ(c->get_pointer<imustruc>("device[" + std::to_string(i) + "].imu"), &c->device[i].imu);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].imu.align"), &c->device[i].imu.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].imu.align.d"), &c->device[i].imu.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.align.d.x"), &c->device[i].imu.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.align.d.y"), &c->device[i].imu.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.align.d.z"), &c->device[i].imu.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.align.w"), &c->device[i].imu.align.w);
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].imu.accel"), &c->device[i].imu.accel);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].imu.accel.col"), &c->device[i].imu.accel.col);
		for(size_t j = 0; j < sizeof(c->device[i].imu.accel.col)/sizeof(c->device[i].imu.accel.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.accel.col[" + std::to_string(j) + "]"), &c->device[i].imu.accel.col[j]);
		}
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].imu.theta"), &c->device[i].imu.theta);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].imu.theta.d"), &c->device[i].imu.theta.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.theta.d.x"), &c->device[i].imu.theta.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.theta.d.y"), &c->device[i].imu.theta.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.theta.d.z"), &c->device[i].imu.theta.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.theta.w"), &c->device[i].imu.theta.w);
		EXPECT_EQ(c->get_pointer<avector>("device[" + std::to_string(i) + "].imu.euler"), &c->device[i].imu.euler);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.euler.h"), &c->device[i].imu.euler.h);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.euler.e"), &c->device[i].imu.euler.e);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.euler.b"), &c->device[i].imu.euler.b);
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].imu.omega"), &c->device[i].imu.omega);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].imu.omega.col"), &c->device[i].imu.omega.col);
		for(size_t j = 0; j < sizeof(c->device[i].imu.omega.col)/sizeof(c->device[i].imu.omega.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.omega.col[" + std::to_string(j) + "]"), &c->device[i].imu.omega.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].imu.alpha"), &c->device[i].imu.alpha);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].imu.alpha.col"), &c->device[i].imu.alpha.col);
		for(size_t j = 0; j < sizeof(c->device[i].imu.alpha.col)/sizeof(c->device[i].imu.alpha.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.alpha.col[" + std::to_string(j) + "]"), &c->device[i].imu.alpha.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].imu.mag"), &c->device[i].imu.mag);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].imu.mag.col"), &c->device[i].imu.mag.col);
		for(size_t j = 0; j < sizeof(c->device[i].imu.mag.col)/sizeof(c->device[i].imu.mag.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.mag.col[" + std::to_string(j) + "]"), &c->device[i].imu.mag.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].imu.bdot"), &c->device[i].imu.bdot);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].imu.bdot.col"), &c->device[i].imu.bdot.col);
		for(size_t j = 0; j < sizeof(c->device[i].imu.bdot.col)/sizeof(c->device[i].imu.bdot.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].imu.bdot.col[" + std::to_string(j) + "]"), &c->device[i].imu.bdot.col[j]);
		}
		EXPECT_EQ(c->get_pointer<mccstruc>("device[" + std::to_string(i) + "].mcc"), &c->device[i].mcc);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].mcc.align"), &c->device[i].mcc.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].mcc.align.d"), &c->device[i].mcc.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.align.d.x"), &c->device[i].mcc.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.align.d.y"), &c->device[i].mcc.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.align.d.z"), &c->device[i].mcc.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.align.w"), &c->device[i].mcc.align.w);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].mcc.q"), &c->device[i].mcc.q);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].mcc.q.d"), &c->device[i].mcc.q.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.q.d.x"), &c->device[i].mcc.q.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.q.d.y"), &c->device[i].mcc.q.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.q.d.z"), &c->device[i].mcc.q.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.q.w"), &c->device[i].mcc.q.w);
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].mcc.o"), &c->device[i].mcc.o);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].mcc.o.col"), &c->device[i].mcc.o.col);
		for(size_t j = 0; j < sizeof(c->device[i].mcc.o.col)/sizeof(c->device[i].mcc.o.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.o.col[" + std::to_string(j) + "]"), &c->device[i].mcc.o.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].mcc.a"), &c->device[i].mcc.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].mcc.a.col"), &c->device[i].mcc.a.col);
		for(size_t j = 0; j < sizeof(c->device[i].mcc.a.col)/sizeof(c->device[i].mcc.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mcc.a.col[" + std::to_string(j) + "]"), &c->device[i].mcc.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<motrstruc>("device[" + std::to_string(i) + "].motr"), &c->device[i].motr);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].motr.max"), &c->device[i].motr.max);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].motr.rat"), &c->device[i].motr.rat);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].motr.spd"), &c->device[i].motr.spd);
		EXPECT_EQ(c->get_pointer<mtrstruc>("device[" + std::to_string(i) + "].mtr"), &c->device[i].mtr);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].mtr.align"), &c->device[i].mtr.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].mtr.align.d"), &c->device[i].mtr.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mtr.align.d.x"), &c->device[i].mtr.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mtr.align.d.y"), &c->device[i].mtr.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mtr.align.d.z"), &c->device[i].mtr.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].mtr.align.w"), &c->device[i].mtr.align.w);
		//EXPECT_EQ(c->get_pointer<float[X]>("device[" + std::to_string(i) + "].mtr.npoly"), &c->device[i].mtr.npoly);
		for(size_t j = 0; j < sizeof(c->device[i].mtr.npoly)/sizeof(c->device[i].mtr.npoly[0]); ++j) {
			EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].mtr.npoly[" + std::to_string(j) + "]"), &c->device[i].mtr.npoly[j]);
		}
		//EXPECT_EQ(c->get_pointer<float[X]>("device[" + std::to_string(i) + "].mtr.ppoly"), &c->device[i].mtr.ppoly);
		for(size_t j = 0; j < sizeof(c->device[i].mtr.ppoly)/sizeof(c->device[i].mtr.ppoly[0]); ++j) {
			EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].mtr.ppoly[" + std::to_string(j) + "]"), &c->device[i].mtr.ppoly[j]);
		}
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].mtr.mxmom"), &c->device[i].mtr.mxmom);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].mtr.tc"), &c->device[i].mtr.tc);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].mtr.rmom"), &c->device[i].mtr.rmom);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].mtr.mom"), &c->device[i].mtr.mom);
		EXPECT_EQ(c->get_pointer<ploadstruc>("device[" + std::to_string(i) + "].pload"), &c->device[i].pload);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].pload.key_cnt"), &c->device[i].pload.key_cnt);
		//EXPECT_EQ(c->get_pointer<uint16_t[X]>("device[" + std::to_string(i) + "].pload.keyidx"), &c->device[i].pload.keyidx);
		for(size_t j = 0; j < sizeof(c->device[i].pload.keyidx)/sizeof(c->device[i].pload.keyidx[0]); ++j) {
			EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].pload.keyidx[" + std::to_string(j) + "]"), &c->device[i].pload.keyidx[j]);
		}
		//EXPECT_EQ(c->get_pointer<float[X]>("device[" + std::to_string(i) + "].pload.keyval"), &c->device[i].pload.keyval);
		for(size_t j = 0; j < sizeof(c->device[i].pload.keyval)/sizeof(c->device[i].pload.keyval[0]); ++j) {
			EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].pload.keyval[" + std::to_string(j) + "]"), &c->device[i].pload.keyval[j]);
		}
		EXPECT_EQ(c->get_pointer<propstruc>("device[" + std::to_string(i) + "].prop"), &c->device[i].prop);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].prop.cap"), &c->device[i].prop.cap);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].prop.lev"), &c->device[i].prop.lev);
		EXPECT_EQ(c->get_pointer<psenstruc>("device[" + std::to_string(i) + "].psen"), &c->device[i].psen);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].psen.press"), &c->device[i].psen.press);
		EXPECT_EQ(c->get_pointer<pvstrgstruc>("device[" + std::to_string(i) + "].pvstrg"), &c->device[i].pvstrg);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].pvstrg.bcidx"), &c->device[i].pvstrg.bcidx);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].pvstrg.effbase"), &c->device[i].pvstrg.effbase);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].pvstrg.effslope"), &c->device[i].pvstrg.effslope);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].pvstrg.maxpower"), &c->device[i].pvstrg.maxpower);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].pvstrg.power"), &c->device[i].pvstrg.power);
		EXPECT_EQ(c->get_pointer<rotstruc>("device[" + std::to_string(i) + "].rot"), &c->device[i].rot);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rot.angle"), &c->device[i].rot.angle);
		EXPECT_EQ(c->get_pointer<rwstruc>("device[" + std::to_string(i) + "].rw"), &c->device[i].rw);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].rw.align"), &c->device[i].rw.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].rw.align.d"), &c->device[i].rw.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rw.align.d.x"), &c->device[i].rw.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rw.align.d.y"), &c->device[i].rw.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rw.align.d.z"), &c->device[i].rw.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rw.align.w"), &c->device[i].rw.align.w);
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].rw.mom"), &c->device[i].rw.mom);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].rw.mom.col"), &c->device[i].rw.mom.col);
		for(size_t j = 0; j < sizeof(c->device[i].rw.mom.col)/sizeof(c->device[i].rw.mom.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rw.mom.col[" + std::to_string(j) + "]"), &c->device[i].rw.mom.col[j]);
		}
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rw.mxomg"), &c->device[i].rw.mxomg);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rw.mxalp"), &c->device[i].rw.mxalp);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rw.tc"), &c->device[i].rw.tc);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rw.omg"), &c->device[i].rw.omg);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rw.alp"), &c->device[i].rw.alp);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rw.romg"), &c->device[i].rw.romg);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rw.ralp"), &c->device[i].rw.ralp);
		EXPECT_EQ(c->get_pointer<rxrstruc>("device[" + std::to_string(i) + "].rxr"), &c->device[i].rxr);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].rxr.opmode"), &c->device[i].rxr.opmode);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].rxr.modulation"), &c->device[i].rxr.modulation);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].rxr.rssi"), &c->device[i].rxr.rssi);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].rxr.pktsize"), &c->device[i].rxr.pktsize);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rxr.freq"), &c->device[i].rxr.freq);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rxr.maxfreq"), &c->device[i].rxr.maxfreq);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rxr.minfreq"), &c->device[i].rxr.minfreq);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rxr.powerin"), &c->device[i].rxr.powerin);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rxr.powerout"), &c->device[i].rxr.powerout);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rxr.maxpower"), &c->device[i].rxr.maxpower);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rxr.band"), &c->device[i].rxr.band);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].rxr.squelch_tone"), &c->device[i].rxr.squelch_tone);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rxr.goodratio"), &c->device[i].rxr.goodratio);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rxr.rxutc"), &c->device[i].rxr.rxutc);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].rxr.uptime"), &c->device[i].rxr.uptime);
		EXPECT_EQ(c->get_pointer<ssenstruc>("device[" + std::to_string(i) + "].ssen"), &c->device[i].ssen);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].ssen.align"), &c->device[i].ssen.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].ssen.align.d"), &c->device[i].ssen.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ssen.align.d.x"), &c->device[i].ssen.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ssen.align.d.y"), &c->device[i].ssen.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ssen.align.d.z"), &c->device[i].ssen.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].ssen.align.w"), &c->device[i].ssen.align.w);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ssen.qva"), &c->device[i].ssen.qva);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ssen.qvb"), &c->device[i].ssen.qvb);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ssen.qvc"), &c->device[i].ssen.qvc);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ssen.qvd"), &c->device[i].ssen.qvd);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ssen.azimuth"), &c->device[i].ssen.azimuth);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].ssen.elevation"), &c->device[i].ssen.elevation);
		EXPECT_EQ(c->get_pointer<sttstruc>("device[" + std::to_string(i) + "].stt"), &c->device[i].stt);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].stt.align"), &c->device[i].stt.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].stt.align.d"), &c->device[i].stt.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.align.d.x"), &c->device[i].stt.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.align.d.y"), &c->device[i].stt.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.align.d.z"), &c->device[i].stt.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.align.w"), &c->device[i].stt.align.w);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].stt.att"), &c->device[i].stt.att);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].stt.att.d"), &c->device[i].stt.att.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.att.d.x"), &c->device[i].stt.att.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.att.d.y"), &c->device[i].stt.att.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.att.d.z"), &c->device[i].stt.att.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.att.w"), &c->device[i].stt.att.w);
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].stt.omega"), &c->device[i].stt.omega);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].stt.omega.col"), &c->device[i].stt.omega.col);
		for(size_t j = 0; j < sizeof(c->device[i].stt.omega.col)/sizeof(c->device[i].stt.omega.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.omega.col[" + std::to_string(j) + "]"), &c->device[i].stt.omega.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("device[" + std::to_string(i) + "].stt.alpha"), &c->device[i].stt.alpha);
		//EXPECT_EQ(c->get_pointer<double[X]>("device[" + std::to_string(i) + "].stt.alpha.col"), &c->device[i].stt.alpha.col);
		for(size_t j = 0; j < sizeof(c->device[i].stt.alpha.col)/sizeof(c->device[i].stt.alpha.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].stt.alpha.col[" + std::to_string(j) + "]"), &c->device[i].stt.alpha.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].stt.retcode"), &c->device[i].stt.retcode);
		EXPECT_EQ(c->get_pointer<uint32_t>("device[" + std::to_string(i) + "].stt.status"), &c->device[i].stt.status);
		EXPECT_EQ(c->get_pointer<suchistruc>("device[" + std::to_string(i) + "].suchi"), &c->device[i].suchi);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].suchi.align"), &c->device[i].suchi.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].suchi.align.d"), &c->device[i].suchi.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].suchi.align.d.x"), &c->device[i].suchi.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].suchi.align.d.y"), &c->device[i].suchi.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].suchi.align.d.z"), &c->device[i].suchi.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].suchi.align.w"), &c->device[i].suchi.align.w);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].suchi.press"), &c->device[i].suchi.press);
		//EXPECT_EQ(c->get_pointer<float[X]>("device[" + std::to_string(i) + "].suchi.temps"), &c->device[i].suchi.temps);
		for(size_t j = 0; j < sizeof(c->device[i].suchi.temps)/sizeof(c->device[i].suchi.temps[0]); ++j) {
			EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].suchi.temps[" + std::to_string(j) + "]"), &c->device[i].suchi.temps[j]);
		}
		EXPECT_EQ(c->get_pointer<swchstruc>("device[" + std::to_string(i) + "].swch"), &c->device[i].swch);
		EXPECT_EQ(c->get_pointer<tcustruc>("device[" + std::to_string(i) + "].tcu"), &c->device[i].tcu);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].tcu.mcnt"), &c->device[i].tcu.mcnt);
		//EXPECT_EQ(c->get_pointer<uint16_t[X]>("device[" + std::to_string(i) + "].tcu.mcidx"), &c->device[i].tcu.mcidx);
		for(size_t j = 0; j < sizeof(c->device[i].tcu.mcidx)/sizeof(c->device[i].tcu.mcidx[0]); ++j) {
			EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].tcu.mcidx[" + std::to_string(j) + "]"), &c->device[i].tcu.mcidx[j]);
		}
		EXPECT_EQ(c->get_pointer<tcvstruc>("device[" + std::to_string(i) + "].tcv"), &c->device[i].tcv);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].tcv.opmode"), &c->device[i].tcv.opmode);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].tcv.modulation"), &c->device[i].tcv.modulation);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].tcv.rssi"), &c->device[i].tcv.rssi);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].tcv.pktsize"), &c->device[i].tcv.pktsize);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].tcv.freq"), &c->device[i].tcv.freq);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].tcv.maxfreq"), &c->device[i].tcv.maxfreq);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].tcv.minfreq"), &c->device[i].tcv.minfreq);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].tcv.powerin"), &c->device[i].tcv.powerin);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].tcv.powerout"), &c->device[i].tcv.powerout);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].tcv.maxpower"), &c->device[i].tcv.maxpower);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].tcv.band"), &c->device[i].tcv.band);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].tcv.squelch_tone"), &c->device[i].tcv.squelch_tone);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].tcv.goodratio"), &c->device[i].tcv.goodratio);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].tcv.txutc"), &c->device[i].tcv.txutc);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].tcv.rxutc"), &c->device[i].tcv.rxutc);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].tcv.uptime"), &c->device[i].tcv.uptime);
		EXPECT_EQ(c->get_pointer<telemstruc>("device[" + std::to_string(i) + "].telem"), &c->device[i].telem);
		EXPECT_EQ(c->get_pointer<uint8_t>("device[" + std::to_string(i) + "].telem.vuint8"), &c->device[i].telem.vuint8);
		EXPECT_EQ(c->get_pointer<int8_t>("device[" + std::to_string(i) + "].telem.vint8"), &c->device[i].telem.vint8);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].telem.vuint16"), &c->device[i].telem.vuint16);
		EXPECT_EQ(c->get_pointer<int16_t>("device[" + std::to_string(i) + "].telem.vint16"), &c->device[i].telem.vint16);
		EXPECT_EQ(c->get_pointer<uint32_t>("device[" + std::to_string(i) + "].telem.vuint32"), &c->device[i].telem.vuint32);
		EXPECT_EQ(c->get_pointer<int32_t>("device[" + std::to_string(i) + "].telem.vint32"), &c->device[i].telem.vint32);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].telem.vfloat"), &c->device[i].telem.vfloat);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].telem.vdouble"), &c->device[i].telem.vdouble);
		//EXPECT_EQ(c->get_pointer<char[X]>("device[" + std::to_string(i) + "].telem.vstring"), &c->device[i].telem.vstring);
		EXPECT_EQ(c->get_pointer<thststruc>("device[" + std::to_string(i) + "].thst"), &c->device[i].thst);
		EXPECT_EQ(c->get_pointer<quaternion>("device[" + std::to_string(i) + "].thst.align"), &c->device[i].thst.align);
		EXPECT_EQ(c->get_pointer<cvector>("device[" + std::to_string(i) + "].thst.align.d"), &c->device[i].thst.align.d);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].thst.align.d.x"), &c->device[i].thst.align.d.x);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].thst.align.d.y"), &c->device[i].thst.align.d.y);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].thst.align.d.z"), &c->device[i].thst.align.d.z);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].thst.align.w"), &c->device[i].thst.align.w);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].thst.flw"), &c->device[i].thst.flw);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].thst.isp"), &c->device[i].thst.isp);
		EXPECT_EQ(c->get_pointer<tncstruc>("device[" + std::to_string(i) + "].tnc"), &c->device[i].tnc);
		EXPECT_EQ(c->get_pointer<tsenstruc>("device[" + std::to_string(i) + "].tsen"), &c->device[i].tsen);
		EXPECT_EQ(c->get_pointer<txrstruc>("device[" + std::to_string(i) + "].txr"), &c->device[i].txr);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].txr.opmode"), &c->device[i].txr.opmode);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].txr.modulation"), &c->device[i].txr.modulation);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].txr.rssi"), &c->device[i].txr.rssi);
		EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].txr.pktsize"), &c->device[i].txr.pktsize);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].txr.freq"), &c->device[i].txr.freq);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].txr.maxfreq"), &c->device[i].txr.maxfreq);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].txr.minfreq"), &c->device[i].txr.minfreq);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].txr.powerin"), &c->device[i].txr.powerin);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].txr.powerout"), &c->device[i].txr.powerout);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].txr.maxpower"), &c->device[i].txr.maxpower);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].txr.band"), &c->device[i].txr.band);
		EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].txr.squelch_tone"), &c->device[i].txr.squelch_tone);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].txr.goodratio"), &c->device[i].txr.goodratio);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].txr.txutc"), &c->device[i].txr.txutc);
		EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].txr.uptime"), &c->device[i].txr.uptime);
	}

	// devspecstruc devspec
	EXPECT_EQ(c->get_pointer<devspecstruc>("devspec"), &c->devspec);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.all_cnt"), &c->devspec.all_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.ant_cnt"), &c->devspec.ant_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.batt_cnt"), &c->devspec.batt_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.bus_cnt"), &c->devspec.bus_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.cam_cnt"), &c->devspec.cam_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.cpu_cnt"), &c->devspec.cpu_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.disk_cnt"), &c->devspec.disk_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.gps_cnt"), &c->devspec.gps_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.htr_cnt"), &c->devspec.htr_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.imu_cnt"), &c->devspec.imu_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.mcc_cnt"), &c->devspec.mcc_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.motr_cnt"), &c->devspec.motr_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.mtr_cnt"), &c->devspec.mtr_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.pload_cnt"), &c->devspec.pload_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.prop_cnt"), &c->devspec.prop_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.psen_cnt"), &c->devspec.psen_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.bcreg_cnt"), &c->devspec.bcreg_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.rot_cnt"), &c->devspec.rot_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.rw_cnt"), &c->devspec.rw_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.rxr_cnt"), &c->devspec.rxr_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.ssen_cnt"), &c->devspec.ssen_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.pvstrg_cnt"), &c->devspec.pvstrg_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.stt_cnt"), &c->devspec.stt_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.suchi_cnt"), &c->devspec.suchi_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.swch_cnt"), &c->devspec.swch_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcu_cnt"), &c->devspec.tcu_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcv_cnt"), &c->devspec.tcv_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.telem_cnt"), &c->devspec.telem_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.thst_cnt"), &c->devspec.thst_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tsen_cnt"), &c->devspec.tsen_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tnc_cnt"), &c->devspec.tnc_cnt);
	EXPECT_EQ(c->get_pointer<uint16_t>("devspec.txr_cnt"), &c->devspec.txr_cnt);

	// vector<portstruc> port
	EXPECT_EQ(c->get_pointer<vector<portstruc>>("port"), &c->port);
	for(size_t i = 0; i < c->port.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<portstruc>("port[" + std::to_string(i) + "]"), &c->port[i]);
		EXPECT_EQ(c->get_pointer<PORT_TYPE>("port[" + std::to_string(i) + "].type"), &c->port[i].type);
		//EXPECT_EQ(c->get_pointer<char[X]>("port[" + std::to_string(i) + "].name"), &c->port[i].name);
	}

	// vector<agentstruc> agent
	EXPECT_EQ(c->get_pointer<vector<agentstruc>>("agent"), &c->agent);
	for(size_t i = 0; i < c->agent.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<agentstruc>("agent[" + std::to_string(i) + "]"), &c->agent[i]);
		EXPECT_EQ(c->get_pointer<bool>("agent[" + std::to_string(i) + "].client"), &c->agent[i].client);
		EXPECT_EQ(c->get_pointer<socket_channel>("agent[" + std::to_string(i) + "].sub"), &c->agent[i].sub);
		EXPECT_EQ(c->get_pointer<NetworkType>("agent[" + std::to_string(i) + "].sub.type"), &c->agent[i].sub.type);
		EXPECT_EQ(c->get_pointer<int32_t>("agent[" + std::to_string(i) + "].sub.cudp"), &c->agent[i].sub.cudp);
		EXPECT_EQ(c->get_pointer<bool>("agent[" + std::to_string(i) + "].server"), &c->agent[i].server);
		EXPECT_EQ(c->get_pointer<size_t>("agent[" + std::to_string(i) + "].ifcnt"), &c->agent[i].ifcnt);
		//EXPECT_EQ(c->get_pointer<socket_channel[X]>("agent[" + std::to_string(i) + "].pub"), &c->agent[i].pub);
		for(size_t j = 0; j < sizeof(c->agent[i].pub)/sizeof(c->agent[i].pub[0]); ++j) {
			EXPECT_EQ(c->get_pointer<socket_channel>("agent[" + std::to_string(i) + "].pub[" + std::to_string(j) + "]"), &c->agent[i].pub[j]);
			EXPECT_EQ(c->get_pointer<NetworkType>("agent[" + std::to_string(i) + "].pub[" + std::to_string(j) + "].type"), &c->agent[i].pub[j].type);
			EXPECT_EQ(c->get_pointer<int32_t>("agent[" + std::to_string(i) + "].pub[" + std::to_string(j) + "].cudp"), &c->agent[i].pub[j].cudp);
		}
		EXPECT_EQ(c->get_pointer<socket_channel>("agent[" + std::to_string(i) + "].req"), &c->agent[i].req);
		EXPECT_EQ(c->get_pointer<NetworkType>("agent[" + std::to_string(i) + "].req.type"), &c->agent[i].req.type);
		EXPECT_EQ(c->get_pointer<int32_t>("agent[" + std::to_string(i) + "].req.cudp"), &c->agent[i].req.cudp);
		EXPECT_EQ(c->get_pointer<int32_t>("agent[" + std::to_string(i) + "].pid"), &c->agent[i].pid);
		EXPECT_EQ(c->get_pointer<double>("agent[" + std::to_string(i) + "].aprd"), &c->agent[i].aprd);
		EXPECT_EQ(c->get_pointer<uint16_t>("agent[" + std::to_string(i) + "].stateflag"), &c->agent[i].stateflag);
		EXPECT_EQ(c->get_pointer<vector<agent_request_entry>>("agent[" + std::to_string(i) + "].reqs"), &c->agent[i].reqs);
		for(size_t j = 0; j < c->agent[i].reqs.capacity(); ++j) {
			EXPECT_EQ(c->get_pointer<agent_request_entry>("agent[" + std::to_string(i) + "].reqs[" + std::to_string(j) + "]"), &c->agent[i].reqs[j]);
			EXPECT_EQ(c->get_pointer<string>("agent[" + std::to_string(i) + "].reqs[" + std::to_string(j) + "].token"), &c->agent[i].reqs[j].token);
			EXPECT_EQ(c->get_pointer<agent_request_function>("agent[" + std::to_string(i) + "].reqs[" + std::to_string(j) + "].function"), &c->agent[i].reqs[j].function);
			EXPECT_EQ(c->get_pointer<string>("agent[" + std::to_string(i) + "].reqs[" + std::to_string(j) + "].synopsis"), &c->agent[i].reqs[j].synopsis);
			EXPECT_EQ(c->get_pointer<string>("agent[" + std::to_string(i) + "].reqs[" + std::to_string(j) + "].description"), &c->agent[i].reqs[j].description);
		}
		EXPECT_EQ(c->get_pointer<beatstruc>("agent[" + std::to_string(i) + "].beat"), &c->agent[i].beat);
		EXPECT_EQ(c->get_pointer<double>("agent[" + std::to_string(i) + "].beat.utc"), &c->agent[i].beat.utc);
		//EXPECT_EQ(c->get_pointer<char[X]>("agent[" + std::to_string(i) + "].beat.node"), &c->agent[i].beat.node);
		//EXPECT_EQ(c->get_pointer<char[X]>("agent[" + std::to_string(i) + "].beat.proc"), &c->agent[i].beat.proc);
		EXPECT_EQ(c->get_pointer<NetworkType>("agent[" + std::to_string(i) + "].beat.ntype"), &c->agent[i].beat.ntype);
		//EXPECT_EQ(c->get_pointer<char[X]>("agent[" + std::to_string(i) + "].beat.addr"), &c->agent[i].beat.addr);
		EXPECT_EQ(c->get_pointer<uint16_t>("agent[" + std::to_string(i) + "].beat.port"), &c->agent[i].beat.port);
		EXPECT_EQ(c->get_pointer<uint32_t>("agent[" + std::to_string(i) + "].beat.bsz"), &c->agent[i].beat.bsz);
		EXPECT_EQ(c->get_pointer<double>("agent[" + std::to_string(i) + "].beat.bprd"), &c->agent[i].beat.bprd);
		//EXPECT_EQ(c->get_pointer<char[X]>("agent[" + std::to_string(i) + "].beat.user"), &c->agent[i].beat.user);
		EXPECT_EQ(c->get_pointer<float>("agent[" + std::to_string(i) + "].beat.cpu"), &c->agent[i].beat.cpu);
		EXPECT_EQ(c->get_pointer<float>("agent[" + std::to_string(i) + "].beat.memory"), &c->agent[i].beat.memory);
		EXPECT_EQ(c->get_pointer<double>("agent[" + std::to_string(i) + "].beat.jitter"), &c->agent[i].beat.jitter);
		EXPECT_EQ(c->get_pointer<bool>("agent[" + std::to_string(i) + "].beat.exists"), &c->agent[i].beat.exists);
	}

	// vector<eventstruc> event
	EXPECT_EQ(c->get_pointer<vector<eventstruc>>("event"), &c->event);
	for(size_t i = 0; i < c->event.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<eventstruc>("event[" + std::to_string(i) + "]"), &c->event[i]);
		EXPECT_EQ(c->get_pointer<double>("event[" + std::to_string(i) + "].utc"), &c->event[i].utc);
		EXPECT_EQ(c->get_pointer<double>("event[" + std::to_string(i) + "].utcexec"), &c->event[i].utcexec);
		//EXPECT_EQ(c->get_pointer<char[X]>("event[" + std::to_string(i) + "].node"), &c->event[i].node);
		//EXPECT_EQ(c->get_pointer<char[X]>("event[" + std::to_string(i) + "].name"), &c->event[i].name);
		//EXPECT_EQ(c->get_pointer<char[X]>("event[" + std::to_string(i) + "].user"), &c->event[i].user);
		EXPECT_EQ(c->get_pointer<uint32_t>("event[" + std::to_string(i) + "].flag"), &c->event[i].flag);
		EXPECT_EQ(c->get_pointer<uint32_t>("event[" + std::to_string(i) + "].type"), &c->event[i].type);
		EXPECT_EQ(c->get_pointer<double>("event[" + std::to_string(i) + "].value"), &c->event[i].value);
		EXPECT_EQ(c->get_pointer<double>("event[" + std::to_string(i) + "].dtime"), &c->event[i].dtime);
		EXPECT_EQ(c->get_pointer<double>("event[" + std::to_string(i) + "].ctime"), &c->event[i].ctime);
		EXPECT_EQ(c->get_pointer<float>("event[" + std::to_string(i) + "].denergy"), &c->event[i].denergy);
		EXPECT_EQ(c->get_pointer<float>("event[" + std::to_string(i) + "].cenergy"), &c->event[i].cenergy);
		EXPECT_EQ(c->get_pointer<float>("event[" + std::to_string(i) + "].dmass"), &c->event[i].dmass);
		EXPECT_EQ(c->get_pointer<float>("event[" + std::to_string(i) + "].cmass"), &c->event[i].cmass);
		EXPECT_EQ(c->get_pointer<float>("event[" + std::to_string(i) + "].dbytes"), &c->event[i].dbytes);
		EXPECT_EQ(c->get_pointer<float>("event[" + std::to_string(i) + "].cbytes"), &c->event[i].cbytes);
		EXPECT_EQ(c->get_pointer<jsonhandle>("event[" + std::to_string(i) + "].handle"), &c->event[i].handle);
		EXPECT_EQ(c->get_pointer<uint16_t>("event[" + std::to_string(i) + "].handle.hash"), &c->event[i].handle.hash);
		EXPECT_EQ(c->get_pointer<uint16_t>("event[" + std::to_string(i) + "].handle.index"), &c->event[i].handle.index);
		//EXPECT_EQ(c->get_pointer<char[X]>("event[" + std::to_string(i) + "].data"), &c->event[i].data);
		//EXPECT_EQ(c->get_pointer<char[X]>("event[" + std::to_string(i) + "].condition"), &c->event[i].condition);
	}

	// vector<targetstruc> target
	EXPECT_EQ(c->get_pointer<vector<targetstruc>>("target"), &c->target);
	for(size_t i = 0; i < c->target.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<targetstruc>("target[" + std::to_string(i) + "]"), &c->target[i]);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].utc"), &c->target[i].utc);
		//EXPECT_EQ(c->get_pointer<char[X]>("target[" + std::to_string(i) + "].name"), &c->target[i].name);
		EXPECT_EQ(c->get_pointer<uint16_t>("target[" + std::to_string(i) + "].type"), &c->target[i].type);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].azfrom"), &c->target[i].azfrom);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].elfrom"), &c->target[i].elfrom);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].azto"), &c->target[i].azto);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].elto"), &c->target[i].elto);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].range"), &c->target[i].range);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].close"), &c->target[i].close);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].min"), &c->target[i].min);
		EXPECT_EQ(c->get_pointer<locstruc>("target[" + std::to_string(i) + "].loc"), &c->target[i].loc);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.utc"), &c->target[i].loc.utc);
		EXPECT_EQ(c->get_pointer<posstruc>("target[" + std::to_string(i) + "].loc.pos"), &c->target[i].loc.pos);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.utc"), &c->target[i].loc.pos.utc);
		EXPECT_EQ(c->get_pointer<cartpos>("target[" + std::to_string(i) + "].loc.pos.icrf"), &c->target[i].loc.pos.icrf);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.icrf.utc"), &c->target[i].loc.pos.icrf.utc);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.icrf.s"), &c->target[i].loc.pos.icrf.s);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.icrf.s.col"), &c->target[i].loc.pos.icrf.s.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.icrf.s.col)/sizeof(c->target[i].loc.pos.icrf.s.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.icrf.s.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.icrf.s.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.icrf.v"), &c->target[i].loc.pos.icrf.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.icrf.v.col"), &c->target[i].loc.pos.icrf.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.icrf.v.col)/sizeof(c->target[i].loc.pos.icrf.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.icrf.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.icrf.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.icrf.a"), &c->target[i].loc.pos.icrf.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.icrf.a.col"), &c->target[i].loc.pos.icrf.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.icrf.a.col)/sizeof(c->target[i].loc.pos.icrf.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.icrf.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.icrf.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.icrf.pass"), &c->target[i].loc.pos.icrf.pass);
		EXPECT_EQ(c->get_pointer<cartpos>("target[" + std::to_string(i) + "].loc.pos.eci"), &c->target[i].loc.pos.eci);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.eci.utc"), &c->target[i].loc.pos.eci.utc);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.eci.s"), &c->target[i].loc.pos.eci.s);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.eci.s.col"), &c->target[i].loc.pos.eci.s.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.eci.s.col)/sizeof(c->target[i].loc.pos.eci.s.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.eci.s.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.eci.s.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.eci.v"), &c->target[i].loc.pos.eci.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.eci.v.col"), &c->target[i].loc.pos.eci.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.eci.v.col)/sizeof(c->target[i].loc.pos.eci.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.eci.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.eci.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.eci.a"), &c->target[i].loc.pos.eci.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.eci.a.col"), &c->target[i].loc.pos.eci.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.eci.a.col)/sizeof(c->target[i].loc.pos.eci.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.eci.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.eci.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.eci.pass"), &c->target[i].loc.pos.eci.pass);
		EXPECT_EQ(c->get_pointer<cartpos>("target[" + std::to_string(i) + "].loc.pos.sci"), &c->target[i].loc.pos.sci);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.sci.utc"), &c->target[i].loc.pos.sci.utc);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.sci.s"), &c->target[i].loc.pos.sci.s);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.sci.s.col"), &c->target[i].loc.pos.sci.s.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.sci.s.col)/sizeof(c->target[i].loc.pos.sci.s.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.sci.s.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.sci.s.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.sci.v"), &c->target[i].loc.pos.sci.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.sci.v.col"), &c->target[i].loc.pos.sci.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.sci.v.col)/sizeof(c->target[i].loc.pos.sci.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.sci.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.sci.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.sci.a"), &c->target[i].loc.pos.sci.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.sci.a.col"), &c->target[i].loc.pos.sci.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.sci.a.col)/sizeof(c->target[i].loc.pos.sci.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.sci.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.sci.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.sci.pass"), &c->target[i].loc.pos.sci.pass);
		EXPECT_EQ(c->get_pointer<cartpos>("target[" + std::to_string(i) + "].loc.pos.geoc"), &c->target[i].loc.pos.geoc);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geoc.utc"), &c->target[i].loc.pos.geoc.utc);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.geoc.s"), &c->target[i].loc.pos.geoc.s);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.geoc.s.col"), &c->target[i].loc.pos.geoc.s.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.geoc.s.col)/sizeof(c->target[i].loc.pos.geoc.s.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geoc.s.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.geoc.s.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.geoc.v"), &c->target[i].loc.pos.geoc.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.geoc.v.col"), &c->target[i].loc.pos.geoc.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.geoc.v.col)/sizeof(c->target[i].loc.pos.geoc.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geoc.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.geoc.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.geoc.a"), &c->target[i].loc.pos.geoc.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.geoc.a.col"), &c->target[i].loc.pos.geoc.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.geoc.a.col)/sizeof(c->target[i].loc.pos.geoc.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geoc.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.geoc.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.geoc.pass"), &c->target[i].loc.pos.geoc.pass);
		EXPECT_EQ(c->get_pointer<cartpos>("target[" + std::to_string(i) + "].loc.pos.selc"), &c->target[i].loc.pos.selc);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selc.utc"), &c->target[i].loc.pos.selc.utc);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.selc.s"), &c->target[i].loc.pos.selc.s);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.selc.s.col"), &c->target[i].loc.pos.selc.s.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.selc.s.col)/sizeof(c->target[i].loc.pos.selc.s.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selc.s.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.selc.s.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.selc.v"), &c->target[i].loc.pos.selc.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.selc.v.col"), &c->target[i].loc.pos.selc.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.selc.v.col)/sizeof(c->target[i].loc.pos.selc.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selc.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.selc.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.selc.a"), &c->target[i].loc.pos.selc.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.selc.a.col"), &c->target[i].loc.pos.selc.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.selc.a.col)/sizeof(c->target[i].loc.pos.selc.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selc.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.selc.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.selc.pass"), &c->target[i].loc.pos.selc.pass);
		EXPECT_EQ(c->get_pointer<geoidpos>("target[" + std::to_string(i) + "].loc.pos.geod"), &c->target[i].loc.pos.geod);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.utc"), &c->target[i].loc.pos.geod.utc);
		EXPECT_EQ(c->get_pointer<gvector>("target[" + std::to_string(i) + "].loc.pos.geod.s"), &c->target[i].loc.pos.geod.s);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.s.lat"), &c->target[i].loc.pos.geod.s.lat);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.s.lon"), &c->target[i].loc.pos.geod.s.lon);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.s.h"), &c->target[i].loc.pos.geod.s.h);
		EXPECT_EQ(c->get_pointer<gvector>("target[" + std::to_string(i) + "].loc.pos.geod.v"), &c->target[i].loc.pos.geod.v);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.v.lat"), &c->target[i].loc.pos.geod.v.lat);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.v.lon"), &c->target[i].loc.pos.geod.v.lon);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.v.h"), &c->target[i].loc.pos.geod.v.h);
		EXPECT_EQ(c->get_pointer<gvector>("target[" + std::to_string(i) + "].loc.pos.geod.a"), &c->target[i].loc.pos.geod.a);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.a.lat"), &c->target[i].loc.pos.geod.a.lat);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.a.lon"), &c->target[i].loc.pos.geod.a.lon);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geod.a.h"), &c->target[i].loc.pos.geod.a.h);
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.geod.pass"), &c->target[i].loc.pos.geod.pass);
		EXPECT_EQ(c->get_pointer<geoidpos>("target[" + std::to_string(i) + "].loc.pos.selg"), &c->target[i].loc.pos.selg);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.utc"), &c->target[i].loc.pos.selg.utc);
		EXPECT_EQ(c->get_pointer<gvector>("target[" + std::to_string(i) + "].loc.pos.selg.s"), &c->target[i].loc.pos.selg.s);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.s.lat"), &c->target[i].loc.pos.selg.s.lat);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.s.lon"), &c->target[i].loc.pos.selg.s.lon);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.s.h"), &c->target[i].loc.pos.selg.s.h);
		EXPECT_EQ(c->get_pointer<gvector>("target[" + std::to_string(i) + "].loc.pos.selg.v"), &c->target[i].loc.pos.selg.v);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.v.lat"), &c->target[i].loc.pos.selg.v.lat);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.v.lon"), &c->target[i].loc.pos.selg.v.lon);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.v.h"), &c->target[i].loc.pos.selg.v.h);
		EXPECT_EQ(c->get_pointer<gvector>("target[" + std::to_string(i) + "].loc.pos.selg.a"), &c->target[i].loc.pos.selg.a);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.a.lat"), &c->target[i].loc.pos.selg.a.lat);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.a.lon"), &c->target[i].loc.pos.selg.a.lon);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.selg.a.h"), &c->target[i].loc.pos.selg.a.h);
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.selg.pass"), &c->target[i].loc.pos.selg.pass);
		EXPECT_EQ(c->get_pointer<spherpos>("target[" + std::to_string(i) + "].loc.pos.geos"), &c->target[i].loc.pos.geos);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.utc"), &c->target[i].loc.pos.geos.utc);
		EXPECT_EQ(c->get_pointer<svector>("target[" + std::to_string(i) + "].loc.pos.geos.s"), &c->target[i].loc.pos.geos.s);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.s.phi"), &c->target[i].loc.pos.geos.s.phi);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.s.lambda"), &c->target[i].loc.pos.geos.s.lambda);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.s.r"), &c->target[i].loc.pos.geos.s.r);
		EXPECT_EQ(c->get_pointer<svector>("target[" + std::to_string(i) + "].loc.pos.geos.v"), &c->target[i].loc.pos.geos.v);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.v.phi"), &c->target[i].loc.pos.geos.v.phi);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.v.lambda"), &c->target[i].loc.pos.geos.v.lambda);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.v.r"), &c->target[i].loc.pos.geos.v.r);
		EXPECT_EQ(c->get_pointer<svector>("target[" + std::to_string(i) + "].loc.pos.geos.a"), &c->target[i].loc.pos.geos.a);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.a.phi"), &c->target[i].loc.pos.geos.a.phi);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.a.lambda"), &c->target[i].loc.pos.geos.a.lambda);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.geos.a.r"), &c->target[i].loc.pos.geos.a.r);
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.geos.pass"), &c->target[i].loc.pos.geos.pass);
		EXPECT_EQ(c->get_pointer<extrapos>("target[" + std::to_string(i) + "].loc.pos.extra"), &c->target[i].loc.pos.extra);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.utc"), &c->target[i].loc.pos.extra.utc);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.tt"), &c->target[i].loc.pos.extra.tt);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.ut"), &c->target[i].loc.pos.extra.ut);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.tdb"), &c->target[i].loc.pos.extra.tdb);
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.j2e"), &c->target[i].loc.pos.extra.j2e);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.j2e.row"), &c->target[i].loc.pos.extra.j2e.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.j2e.row)/sizeof(c->target[i].loc.pos.extra.j2e.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.j2e.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.j2e.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.j2e.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.j2e.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.j2e.row[j].col)/sizeof(c->target[i].loc.pos.extra.j2e.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.j2e.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.j2e.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.dj2e"), &c->target[i].loc.pos.extra.dj2e);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.dj2e.row"), &c->target[i].loc.pos.extra.dj2e.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.dj2e.row)/sizeof(c->target[i].loc.pos.extra.dj2e.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.dj2e.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.dj2e.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.dj2e.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.dj2e.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.dj2e.row[j].col)/sizeof(c->target[i].loc.pos.extra.dj2e.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.dj2e.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.dj2e.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.ddj2e"), &c->target[i].loc.pos.extra.ddj2e);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.ddj2e.row"), &c->target[i].loc.pos.extra.ddj2e.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.ddj2e.row)/sizeof(c->target[i].loc.pos.extra.ddj2e.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.ddj2e.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.ddj2e.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.ddj2e.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.ddj2e.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.ddj2e.row[j].col)/sizeof(c->target[i].loc.pos.extra.ddj2e.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.ddj2e.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.ddj2e.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.e2j"), &c->target[i].loc.pos.extra.e2j);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.e2j.row"), &c->target[i].loc.pos.extra.e2j.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.e2j.row)/sizeof(c->target[i].loc.pos.extra.e2j.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.e2j.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.e2j.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.e2j.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.e2j.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.e2j.row[j].col)/sizeof(c->target[i].loc.pos.extra.e2j.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.e2j.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.e2j.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.de2j"), &c->target[i].loc.pos.extra.de2j);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.de2j.row"), &c->target[i].loc.pos.extra.de2j.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.de2j.row)/sizeof(c->target[i].loc.pos.extra.de2j.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.de2j.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.de2j.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.de2j.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.de2j.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.de2j.row[j].col)/sizeof(c->target[i].loc.pos.extra.de2j.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.de2j.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.de2j.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.dde2j"), &c->target[i].loc.pos.extra.dde2j);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.dde2j.row"), &c->target[i].loc.pos.extra.dde2j.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.dde2j.row)/sizeof(c->target[i].loc.pos.extra.dde2j.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.dde2j.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.dde2j.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.dde2j.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.dde2j.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.dde2j.row[j].col)/sizeof(c->target[i].loc.pos.extra.dde2j.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.dde2j.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.dde2j.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.j2t"), &c->target[i].loc.pos.extra.j2t);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.j2t.row"), &c->target[i].loc.pos.extra.j2t.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.j2t.row)/sizeof(c->target[i].loc.pos.extra.j2t.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.j2t.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.j2t.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.j2t.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.j2t.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.j2t.row[j].col)/sizeof(c->target[i].loc.pos.extra.j2t.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.j2t.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.j2t.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.j2s"), &c->target[i].loc.pos.extra.j2s);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.j2s.row"), &c->target[i].loc.pos.extra.j2s.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.j2s.row)/sizeof(c->target[i].loc.pos.extra.j2s.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.j2s.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.j2s.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.j2s.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.j2s.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.j2s.row[j].col)/sizeof(c->target[i].loc.pos.extra.j2s.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.j2s.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.j2s.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.t2j"), &c->target[i].loc.pos.extra.t2j);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.t2j.row"), &c->target[i].loc.pos.extra.t2j.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.t2j.row)/sizeof(c->target[i].loc.pos.extra.t2j.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.t2j.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.t2j.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.t2j.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.t2j.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.t2j.row[j].col)/sizeof(c->target[i].loc.pos.extra.t2j.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.t2j.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.t2j.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.s2j"), &c->target[i].loc.pos.extra.s2j);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.s2j.row"), &c->target[i].loc.pos.extra.s2j.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.s2j.row)/sizeof(c->target[i].loc.pos.extra.s2j.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.s2j.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.s2j.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.s2j.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.s2j.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.s2j.row[j].col)/sizeof(c->target[i].loc.pos.extra.s2j.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.s2j.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.s2j.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.s2t"), &c->target[i].loc.pos.extra.s2t);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.s2t.row"), &c->target[i].loc.pos.extra.s2t.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.s2t.row)/sizeof(c->target[i].loc.pos.extra.s2t.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.s2t.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.s2t.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.s2t.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.s2t.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.s2t.row[j].col)/sizeof(c->target[i].loc.pos.extra.s2t.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.s2t.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.s2t.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.ds2t"), &c->target[i].loc.pos.extra.ds2t);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.ds2t.row"), &c->target[i].loc.pos.extra.ds2t.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.ds2t.row)/sizeof(c->target[i].loc.pos.extra.ds2t.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.ds2t.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.ds2t.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.ds2t.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.ds2t.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.ds2t.row[j].col)/sizeof(c->target[i].loc.pos.extra.ds2t.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.ds2t.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.ds2t.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.t2s"), &c->target[i].loc.pos.extra.t2s);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.t2s.row"), &c->target[i].loc.pos.extra.t2s.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.t2s.row)/sizeof(c->target[i].loc.pos.extra.t2s.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.t2s.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.t2s.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.t2s.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.t2s.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.t2s.row[j].col)/sizeof(c->target[i].loc.pos.extra.t2s.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.t2s.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.t2s.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.pos.extra.dt2s"), &c->target[i].loc.pos.extra.dt2s);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.pos.extra.dt2s.row"), &c->target[i].loc.pos.extra.dt2s.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.dt2s.row)/sizeof(c->target[i].loc.pos.extra.dt2s.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.dt2s.row[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.dt2s.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.dt2s.row[" + std::to_string(j) + "].col"), &c->target[i].loc.pos.extra.dt2s.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.pos.extra.dt2s.row[j].col)/sizeof(c->target[i].loc.pos.extra.dt2s.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.dt2s.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.pos.extra.dt2s.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<cartpos>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth"), &c->target[i].loc.pos.extra.sun2earth);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.utc"), &c->target[i].loc.pos.extra.sun2earth.utc);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.s"), &c->target[i].loc.pos.extra.sun2earth.s);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.s.col"), &c->target[i].loc.pos.extra.sun2earth.s.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.sun2earth.s.col)/sizeof(c->target[i].loc.pos.extra.sun2earth.s.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.s.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.sun2earth.s.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.v"), &c->target[i].loc.pos.extra.sun2earth.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.v.col"), &c->target[i].loc.pos.extra.sun2earth.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.sun2earth.v.col)/sizeof(c->target[i].loc.pos.extra.sun2earth.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.sun2earth.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.a"), &c->target[i].loc.pos.extra.sun2earth.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.a.col"), &c->target[i].loc.pos.extra.sun2earth.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.sun2earth.a.col)/sizeof(c->target[i].loc.pos.extra.sun2earth.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.sun2earth.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.extra.sun2earth.pass"), &c->target[i].loc.pos.extra.sun2earth.pass);
		EXPECT_EQ(c->get_pointer<cartpos>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon"), &c->target[i].loc.pos.extra.sun2moon);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.utc"), &c->target[i].loc.pos.extra.sun2moon.utc);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.s"), &c->target[i].loc.pos.extra.sun2moon.s);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.s.col"), &c->target[i].loc.pos.extra.sun2moon.s.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.sun2moon.s.col)/sizeof(c->target[i].loc.pos.extra.sun2moon.s.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.s.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.sun2moon.s.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.v"), &c->target[i].loc.pos.extra.sun2moon.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.v.col"), &c->target[i].loc.pos.extra.sun2moon.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.sun2moon.v.col)/sizeof(c->target[i].loc.pos.extra.sun2moon.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.sun2moon.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.a"), &c->target[i].loc.pos.extra.sun2moon.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.a.col"), &c->target[i].loc.pos.extra.sun2moon.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.extra.sun2moon.a.col)/sizeof(c->target[i].loc.pos.extra.sun2moon.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.extra.sun2moon.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.pos.extra.sun2moon.pass"), &c->target[i].loc.pos.extra.sun2moon.pass);
		EXPECT_EQ(c->get_pointer<uint16_t>("target[" + std::to_string(i) + "].loc.pos.extra.closest"), &c->target[i].loc.pos.extra.closest);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].loc.pos.earthsep"), &c->target[i].loc.pos.earthsep);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].loc.pos.moonsep"), &c->target[i].loc.pos.moonsep);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].loc.pos.sunsize"), &c->target[i].loc.pos.sunsize);
		EXPECT_EQ(c->get_pointer<float>("target[" + std::to_string(i) + "].loc.pos.sunradiance"), &c->target[i].loc.pos.sunradiance);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.pos.bearth"), &c->target[i].loc.pos.bearth);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.pos.bearth.col"), &c->target[i].loc.pos.bearth.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.pos.bearth.col)/sizeof(c->target[i].loc.pos.bearth.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.bearth.col[" + std::to_string(j) + "]"), &c->target[i].loc.pos.bearth.col[j]);
		}
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.pos.orbit"), &c->target[i].loc.pos.orbit);
		EXPECT_EQ(c->get_pointer<attstruc>("target[" + std::to_string(i) + "].loc.att"), &c->target[i].loc.att);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.utc"), &c->target[i].loc.att.utc);
		EXPECT_EQ(c->get_pointer<qatt>("target[" + std::to_string(i) + "].loc.att.topo"), &c->target[i].loc.att.topo);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.topo.utc"), &c->target[i].loc.att.topo.utc);
		EXPECT_EQ(c->get_pointer<quaternion>("target[" + std::to_string(i) + "].loc.att.topo.s"), &c->target[i].loc.att.topo.s);
		EXPECT_EQ(c->get_pointer<cvector>("target[" + std::to_string(i) + "].loc.att.topo.s.d"), &c->target[i].loc.att.topo.s.d);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.topo.s.d.x"), &c->target[i].loc.att.topo.s.d.x);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.topo.s.d.y"), &c->target[i].loc.att.topo.s.d.y);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.topo.s.d.z"), &c->target[i].loc.att.topo.s.d.z);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.topo.s.w"), &c->target[i].loc.att.topo.s.w);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.topo.v"), &c->target[i].loc.att.topo.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.topo.v.col"), &c->target[i].loc.att.topo.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.topo.v.col)/sizeof(c->target[i].loc.att.topo.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.topo.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.topo.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.topo.a"), &c->target[i].loc.att.topo.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.topo.a.col"), &c->target[i].loc.att.topo.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.topo.a.col)/sizeof(c->target[i].loc.att.topo.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.topo.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.topo.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.att.topo.pass"), &c->target[i].loc.att.topo.pass);
		EXPECT_EQ(c->get_pointer<qatt>("target[" + std::to_string(i) + "].loc.att.lvlh"), &c->target[i].loc.att.lvlh);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.lvlh.utc"), &c->target[i].loc.att.lvlh.utc);
		EXPECT_EQ(c->get_pointer<quaternion>("target[" + std::to_string(i) + "].loc.att.lvlh.s"), &c->target[i].loc.att.lvlh.s);
		EXPECT_EQ(c->get_pointer<cvector>("target[" + std::to_string(i) + "].loc.att.lvlh.s.d"), &c->target[i].loc.att.lvlh.s.d);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.lvlh.s.d.x"), &c->target[i].loc.att.lvlh.s.d.x);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.lvlh.s.d.y"), &c->target[i].loc.att.lvlh.s.d.y);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.lvlh.s.d.z"), &c->target[i].loc.att.lvlh.s.d.z);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.lvlh.s.w"), &c->target[i].loc.att.lvlh.s.w);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.lvlh.v"), &c->target[i].loc.att.lvlh.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.lvlh.v.col"), &c->target[i].loc.att.lvlh.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.lvlh.v.col)/sizeof(c->target[i].loc.att.lvlh.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.lvlh.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.lvlh.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.lvlh.a"), &c->target[i].loc.att.lvlh.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.lvlh.a.col"), &c->target[i].loc.att.lvlh.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.lvlh.a.col)/sizeof(c->target[i].loc.att.lvlh.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.lvlh.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.lvlh.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.att.lvlh.pass"), &c->target[i].loc.att.lvlh.pass);
		EXPECT_EQ(c->get_pointer<qatt>("target[" + std::to_string(i) + "].loc.att.geoc"), &c->target[i].loc.att.geoc);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.geoc.utc"), &c->target[i].loc.att.geoc.utc);
		EXPECT_EQ(c->get_pointer<quaternion>("target[" + std::to_string(i) + "].loc.att.geoc.s"), &c->target[i].loc.att.geoc.s);
		EXPECT_EQ(c->get_pointer<cvector>("target[" + std::to_string(i) + "].loc.att.geoc.s.d"), &c->target[i].loc.att.geoc.s.d);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.geoc.s.d.x"), &c->target[i].loc.att.geoc.s.d.x);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.geoc.s.d.y"), &c->target[i].loc.att.geoc.s.d.y);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.geoc.s.d.z"), &c->target[i].loc.att.geoc.s.d.z);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.geoc.s.w"), &c->target[i].loc.att.geoc.s.w);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.geoc.v"), &c->target[i].loc.att.geoc.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.geoc.v.col"), &c->target[i].loc.att.geoc.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.geoc.v.col)/sizeof(c->target[i].loc.att.geoc.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.geoc.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.geoc.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.geoc.a"), &c->target[i].loc.att.geoc.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.geoc.a.col"), &c->target[i].loc.att.geoc.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.geoc.a.col)/sizeof(c->target[i].loc.att.geoc.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.geoc.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.geoc.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.att.geoc.pass"), &c->target[i].loc.att.geoc.pass);
		EXPECT_EQ(c->get_pointer<qatt>("target[" + std::to_string(i) + "].loc.att.selc"), &c->target[i].loc.att.selc);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.selc.utc"), &c->target[i].loc.att.selc.utc);
		EXPECT_EQ(c->get_pointer<quaternion>("target[" + std::to_string(i) + "].loc.att.selc.s"), &c->target[i].loc.att.selc.s);
		EXPECT_EQ(c->get_pointer<cvector>("target[" + std::to_string(i) + "].loc.att.selc.s.d"), &c->target[i].loc.att.selc.s.d);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.selc.s.d.x"), &c->target[i].loc.att.selc.s.d.x);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.selc.s.d.y"), &c->target[i].loc.att.selc.s.d.y);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.selc.s.d.z"), &c->target[i].loc.att.selc.s.d.z);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.selc.s.w"), &c->target[i].loc.att.selc.s.w);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.selc.v"), &c->target[i].loc.att.selc.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.selc.v.col"), &c->target[i].loc.att.selc.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.selc.v.col)/sizeof(c->target[i].loc.att.selc.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.selc.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.selc.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.selc.a"), &c->target[i].loc.att.selc.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.selc.a.col"), &c->target[i].loc.att.selc.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.selc.a.col)/sizeof(c->target[i].loc.att.selc.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.selc.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.selc.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.att.selc.pass"), &c->target[i].loc.att.selc.pass);
		EXPECT_EQ(c->get_pointer<qatt>("target[" + std::to_string(i) + "].loc.att.icrf"), &c->target[i].loc.att.icrf);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.icrf.utc"), &c->target[i].loc.att.icrf.utc);
		EXPECT_EQ(c->get_pointer<quaternion>("target[" + std::to_string(i) + "].loc.att.icrf.s"), &c->target[i].loc.att.icrf.s);
		EXPECT_EQ(c->get_pointer<cvector>("target[" + std::to_string(i) + "].loc.att.icrf.s.d"), &c->target[i].loc.att.icrf.s.d);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.icrf.s.d.x"), &c->target[i].loc.att.icrf.s.d.x);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.icrf.s.d.y"), &c->target[i].loc.att.icrf.s.d.y);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.icrf.s.d.z"), &c->target[i].loc.att.icrf.s.d.z);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.icrf.s.w"), &c->target[i].loc.att.icrf.s.w);
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.icrf.v"), &c->target[i].loc.att.icrf.v);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.icrf.v.col"), &c->target[i].loc.att.icrf.v.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.icrf.v.col)/sizeof(c->target[i].loc.att.icrf.v.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.icrf.v.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.icrf.v.col[j]);
		}
		EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.icrf.a"), &c->target[i].loc.att.icrf.a);
		//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.icrf.a.col"), &c->target[i].loc.att.icrf.a.col);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.icrf.a.col)/sizeof(c->target[i].loc.att.icrf.a.col[0]); ++j) {
			EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.icrf.a.col[" + std::to_string(j) + "]"), &c->target[i].loc.att.icrf.a.col[j]);
		}
		EXPECT_EQ(c->get_pointer<uint32_t>("target[" + std::to_string(i) + "].loc.att.icrf.pass"), &c->target[i].loc.att.icrf.pass);
		EXPECT_EQ(c->get_pointer<extraatt>("target[" + std::to_string(i) + "].loc.att.extra"), &c->target[i].loc.att.extra);
		EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.extra.utc"), &c->target[i].loc.att.extra.utc);
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.att.extra.j2b"), &c->target[i].loc.att.extra.j2b);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.att.extra.j2b.row"), &c->target[i].loc.att.extra.j2b.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.extra.j2b.row)/sizeof(c->target[i].loc.att.extra.j2b.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.extra.j2b.row[" + std::to_string(j) + "]"), &c->target[i].loc.att.extra.j2b.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.extra.j2b.row[" + std::to_string(j) + "].col"), &c->target[i].loc.att.extra.j2b.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.att.extra.j2b.row[j].col)/sizeof(c->target[i].loc.att.extra.j2b.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.extra.j2b.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.att.extra.j2b.row[j].col[k]);
			}
		}
		EXPECT_EQ(c->get_pointer<rmatrix>("target[" + std::to_string(i) + "].loc.att.extra.b2j"), &c->target[i].loc.att.extra.b2j);
		//EXPECT_EQ(c->get_pointer<rvector[X]>("target[" + std::to_string(i) + "].loc.att.extra.b2j.row"), &c->target[i].loc.att.extra.b2j.row);
		for(size_t j = 0; j < sizeof(c->target[i].loc.att.extra.b2j.row)/sizeof(c->target[i].loc.att.extra.b2j.row[0]); ++j) {
			EXPECT_EQ(c->get_pointer<rvector>("target[" + std::to_string(i) + "].loc.att.extra.b2j.row[" + std::to_string(j) + "]"), &c->target[i].loc.att.extra.b2j.row[j]);
			//EXPECT_EQ(c->get_pointer<double[X]>("target[" + std::to_string(i) + "].loc.att.extra.b2j.row[" + std::to_string(j) + "].col"), &c->target[i].loc.att.extra.b2j.row[j].col);
			for(size_t k = 0; k < sizeof(c->target[i].loc.att.extra.b2j.row[j].col)/sizeof(c->target[i].loc.att.extra.b2j.row[j].col[0]); ++k) {
				EXPECT_EQ(c->get_pointer<double>("target[" + std::to_string(i) + "].loc.att.extra.b2j.row[" + std::to_string(j) + "].col[" + std::to_string(k) + "]"), &c->target[i].loc.att.extra.b2j.row[j].col[k]);
			}
		}
	}

	// vector<userstruc> user
	EXPECT_EQ(c->get_pointer<vector<userstruc>>("user"), &c->user);
	for(size_t i = 0; i < c->user.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<userstruc>("user[" + std::to_string(i) + "]"), &c->user[i]);
		EXPECT_EQ(c->get_pointer<string>("user[" + std::to_string(i) + "].name"), &c->user[i].name);
		EXPECT_EQ(c->get_pointer<string>("user[" + std::to_string(i) + "].node"), &c->user[i].node);
		EXPECT_EQ(c->get_pointer<string>("user[" + std::to_string(i) + "].tool"), &c->user[i].tool);
		EXPECT_EQ(c->get_pointer<string>("user[" + std::to_string(i) + "].cpu"), &c->user[i].cpu);
	}

	// vector<tlestruc> tle
	EXPECT_EQ(c->get_pointer<vector<tlestruc>>("tle"), &c->tle);
	for(size_t i = 0; i < c->tle.capacity(); ++i) {
		EXPECT_EQ(c->get_pointer<tlestruc>("tle[" + std::to_string(i) + "]"), &c->tle[i]);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].utc"), &c->tle[i].utc);
		//EXPECT_EQ(c->get_pointer<char[X]>("tle[" + std::to_string(i) + "].name"), &c->tle[i].name);
		EXPECT_EQ(c->get_pointer<uint16_t>("tle[" + std::to_string(i) + "].snumber"), &c->tle[i].snumber);
		//EXPECT_EQ(c->get_pointer<char[X]>("tle[" + std::to_string(i) + "].id"), &c->tle[i].id);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].bstar"), &c->tle[i].bstar);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].i"), &c->tle[i].i);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].raan"), &c->tle[i].raan);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].e"), &c->tle[i].e);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].ap"), &c->tle[i].ap);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].ma"), &c->tle[i].ma);
		EXPECT_EQ(c->get_pointer<double>("tle[" + std::to_string(i) + "].mm"), &c->tle[i].mm);
		EXPECT_EQ(c->get_pointer<uint32_t>("tle[" + std::to_string(i) + "].orbit"), &c->tle[i].orbit);
	}

	// jsonnode json
	EXPECT_EQ(c->get_pointer<jsonnode>("json"), &c->json);
	EXPECT_EQ(c->get_pointer<string>("json.name"), &c->json.name);
	EXPECT_EQ(c->get_pointer<string>("json.node"), &c->json.node);
	EXPECT_EQ(c->get_pointer<string>("json.state"), &c->json.state);
	EXPECT_EQ(c->get_pointer<string>("json.utcstart"), &c->json.utcstart);
	EXPECT_EQ(c->get_pointer<string>("json.vertexs"), &c->json.vertexs);
	EXPECT_EQ(c->get_pointer<string>("json.faces"), &c->json.faces);
	EXPECT_EQ(c->get_pointer<string>("json.pieces"), &c->json.pieces);
	EXPECT_EQ(c->get_pointer<string>("json.devgen"), &c->json.devgen);
	EXPECT_EQ(c->get_pointer<string>("json.devspec"), &c->json.devspec);
	EXPECT_EQ(c->get_pointer<string>("json.ports"), &c->json.ports);
	EXPECT_EQ(c->get_pointer<string>("json.targets"), &c->json.targets);


}

#endif
