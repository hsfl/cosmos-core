#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <thread>
#include <ctime>
#include <map>
#include <unistd.h>  // usleep
#include <chrono>
#include <thread>
#include <iomanip>
#include <cmath>
#include <cstdlib>	// system
#include <algorithm>

// THE PRODIGAL JSON 
#include "json11.hpp"

// Bad!   Pollute the namespace much?
// TODO:  Add 'using' for specific names...
using namespace json11;
using namespace std;

// Good!  Easier to read...
using name_map = map<string,void*>;
using name_mapping = pair<string,void*>;

// helper functions (assume objects overload <<)

// print vector of arbitrary object
template<class T>
ostream& operator<<(ostream& out, const vector<T>& v)	{
	for(size_t i = 0; i < v.size(); ++i) { out<<v[i]<<" "; }
	return out;
}

// print vector of vectors of arbitrary object
template<class T>
ostream& operator<<(ostream& out, const vector<vector<T>>& v)	{
	for(size_t i = 0; i < v.size(); ++i) { 
		for(size_t j = 0; j < v[i].size(); ++j) { out<<v[i][j]<<" "; }
	}
	return out;
}

// sample cosmos data classes
struct unitstruc
{
    //! JSON Unit Name
    string name = "";
    //! JSON Unit conversion type
    uint16_t type = 123;
    //! 0th derivative term
    float p0 = 0.f;
    //! 1th derivative term
    float p1 = 0.f;
    //! 2th derivative term
    float p2 = 0.f;


	// needed to get class contents as JSON object (internal to json11)
	Json to_json() const {
		return Json::object {
			{ "name", name },
			{ "type", type },
			{ "p0", p0 },
			{ "p1", p1 },
			{ "p2", p2 }
		};
	}

	// needed to set class contents from JSON string
	void from_json(const string& s)	{
		string error;
		Json parsed = Json::parse(s,error);
		if(error.empty())	{
			name = parsed["name"].string_value();
			type = parsed["type"].number_value();
			p0 = parsed["p0"].number_value();
			p1 = parsed["p1"].number_value();
			p2 = parsed["p2"].number_value();
		}
		return;
	}
};

ostream& operator<<(ostream& out, const unitstruc& u)	{ return out<<u.name<<" "<<u.type<<" "<<u.p0<<" "<<u.p1<<" "<<u.p2; }

//! Specific Device structure
/*! Counts and arrays of pointers to each type of device, ordered by type.
*/
struct devspecstruc
{
    uint16_t ant_cnt;
    uint16_t batt_cnt;
    uint16_t bus_cnt;
    uint16_t cam_cnt;
    uint16_t cpu_cnt;
    uint16_t disk_cnt;
    uint16_t gps_cnt;
    uint16_t htr_cnt;
    uint16_t imu_cnt;
    uint16_t mcc_cnt;
    uint16_t motr_cnt;
    uint16_t mtr_cnt;
    //uint16_t pload_cnt = 0;
    uint16_t pload_cnt;
    uint16_t prop_cnt;
    uint16_t psen_cnt;
    uint16_t bcreg_cnt;
    uint16_t rot_cnt;
    uint16_t rw_cnt;
    uint16_t rxr_cnt;
    uint16_t ssen_cnt;
    uint16_t pvstrg_cnt;
    uint16_t stt_cnt;
    uint16_t suchi_cnt;
    uint16_t swch_cnt;
    uint16_t tcu_cnt;
    uint16_t tcv_cnt;
    uint16_t telem_cnt;
    uint16_t thst_cnt;
    uint16_t tsen_cnt;
    uint16_t tnc_cnt;
    uint16_t txr_cnt;
    vector<uint16_t>all;
    vector<uint16_t>ant;
    vector<uint16_t>batt;
    vector<uint16_t>bcreg;
    vector<uint16_t>bus;
    vector<uint16_t>cam;
    vector<uint16_t>cpu;
    vector<uint16_t>disk;
    vector<uint16_t>gps;
    vector<uint16_t>htr;
    vector<uint16_t>imu;
    vector<uint16_t>mcc;
    vector<uint16_t>motr;
    vector<uint16_t>mtr;
    vector<uint16_t>pload;
    vector<uint16_t>prop;
    vector<uint16_t>psen;
    vector<uint16_t>pvstrg;
    vector<uint16_t>rot;
    vector<uint16_t>rw;
    vector<uint16_t>rxr;
    vector<uint16_t>ssen;
    vector<uint16_t>stt;
    vector<uint16_t>suchi;
    vector<uint16_t>swch;
    vector<uint16_t>tcu;
    vector<uint16_t>tcv;
    vector<uint16_t>telem;
    vector<uint16_t>thst;
    vector<uint16_t>tnc;
    vector<uint16_t>tsen;
    vector<uint16_t>txr;
};


