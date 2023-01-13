#include "beacon_ut.h"
#include "support/jsondef.h"
#include "support/json11.hpp"
#include <limits>
#include <sstream>

namespace Cosmos {
namespace Test {
namespace Unit {
namespace Beacon {


void BeaconUTest::SetUp()
{
    // Clear previously created test directory, if any
    rmdir(get_cosmosnodes() + TEST_NODE_NAME);

    srand(decisec());
    agent = new Agent(0);
    // Initialize COSMOS data space
    agent->cinfo = json_init();
    ASSERT_NE(agent->cinfo, nullptr);
    agent->cinfo->node.name = TEST_NODE_NAME;
    beacon.Init();
    int32_t iretn;

    // CPU
    for (size_t i=0; i < num_cpus; ++i)
    {
        iretn = json_createpiece(agent->cinfo, "cpu"+std::to_string(i), DeviceType::CPU);
        ASSERT_GE(iretn, 0);
    }

    // Reaction wheels
    for (size_t i=0; i < num_rws; ++i)
    {
        iretn = json_createpiece(agent->cinfo, "rw"+std::to_string(i), DeviceType::RW);
        ASSERT_GE(iretn, 0);
    }

    // Magnetorquers
    for (size_t i=0; i < num_mtrs; ++i)
    {
        iretn = json_createpiece(agent->cinfo, "mtr"+std::to_string(i), DeviceType::MTR);
        ASSERT_GE(iretn, 0);
    }

    // Gyros
    for (size_t i=0; i < num_gyros; ++i)
    {
        iretn = json_createpiece(agent->cinfo, "gyro"+std::to_string(i), DeviceType::GYRO);
        ASSERT_GE(iretn, 0);
    }
    // Dump ini files, then reload agent
    json_dump_node(agent->cinfo);
    delete agent->cinfo;
    agent->cinfo = nullptr;
    delete agent;
    agent = new Agent(0);
    // Initialize COSMOS data space
    agent->cinfo = json_init();
    ASSERT_NE(agent->cinfo, nullptr);
    agent->cinfo->node.name = TEST_NODE_NAME;
    iretn = json_setup_node(agent->cinfo->node.name, agent->cinfo);
    ASSERT_EQ(iretn, 0);
    // Receiving agent
    agent2 = new Agent(0);
    agent2->cinfo = json_init();
    ASSERT_NE(agent->cinfo, nullptr);
    agent2->cinfo->node.name = TEST_NODE_NAME;
    iretn = json_setup_node(agent2->cinfo->node.name, agent2->cinfo);
    ASSERT_EQ(iretn, 0);
}

// Cleanup for text fixture class
void BeaconUTest::TearDown()
{
    rmdir(get_cosmosnodes() + TEST_NODE_NAME);
    delete agent->cinfo;
    agent->cinfo = nullptr;
    delete agent;
    delete agent2->cinfo;
    agent2->cinfo = nullptr;
    delete agent2;
}

TEST_F(BeaconUTest, CPU_beacon_json_is_populated)
{
    ASSERT_EQ(agent->cinfo->devspec.cpu.size(), num_cpus);
    ASSERT_EQ(agent2->cinfo->devspec.cpu.size(), num_cpus);
    agent->cinfo->node.utcstart = currentmjd();
    for(size_t i=0; i<num_cpus; ++i)
    {
        agent->cinfo->devspec.cpu[i].uptime = rand();
        agent->cinfo->devspec.cpu[i].boot_count = rand();
        agent->cinfo->devspec.cpu[i].load = rand()/ (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.cpu[i].gib = rand() / (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.cpu[i].storage = rand() / (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.cpu[i].temp = rand() / (RAND_MAX/std::numeric_limits<float>::max());
    }

    string response;
    PacketComm packet;
    beacon.EncodeBinary(Support::Beacon::TypeId::CPUBeacon, agent->cinfo, packet.data);
    // Recieved by agent2
    beacon.Decode(packet.data, agent2->cinfo);
    beacon.EncodeJson(Support::Beacon::TypeId::CPUBeacon, agent2->cinfo, response);
    string error;
    json11::Json j = json11::Json::parse(response, error);
    ASSERT_EQ(error.empty(), true);
    for (size_t i=0; i<num_cpus; ++i)
    {
        std::ostringstream didx;
        didx << std::setw(3) << std::setfill('0') << i;
        EXPECT_DOUBLE_EQ(j["device_cpu_utc_"+didx.str()].number_value(), decisec2mjd(agent->cinfo->node.deci));
        EXPECT_FLOAT_EQ(j["node_utcstart"].number_value(),              agent->cinfo->node.utcstart);
        EXPECT_EQ(j["device_cpu_uptime_"+didx.str()].int_value(),       agent->cinfo->devspec.cpu[i].uptime);
        EXPECT_EQ(j["device_cpu_boot_count_"+didx.str()].int_value(),   agent->cinfo->devspec.cpu[i].boot_count);
        EXPECT_NEAR(j["device_cpu_load_"+didx.str()].number_value(),    std::min(agent->cinfo->devspec.cpu[i].load, 65.535f),   0.1);
        EXPECT_NEAR(j["device_cpu_gib_"+didx.str()].number_value(),     std::min(agent->cinfo->devspec.cpu[i].gib, 65.535f),    0.1);
        EXPECT_NEAR(j["device_cpu_storage_"+didx.str()].number_value(), std::min(agent->cinfo->devspec.cpu[i].storage, 65.535f),0.1);
        EXPECT_NEAR(j["device_cpu_temp_"+didx.str()].number_value(),    std::min(agent->cinfo->devspec.cpu[i].temp, 655.35f),   0.1);
    }
}

TEST_F(BeaconUTest, MTR_beacon_json_is_populated)
{
    ASSERT_EQ(agent->cinfo->devspec.mtr.size(), num_mtrs);
    ASSERT_EQ(agent2->cinfo->devspec.mtr.size(), num_mtrs);
    for(size_t i=0; i<num_mtrs; ++i)
    {
        // What happens if limit is up to max double? (not that anything above 10e7 is probably meaningful)
        agent->cinfo->devspec.mtr[i].mom = rand()/ (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.mtr[i].align.w = rand()/ (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.mtr[i].align.d.x = rand()/ (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.mtr[i].align.d.y = rand()/ (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.mtr[i].align.d.z = rand()/ (RAND_MAX/std::numeric_limits<float>::max());
    }

    string response;
    PacketComm packet;
    beacon.EncodeBinary(Support::Beacon::TypeId::ADCSMTRBeacon, agent->cinfo, packet.data);
    // Recieved by agent2
    beacon.Decode(packet.data, agent2->cinfo);
    beacon.EncodeJson(Support::Beacon::TypeId::ADCSMTRBeacon, agent2->cinfo, response);
    string error;
    json11::Json j = json11::Json::parse(response, error);
    ASSERT_EQ(error.empty(), true);
    for (size_t i=0; i<num_mtrs; ++i)
    {
        std::ostringstream didx;
        didx << std::setw(3) << std::setfill('0') << i;
        EXPECT_DOUBLE_EQ(j["device_mtr_utc_"+didx.str()].number_value(), decisec2mjd(agent->cinfo->node.deci));
        EXPECT_EQ(j["device_mtr_name_"+didx.str()].string_value(), "mtr"+std::to_string(i));
        EXPECT_FLOAT_EQ(j["device_mtr_mom_"+didx.str()].number_value(), agent->cinfo->devspec.mtr[i].mom);
        ASSERT_EQ(j["device_mtr_align_"+didx.str()].is_object(), true);
        auto align = j["device_mtr_align_"+didx.str()].object_items();
        ASSERT_EQ(align["d"].is_object(), true);
        auto d = align["d"].object_items();
        // Beacon stores as float, so expect some loss of precision (i.e., double -> float -> double)
        EXPECT_FLOAT_EQ(align["w"].number_value(), agent->cinfo->devspec.mtr[i].align.w);
        EXPECT_FLOAT_EQ(d["x"].number_value(), agent->cinfo->devspec.mtr[i].align.d.x);
        EXPECT_FLOAT_EQ(d["y"].number_value(), agent->cinfo->devspec.mtr[i].align.d.y);
        EXPECT_FLOAT_EQ(d["z"].number_value(), agent->cinfo->devspec.mtr[i].align.d.z);
    }
}

TEST_F(BeaconUTest, RW_beacon_json_is_populated)
{
    ASSERT_EQ(agent->cinfo->devspec.rw.size(), num_rws);
    ASSERT_EQ(agent2->cinfo->devspec.rw.size(), num_rws);
    for(size_t i=0; i<num_rws; ++i)
    {
        agent->cinfo->devspec.rw[i].amp = rand() / (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.rw[i].omg = rand() / (RAND_MAX/std::numeric_limits<float>::max());
        agent->cinfo->devspec.rw[i].romg = rand() / (RAND_MAX/std::numeric_limits<float>::max());
    }

    string response;
    PacketComm packet;
    beacon.EncodeBinary(Support::Beacon::TypeId::ADCSRWBeacon, agent->cinfo, packet.data);
    // Recieved by agent2
    beacon.Decode(packet.data, agent2->cinfo);
    beacon.EncodeJson(Support::Beacon::TypeId::ADCSRWBeacon, agent2->cinfo, response);
    string error;
    json11::Json j = json11::Json::parse(response, error);
    ASSERT_EQ(error.empty(), true);
    for (size_t i=0; i<num_rws; ++i)
    {
        std::ostringstream didx;
        didx << std::setw(3) << std::setfill('0') << i;
        EXPECT_EQ(j["device_rw_name_"+didx.str()].string_value(), "rw"+std::to_string(i));
        EXPECT_DOUBLE_EQ(j["device_rw_utc_"+didx.str()].number_value(), decisec2mjd(agent->cinfo->node.deci));
        EXPECT_FLOAT_EQ(j["device_rw_amp_"+didx.str()].number_value(), agent->cinfo->devspec.rw[i].amp);
        EXPECT_FLOAT_EQ(j["device_rw_omg_"+didx.str()].number_value(), agent->cinfo->devspec.rw[i].omg);
        EXPECT_FLOAT_EQ(j["device_rw_romg_"+didx.str()].number_value(), agent->cinfo->devspec.rw[i].romg);

    }
}


} // End namespace Beacon
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos
