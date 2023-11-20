#ifndef FILE_MODULE_H
#define FILE_MODULE_H

// This subagent handles file transfers

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/packetcomm.h"
#include "support/transferclass.h"

namespace Cosmos
{

    namespace Module
    {
        class FileModule
        {
        public:
            FileModule();
            /**
             * @brief Initialize FileModule
             * 
             * @param agent Reference to main agent of this channel
             * @param file_transfer_contact_nodes Names of nodes in nodes.ini that this node can transfer files with
             * @return int32_t 
             */
            int32_t Init(Agent *agent, const vector<string> file_transfer_contact_nodes);
            void Loop();

            //! Sets the radios in radios_channel_number to use in order of their priority
            void set_radios(vector<uint8_t> radios);

        private:
            //! The transfer class, holds all info about ongoing transfers
            Transfer transfer;
            //! Vector of radios that are available to use, sorted by use priority, highest priority first
            vector<bool> radios_available;
            //! Maps index of available_radios to a radio channel number, sorted by use priority, highest priority first
            vector<uint8_t> radios_channel_number;
            // Current radio in use to send packets out of
            uint8_t out_radio = 0;
            bool file_transfer_enabled = false;
            bool file_transfer_respond = false;
            //! The nodes that this node has file transfer capabilities with
            vector<uint8_t> contact_nodes;

            //! The speed of packet queueing (packets/sec)
            //! Internally keeps track of the queueing rate of the system,
            //! used to determine how quickly and much to queue packets.
            double packet_rate = 500.;
            //! Don't let system weirdnesses let packet_rate below this value (packets/sec)
            const double PACKET_RATE_LOWER_BOUND = 20.;
            //! The timer to used to help determine the packet_rate
            ElapsedTime queueing_timer;
            //! About the amount of time (in seconds) to wish to continually stream out of a radio for before doing other logic.
            //! TODO: Consider having a separate listening thread to not need stuff like this.
            double continual_stream_time = 5.;

            int32_t mychannel = 0;
            bool running = false;
            Agent *agent;

            /**
             * @brief Sets the availability of a radio
             * 
             * This calls determine_out_radio() after changing radio availabilities to determine newest available radio.
             * 
             * @param radio Radio to set availability for
             * @param availability bool
             */
            void set_radio_availability(uint8_t radio, bool availability);
            //! Sets the out_radio according to priority, called by set_radio_availability()
            void determine_out_radio();
            //! Reads file directory into transfer
        };
    }
    
}

#endif // FILE_MODULE_H
