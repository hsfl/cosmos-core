#ifndef WEBSOCKET_MODULE_H
#define WEBSOCKET_MODULE_H

// This subagent handles communication in/out over websockets

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/packetcomm.h"
#include "support/socketlib.h"

namespace Cosmos
{
    namespace Module
    {
        class WebsocketModule
        {
        public:
            WebsocketModule();
            /**
             * @brief Initialize WebsocketModule.
             * 
             * @param parent_agent Pointer to this subagent's parent agent
             * @param ip IP address of destination node, used with port_out
             * @param port_in Port to listen on
             * @param port_out Port to send on
             * @param channel_name Name of this subagent's channel
             * @return 0 on success, negative on error
             */
            int32_t Init(Agent* parent_agent, const string& ip, uint16_t port_in, uint16_t port_out, const string& channel_name);

            /**
             * @brief Loops endlessly until agent is shutdown.
             * 
             * Call this function in a new thread.
             */
            void Loop();

        private:
            //! Receive a packet from socket
            void Receive();
            //! Transmit a packet over socket
            void Transmit();

            Agent *agent;

            string mychannel_name = "";
            int32_t mychannel = 0;
            uint16_t mydatasize = 0;
            uint16_t myrawsize = 0;
            float mydataspeed = 0;
            socket_channel sock_in;
            socket_channel sock_out;
        };
    }
    
}

#endif // WEBSOCKET_MODULE_H
