#include "FileSender.h"

void FileSender::init(const std::string& filename)
{
    outfile = ofstream(filename, std::ios::binary);
    if (!outfile.is_open())
    {
        cerr << "Error opening file for writing: " << filename << endl;
        throw std::runtime_error("Failed to open file for writing");
    }
}

FileSender::~FileSender()
{
    if (outfile.is_open())
    {
        outfile.flush();
        outfile.close();
    }
}

SendRetVal FileSender::send(PacketComm& packet)
{
    packet.RawPacketize();
    outfile.write(reinterpret_cast<const char*>(packet.packetized.data()), packet.packetized.size());
    return SendRetVal::SUCCESS;
}
