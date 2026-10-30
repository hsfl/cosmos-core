#include "agent/command_queue.h"
#include "gtest/gtest.h"
#include <filesystem>

namespace {

// Keep anything the queue logs out of the real COSMOS tree
class TempCosmosEnvironment : public ::testing::Environment
{
public:
    static string dir;

    void SetUp() override
    {
        string templ = (std::filesystem::temp_directory_path() / "command_queue_ut.XXXXXX").string();
        ASSERT_NE(mkdtemp(&templ[0]), nullptr);
        dir = templ + "/";
        std::filesystem::create_directories(dir + "nodes");
        setenv("COSMOS", dir.c_str(), 1);
        setenv("COSMOSNODES", (dir + "nodes").c_str(), 1);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(dir);
    }
};
string TempCosmosEnvironment::dir;
::testing::Environment* const temp_env = ::testing::AddGlobalTestEnvironment(new TempCosmosEnvironment);

PacketComm make_ping(vector<uint8_t> data = {1, 2, 3, 4, 5})
{
    PacketComm packet;
    packet.header.type = PacketComm::TypeId::CommandObcPing;
    packet.header.nodeorig = 3;
    packet.header.nodedest = 7;
    packet.data = data;
    return packet;
}

void expect_same_packet(PacketComm &actual, PacketComm &expected)
{
    EXPECT_EQ(actual.header.type, expected.header.type);
    EXPECT_EQ(actual.header.nodeorig, expected.header.nodeorig);
    EXPECT_EQ(actual.header.nodedest, expected.header.nodedest);
    EXPECT_EQ(actual.data, expected.data);
}

double past() { return currentmjd() - 1. / 86400.; }
double future() { return currentmjd() + 1.; }

class CommandQueueTest : public ::testing::Test
{
protected:
    Agent *agent = nullptr;
    CommandQueue queue;

    void SetUp() override
    {
        agent = new Agent("", "cqtest", "", 0., AGENTMAXBUFFER, false, 0, NetworkType::UDP, 0);
        ASSERT_GE(agent->last_error(), 0);
        ASSERT_GE(agent->channel_number("EXEC"), 0);
    }

    void TearDown() override
    {
        delete agent;
    }

    int32_t pending(const string &channel)
    {
        int32_t count = 0;
        PacketComm packet;
        while (agent->channel_pull(channel, packet) > 0)
        {
            ++count;
        }
        return count;
    }
};

} // namespace

// Packet validation

TEST(CommandQueuePacket, Non_v2_packet_is_not_queued)
{
    CommandQueue queue;
    PacketComm packet = make_ping();
    packet.style = PacketComm::PacketStyle::Minimal;
    EXPECT_LT(queue.add_packet(packet, future(), "minimal"), 0);
    EXPECT_EQ(queue.get_command_size(), 0u);
}

// Scheduling

TEST_F(CommandQueueTest, Future_packet_is_not_pushed)
{
    PacketComm packet = make_ping();
    EXPECT_EQ(queue.add_packet(packet, future(), "later"), 1);
    queue.run_commands(agent, "cqtest", currentmjd());
    EXPECT_EQ(pending("EXEC"), 0);
    EXPECT_EQ(queue.get_command_size(), 1u);
}

TEST_F(CommandQueueTest, Due_packet_is_pushed_once_to_exec)
{
    PacketComm packet = make_ping();
    queue.add_packet(packet, past(), "now");
    queue.run_commands(agent, "cqtest", currentmjd());

    PacketComm pulled;
    ASSERT_GT(agent->channel_pull("EXEC", pulled), 0);
    expect_same_packet(pulled, packet);
    EXPECT_EQ(pending("EXEC"), 0);
    EXPECT_EQ(queue.get_command_size(), 0u);
    ASSERT_EQ(queue.get_event_size(), 1u);
    EXPECT_EQ(queue.get_event(0).get_name(), "now");

    queue.run_commands(agent, "cqtest", currentmjd());
    EXPECT_EQ(pending("EXEC"), 0);
}

TEST_F(CommandQueueTest, Packet_channel_can_be_changed)
{
    PacketComm packet = make_ping();
    queue.set_packet_channel("OBC");
    queue.add_packet(packet, past(), "obc");
    queue.run_commands(agent, "cqtest", currentmjd());
    EXPECT_EQ(pending("EXEC"), 0);
    EXPECT_EQ(pending("OBC"), 1);
}

