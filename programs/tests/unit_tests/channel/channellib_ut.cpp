#include "channellib_ut.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace Channel {


void ChannelUTest::SetUp()
{
    channel.Init();
}

// Cleanup for text fixture class
void ChannelUTest::TearDown()
{
    for (auto& ch : channel.channel)
    {
        delete ch.mtx;
        ch.mtx = nullptr;
    }
    channel.channel.clear();
}

TEST_F(ChannelUTest, Fixture_class_initializes_correctly)
{
    SetUp();
    vector<string> channel_names = {"SELF", "OBC", "EPS", "ADCS", "FILE", "EXEC", "LOG", "COMM"};
    EXPECT_EQ(channel.channel.size(), 8);
    for (size_t i=0; i < channel.channel.size(); ++i)
    {
        EXPECT_EQ(channel.channel[i].name, channel_names[i]);
        EXPECT_EQ(channel.channel[i].datasize, 1390);
        EXPECT_EQ(channel.channel[i].rawsize, 1400);
        EXPECT_EQ(channel.channel[i].maximum, 1000);
    }
}

// No frills basic transfer across two nodes, from node1 to node2
TEST_F(ChannelUTest, Pulling_corrupt_packet_fails)
{
    SetUp();
    PacketComm packet;
    packet.data = {0, 1, 2, 3, 4, 5};
    packet.Wrap();

    channel.Push(0, packet);
    packet.wrapped[sizeof(PacketComm::Header)]++;
    int32_t iretn = packet.Unwrap();
    EXPECT_LT(iretn, 0);

    // Corrupt the packet
    channel.channel[0].quu.back().wrapped.at(sizeof(PacketComm::Header))++;
    iretn = channel.Pull(0, packet);
    EXPECT_LT(iretn, 0);

}


} // End namespace Channel
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos
