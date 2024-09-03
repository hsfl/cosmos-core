#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "support/timelib.h"
#include "support/jsondef.h"
#include "gtest/gtest.h"


using namespace Convert;
using namespace std;

// todo: add test_parameters_ECI

// Constants
const double EARTH_RADIUS_METERS = 6371000.0; // Earth's radius in meters

// Function to convert latitude error in radians to meters
double lat_error_to_meters(double error_in_radians, double height) {
   return error_in_radians * (EARTH_RADIUS_METERS + height);
}

// Function to convert longitude error in radians to meters
double lon_error_to_meters(double lat, double error_in_radians, double height) {
   double lat_in_radians = lat * M_PI / 180.0;
   return error_in_radians * (EARTH_RADIUS_METERS + height) * cos(lat_in_radians);
}

// Structures to hold test parameters
struct test_parameters_GEOD {
   double lat;
   double lon;
   double h;
};

struct test_parameters_ECI {
   double x;
   double y;
   double z;
};

// Overload the PrintTo functions to print test parameters cleanly
void PrintTo(const test_parameters_GEOD& params, ::std::ostream* os) {
   *os << "GEOD:\tlat: " << params.lat << ",\tlon: " << params.lon << ",\th: " << params.h;
}

void PrintTo(const test_parameters_ECI& params, ::std::ostream* os) {
   *os << "ECI:\t(" << params.x << ",\t" << params.y << ",\t" << params.z << ")";
}

// Test fixture for parameterized tests
class test_GEOD_to_ECI_transformation : public ::testing::TestWithParam<test_parameters_GEOD> {};
class test_ECI_to_GEOD_transformation : public ::testing::TestWithParam<test_parameters_ECI> {};

// Parameterized test cases

// Tests ECI to GEOD
TEST_P(test_ECI_to_GEOD_transformation, ECI_to_GEOD) {
   cout<<"getting ECI params"<<endl;
   test_parameters_ECI params = GetParam();
   cout<<"done"<<endl;
   locstruc l;
   l.pos.eci.utc = 59270.949421299999813; // add this parameters
   l.pos.eci.s.col[0] = params.x;
   l.pos.eci.s.col[1] = params.y;
   l.pos.eci.s.col[2] = params.z;
   rvector before = l.pos.eci.s;
  
   l.pos.eci.pass++;
   pos_eci(l);     //ECI to GEOD


   // Check the GEOD values after the conversion
   gvector geod_after = l.pos.geod.s;

   l.pos.eci.s.col[0] = 0.0;
   l.pos.eci.s.col[1] = 0.0;
   l.pos.eci.s.col[2] = 0.0;
   l.pos.geod.pass++;
   pos_geod(l);
   rvector after = l.pos.eci.s;

   bool failed = false;
   std::ostringstream oss;
  
   //if (lat_error_to_meters(abs(before.lat - after.lat), before.h) > 100.00) {
   if (abs(before.col[0] - after.col[0]) > 0.001) {
       oss << "X mismatch at (" << params.x << ", " << params.y << ", " << params.z << "): "
           << "before.x = " << before.col[0] << "\tafter.x = " << after.col[0] << "\terror = " << abs(before.col[0] - after.col[0]) << "\n";
       failed = true;
   }


   //if (lon_error_to_meters(params.lat, abs(before.lon - after.lon), before.h) > 100.00) {
   if (abs(before.col[1] - after.col[1]) > 0.001) {
       oss << "Y mismatch at (" << params.x << ", " << params.y << ", " << params.z << "): "
           << "before.y = " << before.col[1] << "\tafter.y = " << after.col[1] << "\terror = " << abs(before.col[1] - after.col[1]) << "\n";
       failed = true;
   }

   //if (abs(before.h - after.h) > 1.0) {
   if (abs(before.col[2] - after.col[2]) > 0.01) {
       oss << "Z mismatch at (" << params.x << ", " << params.y << ", " << params.z << "): "
           << "before.z = " << before.col[2] << "\tafter.z   = " << after.col[2] << "\terror = " << abs(before.col[2] - after.col[2]) << "\n";
   }

   if (failed) {
       GTEST_FAIL() << oss.str();
   }
}


