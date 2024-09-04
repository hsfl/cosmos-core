#include "support/configCosmos.h"
#include "support/stringlib.h"
#include "support/convertdef.h"
#include "support/convertlib.h"
#include "support/jsondef.h"
#include <fstream>
//#include "support/json11.hpp"
//#include "../../../../books/C++/sandbox/include/jsonface.h"

using namespace json11;

int main(int argc, char *argv[])
{
	/*
    for (uint16_t precision=0; precision<10; ++precision)
    {
        printf("%u ", precision);
        int16_t power = -9;
        for (float value=1e-10; value<1e10; value=pow(10., ++power))
        {
            printf("%s ", to_floating(value, precision).c_str());
        }
        printf("\n");
        printf("%u ", precision);
        power = -9;
        for (float value=1e-10; value<1e10; value=pow(10., ++power))
        {
            printf("%s ", to_floatexp(value, precision).c_str());
        }
        printf("\n");
        printf("%u ", precision);
        power = -9;
        for (float value=1e-10; value<1e10; value=pow(10., ++power))
        {
            printf("%s ", to_floatany(value, precision).c_str());
        }
        printf("\n");
    }
*/

	bool make_output_files = false;
	Cosmos::Convert::tlestruc my_tle;
	Json mj(my_tle);
	cout<<mj.dump()<<endl;

	// jsonface
	cout<<mj<<endl;


	// change a value
	//update_
	stringstream ss;
	ss << mj;

	Json mj2;

	cout<<"before = "<<mj2<<endl;
	ss >> mj2;

	cout<<"after = "<<mj2<<endl;

	double my_utc = find_json_value<double>(my_tle, "utc");

	cout<<"my_utc = "<<my_utc<<endl;

	Json huh;
	huh = find_json_object(my_tle, "utc");
	cout<<"huh = "<<huh<<endl;

	update_json_value(mj2, "utc", 123.45);

	cout<<"after = "<<mj2<<endl;

	cout<<Cosmos::Convert::make_tle_information_object()<<endl;

	// get the tlestruc from the JSON

	Json TLE = find_json_value<Json>(Cosmos::Convert::make_tle_information_object(), "tlestruc");
	cout<<"TLE from JSON = "<<TLE<<endl;

	Cosmos::Convert::tlestruc TTT;
	TTT.from_json(TLE.dump());

	cout<<"TLE from struc  "<<TTT.to_json()<<endl;

	Json that_tle = TTT;

	cout<<"TLE from Json object  "<<that_tle<<endl;



	double utc = Cosmos::Support::currentmjd();

	cout<<"The current time = "<< std::fixed << setprecision(7) << utc <<endl;

	Cosmos::Convert::cartpos eci;
	cout<<eci<<endl;
	tle2eci(utc, TTT, eci);
	cout<<eci<<endl;

	cout<<"now to test an orbit....\n\n\n";

	ofstream outfile_sat1("/home/user/cosmos/source/core/build/eci_orbit_sat1.dat");
	ofstream outfile_sat2("/home/user/cosmos/source/core/build/eci_orbit_sat2.dat");
	ofstream outfile_sat3("/home/user/cosmos/source/core/build/eci_orbit_sat3.dat");
	ofstream outfile_sat4("/home/user/cosmos/source/core/build/eci_orbit_sat4.dat");
	ofstream outfile_sat5("/home/user/cosmos/source/core/build/eci_orbit_sat5.dat");
	ofstream outfile_target1("/home/user/cosmos/source/core/build/eci_target1.dat");
	ofstream outfile_target2("/home/user/cosmos/source/core/build/eci_target2.dat");
	ofstream outfile_target3("/home/user/cosmos/source/core/build/eci_target3.dat");
	ofstream outfile_target4("/home/user/cosmos/source/core/build/eci_target4.dat");
	ofstream outfile_target5("/home/user/cosmos/source/core/build/eci_target5.dat");
	ofstream outfile_target6("/home/user/cosmos/source/core/build/eci_target6.dat");

	int max_count = 24*60*60;
	int spacing_offset = 100; // get rid of this bs way to do string of pearls, do it with TLE

	int count = 0;
	for(double t = utc - 0 * spacing_offset/86400.; t < utc+10.00; t+=1./86400.)	{
		tle2eci(t, TTT, eci);
		if(make_output_files) if(outfile_sat1.is_open())	outfile_sat1<<fixed<<eci.s;
		if(make_output_files) if(outfile_sat1.is_open())	outfile_sat1<<","<<t+0.*spacing_offset/86400.<<endl;
		if(++count>=max_count)	break;
	}
	count = 0;

	for(double t = utc - 1 * spacing_offset/86400.; t < utc+10.00; t+=1./86400.)	{
		tle2eci(t, TTT, eci);
		if(make_output_files) if(outfile_sat2.is_open())	outfile_sat2<<fixed<<eci.s;
		if(make_output_files) if(outfile_sat2.is_open())	outfile_sat2<<","<<t+1.*spacing_offset/86400.<<endl;
		if(++count>=max_count)	break;
	}
	count = 0;

	for(double t = utc - 2 * spacing_offset/86400.; t < utc+10.00; t+=1./86400.)	{
		tle2eci(t, TTT, eci);
		if(make_output_files) if(outfile_sat3.is_open())	outfile_sat3<<fixed<<eci.s;
		if(make_output_files) if(outfile_sat3.is_open())	outfile_sat3<<","<<t+2.*spacing_offset/86400.<<endl;
		if(++count>=max_count)	break;
	}
	count = 0;

	for(double t = utc - 3 * spacing_offset/86400.; t < utc+10.00; t+=1./86400.)	{
		tle2eci(t, TTT, eci);
		if(make_output_files) if(outfile_sat4.is_open())	outfile_sat4<<fixed<<eci.s;
		if(make_output_files) if(outfile_sat4.is_open())	outfile_sat4<<","<<t+3.*spacing_offset/86400.<<endl;
		if(++count>=max_count)	break;
	}
	count = 0;

	for(double t = utc - 4 * spacing_offset/86400.; t < utc+10.00; t+=1./86400.)	{
		tle2eci(t, TTT, eci);
		if(make_output_files) if(outfile_sat5.is_open())	outfile_sat5<<fixed<<eci.s;
		if(make_output_files) if(outfile_sat5.is_open())	outfile_sat5<<","<<t+4.*spacing_offset/86400.<<endl;
		if(++count>=max_count)	break;
	}
	count = 0;


	// make an outfile for a target
	Cosmos::Support::targetstruc target;
	target.loc.pos.geod.s.h = 10;
	target.loc.pos.geod.s.lat = DEG2RAD(21.351);
	target.loc.pos.geod.s.lon = DEG2RAD(-157.980);
	target.loc.pos.geod.utc = utc;

	// some latitude lines
	Cosmos::Support::targetstruc target2;
	target2.loc.pos.geod.s.h = 0;
	target2.loc.pos.geod.s.lat = DEG2RAD(60);
	target2.loc.pos.geod.s.lon = DEG2RAD(0);
	target2.loc.pos.geod.utc = utc;

	Cosmos::Support::targetstruc target3;
	target3.loc.pos.geod.s.h = 0;
	target3.loc.pos.geod.s.lat = DEG2RAD(30);
	target3.loc.pos.geod.s.lon = DEG2RAD(0);
	target3.loc.pos.geod.utc = utc;

	// equator
	Cosmos::Support::targetstruc target4;
	target4.loc.pos.geod.s.h = 0;
	target4.loc.pos.geod.s.lat = DEG2RAD(0);
	target4.loc.pos.geod.s.lon = DEG2RAD(0);
	target4.loc.pos.geod.utc = utc;

	Cosmos::Support::targetstruc target5;
	target5.loc.pos.geod.s.h = 0;
	target5.loc.pos.geod.s.lat = DEG2RAD(-30);
	target5.loc.pos.geod.s.lon = DEG2RAD(0);
	target5.loc.pos.geod.utc = utc;

	Cosmos::Support::targetstruc target6;
	target6.loc.pos.geod.s.h = 0;
	target6.loc.pos.geod.s.lat = DEG2RAD(-60);
	target6.loc.pos.geod.s.lon = DEG2RAD(0);
	target6.loc.pos.geod.utc = utc;

	for(double t = utc - 0 * spacing_offset/86400.; t < utc+10.00; t+=1./86400.)	{

		target.loc.pos.geod.utc = t;
		target.loc.pos.geod.pass++;
		pos_geod(target.loc);
		if(make_output_files) if(outfile_target1.is_open())	outfile_target1<<fixed<<target.loc.pos.eci.s;

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

		if(make_output_files) if(outfile_target1.is_open())	outfile_target1<<","<<t<<endl;
		if(make_output_files) if(outfile_target2.is_open())	outfile_target2<<","<<t<<endl;
		if(make_output_files) if(outfile_target3.is_open())	outfile_target3<<","<<t<<endl;
		if(make_output_files) if(outfile_target4.is_open())	outfile_target4<<","<<t<<endl;
		if(make_output_files) if(outfile_target5.is_open())	outfile_target5<<","<<t<<endl;
		if(make_output_files) if(outfile_target6.is_open())	outfile_target6<<","<<t<<endl;

		if(++count>=max_count)	break;

	}
	count = 0;

	if(make_output_files) if(outfile_sat1.is_open())	outfile_sat1.close();
	if(make_output_files) if(outfile_sat2.is_open())	outfile_sat2.close();
	if(make_output_files) if(outfile_sat3.is_open())	outfile_sat3.close();
	if(make_output_files) if(outfile_sat4.is_open())	outfile_sat4.close();
	if(make_output_files) if(outfile_sat5.is_open())	outfile_sat5.close();
	if(make_output_files) if(outfile_target1.is_open())	outfile_target1.close();
	if(make_output_files) if(outfile_target2.is_open())	outfile_target2.close();
	if(make_output_files) if(outfile_target3.is_open())	outfile_target3.close();
	if(make_output_files) if(outfile_target4.is_open())	outfile_target4.close();
	if(make_output_files) if(outfile_target5.is_open())	outfile_target5.close();
	if(make_output_files) if(outfile_target6.is_open())	outfile_target6.close();

	return 0;

}
