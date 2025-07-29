#ifndef UDP_SENDER_H_
#define UDP_SENDER_H_

#include "support/socketlib.h"
#include "support/transferclass.h"

namespace Cosmos {
class UdpSender : public Cosmos::Support::Sender
{
public:
    /**
     * @brief Implementation of the Sender interface that sends packets out over UDP.
     */
    UdpSender() = default;
    void init(const std::string& ip, uint16_t port);
    ~UdpSender();
    Cosmos::Support::SendRetVal send(PacketComm& packet) override;
private:
    //! Socket for sending data
    socket_channel sock;
};
}

#endif