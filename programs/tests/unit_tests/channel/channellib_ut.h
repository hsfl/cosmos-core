#ifndef __CHANNEL_UT_H__
#define __CHANNEL_UT_H__

#include "support/channellib.h"
#include "gtest/gtest.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace Channel {

// Helper test fixture class for testing channels
class ChannelUTest : public ::testing::Test {
protected:
    ChannelUTest() {}
    ~ChannelUTest() {}

    // Initializes the text fixture class
    void SetUp() override;
    // Clean up for text fixture class
    void TearDown() override;

    // Member variables
    // Channel class to test with
    Support::Channel channel;

    
}; // End class ChannelUTest

} // End namespace Channel
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos

#endif // End __CHANNEL_UT_H__
