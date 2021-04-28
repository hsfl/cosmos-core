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
	EXPECT_EQ(c->json.aliases, "");
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
    EXPECT_EQ(c->get_pointer<vector<devicestruc*>>("device"), &c->device);
	for(size_t i = 0; i < c->device.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<devicestruc>("device[" + std::to_string(i) + "]"), c->device[i]);
        EXPECT_EQ(c->get_pointer<bool>("device[" + std::to_string(i) + "].enabled"), &c->device[i]->enabled);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].type"), &c->device[i]->type);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].model"), &c->device[i]->model);
        EXPECT_EQ(c->get_pointer<uint32_t>("device[" + std::to_string(i) + "].flag"), &c->device[i]->flag);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].addr"), &c->device[i]->addr);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].cidx"), &c->device[i]->cidx);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all->didx"), &c->device[i]->didx);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].all->pidx"), &c->device[i]->pidx);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].bidx"), &c->device[i]->bidx);
        EXPECT_EQ(c->get_pointer<uint16_t>("device[" + std::to_string(i) + "].portidx"), &c->device[i]->portidx);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].namp"), &c->device[i]->namp);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].nvolt"), &c->device[i]->nvolt);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].amp"), &c->device[i]->amp);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].volt"), &c->device[i]->volt);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].power"), &c->device[i]->power);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].energy"), &c->device[i]->energy);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].drate"), &c->device[i]->drate);
        EXPECT_EQ(c->get_pointer<float>("device[" + std::to_string(i) + "].temp"), &c->device[i]->temp);
        EXPECT_EQ(c->get_pointer<double>("device[" + std::to_string(i) + "].utc"), &c->device[i]->utc);
    }
    for(size_t i = 0; i < c->devspec.ant.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<antstruc>("devspec.ant[" + std::to_string(i) + "]"), &c->devspec.ant[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.ant[" + std::to_string(i) + "].align"), &c->devspec.ant[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.ant[" + std::to_string(i) + "].align.d"), &c->devspec.ant[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.ant[" + std::to_string(i) + "].align.d.x"), &c->devspec.ant[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.ant[" + std::to_string(i) + "].align.d.y"), &c->devspec.ant[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.ant[" + std::to_string(i) + "].align.d.z"), &c->devspec.ant[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.ant[" + std::to_string(i) + "].align.w"), &c->devspec.ant[i].align.w);
        EXPECT_EQ(c->get_pointer<float>("devspec.ant[" + std::to_string(i) + "].azim"), &c->devspec.ant[i].azim);
        EXPECT_EQ(c->get_pointer<float>("devspec.ant[" + std::to_string(i) + "].elev"), &c->devspec.ant[i].elev);
        EXPECT_EQ(c->get_pointer<float>("devspec.ant[" + std::to_string(i) + "].minelev"), &c->devspec.ant[i].minelev);
        EXPECT_EQ(c->get_pointer<float>("devspec.ant[" + std::to_string(i) + "].maxelev"), &c->devspec.ant[i].maxelev);
        EXPECT_EQ(c->get_pointer<float>("devspec.ant[" + std::to_string(i) + "].minazim"), &c->devspec.ant[i].minazim);
        EXPECT_EQ(c->get_pointer<float>("devspec.ant[" + std::to_string(i) + "].maxazim"), &c->devspec.ant[i].maxazim);
        EXPECT_EQ(c->get_pointer<float>("devspec.ant[" + std::to_string(i) + "].threshelev"), &c->devspec.ant[i].threshelev);
    }
    for(size_t i = 0; i < c->devspec.batt.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<battstruc>("devspec.batt[" + std::to_string(i) + "]"), &c->devspec.batt[i]);
        EXPECT_EQ(c->get_pointer<float>("devspec.batt[" + std::to_string(i) + "].capacity"), &c->devspec.batt[i].capacity);
        EXPECT_EQ(c->get_pointer<float>("devspec.batt[" + std::to_string(i) + "].efficiency"), &c->devspec.batt[i].efficiency);
        EXPECT_EQ(c->get_pointer<float>("devspec.batt[" + std::to_string(i) + "].charge"), &c->devspec.batt[i].charge);
        EXPECT_EQ(c->get_pointer<float>("devspec.batt[" + std::to_string(i) + "].r_in"), &c->devspec.batt[i].r_in);
        EXPECT_EQ(c->get_pointer<float>("devspec.batt[" + std::to_string(i) + "].r_out"), &c->devspec.batt[i].r_out);
        EXPECT_EQ(c->get_pointer<float>("devspec.batt[" + std::to_string(i) + "].percentage"), &c->devspec.batt[i].percentage);
        EXPECT_EQ(c->get_pointer<float>("devspec.batt[" + std::to_string(i) + "].time_remaining"), &c->devspec.batt[i].time_remaining);
    }
    for(size_t i = 0; i < c->devspec.bcreg.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<bcregstruc>("devspec.bcreg[" + std::to_string(i) + "]"), &c->devspec.bcreg[i]);
    }
    for(size_t i = 0; i < c->devspec.bus.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<busstruc>("devspec.bus[" + std::to_string(i) + "]"), &c->devspec.bus[i]);
        EXPECT_EQ(c->get_pointer<float>("devspec.bus[" + std::to_string(i) + "].wdt"), &c->devspec.bus[i].wdt);
    }
    for(size_t i = 0; i < c->devspec.cam.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<camstruc>("devspec.cam[" + std::to_string(i) + "]"), &c->devspec.cam[i]);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.cam[" + std::to_string(i) + "].pwidth"), &c->devspec.cam[i].pwidth);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.cam[" + std::to_string(i) + "].pheight"), &c->devspec.cam[i].pheight);
        EXPECT_EQ(c->get_pointer<float>("devspec.cam[" + std::to_string(i) + "].width"), &c->devspec.cam[i].width);
        EXPECT_EQ(c->get_pointer<float>("devspec.cam[" + std::to_string(i) + "].height"), &c->devspec.cam[i].height);
        EXPECT_EQ(c->get_pointer<float>("devspec.cam[" + std::to_string(i) + "].flength"), &c->devspec.cam[i].flength);
    }
    for(size_t i = 0; i < c->devspec.cpu.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<cpustruc>("devspec.cpu[" + std::to_string(i) + "]"), &c->devspec.cpu[i]);
        EXPECT_EQ(c->get_pointer<uint32_t>("devspec.cpu[" + std::to_string(i) + "].uptime"), &c->devspec.cpu[i].uptime);
        EXPECT_EQ(c->get_pointer<float>("devspec.cpu[" + std::to_string(i) + "].load"), &c->devspec.cpu[i].load);
        EXPECT_EQ(c->get_pointer<float>("devspec.cpu[" + std::to_string(i) + "].maxload"), &c->devspec.cpu[i].maxload);
        EXPECT_EQ(c->get_pointer<float>("devspec.cpu[" + std::to_string(i) + "].maxgib"), &c->devspec.cpu[i].maxgib);
        EXPECT_EQ(c->get_pointer<float>("devspec.cpu[" + std::to_string(i) + "].gib"), &c->devspec.cpu[i].gib);
        EXPECT_EQ(c->get_pointer<uint32_t>("devspec.cpu[" + std::to_string(i) + "].boot_count"), &c->devspec.cpu[i].boot_count);
    }
    for(size_t i = 0; i < c->devspec.disk.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<diskstruc>("devspec.disk[" + std::to_string(i) + "]"), &c->devspec.disk[i]);
        EXPECT_EQ(c->get_pointer<float>("devspec.disk[" + std::to_string(i) + "].maxgib"), &c->devspec.disk[i].maxgib);
        EXPECT_EQ(c->get_pointer<float>("devspec.disk[" + std::to_string(i) + "].gib"), &c->devspec.disk[i].gib);
        //EXPECT_EQ(c->get_pointer<char[X]>("devspec.disk[" + std::to_string(i) + "].path"), &c->devspec.disk[i].path);
    }
    for(size_t i = 0; i < c->devspec.gps.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<gpsstruc>("devspec.gps[" + std::to_string(i) + "]"), &c->devspec.gps[i]);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dutc"), &c->devspec.gps[i].dutc);
        EXPECT_EQ(c->get_pointer<rvector>("devspec.gps[" + std::to_string(i) + "].geocs"), &c->devspec.gps[i].geocs);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.gps[" + std::to_string(i) + "].geocs.col"), &c->devspec.gps[i].geocs.col);
        for(size_t j = 0; j < sizeof(c->devspec.gps[i].geocs.col)/sizeof(c->devspec.gps[i].geocs.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geocs.col[" + std::to_string(j) + "]"), &c->devspec.gps[i].geocs.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.gps[" + std::to_string(i) + "].geocv"), &c->devspec.gps[i].geocv);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.gps[" + std::to_string(i) + "].geocv.col"), &c->devspec.gps[i].geocv.col);
        for(size_t j = 0; j < sizeof(c->devspec.gps[i].geocv.col)/sizeof(c->devspec.gps[i].geocv.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geocv.col[" + std::to_string(j) + "]"), &c->devspec.gps[i].geocv.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.gps[" + std::to_string(i) + "].dgeocs"), &c->devspec.gps[i].dgeocs);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.gps[" + std::to_string(i) + "].dgeocs.col"), &c->devspec.gps[i].dgeocs.col);
        for(size_t j = 0; j < sizeof(c->devspec.gps[i].dgeocs.col)/sizeof(c->devspec.gps[i].dgeocs.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeocs.col[" + std::to_string(j) + "]"), &c->devspec.gps[i].dgeocs.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.gps[" + std::to_string(i) + "].dgeocv"), &c->devspec.gps[i].dgeocv);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.gps[" + std::to_string(i) + "].dgeocv.col"), &c->devspec.gps[i].dgeocv.col);
        for(size_t j = 0; j < sizeof(c->devspec.gps[i].dgeocv.col)/sizeof(c->devspec.gps[i].dgeocv.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeocv.col[" + std::to_string(j) + "]"), &c->devspec.gps[i].dgeocv.col[j]);
		}
        EXPECT_EQ(c->get_pointer<gvector>("devspec.gps[" + std::to_string(i) + "].geods"), &c->devspec.gps[i].geods);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geods.lat"), &c->devspec.gps[i].geods.lat);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geods.lon"), &c->devspec.gps[i].geods.lon);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geods.h"), &c->devspec.gps[i].geods.h);
        EXPECT_EQ(c->get_pointer<gvector>("devspec.gps[" + std::to_string(i) + "].geodv"), &c->devspec.gps[i].geodv);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geodv.lat"), &c->devspec.gps[i].geodv.lat);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geodv.lon"), &c->devspec.gps[i].geodv.lon);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].geodv.h"), &c->devspec.gps[i].geodv.h);
        EXPECT_EQ(c->get_pointer<gvector>("devspec.gps[" + std::to_string(i) + "].dgeods"), &c->devspec.gps[i].dgeods);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeods.lat"), &c->devspec.gps[i].dgeods.lat);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeods.lon"), &c->devspec.gps[i].dgeods.lon);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeods.h"), &c->devspec.gps[i].dgeods.h);
        EXPECT_EQ(c->get_pointer<gvector>("devspec.gps[" + std::to_string(i) + "].dgeodv"), &c->devspec.gps[i].dgeodv);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeodv.lat"), &c->devspec.gps[i].dgeodv.lat);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeodv.lon"), &c->devspec.gps[i].dgeodv.lon);
        EXPECT_EQ(c->get_pointer<double>("devspec.gps[" + std::to_string(i) + "].dgeodv.h"), &c->devspec.gps[i].dgeodv.h);
        EXPECT_EQ(c->get_pointer<float>("devspec.gps[" + std::to_string(i) + "].heading"), &c->devspec.gps[i].heading);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.gps[" + std::to_string(i) + "].sats_used"), &c->devspec.gps[i].sats_used);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.gps[" + std::to_string(i) + "].sats_visible"), &c->devspec.gps[i].sats_visible);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.gps[" + std::to_string(i) + "].time_status"), &c->devspec.gps[i].time_status);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.gps[" + std::to_string(i) + "].position_type"), &c->devspec.gps[i].position_type);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.gps[" + std::to_string(i) + "].solution_status"), &c->devspec.gps[i].solution_status);
    }
    for(size_t i = 0; i < c->devspec.htr.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<htrstruc>("devspec.htr[" + std::to_string(i) + "]"), &c->devspec.htr[i]);
        EXPECT_EQ(c->get_pointer<bool>("devspec.htr[" + std::to_string(i) + "].state"), &c->devspec.htr[i].state);
        EXPECT_EQ(c->get_pointer<float>("devspec.htr[" + std::to_string(i) + "].setvertex"), &c->devspec.htr[i].setvertex);
    }
    for(size_t i = 0; i < c->devspec.imu.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<imustruc>("devspec.imu[" + std::to_string(i) + "]"), &c->devspec.imu[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.imu[" + std::to_string(i) + "].align"), &c->devspec.imu[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.imu[" + std::to_string(i) + "].align.d"), &c->devspec.imu[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].align.d.x"), &c->devspec.imu[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].align.d.y"), &c->devspec.imu[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].align.d.z"), &c->devspec.imu[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].align.w"), &c->devspec.imu[i].align.w);
        EXPECT_EQ(c->get_pointer<rvector>("devspec.imu[" + std::to_string(i) + "].accel"), &c->devspec.imu[i].accel);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.imu[" + std::to_string(i) + "].accel.col"), &c->devspec.imu[i].accel.col);
        for(size_t j = 0; j < sizeof(c->devspec.imu[i].accel.col)/sizeof(c->devspec.imu[i].accel.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].accel.col[" + std::to_string(j) + "]"), &c->devspec.imu[i].accel.col[j]);
		}
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.imu[" + std::to_string(i) + "].theta"), &c->devspec.imu[i].theta);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.imu[" + std::to_string(i) + "].theta.d"), &c->devspec.imu[i].theta.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].theta.d.x"), &c->devspec.imu[i].theta.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].theta.d.y"), &c->devspec.imu[i].theta.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].theta.d.z"), &c->devspec.imu[i].theta.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].theta.w"), &c->devspec.imu[i].theta.w);
        EXPECT_EQ(c->get_pointer<avector>("devspec.imu[" + std::to_string(i) + "].euler"), &c->devspec.imu[i].euler);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].euler.h"), &c->devspec.imu[i].euler.h);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].euler.e"), &c->devspec.imu[i].euler.e);
        EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].euler.b"), &c->devspec.imu[i].euler.b);
        EXPECT_EQ(c->get_pointer<rvector>("devspec.imu[" + std::to_string(i) + "].omega"), &c->devspec.imu[i].omega);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.imu[" + std::to_string(i) + "].omega.col"), &c->devspec.imu[i].omega.col);
        for(size_t j = 0; j < sizeof(c->devspec.imu[i].omega.col)/sizeof(c->devspec.imu[i].omega.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].omega.col[" + std::to_string(j) + "]"), &c->devspec.imu[i].omega.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.imu[" + std::to_string(i) + "].alpha"), &c->devspec.imu[i].alpha);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.imu[" + std::to_string(i) + "].alpha.col"), &c->devspec.imu[i].alpha.col);
        for(size_t j = 0; j < sizeof(c->devspec.imu[i].alpha.col)/sizeof(c->devspec.imu[i].alpha.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].alpha.col[" + std::to_string(j) + "]"), &c->devspec.imu[i].alpha.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.imu[" + std::to_string(i) + "].mag"), &c->devspec.imu[i].mag);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.imu[" + std::to_string(i) + "].mag.col"), &c->devspec.imu[i].mag.col);
        for(size_t j = 0; j < sizeof(c->devspec.imu[i].mag.col)/sizeof(c->devspec.imu[i].mag.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].mag.col[" + std::to_string(j) + "]"), &c->devspec.imu[i].mag.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.imu[" + std::to_string(i) + "].bdot"), &c->devspec.imu[i].bdot);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.imu[" + std::to_string(i) + "].bdot.col"), &c->devspec.imu[i].bdot.col);
        for(size_t j = 0; j < sizeof(c->devspec.imu[i].bdot.col)/sizeof(c->devspec.imu[i].bdot.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.imu[" + std::to_string(i) + "].bdot.col[" + std::to_string(j) + "]"), &c->devspec.imu[i].bdot.col[j]);
		}
    }
    for(size_t i = 0; i < c->devspec.mcc.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<mccstruc>("devspec.mcc[" + std::to_string(i) + "]"), &c->devspec.mcc[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.mcc[" + std::to_string(i) + "].align"), &c->devspec.mcc[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.mcc[" + std::to_string(i) + "].align.d"), &c->devspec.mcc[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].align.d.x"), &c->devspec.mcc[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].align.d.y"), &c->devspec.mcc[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].align.d.z"), &c->devspec.mcc[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].align.w"), &c->devspec.mcc[i].align.w);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.mcc[" + std::to_string(i) + "].q"), &c->devspec.mcc[i].q);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.mcc[" + std::to_string(i) + "].q.d"), &c->devspec.mcc[i].q.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].q.d.x"), &c->devspec.mcc[i].q.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].q.d.y"), &c->devspec.mcc[i].q.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].q.d.z"), &c->devspec.mcc[i].q.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].q.w"), &c->devspec.mcc[i].q.w);
        EXPECT_EQ(c->get_pointer<rvector>("devspec.mcc[" + std::to_string(i) + "].o"), &c->devspec.mcc[i].o);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.mcc[" + std::to_string(i) + "].o.col"), &c->devspec.mcc[i].o.col);
        for(size_t j = 0; j < sizeof(c->devspec.mcc[i].o.col)/sizeof(c->devspec.mcc[i].o.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].o.col[" + std::to_string(j) + "]"), &c->devspec.mcc[i].o.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.mcc[" + std::to_string(i) + "].a"), &c->devspec.mcc[i].a);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.mcc[" + std::to_string(i) + "].a.col"), &c->devspec.mcc[i].a.col);
        for(size_t j = 0; j < sizeof(c->devspec.mcc[i].a.col)/sizeof(c->devspec.mcc[i].a.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.mcc[" + std::to_string(i) + "].a.col[" + std::to_string(j) + "]"), &c->devspec.mcc[i].a.col[j]);
		}
    }
    for(size_t i = 0; i < c->devspec.motr.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<motrstruc>("devspec.motr[" + std::to_string(i) + "]"), &c->devspec.motr[i]);
        EXPECT_EQ(c->get_pointer<float>("devspec.motr[" + std::to_string(i) + "].max"), &c->devspec.motr[i].max);
        EXPECT_EQ(c->get_pointer<float>("devspec.motr[" + std::to_string(i) + "].rat"), &c->devspec.motr[i].rat);
        EXPECT_EQ(c->get_pointer<float>("devspec.motr[" + std::to_string(i) + "].spd"), &c->devspec.motr[i].spd);
    }
    for(size_t i = 0; i < c->devspec.mtr.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<mtrstruc>("devspec.mtr[" + std::to_string(i) + "]"), &c->devspec.mtr[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.mtr[" + std::to_string(i) + "].align"), &c->devspec.mtr[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.mtr[" + std::to_string(i) + "].align.d"), &c->devspec.mtr[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.mtr[" + std::to_string(i) + "].align.d.x"), &c->devspec.mtr[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.mtr[" + std::to_string(i) + "].align.d.y"), &c->devspec.mtr[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.mtr[" + std::to_string(i) + "].align.d.z"), &c->devspec.mtr[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.mtr[" + std::to_string(i) + "].align.w"), &c->devspec.mtr[i].align.w);
        //EXPECT_EQ(c->get_pointer<float[X]>("devspec.mtr[" + std::to_string(i) + "].npoly"), &c->devspec.mtr[i].npoly);
        for(size_t j = 0; j < sizeof(c->devspec.mtr[i].npoly)/sizeof(c->devspec.mtr[i].npoly[0]); ++j) {
            EXPECT_EQ(c->get_pointer<float>("devspec.mtr[" + std::to_string(i) + "].npoly[" + std::to_string(j) + "]"), &c->devspec.mtr[i].npoly[j]);
		}
        //EXPECT_EQ(c->get_pointer<float[X]>("devspec.mtr[" + std::to_string(i) + "].ppoly"), &c->devspec.mtr[i].ppoly);
        for(size_t j = 0; j < sizeof(c->devspec.mtr[i].ppoly)/sizeof(c->devspec.mtr[i].ppoly[0]); ++j) {
            EXPECT_EQ(c->get_pointer<float>("devspec.mtr[" + std::to_string(i) + "].ppoly[" + std::to_string(j) + "]"), &c->devspec.mtr[i].ppoly[j]);
		}
        EXPECT_EQ(c->get_pointer<float>("devspec.mtr[" + std::to_string(i) + "].mxmom"), &c->devspec.mtr[i].mxmom);
        EXPECT_EQ(c->get_pointer<float>("devspec.mtr[" + std::to_string(i) + "].tc"), &c->devspec.mtr[i].tc);
        EXPECT_EQ(c->get_pointer<float>("devspec.mtr[" + std::to_string(i) + "].rmom"), &c->devspec.mtr[i].rmom);
        EXPECT_EQ(c->get_pointer<float>("devspec.mtr[" + std::to_string(i) + "].mom"), &c->devspec.mtr[i].mom);
    }
    for(size_t i = 0; i < c->devspec.pload.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<ploadstruc>("devspec.pload[" + std::to_string(i) + "]"), &c->devspec.pload[i]);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.pload[" + std::to_string(i) + "].key_cnt"), &c->devspec.pload[i].key_cnt);
        //EXPECT_EQ(c->get_pointer<uint16_t[X]>("devspec.pload[" + std::to_string(i) + "].keyidx"), &c->devspec.pload[i].keyidx);
        for(size_t j = 0; j < sizeof(c->devspec.pload[i].keyidx)/sizeof(c->devspec.pload[i].keyidx[0]); ++j) {
            EXPECT_EQ(c->get_pointer<uint16_t>("devspec.pload[" + std::to_string(i) + "].keyidx[" + std::to_string(j) + "]"), &c->devspec.pload[i].keyidx[j]);
		}
        //EXPECT_EQ(c->get_pointer<float[X]>("devspec.pload[" + std::to_string(i) + "].keyval"), &c->devspec.pload[i].keyval);
        for(size_t j = 0; j < sizeof(c->devspec.pload[i].keyval)/sizeof(c->devspec.pload[i].keyval[0]); ++j) {
            EXPECT_EQ(c->get_pointer<float>("devspec.pload[" + std::to_string(i) + "].keyval[" + std::to_string(j) + "]"), &c->devspec.pload[i].keyval[j]);
		}
    }
    for(size_t i = 0; i < c->devspec.prop.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<propstruc>("devspec.prop[" + std::to_string(i) + "]"), &c->devspec.prop[i]);
        EXPECT_EQ(c->get_pointer<float>("devspec.prop[" + std::to_string(i) + "].cap"), &c->devspec.prop[i].cap);
        EXPECT_EQ(c->get_pointer<float>("devspec.prop[" + std::to_string(i) + "].lev"), &c->devspec.prop[i].lev);
    }
    for(size_t i = 0; i < c->devspec.psen.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<psenstruc>("devspec.psen[" + std::to_string(i) + "]"), &c->devspec.psen[i]);
        EXPECT_EQ(c->get_pointer<float>("devspec.psen[" + std::to_string(i) + "].press"), &c->devspec.psen[i].press);
    }
    for(size_t i = 0; i < c->devspec.pvstrg.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<pvstrgstruc>("devspec.pvstrg[" + std::to_string(i) + "]"), &c->devspec.pvstrg[i]);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.pvstrg[" + std::to_string(i) + "].bcidx"), &c->devspec.pvstrg[i].bcidx);
        EXPECT_EQ(c->get_pointer<float>("devspec.pvstrg[" + std::to_string(i) + "].effbase"), &c->devspec.pvstrg[i].effbase);
        EXPECT_EQ(c->get_pointer<float>("devspec.pvstrg[" + std::to_string(i) + "].effslope"), &c->devspec.pvstrg[i].effslope);
        EXPECT_EQ(c->get_pointer<float>("devspec.pvstrg[" + std::to_string(i) + "].maxpower"), &c->devspec.pvstrg[i].maxpower);
        EXPECT_EQ(c->get_pointer<float>("devspec.pvstrg[" + std::to_string(i) + "].power"), &c->devspec.pvstrg[i].power);
    }
    for(size_t i = 0; i < c->devspec.rot.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<rotstruc>("devspec.rot[" + std::to_string(i) + "]"), &c->devspec.rot[i]);
        EXPECT_EQ(c->get_pointer<float>("devspec.rot[" + std::to_string(i) + "].angle"), &c->devspec.rot[i].angle);
    }
    for(size_t i = 0; i < c->devspec.rw.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<rwstruc>("devspec.rw[" + std::to_string(i) + "]"), &c->devspec.rw[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.rw[" + std::to_string(i) + "].align"), &c->devspec.rw[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.rw[" + std::to_string(i) + "].align.d"), &c->devspec.rw[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.rw[" + std::to_string(i) + "].align.d.x"), &c->devspec.rw[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.rw[" + std::to_string(i) + "].align.d.y"), &c->devspec.rw[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.rw[" + std::to_string(i) + "].align.d.z"), &c->devspec.rw[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.rw[" + std::to_string(i) + "].align.w"), &c->devspec.rw[i].align.w);
        EXPECT_EQ(c->get_pointer<rvector>("devspec.rw[" + std::to_string(i) + "].mom"), &c->devspec.rw[i].mom);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.rw[" + std::to_string(i) + "].mom.col"), &c->devspec.rw[i].mom.col);
        for(size_t j = 0; j < sizeof(c->devspec.rw[i].mom.col)/sizeof(c->devspec.rw[i].mom.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.rw[" + std::to_string(i) + "].mom.col[" + std::to_string(j) + "]"), &c->devspec.rw[i].mom.col[j]);
		}
        EXPECT_EQ(c->get_pointer<float>("devspec.rw[" + std::to_string(i) + "].mxomg"), &c->devspec.rw[i].mxomg);
        EXPECT_EQ(c->get_pointer<float>("devspec.rw[" + std::to_string(i) + "].mxalp"), &c->devspec.rw[i].mxalp);
        EXPECT_EQ(c->get_pointer<float>("devspec.rw[" + std::to_string(i) + "].tc"), &c->devspec.rw[i].tc);
        EXPECT_EQ(c->get_pointer<float>("devspec.rw[" + std::to_string(i) + "].omg"), &c->devspec.rw[i].omg);
        EXPECT_EQ(c->get_pointer<float>("devspec.rw[" + std::to_string(i) + "].alp"), &c->devspec.rw[i].alp);
        EXPECT_EQ(c->get_pointer<float>("devspec.rw[" + std::to_string(i) + "].romg"), &c->devspec.rw[i].romg);
        EXPECT_EQ(c->get_pointer<float>("devspec.rw[" + std::to_string(i) + "].ralp"), &c->devspec.rw[i].ralp);
    }
    for(size_t i = 0; i < c->devspec.rxr.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<rxrstruc>("devspec.rxr[" + std::to_string(i) + "]"), &c->devspec.rxr[i]);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.rxr[" + std::to_string(i) + "].opmode"), &c->devspec.rxr[i].opmode);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.rxr[" + std::to_string(i) + "].modulation"), &c->devspec.rxr[i].modulation);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.rxr[" + std::to_string(i) + "].rssi"), &c->devspec.rxr[i].rssi);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.rxr[" + std::to_string(i) + "].pktsize"), &c->devspec.rxr[i].pktsize);
        EXPECT_EQ(c->get_pointer<double>("devspec.rxr[" + std::to_string(i) + "].freq"), &c->devspec.rxr[i].freq);
        EXPECT_EQ(c->get_pointer<double>("devspec.rxr[" + std::to_string(i) + "].maxfreq"), &c->devspec.rxr[i].maxfreq);
        EXPECT_EQ(c->get_pointer<double>("devspec.rxr[" + std::to_string(i) + "].minfreq"), &c->devspec.rxr[i].minfreq);
        EXPECT_EQ(c->get_pointer<float>("devspec.rxr[" + std::to_string(i) + "].powerin"), &c->devspec.rxr[i].powerin);
        EXPECT_EQ(c->get_pointer<float>("devspec.rxr[" + std::to_string(i) + "].powerout"), &c->devspec.rxr[i].powerout);
        EXPECT_EQ(c->get_pointer<float>("devspec.rxr[" + std::to_string(i) + "].maxpower"), &c->devspec.rxr[i].maxpower);
        EXPECT_EQ(c->get_pointer<float>("devspec.rxr[" + std::to_string(i) + "].band"), &c->devspec.rxr[i].band);
        EXPECT_EQ(c->get_pointer<float>("devspec.rxr[" + std::to_string(i) + "].squelch_tone"), &c->devspec.rxr[i].squelch_tone);
        EXPECT_EQ(c->get_pointer<double>("devspec.rxr[" + std::to_string(i) + "].goodratio"), &c->devspec.rxr[i].goodratio);
        EXPECT_EQ(c->get_pointer<double>("devspec.rxr[" + std::to_string(i) + "].rxutc"), &c->devspec.rxr[i].rxutc);
        EXPECT_EQ(c->get_pointer<double>("devspec.rxr[" + std::to_string(i) + "].uptime"), &c->devspec.rxr[i].uptime);
    }
    for(size_t i = 0; i < c->devspec.ssen.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<ssenstruc>("devspec.ssen[" + std::to_string(i) + "]"), &c->devspec.ssen[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.ssen[" + std::to_string(i) + "].align"), &c->devspec.ssen[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.ssen[" + std::to_string(i) + "].align.d"), &c->devspec.ssen[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.ssen[" + std::to_string(i) + "].align.d.x"), &c->devspec.ssen[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.ssen[" + std::to_string(i) + "].align.d.y"), &c->devspec.ssen[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.ssen[" + std::to_string(i) + "].align.d.z"), &c->devspec.ssen[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.ssen[" + std::to_string(i) + "].align.w"), &c->devspec.ssen[i].align.w);
        EXPECT_EQ(c->get_pointer<float>("devspec.ssen[" + std::to_string(i) + "].qva"), &c->devspec.ssen[i].qva);
        EXPECT_EQ(c->get_pointer<float>("devspec.ssen[" + std::to_string(i) + "].qvb"), &c->devspec.ssen[i].qvb);
        EXPECT_EQ(c->get_pointer<float>("devspec.ssen[" + std::to_string(i) + "].qvc"), &c->devspec.ssen[i].qvc);
        EXPECT_EQ(c->get_pointer<float>("devspec.ssen[" + std::to_string(i) + "].qvd"), &c->devspec.ssen[i].qvd);
        EXPECT_EQ(c->get_pointer<float>("devspec.ssen[" + std::to_string(i) + "].azimuth"), &c->devspec.ssen[i].azimuth);
        EXPECT_EQ(c->get_pointer<float>("devspec.ssen[" + std::to_string(i) + "].elevation"), &c->devspec.ssen[i].elevation);
    }
    for(size_t i = 0; i < c->devspec.stt.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<sttstruc>("devspec.stt[" + std::to_string(i) + "]"), &c->devspec.stt[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.stt[" + std::to_string(i) + "].align"), &c->devspec.stt[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.stt[" + std::to_string(i) + "].align.d"), &c->devspec.stt[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].align.d.x"), &c->devspec.stt[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].align.d.y"), &c->devspec.stt[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].align.d.z"), &c->devspec.stt[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].align.w"), &c->devspec.stt[i].align.w);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.stt[" + std::to_string(i) + "].att"), &c->devspec.stt[i].att);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.stt[" + std::to_string(i) + "].att.d"), &c->devspec.stt[i].att.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].att.d.x"), &c->devspec.stt[i].att.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].att.d.y"), &c->devspec.stt[i].att.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].att.d.z"), &c->devspec.stt[i].att.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].att.w"), &c->devspec.stt[i].att.w);
        EXPECT_EQ(c->get_pointer<rvector>("devspec.stt[" + std::to_string(i) + "].omega"), &c->devspec.stt[i].omega);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.stt[" + std::to_string(i) + "].omega.col"), &c->devspec.stt[i].omega.col);
        for(size_t j = 0; j < sizeof(c->devspec.stt[i].omega.col)/sizeof(c->devspec.stt[i].omega.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].omega.col[" + std::to_string(j) + "]"), &c->devspec.stt[i].omega.col[j]);
		}
        EXPECT_EQ(c->get_pointer<rvector>("devspec.stt[" + std::to_string(i) + "].alpha"), &c->devspec.stt[i].alpha);
        //EXPECT_EQ(c->get_pointer<double[X]>("devspec.stt[" + std::to_string(i) + "].alpha.col"), &c->devspec.stt[i].alpha.col);
        for(size_t j = 0; j < sizeof(c->devspec.stt[i].alpha.col)/sizeof(c->devspec.stt[i].alpha.col[0]); ++j) {
            EXPECT_EQ(c->get_pointer<double>("devspec.stt[" + std::to_string(i) + "].alpha.col[" + std::to_string(j) + "]"), &c->devspec.stt[i].alpha.col[j]);
		}
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.stt[" + std::to_string(i) + "].retcode"), &c->devspec.stt[i].retcode);
        EXPECT_EQ(c->get_pointer<uint32_t>("devspec.stt[" + std::to_string(i) + "].status"), &c->devspec.stt[i].status);
    }
    for(size_t i = 0; i < c->devspec.suchi.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<suchistruc>("devspec.suchi[" + std::to_string(i) + "]"), &c->devspec.suchi[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.suchi[" + std::to_string(i) + "].align"), &c->devspec.suchi[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.suchi[" + std::to_string(i) + "].align.d"), &c->devspec.suchi[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.suchi[" + std::to_string(i) + "].align.d.x"), &c->devspec.suchi[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.suchi[" + std::to_string(i) + "].align.d.y"), &c->devspec.suchi[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.suchi[" + std::to_string(i) + "].align.d.z"), &c->devspec.suchi[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.suchi[" + std::to_string(i) + "].align.w"), &c->devspec.suchi[i].align.w);
        EXPECT_EQ(c->get_pointer<float>("devspec.suchi[" + std::to_string(i) + "].press"), &c->devspec.suchi[i].press);
        //EXPECT_EQ(c->get_pointer<float[X]>("devspec.suchi[" + std::to_string(i) + "].temps"), &c->devspec.suchi[i].temps);
        for(size_t j = 0; j < sizeof(c->devspec.suchi[i].temps)/sizeof(c->devspec.suchi[i].temps[0]); ++j) {
            EXPECT_EQ(c->get_pointer<float>("devspec.suchi[" + std::to_string(i) + "].temps[" + std::to_string(j) + "]"), &c->devspec.suchi[i].temps[j]);
		}
    }
    for(size_t i = 0; i < c->devspec.swch.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<swchstruc>("devspec.swch[" + std::to_string(i) + "]"), &c->devspec.swch[i]);
    }
    for(size_t i = 0; i < c->devspec.tcu.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<tcustruc>("devspec.tcu[" + std::to_string(i) + "]"), &c->devspec.tcu[i]);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcu[" + std::to_string(i) + "].mcnt"), &c->devspec.tcu[i].mcnt);
        //EXPECT_EQ(c->get_pointer<uint16_t[X]>("devspec.tcu[" + std::to_string(i) + "].mcidx"), &c->devspec.tcu[i].mcidx);
        for(size_t j = 0; j < sizeof(c->devspec.tcu[i].mcidx)/sizeof(c->devspec.tcu[i].mcidx[0]); ++j) {
            EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcu[" + std::to_string(i) + "].mcidx[" + std::to_string(j) + "]"), &c->devspec.tcu[i].mcidx[j]);
		}
    }
    for(size_t i = 0; i < c->devspec.tcv.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<tcvstruc>("devspec.tcv[" + std::to_string(i) + "]"), &c->devspec.tcv[i]);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcv[" + std::to_string(i) + "].opmode"), &c->devspec.tcv[i].opmode);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcv[" + std::to_string(i) + "].modulation"), &c->devspec.tcv[i].modulation);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcv[" + std::to_string(i) + "].rssi"), &c->devspec.tcv[i].rssi);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.tcv[" + std::to_string(i) + "].pktsize"), &c->devspec.tcv[i].pktsize);
        EXPECT_EQ(c->get_pointer<double>("devspec.tcv[" + std::to_string(i) + "].freq"), &c->devspec.tcv[i].freq);
        EXPECT_EQ(c->get_pointer<double>("devspec.tcv[" + std::to_string(i) + "].maxfreq"), &c->devspec.tcv[i].maxfreq);
        EXPECT_EQ(c->get_pointer<double>("devspec.tcv[" + std::to_string(i) + "].minfreq"), &c->devspec.tcv[i].minfreq);
        EXPECT_EQ(c->get_pointer<float>("devspec.tcv[" + std::to_string(i) + "].powerin"), &c->devspec.tcv[i].powerin);
        EXPECT_EQ(c->get_pointer<float>("devspec.tcv[" + std::to_string(i) + "].powerout"), &c->devspec.tcv[i].powerout);
        EXPECT_EQ(c->get_pointer<float>("devspec.tcv[" + std::to_string(i) + "].maxpower"), &c->devspec.tcv[i].maxpower);
        EXPECT_EQ(c->get_pointer<float>("devspec.tcv[" + std::to_string(i) + "].band"), &c->devspec.tcv[i].band);
        EXPECT_EQ(c->get_pointer<float>("devspec.tcv[" + std::to_string(i) + "].squelch_tone"), &c->devspec.tcv[i].squelch_tone);
        EXPECT_EQ(c->get_pointer<double>("devspec.tcv[" + std::to_string(i) + "].goodratio"), &c->devspec.tcv[i].goodratio);
        EXPECT_EQ(c->get_pointer<double>("devspec.tcv[" + std::to_string(i) + "].txutc"), &c->devspec.tcv[i].txutc);
        EXPECT_EQ(c->get_pointer<double>("devspec.tcv[" + std::to_string(i) + "].rxutc"), &c->devspec.tcv[i].rxutc);
        EXPECT_EQ(c->get_pointer<double>("devspec.tcv[" + std::to_string(i) + "].uptime"), &c->devspec.tcv[i].uptime);
    }
    for(size_t i = 0; i < c->devspec.telem.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<telemstruc>("devspec.telem[" + std::to_string(i) + "]"), &c->devspec.telem[i]);
        EXPECT_EQ(c->get_pointer<uint8_t>("devspec.telem[" + std::to_string(i) + "].vuint8"), &c->devspec.telem[i].vuint8);
        EXPECT_EQ(c->get_pointer<int8_t>("devspec.telem[" + std::to_string(i) + "].vint8"), &c->devspec.telem[i].vint8);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.telem[" + std::to_string(i) + "].vuint16"), &c->devspec.telem[i].vuint16);
        EXPECT_EQ(c->get_pointer<int16_t>("devspec.telem[" + std::to_string(i) + "].vint16"), &c->devspec.telem[i].vint16);
        EXPECT_EQ(c->get_pointer<uint32_t>("devspec.telem[" + std::to_string(i) + "].vuint32"), &c->devspec.telem[i].vuint32);
        EXPECT_EQ(c->get_pointer<int32_t>("devspec.telem[" + std::to_string(i) + "].vint32"), &c->devspec.telem[i].vint32);
        EXPECT_EQ(c->get_pointer<float>("devspec.telem[" + std::to_string(i) + "].vfloat"), &c->devspec.telem[i].vfloat);
        EXPECT_EQ(c->get_pointer<double>("devspec.telem[" + std::to_string(i) + "].vdouble"), &c->devspec.telem[i].vdouble);
        //EXPECT_EQ(c->get_pointer<char[X]>("devspec.telem[" + std::to_string(i) + "].vstring"), &c->devspec.telem[i].vstring);
    }
    for(size_t i = 0; i < c->devspec.thst.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<thststruc>("devspec.thst[" + std::to_string(i) + "]"), &c->devspec.thst[i]);
        EXPECT_EQ(c->get_pointer<quaternion>("devspec.thst[" + std::to_string(i) + "].align"), &c->devspec.thst[i].align);
        EXPECT_EQ(c->get_pointer<cvector>("devspec.thst[" + std::to_string(i) + "].align.d"), &c->devspec.thst[i].align.d);
        EXPECT_EQ(c->get_pointer<double>("devspec.thst[" + std::to_string(i) + "].align.d.x"), &c->devspec.thst[i].align.d.x);
        EXPECT_EQ(c->get_pointer<double>("devspec.thst[" + std::to_string(i) + "].align.d.y"), &c->devspec.thst[i].align.d.y);
        EXPECT_EQ(c->get_pointer<double>("devspec.thst[" + std::to_string(i) + "].align.d.z"), &c->devspec.thst[i].align.d.z);
        EXPECT_EQ(c->get_pointer<double>("devspec.thst[" + std::to_string(i) + "].align.w"), &c->devspec.thst[i].align.w);
        EXPECT_EQ(c->get_pointer<float>("devspec.thst[" + std::to_string(i) + "].flw"), &c->devspec.thst[i].flw);
        EXPECT_EQ(c->get_pointer<float>("devspec.thst[" + std::to_string(i) + "].isp"), &c->devspec.thst[i].isp);
    }
    for(size_t i = 0; i < c->devspec.tnc.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<tncstruc>("devspec.tnc[" + std::to_string(i) + "]"), &c->devspec.tnc[i]);
    }
    for(size_t i = 0; i < c->devspec.tsen.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<tsenstruc>("devspec.tsen[" + std::to_string(i) + "]"), &c->devspec.tsen[i]);
    }
    for(size_t i = 0; i < c->devspec.txr.capacity(); ++i) {
        EXPECT_EQ(c->get_pointer<txrstruc>("devspec.txr[" + std::to_string(i) + "]"), &c->devspec.txr[i]);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.txr[" + std::to_string(i) + "].opmode"), &c->devspec.txr[i].opmode);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.txr[" + std::to_string(i) + "].modulation"), &c->devspec.txr[i].modulation);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.txr[" + std::to_string(i) + "].rssi"), &c->devspec.txr[i].rssi);
        EXPECT_EQ(c->get_pointer<uint16_t>("devspec.txr[" + std::to_string(i) + "].pktsize"), &c->devspec.txr[i].pktsize);
        EXPECT_EQ(c->get_pointer<double>("devspec.txr[" + std::to_string(i) + "].freq"), &c->devspec.txr[i].freq);
        EXPECT_EQ(c->get_pointer<double>("devspec.txr[" + std::to_string(i) + "].maxfreq"), &c->devspec.txr[i].maxfreq);
        EXPECT_EQ(c->get_pointer<double>("devspec.txr[" + std::to_string(i) + "].minfreq"), &c->devspec.txr[i].minfreq);
        EXPECT_EQ(c->get_pointer<float>("devspec.txr[" + std::to_string(i) + "].powerin"), &c->devspec.txr[i].powerin);
        EXPECT_EQ(c->get_pointer<float>("devspec.txr[" + std::to_string(i) + "].powerout"), &c->devspec.txr[i].powerout);
        EXPECT_EQ(c->get_pointer<float>("devspec.txr[" + std::to_string(i) + "].maxpower"), &c->devspec.txr[i].maxpower);
        EXPECT_EQ(c->get_pointer<float>("devspec.txr[" + std::to_string(i) + "].band"), &c->devspec.txr[i].band);
        EXPECT_EQ(c->get_pointer<float>("devspec.txr[" + std::to_string(i) + "].squelch_tone"), &c->devspec.txr[i].squelch_tone);
        EXPECT_EQ(c->get_pointer<double>("devspec.txr[" + std::to_string(i) + "].goodratio"), &c->devspec.txr[i].goodratio);
        EXPECT_EQ(c->get_pointer<double>("devspec.txr[" + std::to_string(i) + "].txutc"), &c->devspec.txr[i].txutc);
        EXPECT_EQ(c->get_pointer<double>("devspec.txr[" + std::to_string(i) + "].uptime"), &c->devspec.txr[i].uptime);
	}

	// devspecstruc devspec
	EXPECT_EQ(c->get_pointer<devspecstruc>("devspec"), &c->devspec);
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
