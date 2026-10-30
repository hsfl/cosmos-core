#include "module/packethandler_module.h"
#include "agent/command_queue.h"
#include "gtest/gtest.h"
#include <atomic>
#include <filesystem>

namespace {

// Keep anything the queue logs out of the real COSMOS tree
class TempCosmosEnvironment : public ::testing::Environment
{
public:
    static string dir;

    void SetUp() override
    {
        string templ = (std::filesystem::temp_directory_path() / "module_ut.XXXXXX").string();
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

// Handlers are plain function pointers, so they report through statics
std::atomic<int> pings{0};
CommandQueue *test_queue = nullptr;

int32_t CountPing(PacketComm &, string &, Agent *)
{
    ++pings;
    return 0;
}

// Mirrors ExecAddCommand in agent_exec
int32_t AddCommand(PacketComm &packet, string &response, Agent *)
{
    return test_queue->add_command_string(string(packet.data.begin(), packet.data.end()), response, true);
}

class PacketHandlerModuleTest : public ::testing::Test
{
protected:
    Agent *agent = nullptr;
    Module::PacketHandlerModule module;
    CommandQueue queue;
    thread loop;

    void SetUp() override
    {
        pings = 0;
        test_queue = &queue;
        agent = new Agent("", "phtest", "", 0., AGENTMAXBUFFER, false, 0, NetworkType::UDP, 0);
        ASSERT_GE(agent->last_error(), 0);
        ASSERT_GE(module.Init(agent, "EXEC"), 0);
        module.add_func(PacketComm::TypeId::CommandObcPing, CountPing);
        module.add_func(PacketComm::TypeId::CommandExecAddCommand, AddCommand);
        loop = thread([this] { module.Loop(); });
    }

    void TearDown() override
    {
        module.shutdown();
        if (loop.joinable())
        {
            loop.join();
        }
        test_queue = nullptr;
        delete agent;
    }

    // Wait up to timeout seconds for pred to become true
    template <typename Pred>
    bool wait_for(Pred pred, double timeout = 2.)
    {
        ElapsedTime et;
        while (et.split() < timeout)
        {
            if (pred())
            {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return pred();
    }

    PacketComm make_ping()
    {
        PacketComm packet;
        packet.header.type = PacketComm::TypeId::CommandObcPing;
        packet.header.nodeorig = agent->nodeId;
        packet.header.nodedest = agent->nodeId;
        packet.data = {1, 2, 3};
        return packet;
    }
};

} // namespace

TEST_F(PacketHandlerModuleTest, Queued_packet_is_executed_by_handler)
{
    PacketComm ping = make_ping();
    queue.add_packet(ping, currentmjd() - 1. / 86400., "ping");
    queue.run_commands(agent, "phtest", currentmjd());
    EXPECT_TRUE(wait_for([] { return pings.load() == 1; }));
}

TEST_F(PacketHandlerModuleTest, Future_packet_is_not_executed)
{
    PacketComm ping = make_ping();
    queue.add_packet(ping, currentmjd() + 1., "ping");
    queue.run_commands(agent, "phtest", currentmjd());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(pings.load(), 0);
}

TEST_F(PacketHandlerModuleTest, Uplinked_packet_is_queued_then_executed)
{
    PacketComm ping = make_ping();
    ASSERT_GE(PacketHandler::QueueExecAddPacket(ping, currentmjd() - 1. / 86400., "uplinked", agent, 0, "", agent->nodeId, "EXEC"), 0);
    ASSERT_TRUE(wait_for([this] { return queue.get_command_size() == 1; }));
    EXPECT_EQ(pings.load(), 0);

    queue.run_commands(agent, "phtest", currentmjd());
    EXPECT_TRUE(wait_for([] { return pings.load() == 1; }));
}

TEST_F(PacketHandlerModuleTest, Uplinked_shell_command_is_not_queued)
{
    Event cmd;
    cmd.mjd = currentmjd() - 1. / 86400.;
    cmd.name = "shell";
    cmd.type = EVENT_TYPE_COMMAND;
    cmd.flag = 0;
    cmd.data = "touch " + TempCosmosEnvironment::dir + "ran";
    ASSERT_GE(PacketHandler::QueueExecAddCommand(cmd.get_event_string(), agent, agent->nodeId, "EXEC"), 0);

    // Packets are handled in order, so once this one is queued the shell event has been handled
    PacketComm ping = make_ping();
    ASSERT_GE(PacketHandler::QueueExecAddPacket(ping, currentmjd() + 1., "marker", agent, 0, "", agent->nodeId, "EXEC"), 0);
    ASSERT_TRUE(wait_for([this] { return queue.get_command_size() >= 1; }));
    EXPECT_EQ(queue.get_command_size(), 1u);
    EXPECT_EQ(queue.get_command(0).get_name(), "marker");
}

TEST_F(PacketHandlerModuleTest, Oversized_uplink_is_refused)
{
    PacketComm big = make_ping();
    big.data.assign(Channel::PACKETCOMM_DATA_SIZE, 0xaa);
    EXPECT_LT(PacketHandler::QueueExecAddPacket(big, currentmjd(), "big", agent, 0, "", agent->nodeId, "EXEC"), 0);
}

TEST_F(PacketHandlerModuleTest, Uplink_must_fit_outgoing_channel)
{
    PacketComm ping = make_ping();
    ASSERT_GE(agent->channel_update("OBC", 100), 0);
    EXPECT_EQ(PacketHandler::QueueExecAddPacket(ping, currentmjd(), "small", agent, 0, "", agent->nodeId, "OBC"), GENERAL_ERROR_BAD_SIZE);
    EXPECT_GE(PacketHandler::QueueExecAddPacket(ping, currentmjd(), "fits", agent, 0, "", agent->nodeId, "EXEC"), 0);
}

TEST_F(PacketHandlerModuleTest, Uplink_to_unknown_channel_is_refused)
{
    PacketComm ping = make_ping();
    EXPECT_LT(PacketHandler::QueueExecAddPacket(ping, currentmjd(), "lost", agent, 0, "", agent->nodeId, "NOSUCHCHANNEL"), 0);
}

TEST_F(PacketHandlerModuleTest, Non_v2_inner_packet_is_refused)
{
    PacketComm ping = make_ping();
    ping.style = PacketComm::PacketStyle::Minimal;
    EXPECT_EQ(PacketHandler::QueueExecAddPacket(ping, currentmjd(), "minimal", agent, 0, "", agent->nodeId, "EXEC"), GENERAL_ERROR_INPUT);
}
