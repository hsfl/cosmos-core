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
            enum class PacketizeFunction
            {
                Raw, SLIP, ASM, AX25, HDLC, None
            };

            WebsocketModule();
            /**
             * @brief Construct a new Websocket Module object
             * 
             * @param packetize_function The PacketComm packetize function to apply before transmitting.
             * @param unpacketize_function The PacketComm unpacketize function to apply after receiving.
             */
            WebsocketModule(PacketizeFunction packetize_function, PacketizeFunction unpacketize_function) : packetize_function(packetize_function), unpacketize_function(unpacketize_function) {}

            /**
             * @brief Initialize TCP WebsocketModule.
             * 
             * @param parent_agent Pointer to this subagent's parent agent
             * @param ip IP address of destination node
             * @param port TCP Port
             * @param channel_name Name of this subagent's channel
             * @return 0 on success, negative on error
             */
            int32_t Init(Agent* parent_agent, const string& ip, uint16_t port, const string& channel_name);

            /**
             * @brief Initialize UDP WebsocketModule.
             * 
             * @param parent_agent Pointer to this subagent's parent agent
             * @param ip IP address of destination node, used with port_out
             * @param port_in UDP Port to listen on
             * @param port_out UDP Port to send on
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

            /**
             * @brief Shutdown the WebsocketModule.
            */
            void shutdown();

            /**
             * @brief  Set the packetizing function to apply before transmitting.
             *
             * Options are: Raw, SLIP, ASM, AX25, HDLC
             */
            void set_packetize_function(PacketizeFunction packetize_function_choice);

            /**
             * @brief  Set the unpacketizing function to apply after receiving.
             *
             * Options are: Raw, SLIP, ASM, AX25, HDLC
             */
            void set_unpacketize_function(PacketizeFunction unpacketize_function_choice);

            /**
             * @brief Set the AX25 Packetize args if AX25Packetize is selected for the packetizing function.
             * 
             * @param dest_call 
             * @param sour_call 
             * @param flagcount 
             * @param dest_stat 
             * @param sour_stat 
             * @param cont 
             * @param prot 
             */
            void set_AX25_Packetize_args(string dest_call="", string sour_call="", uint8_t flagcount=2, uint8_t dest_stat=0x60, uint8_t sour_stat=0x61, uint8_t cont=0x03, uint8_t prot=0xf0);

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

            //! Returns from loop if this is set to false
            bool is_running = true;

            socket_channel sock_in;
            socket_channel sock_out;
            //! The PacketComm packetizing function to use before transmitting
            PacketizeFunction packetize_function = PacketizeFunction::Raw;
            //! The PacketComm unpacketizing function to use after receiving
            PacketizeFunction unpacketize_function = PacketizeFunction::Raw;


            // Parameters for AX25 packetize
            string dest_call="";
            string sour_call="";
            uint8_t flagcount=2;
            uint8_t dest_stat=0x60;
            uint8_t sour_stat=0x61;
            uint8_t cont=0x03;
            uint8_t prot=0xf0;

            bool tcp_mode = false;

        };
    }
    
}

#endif // WEBSOCKET_MODULE_H
