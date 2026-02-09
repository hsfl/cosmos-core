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

        // Detect if hostname or IPv4 address, convert to latter if necessary
        string ipv4_addr = ip;
        if (ip.find(".") == std::string::npos && ip.find(":") == std::string::npos)
        {
            string response;
            int32_t iretn = hostnameToIP(ip, ipv4_addr, response);
            if (iretn < 0)
            {
                agent->debug_log.Printf("Encountered error in hostnameToIP %s\n", response.c_str());
                return iretn;
            }
        }

        // open up a socket for sending data to destination
        iretn = socket_open(sock_out, NetworkType::TCP, ipv4_addr.c_str(), port, SOCKET_TALK, SOCKET_BLOCKING);
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

        // Detect if hostname or IPv4 address, convert to latter if necessary
        string ipv4_addr = ip;
        if (ip.find(".") == std::string::npos && ip.find(":") == std::string::npos)
        {
            string response;
            int32_t iretn = hostnameToIP(ip, ipv4_addr, response);
            if (iretn < 0)
            {
                agent->debug_log.Printf("Encountered error in hostnameToIP %s\n", response.c_str());
                return iretn;
            }
        }

        // open up a socket for sending data to destination
        iretn = socket_open(sock_out, NetworkType::UDP, ipv4_addr.c_str(), port_out, SOCKET_TALK, SOCKET_BLOCKING);
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
        is_running = true;
        receive_thread = std::thread([=] { Receive(); });
        transmit_thread = std::thread([=] { Transmit(); });
        // Wait for the threads to finish
        receive_thread.join();
        transmit_thread.join();

        socket_close(sock_in);
        socket_close(sock_out);
        return;
    }

    void WebsocketModule::shutdown()
    {
        is_running = false;
    }

    void WebsocketModule::Receive()
    {
        PacketComm packet;
        bool isActive = false;
        while(is_running) {

            // Control
            if (agent->channel_age(mychannel) >= 30.)
            {
                agent->channel_enable(mychannel, 0);
                isActive = false;
            }
            else if (agent->channel_enabled(mychannel) < 2)
            {
                agent->channel_enable(mychannel, 1);
                isActive = true;
            }

            if (
                // In TCP Mode, only sock_out is set up
                ((tcp_mode && socket_recvfrom(sock_out, packet.packetized, 10000) <= 0)
                // In UDP Mode, recv from sock_in
                || socket_recvfrom(sock_in, packet.packetized, 10000) <= 0)
                )
            {
                if (!isActive)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                continue;
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
                continue;
            }
            if (!iret)
            {
                continue;
            }
            switch(packet.header.nodedest)
            {
            // TODO: consider generic packet forwarding to other nodes
            default:
                agent->monitor_unwrapped(mychannel, packet, "Receive");
                agent->channel_push(0, packet); // Note, this call is a bottleneck
            }
        }
    }

    void WebsocketModule::Transmit()
    {
        PacketComm packet;
        bool iret = true;
        packet.packetized.clear();
        while(is_running) {
            if (agent->channel_pull(mychannel, packet) <= 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
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
                continue;
            }
            // Guard against empty or uninitialized vector
            if (packet.packetized.empty() || !iret)
            {
                continue;
            }
            agent->monitor_unwrapped(mychannel, packet, "Transmit");
            if (tcp_mode)
            {
                socket_send(sock_out, packet.packetized);
            }
            else
            {
                int32_t iretn = socket_sendto(sock_out, packet.packetized);
                if (iretn < 0)
                {
                    agent->debug_log.Printf("Error in socket_sendto out, iretn:%d, %s\n", iretn, mychannel_name.c_str());
                    exit(0);
                }
            }
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
