#ifndef CHANNELLIB_H
#define CHANNELLIB_H

/*! \file channellib.h
* \brief Channel Support
*/

#include <mutex>
#include <thread>
#include "support/configCosmos.h"
#include "support/packetcomm.h"
#include "support/timelib.h"

namespace Cosmos {
    namespace Support {
        //! \class Channel channellib.h "support/channellib.h"
        //! This class allows the construction and management of a ::vector of queues with associated
        //! names, sizes, capacities and other properties. Initialization, using ::Init, will create
        //! a set of standard channels: SELF, NET EPS ADCS, FILE, and EXEC. These can be used to
        //! support standard sub-agents by the same name. Channels are then extended using ::Add.
        //! Each channel has a queue of Cosmos::Support::PacketComm packets of ::maximum length.
        //! Packets are added using ::Push, until ::maximum is reached, at which older packets are removed
        //! from the queue. Packets are extracted from the queue using ::Pull.
        class Channel
        {
        public:
            Channel();
            ~Channel();

            //! Storage for channels
            static constexpr uint16_t NATIVE_BUFFER_SIZE = 1400;
            static constexpr uint16_t PACKETCOMM_PACKETIZED_SIZE = NATIVE_BUFFER_SIZE;
            static constexpr uint16_t PACKETCOMM_WRAPPED_SIZE = PACKETCOMM_PACKETIZED_SIZE;
            static constexpr uint16_t PACKETCOMM_DATA_SIZE = PACKETCOMM_WRAPPED_SIZE - (COSMOS_SIZEOF(PacketComm::Header)+2);

            struct channelstruc
            {
                double timestamp = 0.;
                uint32_t packets = 0;
                size_t bytes = 0;
                string name = "";
                queue<PacketComm> quu;
                std::recursive_mutex* mtx = nullptr;
                uint16_t datasize = PACKETCOMM_DATA_SIZE;
                uint16_t rawsize = PACKETCOMM_PACKETIZED_SIZE;
                uint16_t maximum = 100;
                float byte_rate = 0.;
                // Radio test variables
                thread testthread;
                uint8_t testrunning = 0;
                ElapsedTime testet;
                uint32_t testcount = 0;
                float testseconds = 0.;
                float speed = 0.;
                float remaining = 60.;
                uint8_t test_dest = 0;
                uint8_t test_start = 0;
                uint8_t test_byte = test_start;
                uint8_t test_step = 1;
                uint8_t test_stop = 100;
                uint32_t test_id = 0;
                uint32_t test_bytes = 1000;
                int8_t enabled = 0;
            };
            vector<channelstruc> channel;
            int32_t Init(uint32_t verification=0x352e);
            int32_t Check(uint32_t verification);
            int32_t Add(string name, uint16_t datasize=PACKETCOMM_WRAPPED_SIZE, uint16_t rawsize=0, float byte_rate=1e8, uint16_t maximum=100);
            int32_t Find(string name);
            string Find(uint8_t number);
            int32_t Push(string name, PacketComm &packet);
            int32_t Push(uint8_t number, PacketComm &packet);
            int32_t Pull(string name, PacketComm &packet);
            int32_t Pull(uint8_t number, PacketComm &packet);
            int32_t Size(string name="");
            int32_t Size(uint8_t number=0);
            int32_t Clear(string name);
            int32_t Clear(uint8_t number);
            int32_t Enable(string name, int8_t value);
            int32_t Enable(uint8_t number, int8_t value);
            int32_t Enabled(string name);
            int32_t Enabled(uint8_t number);
//            int32_t Disable(string name);
//            int32_t Disable(uint8_t number);
            double Age(string name);
            double Age(uint8_t number);
            size_t Bytes(string name);
            size_t Bytes(uint8_t number);
            float ByteRate(string name);
            float ByteRate(uint8_t number);
            uint32_t Packets(string name);
            uint32_t Packets(uint8_t number);
            double Touch(string name);
            double Touch(uint8_t number);
            ssize_t Increment(string name, size_t byte_count, uint32_t packet_count=1);
            ssize_t Increment(uint8_t number, size_t byte_count, uint32_t packet_count=1);
            ssize_t Decrement(string name, size_t byte_count, uint32_t packet_count=1);
            ssize_t Decrement(uint8_t number, size_t byte_count, uint32_t packet_count=1);
            int32_t TestStart(string name, string radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);
            int32_t TestStart(uint8_t number, uint8_t radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);
            int32_t TestStop(string name, float seconds=5.);
            int32_t TestStop(uint8_t number, float seconds=5.);
            float TestRemaining(string name);
            float TestRemaining(uint8_t number);
            float TestSpeed(string name);
            float TestSpeed(uint8_t number);
            uint32_t TestLoop(uint8_t number, uint8_t radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);

            uint32_t verification = 0x352e;

        private:

        };
    }
}

#endif // CHANNELLIB_H
