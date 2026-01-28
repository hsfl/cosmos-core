#include "UdpSender.h"

void UdpSender::init(const std::string& ip, uint16_t port)
{
    int32_t iretn = socket_open(sock, NetworkType::UDP, ip.c_str(), port, SOCKET_TALK, SOCKET_BLOCKING);
    if (iretn < 0)
    {
        printf("Error in socket_open out: (%d) %s\n", iretn, cosmos_error_string(iretn).c_str());
        throw std::runtime_error("Failed to open UDP socket");
    }
}

UdpSender::~UdpSender()
{
    socket_close(sock);
}

SendRetVal UdpSender::send(PacketComm& packet)
{
    packet.RawPacketize();
    // agent->monitor_unwrapped(mychannel, packet, "Transmit");
    int32_t iretn = socket_sendto(sock, packet.packetized);
    if (iretn < 0)
    {
        return SendRetVal::ERROR_ABORT;
    }
    increment_number_of_packets_sent();
    return SendRetVal::SUCCESS;
}
