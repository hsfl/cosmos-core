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
            static constexpr uint16_t NET_BUFFER_SIZE = 1000;
            static constexpr uint16_t PACKETCOMM_DATA_SIZE = NET_BUFFER_SIZE - (COSMOS_SIZEOF(PacketComm::Header)+2);
            static const int NET_IN_PORT = 10070;
            static const int NET_OUT_PORT = 10071;

            int32_t Queue(queue<PacketComm> &queue, mutex &mtx, const PacketComm &p);
            int32_t DeQueue(queue<PacketComm> &queue, mutex &mtx, PacketComm &p);
            int32_t PacketIn(PacketComm &p);
            int32_t PacketInSize();
            int32_t PacketOut(PacketComm &p);
            int32_t PacketOutSize();
            int32_t Clear(queue<PacketComm> &queue, mutex &mtx);
            int32_t Init(uint32_t speed=1000000, uint16_t inp = 10070, uint16_t outp = 10071);
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
            uint16_t in_port = NET_IN_PORT;
            uint16_t out_port = NET_OUT_PORT;
            double speed;
        };
    }
}
#endif // NETRADIO_H
