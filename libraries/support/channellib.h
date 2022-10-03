#ifndef CHANNELLIB_H
#define CHANNELLIB_H

#include "support/configCosmos.h"
#include "support/packetcomm.h"

namespace Cosmos {
    namespace Support {
        class Channel
        {
        public:
            Channel(uint32_t verification=0x352e);
            ~Channel();

            //! Storage for channels
            static constexpr uint16_t PACKETCOMM_PACKET_SIZE = 1400;
            static constexpr uint16_t PACKETCOMM_DATA_SIZE = PACKETCOMM_PACKET_SIZE - (COSMOS_SIZEOF(PacketComm::Header)+2);

            struct channelstruc
            {
                double timestamp = 0.;
                uint32_t packets = 0;
                size_t bytes = 0;
                string name = "";
                queue<PacketComm> quu;
                mutex* mtx = nullptr;
                uint16_t datasize = PACKETCOMM_DATA_SIZE;
            };
            vector<channelstruc> channel;
            int32_t Init(uint32_t verification=0x352e);
            int32_t Check(uint32_t verification);
            int32_t Add(string name, uint16_t size=PACKETCOMM_PACKET_SIZE);
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
            double Age(string name);
            double Age(uint8_t number);
            size_t Bytes(string name);
            size_t Bytes(uint8_t number);
            uint32_t Packets(string name);
            uint32_t Packets(uint8_t number);
            double Touch(string name);
            double Touch(uint8_t number);
            size_t Increment(uint8_t number, size_t byte_count, uint32_t packet_count=1);

            uint32_t verification;
        };
    }
}

#endif // CHANNELLIB_H
