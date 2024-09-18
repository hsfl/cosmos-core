#include "support/configCosmos.h"
#include "support/stringlib.h"
#include "support/convertdef.h"
#include "support/convertlib.h"
#include "support/jsondef.h"
#include <fstream>
//#include "support/json11.hpp"
//#include "../../../../books/C++/sandbox/include/jsonface.h"

using namespace json11;

//#include <cmath>

// Function to compute the elevation angle in degrees
double elevation_angle(const double target[], const double satellite[]) {
    // Line-of-sight vector from target to satellite
    double los[3];
    los[0] = satellite[0] - target[0];
    los[1] = satellite[1] - target[1];
    los[2] = satellite[2] - target[2];

    // Compute magnitudes
    double los_mag = std::sqrt(los[0]*los[0] + los[1]*los[1] + los[2]*los[2]);
    double tgt_mag = std::sqrt(target[0]*target[0] + target[1]*target[1] + target[2]*target[2]);

    // Normalize vectors
    double los_unit[3] = { los[0]/los_mag, los[1]/los_mag, los[2]/los_mag };
    double up_unit[3] = { target[0]/tgt_mag, target[1]/tgt_mag, target[2]/tgt_mag };

    // Compute dot product
    double dot = los_unit[0]*up_unit[0] + los_unit[1]*up_unit[1] + los_unit[2]*up_unit[2];

    // Ensure dot product is within valid range for asin()
    if (dot > 1.0) dot = 1.0;
    if (dot < -1.0) dot = -1.0;

    // Compute elevation angle in radians
    double elevation_rad = std::asin(dot);

    // Convert to degrees
    double elevation_deg = elevation_rad * (180.0 / M_PI);

    return elevation_deg;
}


