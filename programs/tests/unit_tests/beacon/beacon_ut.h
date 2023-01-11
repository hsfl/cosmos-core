#ifndef __BEACON_UT_H__
#define __BEACON_UT_H__

#include "support/beacon.h"
#include "gtest/gtest.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace Beacon {

// Helper test fixture class for testing beacons
class BeaconUTest : public ::testing::Test {
protected:
    BeaconUTest() {}
    ~BeaconUTest() {}

    // Initializes the text fixture class
    void SetUp() override;
    // Clean up for text fixture class
    void TearDown() override;

    // Member variables
    // Beacon class to test with
    Support::Beacon beacon;
    Support::Agent* agent;
    Support::Agent* agent2;

    // For setting up the devices
    size_t num_cpus = 1;
    size_t num_rws = 1;
    size_t num_mtrs = 1;
    size_t num_gyros = 1;

    const string TEST_NODE_NAME = "beacon_ut_test_node";

    
}; // End class BeaconUTest

} // End namespace Beacon
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos

#endif // End __BEACON_UT_H__
