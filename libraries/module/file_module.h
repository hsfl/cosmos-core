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
        /**
         * @brief A default implementation of the Sender interface that sends packets to a Channel.
         */
        class ChannelSender : public Cosmos::Support::Sender
        {
        public:
            ChannelSender(Agent* agent) : agent_(agent) {}
            Cosmos::Support::SendRetVal send(PacketComm &packet) override;
            void set_outgoing_channels(const vector<uint8_t> &channels)
            {
                channels_ = channels;
            }
        private:
            Agent* agent_;
            vector<uint8_t> channels_;
        };

        class FileModule
        {
        public:
            FileModule(Sender* sender, bool keep_errored_files) : transfer(sender), keep_errored_files{keep_errored_files} {}
            /**
             * @brief Initialize FileModule
             * 
             * @param agent Reference to main agent of this channel
             * @param file_transfer_contact_nodes Names of nodes in nodes.ini that this node can transfer files with
             * @return int32_t 
             */
            int32_t Init(Agent *agent, const vector<string> file_transfer_contact_nodes);
            void Loop();

            /**
             * @brief Stops the loop.
             */
            void soft_shutdown();
            
            /**
             * @brief Shutdown the FileModule. Closes all file pointers.
            */
            void shutdown();

            //! The transfer class, holds all info about ongoing transfers.
            //! Direct manipulation of this object is not thread safe.
            Transfer transfer;

        private:
            bool file_transfer_enabled = false;
            bool file_transfer_respond = false;
            //! The nodes that this node has file transfer capabilities with
            vector<uint8_t> contact_nodes;

            // Whether to keep copies of files that have encountered errors and cancelled
            const bool keep_errored_files = false;

            int32_t mychannel = 0;
            //! Returns from loop if this is set to false
            bool is_running = false;
            Agent *agent;
        };
    }
    
}

#endif // FILE_MODULE_H