class cosmos_data {
public:

	// a sample of the cosmos data structure
	double utc = 0.;
	//! JSON Unit Map matrix: first level is for unit type, second level is for all variants (starting with primary).
	vector<vector<unitstruc>> unit;
	//! Structure for devices (components) special data in node, by type.
    devspecstruc devspec;

	// etc
	// etc
	// etc


// support for the namespace 2.0

	name_map names;

	bool name_exists(const string& s)	{ return (names.find(s) == names.end()) ? false : true; }

	void print_all_names() const	{
		name_map::const_iterator it = names.begin();
		while(it != names.end())	{ cout<<(it++)->first<<endl; }
	}

	void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };
	//TODO:   add remove_name(..), change_name(..) functions
	
	string get_name(void* v)	{
		name_map::const_iterator it = names.begin();
		while(it->second != v && it != names.end())	{ it++; }
		if(it == names.end())	{	cerr<<"address <"<<v<<"> not found!"<<endl; return "";	}
		return it->first;
	}

	template<class T>
	T* get_pointer(const string& s) const	{
		name_map::const_iterator it = names.find(s);
		if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return nullptr;	}
		return (T*)(it->second);
	}

	template<class T>
	T get_value(const string& s) const	{
		// change to static null object?
		T dummy;
		name_map::const_iterator it = names.find(s);
		if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return dummy;	}
		return *get_pointer<T>(s);
	}

	template<class T>
	void set_value(const string& s, const T& value) const	{
		// maybe if not found should be inserted??  hmmm....  ask Eric
		name_map::const_iterator it = names.find(s);
		if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return;	}
		*get_pointer<T>(s) = value;
	}

	template<class T>
	void set_json_value(const string& s, const string& json) const /*how is a set function const? lol*/	{
		get_pointer<T>(s)->from_json(json);
	}

	template<class T>
	string get_json(const string& s)	{
		if(name_exists(s))	{
			Json json = Json::object { { s, this->get_value<T>(s) } };
			return json.dump();
		} else {
			return "";
		}
	}

	// other namespace member functions??
};

ostream& operator<<(ostream& out, const cosmos_data& c)	{ return out; }



// Driver to Explore Functionality of Namespace 2.0 Paradigm

class jim	{
public:
	int x = 0;
};