TEST_F(CommandQueueTest, Unknown_packet_channel_does_not_crash)
{
    PacketComm packet = make_ping();
    queue.set_packet_channel("NOSUCHCHANNEL");
    queue.add_packet(packet, past(), "lost");
    queue.run_commands(agent, "cqtest", currentmjd());
    EXPECT_EQ(pending("EXEC"), 0);
    EXPECT_EQ(queue.get_command_size(), 0u);
    // A failed push is not recorded as executed
    ASSERT_EQ(queue.get_event_size(), 1u);
    EXPECT_EQ(queue.get_event(0).flag & EVENT_FLAG_ACTUAL, 0u);
    EXPECT_EQ(queue.get_event(0).getUtcExec(), 0.);
}

TEST_F(CommandQueueTest, Successful_push_is_recorded_as_executed)
{
    PacketComm packet = make_ping();
    queue.add_packet(packet, past(), "done");
    queue.run_commands(agent, "cqtest", currentmjd());
    ASSERT_EQ(queue.get_event_size(), 1u);
    EXPECT_NE(queue.get_event(0).flag & EVENT_FLAG_ACTUAL, 0u);
    EXPECT_GT(queue.get_event(0).getUtcExec(), 0.);
}

TEST_F(CommandQueueTest, Same_name_replaces_existing_packet)
{
    PacketComm first = make_ping({1});
    PacketComm second = make_ping({2});
    EXPECT_EQ(queue.add_packet(first, past(), "dup"), 1);
    EXPECT_EQ(queue.add_packet(second, past(), "dup"), 0);
    EXPECT_EQ(queue.get_command_size(), 1u);

    queue.run_commands(agent, "cqtest", currentmjd());
    PacketComm pulled;
    ASSERT_GT(agent->channel_pull("EXEC", pulled), 0);
    expect_same_packet(pulled, second);
}

TEST_F(CommandQueueTest, Packets_survive_save_and_restore)
{
    PacketComm packet = make_ping({9, 8, 7});
    queue.add_packet(packet, past(), "saved");
    string dir = TempCosmosEnvironment::dir;
    ASSERT_EQ(queue.save_commands(dir, ".cqtest_queue"), 1);

    CommandQueue restored;
    ASSERT_EQ(restored.restore_commands(dir, ".cqtest_queue"), 1);
    restored.run_commands(agent, "cqtest", currentmjd());
    PacketComm pulled;
    ASSERT_GT(agent->channel_pull("EXEC", pulled), 0);
    expect_same_packet(pulled, packet);
}

TEST_F(CommandQueueTest, Repeating_conditional_packet_fires_once_while_true)
{
    PacketComm packet = make_ping();
    queue.add_packet(packet, past(), "cond", EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT, "(1>0)");
    queue.run_commands(agent, "cqtest", currentmjd());
    queue.run_commands(agent, "cqtest", currentmjd());
    EXPECT_EQ(pending("EXEC"), 1);
    EXPECT_EQ(queue.get_command_size(), 1u);
}

TEST_F(CommandQueueTest, False_conditional_packet_does_not_fire)
{
    PacketComm packet = make_ping();
    queue.add_packet(packet, past(), "never", EVENT_FLAG_CONDITIONAL, "(1<0)");
    queue.run_commands(agent, "cqtest", currentmjd());
    EXPECT_EQ(pending("EXEC"), 0);
    EXPECT_EQ(queue.get_command_size(), 1u);
}

// Adding from JSON

TEST_F(CommandQueueTest, Packet_event_string_is_queued)
{
    PacketComm packet = make_ping();
    Event cmd;
    cmd.mjd = past();
    cmd.name = "json";
    cmd.type = EVENT_TYPE_PACKET;
    cmd.flag = 0;
    cmd.data = packet.ToHexString();

    string response;
    EXPECT_EQ(queue.add_command_string(cmd.get_event_string(), response), 0);
    queue.run_commands(agent, "cqtest", currentmjd());
    PacketComm pulled;
    ASSERT_GT(agent->channel_pull("EXEC", pulled), 0);
    expect_same_packet(pulled, packet);
}

TEST_F(CommandQueueTest, Invalid_packet_event_string_is_rejected)
{
    Event cmd;
    cmd.mjd = past();
    cmd.name = "bad";
    cmd.type = EVENT_TYPE_PACKET;
    cmd.flag = 0;
    cmd.data = "not hex";

    string response;
    EXPECT_LT(queue.add_command_string(cmd.get_event_string(), response), 0);
    EXPECT_EQ(queue.get_command_size(), 0u);
}

