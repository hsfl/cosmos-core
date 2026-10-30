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
        if (mychannel < 0 || (!mychannel && channel_name != "SELF"))
        {
            agent->debug_log.Printf("No channel for radio %s\n", mychannel_name.c_str());
            return Cosmos::Support::ErrorNumbers::COSMOS_GENERAL_ERROR_ERROR;
        }

        packethandler.init(agent);

        // Set here rather than in Loop(), so a shutdown() before the thread starts is not lost
        is_running = true;

        agent->debug_log.Printf("Subagent %s Init success!\n", mychannel_name.c_str());

        return 0;
    }

    int32_t PacketHandlerModule::add_func(PacketComm::TypeId type, PacketHandler::ExternalFunc function)
    {
        return packethandler.add_func(type, function);
    }

    void PacketHandlerModule::Loop()
    {
        agent->debug_log.Printf("Starting %s loop.\n", mychannel_name.c_str());
        PacketComm packet;
        while(is_running)
        {
            // Comm - Internal
            if (agent->channel_pull(mychannel, packet) > 0)
            {
                string response;
                response.clear();
                packethandler.process(packet, response);
                if (response.size() > 0)
                {
                    agent->push_response(packet.header.chanin, mychannel, packet.header.nodeorig, packet.response_id, response);
                }
            }
            else
            {
                // Nothing pending, so avoid spinning a core
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        return;
    }

    void PacketHandlerModule::shutdown()
    {
        is_running = false;
    }

} // End namespace Module
} // End namespace Cosmos