int main(int argc, char** argv)	{

	jim j;
	j.x = 17;

	//// Start COSMOS DATA Test
	cosmos_data cosmos;

	cosmos.add_name("Jim Value", &j.x);
	cout<<"Jim Value = "<<j.x<<endl;

	cosmos.set_value<int>("Jim Value", 42);

	cout<<"Jim Value = "<<cosmos.get_value<int>("Jim Value")<<endl;
	cout<<"Jim Value = "<<j.x<<endl;

	// map names to simple data
	cosmos.add_name("utc forever !@#$^%", &cosmos.utc);
	cosmos.set_value<double>("utc forever !@#$^%", 1000.1);
	cout<<"utc = "<<cosmos.get_value<double>("utc forever !@#$^%")<<endl<<endl;


	// map names to vectors of objects and objects with vectors... whatever you like
	cosmos.add_name("unit", &cosmos.unit);
	cosmos.add_name("devspec", &cosmos.devspec);

	// initialize units (vector of vectors)
	for(size_t j = 0; j < 3; ++j)	{
		vector<unitstruc> v;
		for(size_t i = 0; i < 5; ++i)	{
			unitstruc u;
			u.name = "unit_name_" + std::to_string(j) + "_" + to_string(i);
			v.push_back(u);
		}
		cosmos.unit.push_back(v);
	}

	// add the oject-friendly names to the namespace (for every level)
	for(size_t j = 0; j < 3; ++j)	{
		cosmos.add_name("unit["+to_string(j)+"]", &cosmos.unit[j]);
	for(size_t i = 0; i < 5; ++i)	{
		cosmos.add_name("unit["+to_string(j)+"]["+to_string(i)+"]", &cosmos.unit[j][i]);
		cosmos.add_name("unit["+to_string(j)+"]["+to_string(i)+"].name", &cosmos.unit[j][i].name);
		cosmos.add_name("unit["+to_string(j)+"]["+to_string(i)+"].type", &cosmos.unit[j][i].type);
		cosmos.add_name("unit["+to_string(j)+"]["+to_string(i)+"].p0", &cosmos.unit[j][i].p0);
		cosmos.add_name("unit["+to_string(j)+"]["+to_string(i)+"].p1", &cosmos.unit[j][i].p1);
		cosmos.add_name("unit["+to_string(j)+"]["+to_string(i)+"].p2", &cosmos.unit[j][i].p2);
	}
	}

	// print out all namespace names
	cout<<"All COSMOS Names"<<endl;
	cout<<"================"<<endl;
	cosmos.print_all_names();
	cout<<endl<<endl;

// SET VALUES

	// this works
	cosmos.set_value<float>("unit[2][3].p1", 47.3);

	// this also works
	cosmos.set_value("unit[2][3].p1", 47.3f);

	// this does not work! template deduction thinks 47.3 is a double and f*cks up two entries!
	//cosmos.set_value("unit[2][3].p1", 47.3, names);

// GET VALUES

	cout<<"Print some COSMOS Values  (from cosmos_data vs. address calls)"<<endl;
	cout<<"========================="<<endl;
	// initialize unit values in cosmos data (vector<vector<unitstruc>>)
	for(size_t j = 0; j < 3; ++j)	{
		for(size_t i = 0; i < 5; ++i)	{
			string unit_name = "unit["+to_string(j)+"]["+to_string(i)+"]";
			cout << cosmos.get_name(&cosmos.unit[j][i])<< ": "
				<<cosmos.unit[j][i]
				<< " vs.\t"
			    << cosmos.get_value<unitstruc>(unit_name)<<endl;
		}
	}

// SUPPORT FOR ALIASES

	// add an aliases
	cosmos.add_name("my favorite value", &cosmos.unit[1][1].p2);

	// set value using alias	
	cosmos.set_value<float>("my favorite value", 123.456);

	// get value using alias
	cout<<"\nmy favorite value = "<<cosmos.get_value<float>("my favorite value")<<endl;

// can even get a reference (thru get_pointer)!  multiple names can point to the same data!

	// but cannot do this!  (++ needs an l-value)
	//cout<<"best value ever = "<<++cosmos.get_value<float>("my favorite value", names)<<endl;

	// but can do this!
	cout<<"best value ever = "<<++(*cosmos.get_pointer<float>("my favorite value"))<<endl;
	cout<<"best value ever = "<<++(*cosmos.get_pointer<float>("my favorite value"))<<endl<<endl;

// SUPPORT FOR SETTING OBJECTS

	// create the object (unitstruc)
	unitstruc super_cool_unit;
	super_cool_unit.name = "I am a super cool unit.";
	super_cool_unit.type = 13;
	super_cool_unit.p0 = 1.1f;
	super_cool_unit.p1 = 1.2f;
	super_cool_unit.p2 = 1.3f;

	// set the object (unitstruc)
	cosmos.set_value<unitstruc>("unit[1][4]", super_cool_unit);

// SUPPORT FOR GETTING OBJECTS

	// get an object (unitstruc)
	cout<<cosmos.get_value<unitstruc>("unit[1][4]")<<endl<<endl;

// SUPPORT FOR SETTING VECTORS OF OBJECTS

// SUPPORT FOR GETTING VECTORS OF OBJECTS

// etc.

/*
	// access actual data from name of vector pointer
	cout<<"t.b[0] = "<<cosmos.get_pointer<vector<int>>("t.b")->at(0)<<endl;
	cout<<"t.b[1] = "<<cosmos.get_pointer<vector<int>>("t.b")->at(1)<<endl;
	cout<<"t.b[2] = "<<cosmos.get_pointer<vector<int>>("t.b")->at(2)<<endl;
	// or this notation
	cout<<"t.b[0] = "<<(*cosmos.get_pointer<vector<int>>("t.b",names))[0]<<endl;
	cout<<"t.b[1] = "<<(*cosmos.get_pointer<vector<int>>("t.b",names))[1]<<endl;
	cout<<"t.b[2] = "<<(*cosmos.get_pointer<vector<int>>("t.b",names))[2]<<endl;

	// access actual data from name of vector elements
	cout<<"t.b[0] = "<<*cosmos.get_pointer<int>("t.b[0]")<<endl;
	cout<<"t.b[1] = "<<*cosmos.get_pointer<int>("t.b[1]")<<endl;
	cout<<"t.b[2] = "<<*cosmos.get_pointer<int>("t.b[2]")<<endl;

	// access copy of data from name of vector pointer or vector elements using get_value(..)
*/

// And one more important thing................................

// SUPPORT FOR JSON 

// EASY OUTPUT TO JSON FOR DATA, OBJECTS, VECTORS, AND VECTORS OF VECTORS (OF DATA OR OBJECTS)
	cout<<"JSON Test(s): "<<endl;

	string json_output = cosmos.get_json<string>("unit[1][4].name");
	json_output += cosmos.get_json<uint16_t>("unit[1][4].type");

	// this is bad and wrong and will not work!  must match data type *exactly*
	//json_output += cosmos.get_json<int>("unit[1][4].type");

	json_output += cosmos.get_json<float>("unit[1][4].p0");
	json_output += cosmos.get_json<float>("unit[1][4].p1");
	json_output += cosmos.get_json<float>("unit[1][4].p2");
	cout<<"json named data members output = \n\n\t"<<json_output<<endl<<endl;
	cout<<"(can also be used for input...)"<<endl<<endl;

	string concatenated_json_objects(json_output);

	// GET JSON FROM OBJECTS, VECTORS, AND VECTORS OF VECTORS
	cout<<"json named object output = \n\n\t"
		<<cosmos.get_json<unitstruc>("unit[1][4]")<<endl<<endl;

	cout<<"json named vector of objects output = \n\n\t"
		<<cosmos.get_json<vector<unitstruc>>("unit[1]")<<endl<<endl;

	cout<<"json named vector of vector of objects output = \n\n\t"
		<<cosmos.get_json<vector<vector<unitstruc>>>("unit")<<endl<<endl;

	// get JSON output for *ANY* name in the Namespace
	cout<<"json data output = "<<cosmos.get_json<float>("my favorite value")<<endl<<endl;

// EASY INPUT TO JSON FOR DATA, OBJECTS, VECTORS, AND VECTORS OF VECTORS (OF DATA OR OBJECTS)

	json_output.clear();

	cosmos.add_name("The PAYLOAD Count", &cosmos.devspec.pload_cnt);
	cout<<"The (uninitialized!) value of PAYLOAD Count is = "<<cosmos.get_value<uint16_t>("The PAYLOAD Count")<<endl<<endl;

	string test_input("{\"The PAYLOAD Count\": 42}");

	cout<<"input string = "<<test_input<<endl;

	string error;
	Json my_first_parse = Json::parse(test_input,error);
	cout<<"errors = <"<<error<<">"<<endl;
	cout<<"dump parced results = "<<my_first_parse.dump()<<endl<<endl;
	cout<<" parced data = "<<my_first_parse["The PAYLOAD Count"].number_value()<<endl<<endl;

	Json my_second_parse = Json::parse(cosmos.get_json<vector<vector<unitstruc>>>("unit"),error);
	cout<<"errors = <"<<error<<">"<<endl;
	cout<<"dump parced results = \n\n"<<my_second_parse.dump()<<endl<<endl;
	cout<<my_second_parse.type_name()<<endl;
	cout<<my_second_parse["unit"].type_name()<<endl;
	cout<<my_second_parse["unit"][0].type_name()<<endl;
	cout<<my_second_parse["unit"][0][0].type_name()<<endl;
	cout<<my_second_parse["unit"][0][0]["name"].type_name()<<endl;
	cout<<my_second_parse["unit"][0][0]["type"].type_name()<<endl;
	cout<<my_second_parse["unit"][0][0]["p0"].type_name()<<endl;
	cout<<my_second_parse["unit"][0][0]["p1"].type_name()<<endl;
	cout<<my_second_parse["unit"][0][0]["p2"].type_name()<<endl<<endl;

// this is a multi-JSON-object parser
	cout<<"PARSE ME = \n\n"<<concatenated_json_objects<<endl;
	vector<Json> pro_parser;
	pro_parser = Json::parse_multi(concatenated_json_objects, error);
	cout<<"errors = <"<<error<<">"<<endl;
	cout<<"dump parced results = \n\n";
	for(size_t i = 0; i < pro_parser.size(); ++i)	{
		cout<<pro_parser[i].dump()<<endl;
	}

// parse an entire object into the Namespace from JSON string!!!

	string unitstruc_in_json_form = my_second_parse["unit"][1][4].dump();

	cout<<"before = "<<cosmos.get_value<unitstruc>("unit[2][2]")<<endl;
	cosmos.set_json_value<unitstruc>("unit[2][2]", unitstruc_in_json_form);
	cout<<"after =  "<<cosmos.get_value<unitstruc>("unit[2][2]")<<endl<<endl;

	// reverse lookup tests
	cout<<"name =  "<<cosmos.get_name(&cosmos.unit[2][2])<<endl;
	cout<<"name =  "<<cosmos.get_name(&cosmos.unit[2][2]+1234567)<<endl;

	// big test: parse entire cosmos_data into another cosmos_data
	// maybe use googlettest


	return 0;
}
