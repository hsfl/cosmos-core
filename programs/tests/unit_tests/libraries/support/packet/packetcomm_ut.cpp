#include "support/packetcomm.h"
#include "gtest/gtest.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace PacketComm {

Support::PacketComm make_ping(vector<uint8_t> data = {1, 2, 3, 4, 5})
{
    Support::PacketComm packet;
    packet.header.type = Support::PacketComm::TypeId::CommandObcPing;
    packet.header.nodeorig = 3;
    packet.header.nodedest = 7;
    packet.data = data;
    return packet;
}

void expect_same_packet(Support::PacketComm &actual, Support::PacketComm &expected)
{
    EXPECT_EQ(actual.header.type, expected.header.type);
    EXPECT_EQ(actual.header.nodeorig, expected.header.nodeorig);
    EXPECT_EQ(actual.header.nodedest, expected.header.nodedest);
    EXPECT_EQ(actual.data, expected.data);
}


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


// Hex string encoding

TEST(PacketCommUTest, Packet_survives_encode_and_decode)
{
    Support::PacketComm packet = make_ping();
    string hex = packet.ToHexString();
    Support::PacketComm decoded;
    ASSERT_GE(decoded.FromHexString(hex), 0);
    expect_same_packet(decoded, packet);
}

TEST(PacketCommUTest, Packet_with_empty_data_survives_encode_and_decode)
{
    Support::PacketComm packet = make_ping({});
    Support::PacketComm decoded;
    ASSERT_GE(decoded.FromHexString(packet.ToHexString()), 0);
    expect_same_packet(decoded, packet);
}

TEST(PacketCommUTest, Encoding_is_plain_lowercase_hex)
{
    Support::PacketComm packet = make_ping();
    string hex = packet.ToHexString();
    EXPECT_EQ(hex.size(), 2 * packet.wrapped.size());
    EXPECT_EQ(hex.find_first_not_of("0123456789abcdef"), string::npos);
}

TEST(PacketCommUTest, Malformed_strings_are_rejected)
{
    Support::PacketComm packet = make_ping();
    string hex = packet.ToHexString();
    Support::PacketComm decoded;
    EXPECT_LT(decoded.FromHexString(""), 0);
    EXPECT_LT(decoded.FromHexString(hex.substr(1)), 0);
    EXPECT_LT(decoded.FromHexString("zz" + hex.substr(2)), 0);
    EXPECT_LT(decoded.FromHexString(hex.substr(0, hex.size() - 4)), 0);
}

TEST(PacketCommUTest, Non_v2_packet_is_not_encoded)
{
    Support::PacketComm packet = make_ping();
    packet.style = Support::PacketComm::PacketStyle::Minimal;
    EXPECT_EQ(packet.ToHexString(), "");
    packet.style = Support::PacketComm::PacketStyle::None;
    EXPECT_EQ(packet.ToHexString(), "");
}

TEST(PacketCommUTest, Corrupted_packet_fails_crc)
{
    Support::PacketComm packet = make_ping();
    string hex = packet.ToHexString();
    // Flip a data nibble, leaving the CRC as it was
    size_t pos = hex.size() - 6;
    hex[pos] = hex[pos] == '0' ? '1' : '0';
    Support::PacketComm decoded;
    EXPECT_LT(decoded.FromHexString(hex), 0);
}


} // End namespace PacketComm
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos