#include "file_it.h"

namespace Cosmos {
namespace Test {
namespace Integration {
namespace File {

void FileSubagentTest::SetUp(uint8_t test_num_agents)
{
    // Create log directories
    test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    const string log_base_path = get_cosmosnodes() + "logs_it/";
    if (COSMOS_MKDIR(log_base_path.c_str(),00777) != 0 && errno != EEXIST) { FAIL() << errno; }
    const string test_base_path = log_base_path + test_name + "/";
    if (COSMOS_MKDIR(test_base_path.c_str(),00777) != 0 && errno != EEXIST) { FAIL() << errno; }
    // Main log file for the current test
    test_log.Set(Log::LogType::LOG_FILE_FFLUSH, test_base_path + "TestLog");

    double seed = decisec();
    srand(seed);
    test_log.Printf("Using rand seed:%d\n", seed);
    num_agents = test_num_agents;
    // Agents represent the source and destination nodes
    agents.resize(num_agents);
    // Each agent has one corresponding file subagent + file channel
    file_subagents.resize(num_agents);
    // The packethandler routes stuff sent to the main channel to the file channel
    packethandler_subagents.resize(num_agents);

    // Load the node name:node id table for the tests (note, static data is shared for all agents in this process)
    cosmosstruc cinfo;
    make_temp_nodeids(cinfo, num_agents);


    // Setup the test agents
    for (size_t i=0; i < num_agents; ++i)
    {
        agents[i] = new Agent(0);
        agents[i]->cinfo = new cosmosstruc();
        agents[i]->cinfo->realm = cinfo.realm;
        agents[i]->cinfo->agent0.stateflag = static_cast<uint16_t>(Cosmos::Support::Agent::State::INIT);
        agents[i]->cinfo->node.name = "_tnode_" + std::to_string(i+1);
        agents[i]->nodeId = lookup_node_id(agents[i]->cinfo, agents[i]->cinfo->node.name);
        agents[i]->init_channels();
        string agent_log_name = agents[i]->cinfo->node.name + "_log";
        agents[i]->debug_log.Set(Log::LogType::LOG_FILE_FFLUSH, test_base_path + agent_log_name);
    }
}

void FileSubagentTest::TearDown()
{
    // Reset static node table
//    NodeList::node_ids.clear();
    // Remove created directories
    cleanup(num_agents);
    // Close log file
    test_log.Close();

    // Stop the agents and join all threads
    for (auto& agent : agents) {
        agent->debug_log.Close();
        agent->cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
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

// If a previous run of a test exited prematurely, the test folders won't have been deleted, so clean those up
TEST_F(FileSubagentTest, Delete_leftover_test_folders)
{
    cleanup(num_agents);
}

// Test to ensure that the test agents can be properly created with channels and some extra fluff to make the file subagent work
TEST_F(FileSubagentTest, Initialization_succeeds)
{
    const uint8_t test_num_agents = 3;

    ASSERT_NO_FATAL_FAILURE(SetUp(test_num_agents););
    EXPECT_EQ(num_agents, test_num_agents);
    EXPECT_EQ(agents.size(), num_agents);
    EXPECT_EQ(file_subagents.size(), num_agents);
    EXPECT_EQ(packethandler_subagents.size(), num_agents);
//    EXPECT_EQ(NodeList::node_ids.size(), num_agents);
    EXPECT_EQ(agents[0]->cinfo->realm.node_ids.size()-3, num_agents);

    for (size_t i=0; i < test_num_agents; ++i)
    {
        EXPECT_NE(agents[i]->cinfo, nullptr);
        EXPECT_EQ(agents[i]->running(), true);
        EXPECT_GT(agents[i]->channel_number("FILE"), 0);
        string node_name = "_tnode_" + std::to_string(i+1);
        EXPECT_EQ(agents[i]->cinfo->realm.node_ids[node_name], i+1);
        EXPECT_EQ(agents[i]->cinfo->node.name, node_name);
        EXPECT_EQ(agents[i]->nodeId, i+1);
    }

}

// No frills basic transfer across two nodes, from node1 to node2
TEST_F(FileSubagentTest, Can_perform_basic_transfer)
{
    const uint8_t test_num_agents = 2;
    const size_t file_size_kib = 30;
    // Perform initial test setup
    ASSERT_NO_FATAL_FAILURE(SetUp(test_num_agents));
    num_files = 1;

    // Setup the test conditions
    ASSERT_NO_FATAL_FAILURE(TestSetup());

    // Create files
    int32_t iretn = create_test_files("origin", "destination", file_size_kib, num_files, "");
    ASSERT_GE(iretn, 0);

    // Initiate file transfers
    ASSERT_NO_FATAL_FAILURE(StartTransfers());

    ElapsedTime timeout_timer;
    const double timeout = 5 * 60;
    // Check directories every second
    while (timeout_timer.split() < timeout && state == test_state::UNFINISHED)
    {
        verify_incoming_dir("origin", num_files);
        secondsleep(3.);
    }
    EXPECT_NE(state, test_state::UNFINISHED);
    verify_outgoing_dir("destination", 0);
    verify_temp_dir("origin", 0);
    verify_temp_dir("destination", 0);
}

// No frills basic transfer across two nodes, from node1 to node2
TEST_F(FileSubagentTest, Bad_reqdata_fails_gracefully)
{
    // Perform initial test setup
    ASSERT_NO_FATAL_FAILURE(SetUp(2));
    // Setup the test conditions
    ASSERT_NO_FATAL_FAILURE(TestSetup());
    // Initiate file transfers
    ASSERT_NO_FATAL_FAILURE(StartTransfers());

    // Send a bad reqdata packet
    PacketComm packet;
    packet.header.type = PacketComm::TypeId::DataFileReqData;
    packet.data.resize(12);
//    int32_t iretn = NodeList::lookup_node_id("destination");
    int32_t iretn = NODEIDDEST;
    packet.data[0] = iretn & 0xFF;
    packet.data[1] = 2;
    uint32to(128, &packet.data[4]);
    uint32to(128, &packet.data[8]);
    agents[0]->channel_push("FILE", packet);

    // All is good if it doesn't crash
    secondsleep(8.);
    verify_temp_dir("origin", 0);
    verify_temp_dir("destination", 0);
}


} // End namespace File
} // End namespace Integration
} // End namespace Tests
} // End namespace Cosmos

// Note, was entering into critical region channellib's channel_pull in the if(channel[number].quu.size()) check, which was odd despite
// the quu being empty before and after the if, this when my starting the threads was not in the current order that it is, may be worth
// revisiting. The error happens when channel[number].quu.front() is called on an empty queue, which obviously will not be a happy thing.
