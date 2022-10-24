#include "channellib.h"

namespace Cosmos {
    namespace Support {
        Channel::Channel()
        {
//            this->verification = verification;
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

            channel[0].name = "SELF";
            channel[0].mtx = new mutex;
            channel[0].datasize = 1400;
            channel[0].maximum = 1000;

            channel[1].name = "NET";
            channel[1].mtx = new mutex;
            channel[1].datasize = 1400;
            channel[1].maximum = 1000;

            channel[2].name = "EPS";
            channel[2].mtx = new mutex;
            channel[2].datasize = 1400;
            channel[2].maximum = 1000;

            channel[3].name = "ADCS";
            channel[3].mtx = new mutex;
            channel[3].datasize = 1400;
            channel[3].maximum = 1000;

            channel[4].name = "FILE";
            channel[4].mtx = new mutex;
            channel[4].datasize = 1400;
            channel[4].maximum = 1000;

            channel[5].name = "EXEC";
            channel[5].mtx = new mutex;
            channel[5].datasize = 1400;
            channel[5].maximum = 1000;

            this->verification = verification;
            return channel.size();
        }

        int32_t Channel::Check(uint32_t verification)
        {
            if (verification != this->verification)
            {
                return GENERAL_ERROR_ARGS;
            }
            return verification;
        }

        int32_t Channel::Add(string name, uint16_t datasize, uint16_t rawsize, uint16_t maximum)
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
            channel.back().datasize = datasize;
            if (rawsize)
            {
                channel.back().rawsize = rawsize;
            }
            else
            {
                channel.back().rawsize = datasize;
            }
            channel.back().maximum = maximum;
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

        double Channel::Age(string name)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return Age(i);
                }
            }
            return -999999.;
        }

        double Channel::Age(uint8_t number)
        {
            if (number >= channel.size())
            {
                return -999999.;
            }
            channel[number].mtx->lock();
            double age = 86400. * (currentmjd() - channel[number].timestamp);
            channel[number].mtx->unlock();
            return age;
        }

        size_t Channel::Bytes(string name)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return Bytes(i);
                }
            }
            return 0;
        }

        size_t Channel::Bytes(uint8_t number)
        {
            if (number >= channel.size())
            {
                return 0;
            }
            return channel[number].bytes;
        }

        uint32_t Channel::Packets(string name)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return Packets(i);
                }
            }
            return 0;
        }

        uint32_t Channel::Packets(uint8_t number)
        {
            if (number >= channel.size())
            {
                return 0;
            }
            return channel[number].packets;
        }

        double Channel::Touch(string name)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return Touch(i);
                }
            }
            return -999999;
        }

        double Channel::Touch(uint8_t number)
        {
            if (number >= channel.size())
            {
                return -999999.;
            }
            channel[number].mtx->lock();
            double age = 86400. * (currentmjd() - channel[number].timestamp);
            channel[number].timestamp = currentmjd();
            channel[number].mtx->unlock();
            return age;
        }

        size_t Channel::Increment(uint8_t number, size_t byte_count, uint32_t packet_count)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            channel[number].mtx->lock();
            channel[number].bytes += byte_count;
            channel[number].packets += packet_count;
            size_t total_count = channel[number].bytes * channel[number].packets;
            channel[number].mtx->unlock();
            return total_count;
        }

        int32_t Channel::Push(string name, PacketComm &packet)
        {
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return Push(i, packet);
                }
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        int32_t Channel::Push(uint8_t number, PacketComm &packet)
        {
            int32_t iretn = 0;
            if (number >= channel.size())
            {
                iretn = GENERAL_ERROR_OUTOFRANGE;
            }
            else
            {
                channel[number].mtx->lock();
                channel[number].quu.push(packet);
                channel[number].timestamp = currentmjd();
                if (channel[number].quu.size() > channel[number].maximum)
                {
                    channel[number].quu.pop();
                }
                channel[number].mtx->unlock();
                iretn = number;
            }
            return iretn;
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
            int32_t iretn = 0;
            if (number >= channel.size())
            {
                iretn = GENERAL_ERROR_OUTOFRANGE;
            }
            else
            {
                channel[number].mtx->lock();
                if (channel[number].quu.size())
                {
                    packet = channel[number].quu.front();
                    channel[number].quu.pop();
                    iretn = 1;
                }
                else
                {
                    iretn = 0;
                }
                channel[number].mtx->unlock();
            }
            return iretn;
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
            channel[number].mtx->lock();
            int32_t size = channel[number].quu.size();
            channel[number].mtx->unlock();
            return size;
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
            channel[number].mtx->lock();
            std::queue<PacketComm>().swap(channel[number].quu);
            channel[number].mtx->unlock();
            return number;
        }
    }
}
