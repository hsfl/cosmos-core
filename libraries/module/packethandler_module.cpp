#include "packethandler_module.h"

int32_t radio_transmit(socket_channel &radio_channel, Agent* agent, string name, PacketComm& packet);

namespace Cosmos
{
namespace Module
{
    PacketHandlerModule::PacketHandlerModule() {}

    int32_t PacketHandlerModule::Init(Agent* parent_agent, const string& channel_name)
    {
        agent = parent_agent;

        mychannel_name = channel_name;
        mychannel = agent->channel_number(mychannel_name);
        if (mychannel <= 0)
        {
            agent->debug_error.Printf("No channel for radio %s\n", mychannel_name.c_str());
            return Cosmos::Support::ErrorNumbers::COSMOS_GENERAL_ERROR_ERROR;
        }

        packethandler.init(agent);

        agent->debug_error.Printf("Subagent %s Init success!\n", mychannel_name.c_str());

        return 0;
    }

    void PacketHandlerModule::Loop()
    {
        agent->debug_error.Printf("Starting %s loop.\n", mychannel_name.c_str());
        PacketComm packet;
        while(agent->running())
        {
            // Comm - Internal
            if (agent->channel_pull(mychannel, packet) > 0)
            {
                string response;
                packethandler.process(packet, response);
                if (packet.header.radio > 0)
                {
                    agent->push_response(packet.header.radio, mychannel, packet.header.orig, 0, response);
                }
            }

            std::this_thread::yield();
        }

        return;
    }

} // End namespace Module
} // End namespace Cosmos