//Tests GEOD to ECI
TEST_P(test_GEOD_to_ECI_transformation, GEOD_to_ECI) {
   cout << "getting GEOD params" << endl;
  
   // Retrieve test parameters (latitude, longitude, height)
   test_parameters_GEOD params = GetParam();
  
   // Indicate that parameters have been retrieved
   cout << "done" << endl;

   locstruc l;
   l.pos.geod.utc = 59270.949421299999813; // set the time
   l.pos.geod.s.lat = DEG2RAD(params.lat); // set latitude (converted to radians)
   l.pos.geod.s.lon = DEG2RAD(params.lon); // set longitude (converted to radians)
   l.pos.geod.s.h = params.h; // set height (altitude)
   gvector before = l.pos.geod.s; // store the initial GEOD position

   // Print the initial GEOD values
   cout << "Before conversion:" << endl;
   cout << "Latitude (rad): " << before.lat << ", Longitude (rad): " << before.lon << ", Height: " << before.h << endl;
  
   l.pos.geod.pass++;
   pos_geod(l);


   // Check the ECI values after the conversion
   rvector eci_after = l.pos.eci.s;
   cout << "After GEOD to ECI conversion:" << endl;
   cout << "x: " << eci_after.col[0] << ", y: " << eci_after.col[1] << ", z: " << eci_after.col[2] << endl;


   l.pos.geod.s.lat = 0.0;     // Reset the latitude to 0.0 after the conversion (to check if the conversion back works)
   l.pos.geod.s.lon = 0.0;
   l.pos.geod.s.h = 0.0;
   l.pos.eci.pass++;
   pos_eci(l);     // Perform the ECI to other coordinate systems conversion, including back to GEOD
   gvector after = l.pos.geod.s;        // Store the geodetic position after the round-trip conversion (GEOD -> ECI -> GEOD)


   // Print the final GEOD values after the round-trip conversion
   cout << "After resetting GEOD and converting back:" << endl;
   cout << "Latitude (rad): " << after.lat << ", Longitude (rad): " << after.lon << ", Height: " << after.h << endl;


   bool failed = false;
   std::ostringstream oss;

   //if (lat_error_to_meters(abs(before.lat - after.lat), before.h) > 100.00) {
   if (lat_error_to_meters(abs(before.lat - after.lat), before.h) > 0.001) {
       oss << "Lat mismatch at (" << params.lat << ", " << params.lon << ", " << params.h << "): "
           << "before.lat = " << before.lat << "\tafter.lat = " << after.lat << "\terror = " << abs(before.lat - after.lat) << "\t(" << lat_error_to_meters(abs(before.lat - after.lat), before.h) << " meters)\n";
       failed = true;
   }

   //if (lon_error_to_meters(params.lat, abs(before.lon - after.lon), before.h) > 100.00) {
   if (lon_error_to_meters(params.lat, abs(before.lon - after.lon), before.h) > 0.001) {
       oss << "Lon mismatch at (" << params.lat << ", " << params.lon << ", " << params.h << "): "
           << "before.lon = " << before.lon << "\tafter.lon = " << after.lon << "\terror = " << abs(before.lon - after.lon) << "\t(" << lon_error_to_meters(params.lat, abs(before.lon - after.lon), before.h) << " meters)\n";
       failed = true;
   }

   //if (abs(before.h - after.h) > 1.0) {
   if (abs(before.h - after.h) > 0.01) {
       oss << "  H mismatch at (" << params.lat << ", " << params.lon << ", " << params.h << "): "
           << "  before.h = " << before.h << "\tafter.h   = " << after.h << "\terror = " << abs(before.h - after.h) << "\t(" << abs(before.h - after.h) << " meters)\n";
       failed = true;
   }

   if (failed) {
       GTEST_FAIL() << oss.str();
   }
}

// Generate the test parameters for GEOD on surface of Earth
vector<test_parameters_GEOD> generate_test_parameters_GEOD() {
   vector<test_parameters_GEOD> params;
   for (double lat = -90.0; lat <= 90.0; lat += 2.0) {
       for (double lon = -180.0; lon <= 180.0; lon += 2.0) {
           for (double h = 1000; h >= -100; h -= 100) {
               params.push_back({lat, lon, h});
           }
       }
   }
   return params;
}

// Generate the test parameters for ECI points in LEO
std::vector<test_parameters_ECI> generate_test_parameters_ECI() {
   std::vector<test_parameters_ECI> params;
   for (double lat = -90.0; lat <= 90.0; lat += 2.0) {
       for (double lon = -180.0; lon <= 180.0; lon += 2.0) {
           for (double h = 160000; h <= 2000000; h += 100000) {
               double altitude = EARTH_RADIUS_METERS + h;
               double rad_lat = lat * M_PI / 180.0;
               double rad_lon = lon * M_PI / 180.0;
               double x = altitude * cos(rad_lat) * cos(rad_lon);
               double y = altitude * cos(rad_lat) * sin(rad_lon);
               double z = altitude * sin(rad_lat);
               params.push_back({x, y, z});
           }
       }
   }
   return params;
}

// Instantiate the test cases
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_GEOD_to_ECI_transformation, ::testing::ValuesIn(generate_test_parameters_GEOD()));
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_ECI_to_GEOD_transformation, ::testing::ValuesIn(generate_test_parameters_ECI()));

int main(int argc, char **argv) {
   cout << "Unit Testing COSMOS Coordinate Transformations" << endl;
   cout << "==============================================" << endl;

   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
