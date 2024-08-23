#ifndef PACKETHANDLER_MODULE_H
#define PACKETHANDLER_MODULE_H

// This subagent handles packet rerouting via packethandler

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/packetcomm.h"
#include "support/packethandler.h"

namespace Cosmos
{
    namespace Module
    {
        class PacketHandlerModule
        {
        public:
            PacketHandlerModule();
            /**
             * @brief Initialize PacketHandlerModule.
             * 
             * @param parent_agent Pointer to this subagent's parent agent
             * @param channel_name Name of this subagent's channel
             * @return 0 on success, negative on error
             */
            int32_t Init(Agent* parent_agent, const string& channel_name);

            /**
             * @brief Loops endlessly until agent is shutdown.
             * 
             * Call this function in a new thread.
             */
            void Loop();

            /**
             * @brief Shutdown the module.
            */
            void shutdown();

        private:
            //! Receive a packet from socket
            PacketHandler packethandler;
            socket_channel sock_out;

            Agent *agent;

            //! Returns from loop if this is set to false
            bool is_running = false;

            string mychannel_name = "";
            int32_t mychannel = 0;
        };
    }
    
}

#endif // PACKETHANDLER_MODULE_H
