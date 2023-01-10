#include "websocket_module.h"

int32_t radio_transmit(socket_channel &radio_channel, Agent* agent, string name, PacketComm& packet);

namespace Cosmos
{
namespace Module
{
    WebsocketModule::WebsocketModule() {}

    int32_t WebsocketModule::Init(Agent* parent_agent, const string& ip, uint16_t port_in, uint16_t port_out, const string& channel_name)
    {
        int32_t iretn = 0;

        agent = parent_agent;

        mychannel_name = channel_name;
        mychannel = agent->channel_number(mychannel_name);
        if (mychannel <= 0)
        {
            agent->debug_error.Printf("No channel for radio %s\n", mychannel_name.c_str());
            return Cosmos::Support::ErrorNumbers::COSMOS_GENERAL_ERROR_ERROR;
        }
        mydatasize = agent->channel_datasize(mychannel);
        myrawsize = agent->channel_rawsize(mychannel);
        mydataspeed = agent->channel_speed(mychannel);

        // open up a socket for getting data from destination
        iretn = socket_open(&sock_in, NetworkType::UDP,  "", port_in, SOCKET_LISTEN, SOCKET_BLOCKING, 50000);
        if (iretn < 0)
        {
            agent->debug_error.Printf("Error in socket_open in, iretn:%d, %s\n", iretn, mychannel_name.c_str());
            return iretn;
        }

        // open up a socket for sending data to destination
        iretn = socket_open(&sock_out, NetworkType::UDP, ip.c_str(), port_out, SOCKET_TALK, SOCKET_BLOCKING, 2000000);
        if (iretn < 0)
        {
            agent->debug_error.Printf("Error in socket_open out, iretn:%d, %s\n", iretn, mychannel_name.c_str());
            return iretn;
        }

        agent->debug_error.Printf("Subagent %s Init success!\n", mychannel_name.c_str());

        return 0;
    }

    void WebsocketModule::Loop()
    {
        agent->debug_error.Printf("Starting %s loop.\n", mychannel_name.c_str());
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
        if (socket_recvfrom(sock_in, packet.packetized, 10000) <= 0)
        {
            return;
        }
        agent->channel_touch(mychannel);
        agent->channel_increment(mychannel, packet.packetized.size());
        if (packet.RawUnPacketize() <= 0)
        {
            return;
        }
        switch(packet.header.nodedest)
        {
        // TODO: consider generic packet forwarding to other nodes
        default:
            agent->channel_push(0, packet);
        }
    }

    void WebsocketModule::Transmit()
    {
        PacketComm packet;
        if (agent->channel_pull(mychannel, packet) <= 0)
        {
            return;
        }
        packet.RawPacketize();
        socket_sendto(sock_out, packet.packetized);
    }

} // End namespace Module
} // End namespace Cosmos