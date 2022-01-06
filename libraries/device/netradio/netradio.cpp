#include "netradio.h"
#include "support/timelib.h"

namespace Cosmos {
    namespace Devices {

//        NetRadio::NetRadio()
//        {

//        }

        int32_t NetRadio::Init(uint16_t inp, uint16_t outp)
        {
            int32_t iretn;
            iretn = socket_open(net_channel_out, outp, 200000);
            if (iretn<0)
            {
                return iretn;
            }
            out_port = outp;

            iretn = socket_open(net_channel_in, NetworkType::UDP, "", inp, SOCKET_LISTEN, SOCKET_NONBLOCKING);
            if (iretn<0)
            {
                return iretn;
            }
            in_port = inp;

            // Start queueing thread
            qthread = thread([=] { queue_loop(); });

            return 0;
        }

        int32_t NetRadio::Packetize(PacketComm &packet)
        {
            if (!packet.Pack())
            {
                return 0;
            }
            packet.packetized.clear();
            packet.packetized.insert(packet.packetized.end(), packet.packed.begin(), packet.packed.end());
            return 1;
        }

        int32_t NetRadio::UnPacketize(PacketComm& packet)
        {
            packet.packed = packet.packetized;
            if (packet.Unpack())
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        // Thread-safe way of pushing onto the packet queue
        int32_t NetRadio::Queue(queue<PacketComm> &queue, mutex &mtx, const PacketComm &p)
        {
            std::lock_guard<mutex> lock(mtx);
            queue.push(p);
            return queue.size();
        }

        // Thread-safe way of pulling from the packet queue
        int32_t NetRadio::DeQueue(queue<PacketComm> &queue, mutex &mtx, PacketComm &p)
        {
            std::lock_guard<mutex> lock(mtx);
            if (queue.size())
            {
                p = queue.front();
                queue.pop();
                return 1;
            }
            else
            {
                return 0;
            }
        }

        int32_t NetRadio::Clear(queue<PacketComm> &queue, mutex &mtx)
        {
            std::lock_guard<mutex> lock(mtx);
            queue = {};
            return queue.size();
        }

        int32_t NetRadio::PacketIn(PacketComm &p)
        {
            int32_t iretn;

            iretn = DeQueue(packet_queue_in, qmutex_in, p);
            if (iretn > 0)
            {
                return UnPacketize(p);
            }
            else
            {
                return iretn;
            }
        }

        int32_t NetRadio::PacketOut(PacketComm &p)
        {
            int32_t iretn;

            iretn = Packetize(p);
            if (iretn > 0)
            {
                return Queue(packet_queue_out, qmutex_out, p);
            }
            else
            {
                return iretn;
            }
        }

        void NetRadio::queue_loop()
        {
            int32_t iretn;
            PacketComm packet;

            while (running)
            {
                // Wait for incoming packets
                iretn = socket_recvfrom(net_channel_in, packet.packetized, 10000);
                if (iretn > 0)
                {
                    iretn = UnPacketize(packet);
                    if (iretn > 0)
                    {
                        Queue(packet_queue_in, qmutex_in, packet);
                    }
                }

                // Send any outgoing packets
                if (packet_queue_out.size())
                {
                    iretn = DeQueue(packet_queue_out, qmutex_out, packet);
                    if (iretn > 0)
                    {
                        iretn = Packetize(packet);
                        if (iretn > 0)
                        {
                            iretn = socket_post(net_channel_out, packet.packetized);
                        }
                    }
                }

                secondsleep(.00001);
            }
        }
    }
}
