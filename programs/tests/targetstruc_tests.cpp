#include "support/configCosmos.h"
#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"
#include "support/stringlib.h"
#include "support/convertdef.h"
#include "support/convertlib.h"
#include "support/jsondef.h"
#include <fstream>
#include "physics/physicsclass.h"

using namespace json11;

// this driver is intended to test most important targetstruc functionality
int main(int argc, char *argv[])
{
	cout<<"Declaring cosmosstruc...";
	Cosmos::Support::cosmosstruc c;
	cout<<"done."<<endl;

	cout<<"Declaring default targetstruc...";
	Cosmos::Support::targetstruc t;
	cout<<"done."<<endl;

	// Declare a Simulator
	Physics::Simulator S;

	// Fake a TLE
	Convert::tlestruc tle;

	// Add a Node
	cout<<"nodes = "<<S.cnodes.size()<<endl;
	S.AddNode("mother", "HEX65W80H", Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, tle);
	cout<<"nodes = "<<S.cnodes.size()<<endl;

	// Add a Target
	cout<<"cosmosstruc targets = "<<S.cnodes[0]->currentinfo.target.size()<<endl;
	S.cnodes[0]->currentinfo.target.push_back(t);
	cout<<"cosmosstruc targets = "<<S.cnodes[0]->currentinfo.target.size()<<endl;

	// Add a Target
	S.AddTarget(t);
	cout<<"cosmosstruc targets = "<<S.cnodes[0]->currentinfo.target.size()<<endl;

	// Add a Target
	S.AddTarget("Fake", 1.23, 2.34, 4.001, 13, NODE_TYPE_TARGET);
	cout<<"cosmosstruc targets = "<<S.cnodes[0]->currentinfo.target.size()<<endl;

	// Try to parse the default Target File as a single line
	//S.ParseTargetFile("../python/json/cosmos_targets2.json");
	cout<<"cosmosstruc targets = "<<S.cnodes[0]->currentinfo.target.size()<<endl;

	// Try to parse the default Target File
	std::ifstream infile2("../python/json/cosmos_targets.json");

	// Check if the file was opened successfully
	if (!infile2.is_open()) {
		std::cerr << "Error: Could not open the file!" << std::endl;
		return 1;
	}
	
	// Read the file into a string (json_str)
	string targets_json2((std::istreambuf_iterator<char>(infile2)),std::istreambuf_iterator<char>());
	
	// Close the file
	infile2.close();

	cout<<"string = "<<targets_json2<<endl;
	string estring;
	json11::Json jargs = json11::Json::parse(targets_json2, estring);

	// Check for any parsing errors
	if (!estring.empty()) {
		std::cerr << "Error parsing JSON: " << estring << std::endl;
		return -1;
	}
	S.ParseTargetJson(jargs);





	// Add targets from JSON string

	// File path to the JSON file
	std::ifstream infile("../python/json/cosmos_targets2.json");
	
	// Check if the file was opened successfully
	if (!infile.is_open()) {
		std::cerr << "Error: Could not open the file!" << std::endl;
		return 1;
	}
	
	// Read the file into a string (json_str)
	string targets_json((std::istreambuf_iterator<char>(infile)),std::istreambuf_iterator<char>());
	
	// Close the file
	infile.close();

	// Load Json string into targets
	//S.ParseTargetString(targets_json);
	cout<<"cosmosstruc targets = "<<S.cnodes[0]->currentinfo.target.size()<<endl;
	return 0;

}
