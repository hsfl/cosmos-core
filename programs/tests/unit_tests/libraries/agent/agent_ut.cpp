#include "agent/agentclass.h"
#include "gtest/gtest.h"

// Quickly identify if hidden file directory dependencies are being introduced and overlooked
TEST(AgentTest, Agent_is_successfully_created_without_any_setup) {
    string testname = "agent_ut0";
    Agent* agent = new Agent(testname + "_realm", "", "", 0., AGENTMAXBUFFER, false, 0, NetworkType::UDP, 2);
    ASSERT_GE(agent->last_error(), 0);
    delete agent;

    agent = new Agent("", testname + "_node", "", 0., AGENTMAXBUFFER, false, 0, NetworkType::UDP, 2);
    ASSERT_GE(agent->last_error(), 0);
    delete agent;

    agent = new Agent("", "", testname + "_agent", 0., AGENTMAXBUFFER, false, 0, NetworkType::UDP, 2);
    ASSERT_GE(agent->last_error(), 0);
    delete agent;
}

