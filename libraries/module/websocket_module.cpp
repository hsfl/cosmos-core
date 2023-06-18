#include "websocket_module.h"

int32_t radio_transmit(socket_channel &radio_channel, Agent* agent, string name, PacketComm& packet);

namespace Cosmos
{
namespace Module
{
    WebsocketModule::WebsocketModule() {}

    int32_t WebsocketModule::Init(Agent* parent_agent, const string& ip, uint16_t port, const string& channel_name)
    {
        int32_t iretn = 0;

        agent = parent_agent;
        tcp_mode = true;

        mychannel_name = channel_name;
        mychannel = agent->channel_number(mychannel_name);
        if (mychannel <= 0)
        {
            agent->debug_log.Printf("No channel for radio %s\n", mychannel_name.c_str());
            return Cosmos::Support::ErrorNumbers::COSMOS_GENERAL_ERROR_ERROR;
        }
        mydatasize = agent->channel_datasize(mychannel);
        myrawsize = agent->channel_rawsize(mychannel);
        mydataspeed = agent->channel_speed(mychannel);

        // open up a socket for sending data to destination
        iretn = socket_open(sock_out, NetworkType::TCP, ip.c_str(), port, SOCKET_TALK, SOCKET_NONBLOCKING);
        if (iretn < 0)
        {
            agent->debug_log.Printf("Error in socket_open out, iretn:%d, %s\n", iretn, mychannel_name.c_str());
            return iretn;
        }

        agent->debug_log.Printf("Subagent %s Init success!\n", mychannel_name.c_str());

        return 0;
    }

    int32_t WebsocketModule::Init(Agent* parent_agent, const string& ip, uint16_t port_in, uint16_t port_out, const string& channel_name)
    {
        int32_t iretn = 0;

        agent = parent_agent;

        mychannel_name = channel_name;
        mychannel = agent->channel_number(mychannel_name);
        if (mychannel <= 0)
        {
            agent->debug_log.Printf("No channel for radio %s\n", mychannel_name.c_str());
            return Cosmos::Support::ErrorNumbers::COSMOS_GENERAL_ERROR_ERROR;
        }
        mydatasize = agent->channel_datasize(mychannel);
        myrawsize = agent->channel_rawsize(mychannel);
        mydataspeed = agent->channel_speed(mychannel);

        // open up a socket for getting data from destination
        iretn = socket_open(sock_in, NetworkType::UDP,  "", port_in, SOCKET_LISTEN, SOCKET_BLOCKING, 50000);
        if (iretn < 0)
        {
            agent->debug_log.Printf("Error in socket_open in, iretn:%d, %s\n", iretn, mychannel_name.c_str());
            return iretn;
        }

        // open up a socket for sending data to destination
        iretn = socket_open(sock_out, NetworkType::UDP, ip.c_str(), port_out, SOCKET_TALK, SOCKET_BLOCKING);
        if (iretn < 0)
        {
            agent->debug_log.Printf("Error in socket_open out, iretn:%d, %s\n", iretn, mychannel_name.c_str());
            return iretn;
        }

        agent->debug_log.Printf("Subagent %s Init success!\n", mychannel_name.c_str());

        return 0;
    }

    void WebsocketModule::Loop()
    {
        agent->debug_log.Printf("Starting %s loop.\n", mychannel_name.c_str());
        while(agent->running())
        {
            // Comm - External
            Receive();

            // Comm - Internal
            Transmit();
            

            // Control
            if (agent->channel_age(mychannel) >= 30.)
            {
                agent->channel_enable(mychannel, 0);
            }
            else if (agent->channel_enabled(mychannel) < 2)
            {
                agent->channel_enable(mychannel, 1);
            }

            std::this_thread::yield();
        }

        return;
    }

    void WebsocketModule::Receive()
    {
        PacketComm packet;
        if (
            // In TCP Mode, only sock_out is set up
            ((tcp_mode && socket_recvfrom(sock_out, packet.packetized, 10000 <= 0))
            // In UDP Mode, recv from sock_in
            || socket_recvfrom(sock_in, packet.packetized, 10000) <= 0)
            )
        {
            return;
        }

        agent->channel_touch(mychannel);
        agent->channel_increment(mychannel, packet.packetized.size());

        bool iret = true;
        switch(unpacketize_function)
        {
        case PacketizeFunction::Raw:
            iret = packet.RawUnPacketize();
            break;
        case PacketizeFunction::SLIP:
            iret = packet.SLIPUnPacketize();
            break;
        case PacketizeFunction::ASM:
            iret = packet.ASMUnPacketize();
            break;
        case PacketizeFunction::AX25:
            iret = packet.AX25UnPacketize();
            break;
        case PacketizeFunction::HDLC:
            iret = packet.HDLCUnPacketize();
            break;
        case PacketizeFunction::None:
            packet.data = packet.packetized;
            break;
        default:
            return;
        }
        if (!iret)
        {
            return;
        }
        switch(packet.header.nodedest)
        {
        // TODO: consider generic packet forwarding to other nodes
        default:
            agent->monitor_unwrapped(mychannel, packet, "Receive");
            agent->channel_push(0, packet);
        }
    }

    void WebsocketModule::Transmit()
    {
        PacketComm packet;
        bool iret = true;
        packet.packetized.clear();
        if (agent->channel_pull(mychannel, packet) <= 0)
        {
            return;
        }
        // Packetize with COSMOS packet formatting if necessary
        switch(packetize_function)
        {
        case PacketizeFunction::Raw:
            iret = packet.RawPacketize();
            break;
        case PacketizeFunction::SLIP:
            iret = packet.SLIPPacketize();
            break;
        case PacketizeFunction::ASM:
            iret = packet.ASMPacketize();
            break;
        case PacketizeFunction::AX25:
            iret = packet.AX25Packetize(dest_call, sour_call, flagcount, dest_stat, sour_stat, cont, prot);
            break;
        case PacketizeFunction::HDLC:
            iret = packet.HDLCPacketize();
            break;
        case PacketizeFunction::None:
            packet.packetized = packet.data;
            break;
        default:
            return;
        }
        // Guard against empty or uninitialized vector
        if (packet.packetized.empty() || !iret)
        {
            return;
        }
        agent->monitor_unwrapped(mychannel, packet, "Transmit");
        if (tcp_mode)
        {
            socket_send(sock_out, packet.packetized);
        }
        else
        {
            socket_sendto(sock_out, packet.packetized);
        }
    }

    void WebsocketModule::set_packetize_function(PacketizeFunction packetize_function_choice)
    {
        packetize_function = packetize_function_choice;
    }

    void WebsocketModule::set_unpacketize_function(PacketizeFunction unpacketize_function_choice)
    {
        unpacketize_function = unpacketize_function_choice;
    }

    void WebsocketModule::set_AX25_Packetize_args(string dest_call, string sour_call, uint8_t flagcount, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
    {
        this->dest_call = dest_call;
        this->sour_call = sour_call;
        this->flagcount = flagcount;
        this->dest_stat = dest_stat;
        this->sour_stat = sour_stat;
        this->cont      = cont;
        this->prot      = prot;
    }

} // End namespace Module
} // End namespace Cosmos
