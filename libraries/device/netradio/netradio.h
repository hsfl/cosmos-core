#ifndef NETRADIO_H
#define NETRADIO_H

#include "support/cosmos-errno.h"
#include "support/packetcomm.h"
#include "support/socketlib.h"

namespace Cosmos {
    namespace Devices {
        class NetRadio
        {
        public:
            static constexpr uint16_t PACKETCOMM_PACKET_SIZE = 1400;
            static constexpr uint16_t PACKETCOMM_DATA_SIZE = PACKETCOMM_PACKET_SIZE - (COSMOS_SIZEOF(PacketComm::Header)+2);
            static const int NET_UP_PORT = 10070;
            static const int NET_DOWN_PORT = 10071;

            NetRadio(bool ground=true, uint32_t speed=1000000);
            int32_t Queue(queue<PacketComm> &queue, mutex &mtx, const PacketComm &p);
            int32_t DeQueue(queue<PacketComm> &queue, mutex &mtx, PacketComm &p);
            int32_t PacketIn(PacketComm &p);
            int32_t PacketInSize();
            int32_t PacketOut(PacketComm &p);
            int32_t PacketOutSize();
            int32_t Clear(queue<PacketComm> &queue, mutex &mtx);
            int32_t Init();
            void Join();
            int32_t Packetize(PacketComm& packet);
            int32_t UnPacketize(PacketComm& packet);

        private:
            bool running = true;
            queue<PacketComm> packet_queue_in;
            queue<PacketComm> packet_queue_out;
            void queue_loop();
            thread qthread;
            mutex qmutex_in;
            mutex qmutex_out;
            socket_bus net_channel_out;
            socket_channel net_channel_in;
            uint16_t in_port = 0;
            uint16_t out_port = 0;
            double speed;
        };
    }
}
#endif // NETRADIO_H
