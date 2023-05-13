#ifndef __FILE_IT_HELPERS_H__
#define __FILE_IT_HELPERS_H__

//////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////
#include "support/datalib.h"
#include "file_it.h"

namespace Cosmos {
namespace Test {
namespace Integration {
namespace File {

//! Set up some nodeids for the test to use
void FileSubagentTest::make_temp_nodeids(uint8_t num_agents)
{
    cosmosstruc cinfo;
    cinfo.realm.name = "test";
    // Load in some arbitrary node ids
    // Note: cannot use node_id = 0, as it is reserved for NODEIDUNKNOWN
    for (size_t i=0; i<num_agents; ++i)
    {
        string node_name = "_tnode_" + std::to_string(i+1);
        cinfo.realm.node_ids[node_name] = i+1;
    }
    save_node_ids(&cinfo);
}

void FileSubagentTest::cleanup(uint8_t num_agents)
{
    // Delete created folders, don't touch this
    for (size_t i=0; i<num_agents; ++i)
    {
        string node_name = "_tnode_" + std::to_string(i+1);
        rmdir(get_cosmosnodes() + node_name);
    }
}

void FileSubagentTest::TestSetup()
{
    vector<vector<string>> transfer_node_contacts = {{"_tnode_2"}, {"_tnode_1"}};
    for (size_t i=0; i<agents.size(); ++i)
    {
        // File subagent setup
        file_subagents[i] = new Module::FileModule();
        int32_t iretn = file_subagents[i]->Init(agents[i], transfer_node_contacts[i]);
        ASSERT_GE(iretn, 0);

        // Each agent has a packethandler to route file packets to file channel
        packethandler_subagents[i] = new Module::PacketHandlerModule();
        iretn = packethandler_subagents[i]->Init(agents[i], "SELF");
        ASSERT_GE(iretn, 0);

        // Websocket channel adding
        iretn = agents[i]->channel_add("SOCKRADIO", 224, 234, 1200.);
        ASSERT_GE(iretn, 0);
        iretn = agents[i]->channel_number("SOCKRADIO");
        ASSERT_GE(iretn, 0);

        // File subagent to communicate out of SOCKRADIO
        uint8_t sockradio_id = iretn;
        file_subagents[i]->set_radios({sockradio_id});
    }

    // Setup websocket connections between the agents
    websocket_subagents.resize(2);
    websocket_subagents[0] = new Module::WebsocketModule();
    int32_t iretn = websocket_subagents[0]->Init(agents[0], "127.0.0.1", PORT_OFFSET+0, PORT_OFFSET+1, "SOCKRADIO");
    ASSERT_GE(iretn, 0);
    iretn = agents[1]->channel_add("SOCKRADIO", 224, 234, 1200.);
    ASSERT_GE(iretn, 0);
    websocket_subagents[1] = new Module::WebsocketModule();
    iretn = websocket_subagents[1]->Init(agents[1], "127.0.0.1", PORT_OFFSET+1, PORT_OFFSET+0, "SOCKRADIO");
    ASSERT_GE(iretn, 0);

    // Start threads
    for (auto& file_subagent : file_subagents)
    {
        subagent_threads.push_back(thread([=] { file_subagent->Loop(); }));
    }
    for (auto& websocket_subagent : websocket_subagents)
    {
        subagent_threads.push_back(thread([=] { websocket_subagent->Loop(); }));
    }
    for (auto& packethandler_subagent : packethandler_subagents)
    {
        subagent_threads.push_back(thread([=] { packethandler_subagent->Loop(); }));
    }
}

void FileSubagentTest::StartTransfers()
{
    PacketComm packet;
    packet.header.type = PacketComm::TypeId::CommandFileTransferRadio;
    packet.data.resize(2);
    for (auto& agent : agents)
    {
        int32_t iretn = agents[0]->channel_number("SOCKRADIO");
        ASSERT_GE(iretn, 1);
        packet.data[0] = iretn & 0xFF;
        packet.data[1] = 1;
        agent->channel_push("FILE", packet);
    }
}

int32_t FileSubagentTest::create_file(int32_t kib, std::string file_path)
{
    std::vector<char> bytes(1024, 0);
    for (size_t i=0; i < bytes.size(); ++i)
    {
        bytes[i] = rand() & 0xFF;
    }
    std::ofstream of(file_path, std::ios::binary | std::ios::out);
    for(int i = 0; i < kib; ++i)
    {
        if (!of.write(&bytes[0], bytes.size()))
        {
            return -1;
        }
    }
    return 0;
}

int32_t FileSubagentTest::create_test_files(string orig_node, string dest_node, double file_size_kib, size_t num_files, string suffix)
{
    this->num_files = num_files;
    int32_t file_size_bytes = file_size_kib * 1024;
    orig_out_dir = data_base_path(dest_node, "outgoing", test_name);
    dest_in_dir = data_base_path(orig_node, "incoming", test_name);
    int32_t iretn = 0;
    for (size_t i = 0; i < this->num_files; ++i)
    {
        string tfilename = "file_" + std::to_string(i) + suffix;
        iretn = create_file(file_size_kib, orig_out_dir + "/" + tfilename);
        if (iretn < 0)
        {
            return iretn;
        }
        int32_t iretn = calc_crc.calc_file(orig_out_dir + "/" + tfilename);
        if (iretn < 0)
        {
            return iretn;
        }
        file_crcs[tfilename] = iretn;
        file_sizes[tfilename] = file_size_bytes;
    }
    return 0;
}

void FileSubagentTest::verify_incoming_dir(string orig_node_name, size_t expected_file_num)
{
    vector<filestruc> incoming_dir = data_list_files(orig_node_name, "incoming", test_name);
    if (incoming_dir.size() != expected_file_num)
    {
        return;
    }
    // Perform actual check
    state = test_state::SUCCESS;
    for (filestruc& file : incoming_dir)
    {
        EXPECT_NE(file_crcs.find(file.name), file_crcs.end());
        EXPECT_NE(file_sizes.find(file.name), file_sizes.end());
        if (file_crcs.find(file.name) == file_crcs.end() || file_sizes.find(file.name) == file_sizes.end())
        {
            test_log.Printf("Verification fail: File name error. %s %d\n", file.name.c_str(), file.size);
            state = test_state::FAIL;
            continue;
        }
        int32_t crc_recv = calc_crc.calc_file(dest_in_dir + "/" + file.name);
        EXPECT_GE(crc_recv, 0);
        if (crc_recv < 0)
        {
            test_log.Printf("Error in calc_file. %s %d\n", file.name.c_str(), file_crcs[file.name]);
        }
        EXPECT_EQ(file_crcs[file.name], crc_recv);
        if (file_crcs[file.name] != crc_recv)
        {
            test_log.Printf("Verification fail: CRC mismatch. %s %d:%d\n", file.name.c_str(), file_crcs[file.name], crc_recv);
            state = test_state::FAIL;
        }
        EXPECT_EQ(file.size, file_sizes[file.name]);
        if (file.size != file_sizes[file.name])
        {
            test_log.Printf("Verification fail: File size error. %s %d:%d\n", file.name.c_str(), file.size, file_sizes[file.name]);
            state = test_state::FAIL;
        }
    }
}

void FileSubagentTest::verify_outgoing_dir(string dest_node_name, size_t expected_file_num)
{
    vector<filestruc> outgoing_dir= data_list_files(dest_node_name, "outgoing", test_name);
    EXPECT_EQ(outgoing_dir.size(), expected_file_num);
    if (outgoing_dir.size() != expected_file_num)
    {
        test_log.Printf("Verification fail: File count incorrect. outgoing_dir: %d, expected: %d\n", outgoing_dir.size(), expected_file_num);
        state = test_state::FAIL;
    }
}

void FileSubagentTest::verify_temp_dir(string orig_node_name, size_t expected_file_num)
{
    vector<filestruc> temp_dir= data_list_files(orig_node_name, "temp", "file");
    EXPECT_EQ(temp_dir.size(), expected_file_num);
    if (temp_dir.size() != expected_file_num)
    {
        test_log.Printf("Verification fail: File count incorrect. %s/temp/file: %d, expected: %d\n", orig_node_name.c_str(), temp_dir.size(), expected_file_num);
        state = test_state::FAIL;
    }
}

} // End namespace File
} // End namespace Integration
} // End namespace Tests
} // End namespace Cosmos

#endif // end __FILE_IT_HELPERS_H__
