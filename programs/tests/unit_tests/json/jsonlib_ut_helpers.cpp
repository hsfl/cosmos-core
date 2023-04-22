#ifndef __JSONLIB_UT_HELPERS_H__
#define __JSONLIB_UT_HELPERS_H__

//////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////
#include "jsonlib_ut.h"
#include "physics/physicsclass.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace Json {

//! Creates the .ini files necessary to initialize the cosmosstruc and namespace
void JsonlibUTest::make_devices()
{
    // node.ini
    agent->cinfo->node.type = NODE_TYPE_SHIP;
    // vertices.ini
    // faces.ini
    Physics::Structure structure(&agent->cinfo->node.phys);
    structure.Setup(Physics::Structure::U3);

    // pieces.ini
    // devices_general.ini
    // devices_specific.ini
    int32_t iretn = 0;
    {
        for (size_t i=0; i < num_pload; ++i) {
            iretn = json_createpiece(agent->cinfo, "pload"+std::to_string(i), DeviceType::PLOAD);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_ssen; ++i) {
            iretn = json_createpiece(agent->cinfo, "ssen"+std::to_string(i), DeviceType::SSEN);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_imu; ++i) {
            iretn = json_createpiece(agent->cinfo, "imu"+std::to_string(i), DeviceType::IMU);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_rw; ++i) {
            iretn = json_createpiece(agent->cinfo, "rw"+std::to_string(i), DeviceType::RW);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_mtr; ++i) {
            iretn = json_createpiece(agent->cinfo, "mtr"+std::to_string(i), DeviceType::MTR);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_cpu; ++i) {
            iretn = json_createpiece(agent->cinfo, "cpu"+std::to_string(i), DeviceType::CPU);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_gps; ++i) {
            iretn = json_createpiece(agent->cinfo, "gps"+std::to_string(i), DeviceType::GPS);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_ant; ++i) {
            iretn = json_createpiece(agent->cinfo, "ant"+std::to_string(i), DeviceType::ANT);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_rxr; ++i) {
            iretn = json_createpiece(agent->cinfo, "rxr"+std::to_string(i), DeviceType::RXR);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_txr; ++i) {
            iretn = json_createpiece(agent->cinfo, "txr"+std::to_string(i), DeviceType::TXR);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_tcv; ++i) {
            iretn = json_createpiece(agent->cinfo, "tcv"+std::to_string(i), DeviceType::TCV);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_pvstrg; ++i) {
            iretn = json_createpiece(agent->cinfo, "pvstrg"+std::to_string(i), DeviceType::PVSTRG);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_batt; ++i) {
            iretn = json_createpiece(agent->cinfo, "batt"+std::to_string(i), DeviceType::BATT);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_htr; ++i) {
            iretn = json_createpiece(agent->cinfo, "htr"+std::to_string(i), DeviceType::HTR);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_motr; ++i) {
            iretn = json_createpiece(agent->cinfo, "motr"+std::to_string(i), DeviceType::MOTR);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_tsen; ++i) {
            iretn = json_createpiece(agent->cinfo, "tsen"+std::to_string(i), DeviceType::TSEN);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_thst; ++i) {
            iretn = json_createpiece(agent->cinfo, "thst"+std::to_string(i), DeviceType::THST);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_prop; ++i) {
            iretn = json_createpiece(agent->cinfo, "prop"+std::to_string(i), DeviceType::PROP);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_swch; ++i) {
            iretn = json_createpiece(agent->cinfo, "swch"+std::to_string(i), DeviceType::SWCH);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_rot; ++i) {
            iretn = json_createpiece(agent->cinfo, "rot"+std::to_string(i), DeviceType::ROT);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_stt; ++i) {
            iretn = json_createpiece(agent->cinfo, "stt"+std::to_string(i), DeviceType::STT);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_mcc; ++i) {
            iretn = json_createpiece(agent->cinfo, "mcc"+std::to_string(i), DeviceType::MCC);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_tcu; ++i) {
            iretn = json_createpiece(agent->cinfo, "tcu"+std::to_string(i), DeviceType::TCU);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_bus; ++i) {
            iretn = json_createpiece(agent->cinfo, "bus"+std::to_string(i), DeviceType::BUS);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_psen; ++i) {
            iretn = json_createpiece(agent->cinfo, "psen"+std::to_string(i), DeviceType::PSEN);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_suchi; ++i) {
            iretn = json_createpiece(agent->cinfo, "suchi"+std::to_string(i), DeviceType::SUCHI);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_cam; ++i) {
            iretn = json_createpiece(agent->cinfo, "cam"+std::to_string(i), DeviceType::CAM);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_telem; ++i) {
            iretn = json_createpiece(agent->cinfo, "telem"+std::to_string(i), DeviceType::TELEM);
            ASSERT_GE(iretn, 0);
            agent->cinfo->devspec.telem[json_finddev(agent->cinfo, "telem"+std::to_string(i))].vtype = JSON_TYPE_INT32;
        }
        for (size_t i=0; i < num_disk; ++i) {
            iretn = json_createpiece(agent->cinfo, "disk"+std::to_string(i), DeviceType::DISK);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_tnc; ++i) {
            iretn = json_createpiece(agent->cinfo, "tnc"+std::to_string(i), DeviceType::TNC);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_bcreg; ++i) {
            iretn = json_createpiece(agent->cinfo, "bcreg"+std::to_string(i), DeviceType::BCREG);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_gyro; ++i) {
            iretn = json_createpiece(agent->cinfo, "gyro"+std::to_string(i), DeviceType::GYRO);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_mag; ++i) {
            iretn = json_createpiece(agent->cinfo, "mag"+std::to_string(i), DeviceType::MAG);
            ASSERT_GE(iretn, 0);
        }
        for (size_t i=0; i < num_xyzsen; ++i) {
            iretn = json_createpiece(agent->cinfo, "xyzsen"+std::to_string(i), DeviceType::XYZSEN);
            ASSERT_GE(iretn, 0);
        }
    }

    // ports.ini
    // Going to ignore for now, json_mapportentry() looks a bit strange,
    // also I don't think it's being used at the moment, as far as I know.

    // aliases.ini
    iretn = json_addentry("alias_node_type", "(\"node_type\"+\"node_type\")", agent->cinfo);
    EXPECT_GE(iretn, 0);

    // Update all physical quantities
    node_calc(agent->cinfo);
    // rebase_node(agent->cinfo);

    // Fix pointers
    json_updatecosmosstruc(agent->cinfo);

    // Dump ini files
    json_dump_node(agent->cinfo);
}

void JsonlibUTest::compare_Vector(const Vector& v1, const Vector& v2, size_t LINE)
{
    EXPECT_DOUBLE_EQ(v1.x, v2.x) << "jsonlib_ut: " << LINE;
    EXPECT_DOUBLE_EQ(v1.y, v2.y) << "jsonlib_ut: " << LINE;
    EXPECT_DOUBLE_EQ(v1.z, v2.z) << "jsonlib_ut: " << LINE;
}



} // End namespace Json
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos

#endif // end __JSONLIB_UT_HELPERS_H__