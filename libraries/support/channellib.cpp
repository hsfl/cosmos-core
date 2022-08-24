#include "channellib.h"

namespace Cosmos {
    namespace Support {
        Channel::Channel(uint32_t verification)
        {
            // Set up default channels for internal activity
            channel.resize(6);
            channel[0].name = "Self";
            channel[0].mtx = new mutex;
            channel[0].datasize = 1400;
            channel[1].name = "Net";
            channel[1].mtx = new mutex;
            channel[1].datasize = 1400;
            channel[2].name = "EPS";
            channel[2].mtx = new mutex;
            channel[2].datasize = 1400;
            channel[3].name = "ADCS";
            channel[3].mtx = new mutex;
            channel[3].datasize = 1400;
            channel[4].name = "FILE";
            channel[4].mtx = new mutex;
            channel[4].datasize = 1400;
            channel[5].name = "EXEC";
            channel[5].mtx = new mutex;
            channel[5].datasize = 1400;
            this->verification = verification;
        }

        Channel::~Channel()
        {
            for (channelstruc& chan : channel)
            {
                delete chan.mtx;
                chan.mtx = nullptr;
            }
        }

        int32_t Channel::Init(uint32_t verification)
        {
            // Set up default channels for internal activity
            channel.resize(6);
            channel[0].name = "Self";
            channel[0].mtx = new mutex;
            channel[0].datasize = 1400;
            channel[1].name = "Net";
            channel[1].mtx = new mutex;
            channel[1].datasize = 1400;
            channel[2].name = "EPS";
            channel[2].mtx = new mutex;
            channel[2].datasize = 1400;
            channel[3].name = "ADCS";
            channel[3].mtx = new mutex;
            channel[3].datasize = 1400;
            channel[4].name = "FILE";
            channel[4].mtx = new mutex;
            channel[4].datasize = 1400;
            channel[5].name = "EXEC";
            channel[5].mtx = new mutex;
            channel[5].datasize = 1400;
            this->verification = verification;
            return verification;
        }

        int32_t Channel::Check(uint32_t verification)
        {
            if (verification != this->verification)
            {
                return GENERAL_ERROR_ARGS;
            }
            return verification;
        }

        int32_t Channel::Add(string name, uint16_t size)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return i;
                }
            }
            channel.resize(channel.size()+1);
            channel.back().name = name;
            channel.back().datasize = size;
            channel.back().mtx = new mutex;
            return channel.size() - 1;
        }

        int32_t Channel::Find(string name)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return i;
                }
            }
            return GENERAL_ERROR_NAME;
        }

        string Channel::Find(uint8_t number)
        {
            string result = "";
            if (number > channel.size())
            {
                return result;
            }
            else
            {
                return channel[number].name;
            }
        }

        int32_t Channel::Push(string name, PacketComm &packet)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    std::lock_guard<mutex> lock(*channel[i].mtx);
                    channel[i].quu.push(packet);
                    return i;
                }
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        int32_t Channel::Push(uint8_t number, PacketComm &packet)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            std::lock_guard<mutex> lock(*channel[number].mtx);
            channel[number].quu.push(packet);
            return number;
        }

        int32_t Channel::Pull(string name, PacketComm &packet)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    std::lock_guard<mutex> lock(*channel[i].mtx);
                    if (channel[i].quu.size())
                    {
                        packet = channel[i].quu.front();
                        channel[i].quu.pop();
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        int32_t Channel::Pull(uint8_t number, PacketComm &packet)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            std::lock_guard<mutex> lock(*channel[number].mtx);
            if (channel[number].quu.size())
            {
                packet = channel[number].quu.front();
                channel[number].quu.pop();
                return 1;
            }
            else
            {
                return 0;
            }
        }

        int32_t Channel::Size(string name)
        {
            if (name.empty())
            {
                std::lock_guard<mutex> lock(*channel[0].mtx);
                return channel[0].quu.size();
            }

            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    std::lock_guard<mutex> lock(*channel[i].mtx);
                    return channel[i].quu.size();
                }
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        int32_t Channel::Size(uint8_t number)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            std::lock_guard<mutex> lock(*channel[number].mtx);
            return channel[number].quu.size();
        }

        int32_t Channel::Clear(string name)
        {
            if (name.empty())
            {
                std::lock_guard<mutex> lock(*channel[0].mtx);
                std::queue<PacketComm>().swap(channel[0].quu);
                return 0;
            }

            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    std::lock_guard<mutex> lock(*channel[i].mtx);
                	std::queue<PacketComm>().swap(channel[i].quu);
                    return i;
                }
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        int32_t Channel::Clear(uint8_t number)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            std::lock_guard<mutex> lock(*channel[number].mtx);
            std::queue<PacketComm>().swap(channel[number].quu);
            return number;
        }
    }
}
