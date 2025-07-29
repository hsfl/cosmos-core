#ifndef FILE_SENDER_H_
#define FILE_SENDER_H_

#include "support/transferclass.h"
#include <fstream>

namespace Cosmos {
class FileSender : public Cosmos::Support::Sender
{
public:
    /**
     * @brief Implementation of the Sender interface that writes packets to a file.
     */
    FileSender() = default;
    void init(const std::string& filename);
    ~FileSender();
    Cosmos::Support::SendRetVal send(PacketComm& packet) override;
private:
    ofstream outfile;
};
}

#endif