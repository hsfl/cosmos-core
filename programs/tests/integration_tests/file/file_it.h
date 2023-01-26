#ifndef __FILE_IT_H__
#define __FILE_IT_H__

#include "module/file_module.h"
#include "module/packethandler_module.h"
#include "module/websocket_module.h"
#include "thread"
#include "gtest/gtest.h"

namespace Cosmos {
namespace Test {
namespace Integration {
namespace File {

// Websocket in/out ports use offsets starting with this
const uint16_t PORT_OFFSET = 20000;

// Helper  test fixture class for testing the File subagent
class FileSubagentTest : public ::testing::Test {
protected:
    FileSubagentTest() {}
    ~FileSubagentTest() {}

    //! Call explicitly to setup the initial class settings.
    //! Must call TestSetup() to complete setup.
    //! \param test_num_agents Number of agents to create for the test
    void SetUp(uint8_t test_num_agents);

    void TearDown() override;

    //! Agents to represent source and destinations in the test
    std::vector<Agent*> agents;
    //! Each agent will have one correseponding file subagent
    std::vector<Module::FileModule*> file_subagents;
    //! Each agent will have one subagent to handle packethandling
    std::vector<Module::PacketHandlerModule*> packethandler_subagents;
    //! Variable length communication links between nodes. Each websocket subagent represents a "radio".
    std::vector<Module::WebsocketModule*> websocket_subagents;
    //! Each subagent requires a thread of execution
    std::vector<std::thread> subagent_threads;
    uint8_t num_agents = 0;

    // Defined in file_it_helpers.cpp
    //! Number of files in test
    size_t num_files;
    //! Name of outgoing folder for the origin node
    std::string orig_out_dir;
    //! Name of incoming folder for the destination node
    std::string dest_in_dir;
    // CRC class for calculating crcs
    CRC16 calc_crc;
    //! Check crcs before and after sending
    std::map<std::string, uint16_t> file_crcs;
    //! Size of the created files
    std::map<std::string, int32_t> file_sizes;
    //! Name of the current test. Also used as agent subfolder name.
    string test_name;
    //! Log file for test output
    Error test_log;

    enum class test_state: uint8_t { UNFINISHED, SUCCESS, FAIL };
    test_state state = test_state::UNFINISHED;

    //! Set up some nodeids for the test to use
    void load_temp_nodeids(uint8_t num_agents);

    //! Removes created test directories
    void cleanup(uint8_t num_agents);

    //! Completes the test setup, creating the agents and websocket connections
    void TestSetup();

    //! Sends TransferRadio packets to all agents to start file transfer process
    void StartTransfers();

    //! Verify that any incoming files that were transferred are identical to the originals that were being sent.
    //! \param orig_node_name: Name of the destination node
    //! \param expected_file_num: Number of files you expect to see in the incoming folder
    void verify_incoming_dir(string orig_node_name, size_t expected_file_num);

    //! Verify that outgoing folder is in the final state we expect.
    //! \param dest_node_name: Name of the origin node
    //! \param expected_file_num: Number of files you expect to see in the incoming folder
    void verify_outgoing_dir(string dest_node_name, size_t expected_file_num);

    //! Verify that the temp directory containing the meta files are what is expected
    //! \param orig_node_name: Name of the origin node
    //! \param expected_file_num: Number of files you expect to see in the temp/file folder
    void verify_temp_dir(string orig_node_name, size_t expected_file_num);

    /** Create test files for transfer.
    * Files are created in log_it/TESTNAME
    * Records file sizes and file crcs for later verification.
    * \param orig_node: name of the origin node
    * \param dest_node: name of the destination node
    * \param file_size_kib: size of the created files (in kib)
    * \param num_files: number of files to create
    * \param suffix: any additional string to append to the filename
    * \return 0 on success, negative on error
    */
    int32_t create_test_files(string orig_node, string dest_node, double file_size_kib, size_t num_files, string suffix);

    /** Create a file of kib kibibytes at the file_path.
    * For internal use.
    * \param kib: File size in kibibytes
    * \param file_path Full file path of the file to create
    * \return Returns 0 on success, negative on error
    */
    int32_t create_file(int32_t kib, std::string file_path);

}; // End class FileSubagentTest

} // End namespace File
} // End namespace Integration
} // End namespace Tests
} // End namespace Cosmos

#endif // End __FILE_IT_H__
