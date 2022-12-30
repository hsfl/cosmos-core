#include "file_it.h"
#include "file_it_helpers.h"

namespace Cosmos {
namespace Test {
namespace Integration {
namespace File {

void FileSubagentTest::SetUp(uint8_t test_num_agents)
{
    num_agents = test_num_agents;
    // Agents represent the source and destination nodes
    agents.resize(num_agents);
    // Each agent has one corresponding file subagent + file channel
    file_subagents.resize(num_agents);
    // The packethandler routes stuff sent to the main channel to the file channel
    packethandler_subagents.resize(num_agents);

    // Load the node name:node id table for the tests (note, static data is shared for all agents in this process)
    Cosmos::Test::Integration::File::load_temp_nodeids(num_agents);

    // Setup the test agents
    for (size_t i=0; i < num_agents; ++i)
    {
        agents[i] = new Agent(0);
        agents[i]->cinfo = new cosmosstruc();
        agents[i]->cinfo->agent.resize(1);
        agents[i]->cinfo->agent[0].stateflag = static_cast<uint16_t>(Cosmos::Support::Agent::State::INIT);
        agents[i]->init_channels();

        // Create the file subagents, but finish the setup later
        file_subagents[i] = new Module::FileModule();
        //file_subagents[i]->Init();

        // Each agent has a packethandler to route file packets to file channel
        packethandler_subagents[i] = new Module::PacketHandlerModule();
        packethandler_subagents[i]->Init(agents[i], "SELF");
        subagent_threads.push_back(thread([=] { packethandler_subagents[i]->Loop(); }));

        // Websocket radio comm links defined later
        // websocket_subagents[i] = new Module::WebsocketModule();
        // websocket_subagents[i]->Init(agents[i], "127.0.0.1", PORT_OFFSET+i, PORT_OFFSET+i*2, "SOCKRADIO");
    }
}

void FileSubagentTest::TearDown()
{
    // Reset static node table
    NodeData::node_ids.clear();
    // Remove created directories
    cleanup(num_agents);

    // Stop the agents and join all threads
    for (auto& agent : agents) {
        agent->cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
    }
    for (auto& thread: subagent_threads) {
        thread.join();
    }
    subagent_threads.clear();
    for (auto& agent : agents) {
        delete agent->cinfo;
        // Agent destructor attempts delete of cinfo (and some other things), set to nullptr to avoid headaches
        agent->cinfo = nullptr;
        delete agent;
    }
    agents.clear();

    // Free pointer memory
    for (auto& ptr : file_subagents)          { delete ptr; }
    for (auto& ptr : packethandler_subagents) { delete ptr; }
    for (auto& ptr : websocket_subagents)     { delete ptr; }
    file_subagents.clear();
    packethandler_subagents.clear();
    websocket_subagents.clear();
}

// Test to ensure that the test agents can be properly created with channels and some extra fluff to make the file subagent work
TEST_F(FileSubagentTest, InitialSetup)
{
    const uint8_t test_num_agents = 3;

    SetUp(test_num_agents);
    EXPECT_EQ(num_agents, test_num_agents);
    EXPECT_EQ(agents.size(), num_agents);
    EXPECT_EQ(file_subagents.size(), num_agents);
    EXPECT_EQ(packethandler_subagents.size(), num_agents);
    EXPECT_EQ(NodeData::node_ids.size(), num_agents);

    for (size_t i=0; i < num_agents; ++i)
    {
        EXPECT_NE(agents[i]->cinfo, nullptr);
        EXPECT_EQ(agents[i]->running(), true);
        EXPECT_GT(agents[i]->channel_number("FILE"), 0);
        string node_name = ".tnode_" + std::to_string(i);
        EXPECT_EQ(NodeData::node_ids[node_name], i);
    }

}

// No frills basic transfer across two nodes, from node1 to node2
TEST_F(FileSubagentTest, BasicTransfer)
{
    const uint8_t test_num_agents = 2;
    SetUp(test_num_agents);
}


} // End namespace File
} // End namespace Integration
} // End namespace Tests
} // End namespace Cosmos