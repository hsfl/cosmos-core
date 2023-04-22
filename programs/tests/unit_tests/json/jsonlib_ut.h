#ifndef __CHANNEL_UT_H__
#define __JSONLIB_UT_H__

#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "gtest/gtest.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace Json {

// Helper test fixture class for testing channels
class JsonlibUTest : public ::testing::Test {
protected:
    JsonlibUTest() {}
    ~JsonlibUTest() {}

    // Initializes the text fixture class
    void SetUp() override;
    // Clean up for text fixture class
    void TearDown() override;

    //! Creates the .ini files necessary to initialize the cosmosstruc and namespace
    void make_devices();

    //! Checks the equality of two Vectors
    void compare_Vector(const Vector& v1, const Vector& v2, size_t LINE);

    Support::Agent* agent = nullptr;
    Support::Agent* agent2 = nullptr;
    const string TEST_NODE_NAME = "jsonlib_ut_test_node";

    // For setting up the devices
    size_t num_pload    = 2;
    size_t num_ssen     = 2;
    size_t num_imu      = 2;
    size_t num_rw       = 2;
    size_t num_mtr      = 2;
    size_t num_cpu      = 2;
    size_t num_gps      = 2;
    size_t num_ant      = 2;
    size_t num_rxr      = 2;
    size_t num_txr      = 2;
    size_t num_tcv      = 2;
    size_t num_pvstrg   = 2;
    size_t num_batt     = 2;
    size_t num_htr      = 2;
    size_t num_motr     = 2;
    size_t num_tsen     = 2;
    size_t num_thst     = 2;
    size_t num_prop     = 2;
    size_t num_swch     = 2;
    size_t num_rot      = 2;
    size_t num_stt      = 2;
    size_t num_mcc      = 2;
    size_t num_tcu      = 2;
    size_t num_bus      = 2;
    size_t num_psen     = 2;
    size_t num_suchi    = 2;
    size_t num_cam      = 2;
    size_t num_telem    = 2;
    size_t num_disk     = 2;
    size_t num_tnc      = 2;
    size_t num_bcreg    = 2;
    size_t num_gyro     = 2;
    size_t num_mag      = 2;
    size_t num_xyzsen   = 2;

    
}; // End class JsonlibUTest

} // End namespace Json
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos

#endif // End __JSONLIB_UT_H__