TEST_F(CommandQueueTest, Non_command_string_is_rejected)
{
    string response;
    EXPECT_LT(queue.add_command_string("{\"name\":\"x\",\"type\":0}", response), 0);
    EXPECT_LT(queue.add_command_string("not json", response), 0);
    EXPECT_EQ(queue.get_command_size(), 0u);
}

// Packets only

namespace {
string event_string(uint32_t type, string data, string name="evt")
{
    Event cmd;
    cmd.mjd = currentmjd() + 1.;
    cmd.name = name;
    cmd.type = type;
    cmd.flag = 0;
    cmd.data = data;
    return cmd.get_event_string();
}

bool only_ping(const PacketComm &packet)
{
    return packet.header.type == PacketComm::TypeId::CommandObcPing;
}
}

TEST_F(CommandQueueTest, Packets_only_refuses_shell_and_request_events)
{
    string response;
    EXPECT_LT(queue.add_command_string(event_string(EVENT_TYPE_COMMAND, "ls -l"), response, true), 0);
    EXPECT_NE(response.find("Only packet events"), string::npos);
    EXPECT_LT(queue.add_command_string(event_string(EVENT_TYPE_REQUEST, "getqueuesize"), response, true), 0);
    EXPECT_EQ(queue.get_command_size(), 0u);
}

TEST_F(CommandQueueTest, Packets_only_accepts_packet_events)
{
    PacketComm packet = make_ping();
    string response;
    EXPECT_EQ(queue.add_command_string(event_string(EVENT_TYPE_PACKET, packet.ToHexString()), response, true), 0);
    EXPECT_EQ(queue.get_command_size(), 1u);
}

TEST_F(CommandQueueTest, Without_packets_only_shell_events_are_still_accepted)
{
    string response;
    EXPECT_EQ(queue.add_command_string(event_string(EVENT_TYPE_COMMAND, "ls -l"), response), 0);
    EXPECT_EQ(queue.get_command_size(), 1u);
}

// Packet filter

TEST_F(CommandQueueTest, Packet_filter_refuses_add_packet)
{
    queue.set_packet_filter(only_ping);
    PacketComm ping = make_ping();
    PacketComm reboot = make_ping();
    reboot.header.type = PacketComm::TypeId::CommandObcReboot;
    EXPECT_EQ(queue.add_packet(ping, future(), "ping"), 1);
    EXPECT_LT(queue.add_packet(reboot, future(), "reboot"), 0);
    EXPECT_EQ(queue.get_command_size(), 1u);
}

TEST_F(CommandQueueTest, Packet_filter_refusal_names_the_type)
{
    queue.set_packet_filter(only_ping);
    PacketComm reboot = make_ping();
    reboot.header.type = PacketComm::TypeId::CommandObcReboot;
    string response;
    EXPECT_LT(queue.add_command_string(event_string(EVENT_TYPE_PACKET, reboot.ToHexString()), response), 0);
    EXPECT_NE(response.find("182"), string::npos) << response;
    EXPECT_EQ(queue.get_command_size(), 0u);
}

TEST_F(CommandQueueTest, Packet_filter_does_not_affect_other_event_types)
{
    queue.set_packet_filter(only_ping);
    string response;
    EXPECT_EQ(queue.add_command_string(event_string(EVENT_TYPE_COMMAND, "ls -l"), response), 0);
    EXPECT_EQ(queue.get_command_size(), 1u);
}

TEST_F(CommandQueueTest, Packet_filter_applies_on_restore)
{
    PacketComm ping = make_ping();
    PacketComm reboot = make_ping();
    reboot.header.type = PacketComm::TypeId::CommandObcReboot;
    queue.add_packet(ping, future(), "ping");
    queue.add_packet(reboot, future(), "reboot");
    string dir = TempCosmosEnvironment::dir;
    ASSERT_EQ(queue.save_commands(dir, ".cqtest_filter"), 2);

    CommandQueue restored;
    restored.set_packet_filter(only_ping);
    EXPECT_EQ(restored.restore_commands(dir, ".cqtest_filter"), 1);
    EXPECT_EQ(restored.get_command(0).get_name(), "ping");
}
