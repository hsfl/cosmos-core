#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "support/timelib.h"
#include "support/jsondef.h"
#include "gtest/gtest.h"

using namespace Convert;
using namespace std;

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

template <class T>
void setup_locstruc_geod(locstruc& l, const T& params) {
    l.pos.geod.utc = 59270.949421299999813;
    l.pos.geod.s.lat = DEG2RAD(params.lat);
    l.pos.geod.s.lon = DEG2RAD(params.lon);
    l.pos.geod.s.h = params.h;
}

template <class T>
void setup_locstruc_geoc(locstruc& l, const T& params) {
    l.pos.geoc.utc = 59270.949421299999813;
    l.pos.eci.s.col[0] = params.x;
    l.pos.eci.s.col[1] = params.y;
    l.pos.eci.s.col[2] = params.z;
}

template <class T>
void setup_locstruc_eci(locstruc& l, const T& params) {
    l.pos.eci.utc = 59270.949421299999813;
    l.pos.eci.s.col[0] = params.x;
    l.pos.eci.s.col[1] = params.y;
    l.pos.eci.s.col[2] = params.z;
}

template <class T, class U>
void compare_results_x_y_z(const T& before, const T& after, const U& params, std::ostream& os, double tolerance = 0.001) {
    bool failed = false;

    // Compare X, Y, Z coordinates
    if (abs(before.col[0] - after.col[0]) > tolerance) {
        os << "X mismatch at (" << params.x << ", " << params.y << ", " << params.z << "): "
           << "before.x = " << before.col[0] << "\tafter.x = " << after.col[0] << "\terror = " << abs(before.col[0] - after.col[0]) << "\n";
        failed = true;
    }

    if (abs(before.col[1] - after.col[1]) > tolerance) {
        os << "Y mismatch at (" << params.x << ", " << params.y << ", " << params.z << "): "
           << "before.y = " << before.col[1] << "\tafter.y = " << after.col[1] << "\terror = " << abs(before.col[1] - after.col[1]) << "\n";
        failed = true;
    }

    if (abs(before.col[2] - after.col[2]) > 0.01) {
        os << "Z mismatch at (" << params.x << ", " << params.y << ", " << params.z << "): "
           << "before.z = " << before.col[2] << "\tafter.z = " << after.col[2] << "\terror = " << abs(before.col[2] - after.col[2]) << "\n";
        failed = true;
    }

    // Report failure if there were mismatches
    if (failed) {
        os << "Comparison failed.\n";
    }
}

template <class T, class U>
void compare_results_lat_long_h(const T& before, const T& after, const U& params, std::ostream& os, double tolerance = 0.001) {
    
    bool failed = false;

    //if (lat_error_to_meters(abs(before.lat - after.lat), before.h) > 100.00) {
    if (lat_error_to_meters(abs(before.lat - after.lat), before.h) > tolerance) {
        os << "Lat mismatch at (" << params.lat << ", " << params.lon << ", " << params.h << "): "
           << "before.lat = " << before.lat << "\tafter.lat = " << after.lat << "\terror = " << abs(before.lat - after.lat) << "\t(" << lat_error_to_meters(abs(before.lat - after.lat), before.h) << " meters)\n";
        failed = true;
    }

   //if (lon_error_to_meters(params.lat, abs(before.lon - after.lon), before.h) > 100.00) {
    if (lon_error_to_meters(params.lat, abs(before.lon - after.lon), before.h) > tolerance) {
        os << "Lon mismatch at (" << params.lat << ", " << params.lon << ", " << params.h << "): "
           << "before.lon = " << before.lon << "\tafter.lon = " << after.lon << "\terror = " << abs(before.lon - after.lon) << "\t(" << lon_error_to_meters(params.lat, abs(before.lon - after.lon), before.h) << " meters)\n";
        failed = true;
    }

    //if (abs(before.h - after.h) > 1.0) {
    if (abs(before.h - after.h) > 0.01) {
        os << "  H mismatch at (" << params.lat << ", " << params.lon << ", " << params.h << "): "
           << "  before.h = " << before.h << "\tafter.h   = " << after.h << "\terror = " << abs(before.h - after.h) << "\t(" << abs(before.h - after.h) << " meters)\n";
        failed = true;
    }

    // Report failure if there were mismatches
    if (failed) {
        os << "Comparison failed.\n";
    }
}

// Structures to hold test parameters
struct test_parameters_lat_long_h {
   double lat;
   double lon;
   double h;
};

struct test_parameters_x_y_z {
   double x;
   double y;
   double z;
};

// Overload the PrintTo functions to print test parameters cleanly
void PrintTo(const test_parameters_x_y_z& params, ::std::ostream* os) {
   *os << "ECI to GEOD:\t(" << params.x << ",\t" << params.y << ",\t" << params.z << ")";
}

void PrintTo(const test_parameters_lat_long_h& params, ::std::ostream* os) {
   *os << "GEOD to ECI:\tlat: " << params.lat << ",\tlon: " << params.lon << ",\th: " << params.h;
}

// Test fixture for parameterized tests
class test_ECI_to_GEOD_transformation : public ::testing::TestWithParam<test_parameters_x_y_z> {};
class test_GEOD_to_ECI_transformation : public ::testing::TestWithParam<test_parameters_lat_long_h> {};
class test_GEOD_to_GEOC_transformation : public :: testing::TestWithParam<test_parameters_lat_long_h> {};
class test_GEOC_to_GEOD_transformation : public :: testing::TestWithParam<test_parameters_x_y_z> {};
class test_ECI_to_GEOC_transformation : public :: testing::TestWithParam<test_parameters_x_y_z> {};
class test_GEOC_to_ECI_transformation : public :: testing::TestWithParam<test_parameters_x_y_z> {};

// Tests ECI to GEOD
TEST_P(test_ECI_to_GEOD_transformation, ECI_to_GEOD) {

    locstruc l;

    setup_locstruc_eci(l, GetParam());
    rvector before = l.pos.eci.s;

    l.pos.eci.pass++;
    pos_eci(l);     //ECI to GEOD

    l.pos.geod.pass++;
    pos_geod(l);     //GEOD to ECI
    rvector after = l.pos.eci.s;

    compare_results_x_y_z(before, after, GetParam(), std::cout);
}

//Tests GEOD to ECI
TEST_P(test_GEOD_to_ECI_transformation, GEOD_to_ECI) {

    locstruc l;

    setup_locstruc_geod(l, GetParam());
    gvector before = l.pos.geod.s;

    l.pos.geod.pass++;
    pos_geod(l);     //GEOD to ECI

    l.pos.eci.pass++;
    pos_eci(l);     //ECI to GEOD
    gvector after = l.pos.geod.s;

    compare_results_lat_long_h(before, after, GetParam(), std::cout);
}

//Tests GEOD to GEOC
TEST_P(test_GEOD_to_GEOC_transformation, GEOD_to_GEOC){

    locstruc l; 
    setup_locstruc_geod(l, GetParam());
    gvector before = l.pos.geod.s;

    l.pos.geod.pass++;
    pos_geod2geoc(l);

    l.pos.geoc.pass++;
    pos_geoc2geod(l);
    gvector after = l.pos.geod.s;

    compare_results_lat_long_h(before, after, GetParam(), std::cout);
}

//Tests GEOC to GEOD
TEST_P(test_GEOC_to_GEOD_transformation, GEOC_to_GEOD){

    locstruc l; 
    setup_locstruc_geoc(l, GetParam());
    rvector before = l.pos.geoc.s;

    l.pos.geoc.pass++;
    pos_geoc2geod(l);

    l.pos.geod.pass++;
    pos_geod2geoc(l);
    rvector after = l.pos.geoc.s;

    compare_results_x_y_z(before, after, GetParam(), std::cout);
}

// Tests ECI to GEOC
TEST_P(test_ECI_to_GEOC_transformation, ECI_to_GEOC) {
   
    locstruc l;
    setup_locstruc_eci(l, GetParam());
    rvector before = l.pos.eci.s;
  
    l.pos.eci.pass++;
    pos_eci2geoc(l);     //ECI to GEOC

    l.pos.geoc.pass++;
    pos_geoc2eci(l);     //GEOC to ECI
    rvector after = l.pos.eci.s;

    compare_results_x_y_z(before, after, GetParam(), std::cout);
}

// Tests GEOC to ECI
TEST_P(test_GEOC_to_ECI_transformation, GEOC_to_ECI) {
  
    locstruc l;
    setup_locstruc_geoc(l, GetParam());
    rvector before = l.pos.geoc.s;
  
    l.pos.geoc.pass++;
    pos_geoc2eci(l);     //GEOC to ECI

    l.pos.eci.pass++;
    pos_eci2geoc(l);     //ECI to GEOC
    rvector after = l.pos.geoc.s;

    compare_results_x_y_z(before, after, GetParam(), std::cout);
}

std::vector<test_parameters_x_y_z> GenerateSphericalCoordinates() {
   std::vector<test_parameters_x_y_z> params;
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

vector<test_parameters_lat_long_h> GenerateGeospatialGrid() {
   vector<test_parameters_lat_long_h> params;
   for (double lat = -90.0; lat <= 90.0; lat += 2.0) {
       for (double lon = -180.0; lon <= 180.0; lon += 2.0) {
           for (double h = 1000; h >= -100; h -= 100) {
               params.push_back({lat, lon, h});
           }
       }
   }
   return params;
}

// Instantiate the test cases
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_ECI_to_GEOD_transformation, ::testing::ValuesIn(GenerateSphericalCoordinates()));
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_GEOD_to_ECI_transformation, ::testing::ValuesIn(GenerateGeospatialGrid()));
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_GEOD_to_GEOC_transformation, ::testing::ValuesIn(GenerateGeospatialGrid()));
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_GEOC_to_GEOD_transformation, ::testing::ValuesIn(GenerateSphericalCoordinates()));
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_ECI_to_GEOC_transformation, ::testing::ValuesIn(GenerateSphericalCoordinates()));
INSTANTIATE_TEST_SUITE_P(test_all_coordinate_transformations, test_GEOC_to_ECI_transformation, ::testing::ValuesIn(GenerateSphericalCoordinates()));

int main(int argc, char **argv) {
   cout << "Unit Testing COSMOS Coordinate Transformations" << endl;
   cout << "==============================================" << endl;

   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