int main(int argc, char *argv[])
{

	//bool make_output_files = false;
	bool make_output_files = true;


	// you can default construst Json from tlestruc because to_json() is defined!
	Cosmos::Convert::tlestruc my_tle;
	Json mj(my_tle);

	// you can load it into a stringstream (because << is overloaded for Json objects)
	stringstream ss;
	ss << mj;

	Json mj2;

	//cout<<"before = "<<mj2<<endl;
	// you can load it from a stringstream (because >> is overloaded for Json objects)
	ss >> mj2;

	//cout<<"after = "<<mj2<<endl;

	// you can search for values by key (regardless of their depth)
	double my_utc = find_json_value<double>(my_tle, "utc");
	//cout<<"find_json_value<double>(my_tle, \"utc\") = "<<my_utc<<endl;

	// you can search for Json objects by key (regarless of their depth)
	Json huh;
	huh = find_json_object(my_tle, "utc");
	//cout<<"find_json_object(my_tle, \"utc\") = "<<huh<<endl;

	// you can update json values with matching key
	update_json_value(mj2, "utc", 123.45);

	//cout<<"after = "<<mj2<<endl;

	cout<<"Default TLE information object:"<<endl;
	cout<<Cosmos::Convert::make_tle_information_object()<<endl;
	cout<<endl<<endl;

	// get the tlestruc from the JSON

	Json default_tle = find_json_value<Json>(Cosmos::Convert::make_tle_information_object(), "tlestruc");
	cout<<"default_tle from JSON = "<<default_tle<<endl;

	Cosmos::Convert::tlestruc tle_mothership;
	tle_mothership.from_json(default_tle.dump());

	cout<<"default_tle from struc  "<<tle_mothership.to_json()<<endl;


	int max_count = 7*24*60*60;
	int spacing_offset = 100; // get rid of this bs way to do string of pearls, do it with TLE

	// get TLEs with different utcs to do the string of pearls
		// get rid of this part
	//Cosmos::Convert::tlestruc tle_sat1;
	//Cosmos::Convert::tlestruc tle_sat2;
	//Cosmos::Convert::tlestruc tle_sat3;
	//Cosmos::Convert::tlestruc tle_sat4;
	//Cosmos::Convert::tlestruc tle_sat5;
	//tle_sat1.from_json(default_tle.dump());
	//tle_sat2.from_json(default_tle.dump());
	//tle_sat3.from_json(default_tle.dump());
	//tle_sat4.from_json(default_tle.dump());
	//tle_sat5.from_json(default_tle.dump());

	//tle_sat2.utc = tle_sat1.utc - 1. * spacing_offset / 86400.;
	//tle_sat3.utc = tle_sat1.utc - 2. * spacing_offset / 86400.;
	//tle_sat4.utc = tle_sat1.utc - 3. * spacing_offset / 86400.;
	//tle_sat5.utc = tle_sat1.utc - 4. * spacing_offset / 86400.;

	double utc = 60557.6240022; // make this fixed for debugging
	cout<<"The current time = "<< std::fixed << setprecision(7) << utc <<endl;

	// update the default TLE to the fixed time
	tle_mothership.utc = utc;
	Cosmos::Convert::cartpos eci; // reuable


/* tests for TLE recovery from ECI state
	cout<<endl<<endl;
	cout<<"Now to test tle from state vectors..."<<endl;

	//initial state
	double utc_0 = utc;
	double delta_t = 0.00000001;
	double utc_1 = utc_0 + delta_t;

	tle2eci(utc_0, tle_mothership, eci);
	Cosmos::Convert::cartpos eci_0 = eci;
	cout<<"utc_0 = "<<utc_0<<endl;

	rvector eci_s0 = eci.s;
	cout<<"s0 = "<<eci_s0<<endl;
	rvector eci_v0 = eci.v;
	cout<<"v0 = "<<eci_v0<<endl;

	tle2eci(utc_1, tle_mothership, eci);
	rvector eci_s1 = eci.s;
	cout<<"s1 = "<<eci_s1<<endl;
	rvector eci_v1 = eci.v;
	cout<<"v1 = "<<eci_v1<<endl;

	cout<<"\n  deltas:\n"<<endl;

	cout
		<<(eci_s1.col[0]-eci_s0.col[0])/delta_t/86400.<<"\t"
		<<(eci_s1.col[1]-eci_s0.col[1])/delta_t/86400.<<"\t"
		<<(eci_s1.col[2]-eci_s0.col[2])/delta_t/86400.<<"\t"
		<<(eci_v1.col[0]-eci_v0.col[0])/delta_t/86400.<<"\t"
		<<(eci_v1.col[1]-eci_v0.col[1])/delta_t/86400.<<"\t"
		<<(eci_v1.col[2]-eci_v0.col[2])/delta_t/86400.<<endl;

	cout<<"So looks good"<<endl;

	cout<<"the TLE used = "<<tle_mothership.to_json()<<endl<<endl;

	Cosmos::Convert::tlestruc tle_0;
	eci2tle(eci_0,tle_0);

	cout<<"the TLE recovered via eci2tle = "<<tle_0.to_json()<<endl<<endl;

	Cosmos::Convert::tlestruc tle_1;
	eci2tle2(eci_0,tle_1);
	cout<<"the TLE recovered via eci2tle2= "<<tle_1.to_json()<<endl<<endl;

	std::cout << std::showpos; // Force showing + sign

	cout<<"Analsis of TLE Error for eci2tle(..):\n\n";
	cout<<"  utc error =\t"<<tle_0.utc-tle_mothership.utc<<endl;
	cout<<"  bstar error\t= "<<tle_0.bstar-tle_mothership.bstar<<endl;
	cout<<"  i error\t= "<<tle_0.i-tle_mothership.i<<endl;
	cout<<"  raan error\t= "<<tle_0.raan-tle_mothership.raan<<endl;
	cout<<"  e error\t= "<<tle_0.e-tle_mothership.e<<endl;
	cout<<"  ap error\t= "<<tle_0.ap-tle_mothership.ap<<endl;
	cout<<"  ma error\t= "<<tle_0.ma-tle_mothership.ma<<endl;
	cout<<"  mm error\t= "<<tle_0.mm-tle_mothership.mm<<endl;
	cout<<"  dmm error\t= "<<tle_0.dmm-tle_mothership.dmm<<endl;
	cout<<"  ddmm error\t= "<<tle_0.ddmm-tle_mothership.ddmm<<endl;

	cout<<endl<<endl;
	cout<<"Analsis of TLE Error for eci2tle2(..):\n\n";
	cout<<"  utc error =\t"<<tle_1.utc-tle_mothership.utc<<endl;
	cout<<"  bstar error\t= "<<tle_1.bstar-tle_mothership.bstar<<endl;
	cout<<"  i error\t= "<<tle_1.i-tle_mothership.i<<endl;
	cout<<"  raan error\t= "<<tle_1.raan-tle_mothership.raan<<endl;
	cout<<"  e error\t= "<<tle_1.e-tle_mothership.e<<endl;
	cout<<"  ap error\t= "<<tle_1.ap-tle_mothership.ap<<endl;
	cout<<"  ma error\t= "<<tle_1.ma-tle_mothership.ma<<endl;
	cout<<"  mm error\t= "<<tle_1.mm-tle_mothership.mm<<endl;
	cout<<"  dmm error\t= "<<tle_1.dmm-tle_mothership.dmm<<endl;
	cout<<"  ddmm error\t= "<<tle_1.ddmm-tle_mothership.ddmm<<endl;

	std::cout << std::noshowpos; // Force showing + sign
*/

	Json my_swarm_object = Cosmos::Convert::make_swarm_information_object("Swarm #1", "The 1st Swarm among many...", { "mothership", "childsat1", "childsat2", "childsat3", "childsat4" });

	cout<<"my_swarm_object = "<<my_swarm_object<<endl;
	// test for Node Information Object
	cout<<"node_information_object = "<<Cosmos::Convert::make_node_information_object(my_swarm_object)<<endl;


	cout<<"Testing with cosmosstruc"<<endl;
	Cosmos::Support::cosmosstruc c;
	//vector<Cosmos::Convert::tlestruc> my_json_tle_vector = find_json_value<vector<Cosmos::Convert::tlestruc>>(c, "tle");
	vector<Cosmos::Convert::tlestruc> my_json_tle_vector;
	cout<<"my_json_tle_vector = "<<my_json_tle_vector<<endl;
	cout<<"my_cosmos_tle_vector = "<<c.tle<<endl;
	

	if(!make_output_files) return 0;

// this RIC orbit position generation is skipped for now
/*
	cout<<endl;
	cout<<"Test for RIC...."<<endl<<endl;

	// Mothership.dat
	ofstream outfile_mothership("/home/user/cosmos/source/core/build/newdat2/mothership.dat");
	ofstream outfile_childsat1("/home/user/cosmos/source/core/build/newdat2/childsat1.dat");
	ofstream outfile_childsat2("/home/user/cosmos/source/core/build/newdat2/childsat2.dat");
	ofstream outfile_childsat3("/home/user/cosmos/source/core/build/newdat2/childsat3.dat");
	ofstream outfile_childsat4("/home/user/cosmos/source/core/build/newdat2/childsat4.dat");

	cout<<"Mothership TLE = "<<tle_mothership.to_json()<<endl;

	// 1) get initial position and velocity of mothership
	tle2eci(utc, tle_mothership, eci);
	cout<<"Initial utc            = "<<utc<<endl;
	cout<<"Initial mothership tle = "<<tle_mothership.to_json()<<endl;
	cout<<"Initial mothership eci = "<<eci.to_json()<<endl;

	// 2) use that as origin for RIC setup of ChildSat1
	// RIC = (-10000, 0, 0)
	rvector	ric_childsat1;
	rvector	ric_childsat2;
	rvector	ric_childsat3;
	rvector	ric_childsat4;

	ric_childsat1.col[0] = 0;
	ric_childsat1.col[1] = 0;
	ric_childsat1.col[2] = -100000.;

	ric_childsat2.col[0] = 0;
	ric_childsat2.col[1] = 0;
	ric_childsat2.col[2] = 100000.;

	ric_childsat3.col[0] = 0;
	ric_childsat3.col[1] = -100000;
	ric_childsat3.col[2] = 0;

	ric_childsat4.col[0] = 0;
	ric_childsat4.col[1] = +100000;
	ric_childsat4.col[2] = 0;

	// find the tle for childsat1
	Cosmos::Convert::cartpos result1;
	ric2eci(eci, ric_childsat1, result1);
	Cosmos::Convert::tlestruc tle_childsat1;
	result1.utc = utc; // TODO: fix, this should be happening in ric2eci...
	eci2tle(result1,tle_childsat1);
	cout<<"childsat1 TLE recovered via eci2tle = "<<tle_childsat1.to_json()<<endl<<endl;

	// find the tle for childsat2
	Cosmos::Convert::cartpos result2;
	ric2eci(eci, ric_childsat2, result2);
	Cosmos::Convert::tlestruc tle_childsat2;
	result2.utc = utc; // TODO: fix, this should be happening in ric2eci...
	eci2tle(result2,tle_childsat2);
	cout<<"childsat2 TLE recovered via eci2tle = "<<tle_childsat2.to_json()<<endl<<endl;

	// find the tle for childsat3
	Cosmos::Convert::cartpos result3;
	ric2eci(eci, ric_childsat3, result3);
	Cosmos::Convert::tlestruc tle_childsat3;
	result3.utc = utc; // TODO: fix, this should be happening in ric2eci...
	eci2tle(result3,tle_childsat3);
	cout<<"childsat3 TLE recovered via eci2tle = "<<tle_childsat3.to_json()<<endl<<endl;

	// find the tle for childsat4
	Cosmos::Convert::cartpos result4;
	ric2eci(eci, ric_childsat4, result4);
	Cosmos::Convert::tlestruc tle_childsat4;
	result4.utc = utc; // TODO: fix, this should be happening in ric2eci...
	eci2tle(result4,tle_childsat4);
	cout<<"childsat4 TLE recovered via eci2tle = "<<tle_childsat4.to_json()<<endl<<endl;

	// now we have TLE for mothership and all trailing childsat

	int count = 0;
	for(double t = utc; t < utc+10.00; t+=1./86400.)	{
		tle2eci(t, tle_mothership, eci);
		outfile_mothership<<fixed<<eci.s<<","<<t<<endl;

		tle2eci(t, tle_childsat1, eci);
		outfile_childsat1<<fixed<<eci.s<<","<<t<<endl;

		tle2eci(t, tle_childsat2, eci);
		outfile_childsat2<<fixed<<eci.s<<","<<t<<endl;

		tle2eci(t, tle_childsat3, eci);
		outfile_childsat3<<fixed<<eci.s<<","<<t<<endl;

		tle2eci(t, tle_childsat4, eci);
		outfile_childsat4<<fixed<<eci.s<<","<<t<<endl;

		if(++count>=max_count)	break;
	}

*/

	cout<<endl<<endl;
	//if(!make_output_files) return 0;
	cout<<"now to test the targets....\n\n\n";

	ofstream outfile_target1("/home/user/cosmos/source/core/build/newdat2/targets/Honolulu.eci");
	ofstream outfile_target2("/home/user/cosmos/source/core/build/newdat2/targets/Hilo.dat");
	ofstream outfile_target3("/home/user/cosmos/source/core/build/newdat2/targets/Kahului.dat");
	ofstream outfile_target4("/home/user/cosmos/source/core/build/newdat2/targets/Kapaa.dat");
	ofstream outfile_target5("/home/user/cosmos/source/core/build/newdat2/targets/Kaunakakai.dat");
	ofstream outfile_target6("/home/user/cosmos/source/core/build/newdat2/targets/Lanai City.dat");
	ofstream outfile_target7("/home/user/cosmos/source/core/build/newdat2/targets/Puuwai.dat");


// load these from target.dat file

	// Honolulu -- update if diff successful
	Cosmos::Support::targetstruc target1;
	target1.loc.pos.geod.s.h = 10;
	target1.loc.pos.geod.s.lat = DEG2RAD(21.351);
	target1.loc.pos.geod.s.lon = DEG2RAD(-157.980);
	target1.loc.pos.geod.utc = utc;

	// Hilo
	Cosmos::Support::targetstruc target2;
	target2.loc.pos.geod.s.h = 18;
	target2.loc.pos.geod.s.lat = DEG2RAD(19.7070);
	target2.loc.pos.geod.s.lon = DEG2RAD(155.0899);
	target2.loc.pos.geod.utc = utc;

	// Kahului
	Cosmos::Support::targetstruc target3;
	target3.loc.pos.geod.s.h = 3;
	target3.loc.pos.geod.s.lat = DEG2RAD(20.8890);
	target3.loc.pos.geod.s.lon = DEG2RAD(156.4729);
	target3.loc.pos.geod.utc = utc;

	// Kapaa
	Cosmos::Support::targetstruc target4;
	target4.loc.pos.geod.s.h = 6;
	target4.loc.pos.geod.s.lat = DEG2RAD(22.0883);
	target4.loc.pos.geod.s.lon = DEG2RAD(159.3379);
	target4.loc.pos.geod.utc = utc;

	// Kaunakakai
	Cosmos::Support::targetstruc target5;
	target5.loc.pos.geod.s.h = 3;
	target5.loc.pos.geod.s.lat = DEG2RAD(21.0939);
	target5.loc.pos.geod.s.lon = DEG2RAD(157.0194);
	target5.loc.pos.geod.utc = utc;

	// Lanai
	Cosmos::Support::targetstruc target6;
	target6.loc.pos.geod.s.h = 500;
	target6.loc.pos.geod.s.lat = DEG2RAD(20.8270);
	target6.loc.pos.geod.s.lon = DEG2RAD(156.9208);
	target6.loc.pos.geod.utc = utc;

	// Niihau
	Cosmos::Support::targetstruc target7;
	target7.loc.pos.geod.s.h = 10;
	target7.loc.pos.geod.s.lat = DEG2RAD(21.8969);
	target7.loc.pos.geod.s.lon = DEG2RAD(160.1583);
	target7.loc.pos.geod.utc = utc;

	int count = 0;
	for(double t = utc; t < utc+10.00; t+=1./86400.)	{

		target1.loc.pos.geod.utc = t;
		target1.loc.pos.geod.pass++;
		pos_geod(target1.loc);
		if(make_output_files) if(outfile_target1.is_open())	outfile_target1<<fixed<<target1.loc.pos.eci.s;

		target2.loc.pos.geod.utc = t;
		target2.loc.pos.geod.pass++;
		pos_geod(target2.loc);
		if(make_output_files) if(outfile_target2.is_open())	outfile_target2<<fixed<<target2.loc.pos.eci.s;

		target3.loc.pos.geod.utc = t;
		target3.loc.pos.geod.pass++;
		pos_geod(target3.loc);
		if(make_output_files) if(outfile_target3.is_open())	outfile_target3<<fixed<<target3.loc.pos.eci.s;

		target4.loc.pos.geod.utc = t;
		target4.loc.pos.geod.pass++;
		pos_geod(target4.loc);
		if(make_output_files) if(outfile_target4.is_open())	outfile_target4<<fixed<<target4.loc.pos.eci.s;

		target5.loc.pos.geod.utc = t;
		target5.loc.pos.geod.pass++;
		pos_geod(target5.loc);
		if(make_output_files) if(outfile_target5.is_open())	outfile_target5<<fixed<<target5.loc.pos.eci.s;

		target6.loc.pos.geod.utc = t;
		target6.loc.pos.geod.pass++;
		pos_geod(target6.loc);
		if(make_output_files) if(outfile_target6.is_open())	outfile_target6<<fixed<<target6.loc.pos.eci.s;

		target7.loc.pos.geod.utc = t;
		target7.loc.pos.geod.pass++;
		pos_geod(target7.loc);
		if(make_output_files) if(outfile_target7.is_open())	outfile_target7<<fixed<<target7.loc.pos.eci.s;



		if(make_output_files) if(outfile_target1.is_open())	outfile_target1<<","<<t<<endl;
		if(make_output_files) if(outfile_target2.is_open())	outfile_target2<<","<<t<<endl;
		if(make_output_files) if(outfile_target3.is_open())	outfile_target3<<","<<t<<endl;
		if(make_output_files) if(outfile_target4.is_open())	outfile_target4<<","<<t<<endl;
		if(make_output_files) if(outfile_target5.is_open())	outfile_target5<<","<<t<<endl;
		if(make_output_files) if(outfile_target6.is_open())	outfile_target6<<","<<t<<endl;
		if(make_output_files) if(outfile_target7.is_open())	outfile_target7<<","<<t<<endl;

		if(++count>=max_count)	break;

	}
	count = 0;

	if(make_output_files) if(outfile_target1.is_open())	outfile_target1.close();
	if(make_output_files) if(outfile_target2.is_open())	outfile_target2.close();
	if(make_output_files) if(outfile_target3.is_open())	outfile_target3.close();
	if(make_output_files) if(outfile_target4.is_open())	outfile_target4.close();
	if(make_output_files) if(outfile_target5.is_open())	outfile_target5.close();
	if(make_output_files) if(outfile_target6.is_open())	outfile_target6.close();
	if(make_output_files) if(outfile_target7.is_open())	outfile_target7.close();

	return 0;

}
