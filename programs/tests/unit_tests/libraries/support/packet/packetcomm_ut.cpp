#include "support/packetcomm.h"
#include "gtest/gtest.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace PacketComm {


TEST(PacketCommUTest, Unwrap_fails_if_packet_is_corrupted)
{
    Support::PacketComm packet;
    vector<uint8_t> mock_bytes = {0, 1, 2, 3, 4, 5};
    packet.data = mock_bytes;
    packet.Wrap();

    // Corrupt the packet
    packet.wrapped[sizeof(Support::PacketComm::Header)]++;

    // Unwrap should fail
    int32_t iretn = packet.Unwrap();
    EXPECT_LT(iretn, 0);
}


} // End namespace PacketComm
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos