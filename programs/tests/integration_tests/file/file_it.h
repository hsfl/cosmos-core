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
}; // End class FileSubagentTest

} // End namespace File
} // End namespace Integration
} // End namespace Tests
} // End namespace Cosmos

#endif // End __FILE_IT_H__
