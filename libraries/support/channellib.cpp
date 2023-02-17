#include "channellib.h"
#include "support/timelib.h"

namespace Cosmos {
    namespace Support {
        //! \brief Constructor
        Channel::Channel()
        {
            //            this->verification = verification;
        }

        //! \brief Cleans up after the channel construction process.
        Channel::~Channel()
        {
            for (channelstruc& chan : channel)
            {
                delete chan.mtx;
                chan.mtx = nullptr;
            }
        }

        //! \brief Establishes default channels. Allows you to change verification code for protected
        //! acitivities from default value of 0x352e.
        //! \param verification Code for verification. Defaults to 0x352e.
        //! \return Number of default channels created.
        int32_t Channel::Init(uint32_t verification)
        {
            // Set up default channels for internal activity
            channel.resize(8);

            channel[0].name = "SELF";
            channel[0].mtx = new std::recursive_mutex;
            channel[0].maximum = 1000;

            channel[1].name = "OBC";
            channel[1].mtx = new std::recursive_mutex;
            channel[1].maximum = 1000;

            channel[2].name = "EPS";
            channel[2].mtx = new std::recursive_mutex;
            channel[2].maximum = 1000;

            channel[3].name = "ADCS";
            channel[3].mtx = new std::recursive_mutex;
            channel[3].maximum = 1000;

            channel[4].name = "FILE";
            channel[4].mtx = new std::recursive_mutex;
            channel[4].maximum = 1000;

            channel[5].name = "EXEC";
            channel[5].mtx = new std::recursive_mutex;
            channel[5].maximum = 1000;

            channel[6].name = "LOG";
            channel[6].mtx = new std::recursive_mutex;
            channel[6].maximum = 1000;

            channel[7].name = "COMM";
            channel[7].mtx = new std::recursive_mutex;
            channel[7].maximum = 1000;

            this->verification = verification;
            return channel.size();
        }

        //! \brief Checks against internal verification code.
        //! \param verification Code for testing.
        //! \return Zero or negative error from ::ErrorNumbers.
        int32_t Channel::Check(uint32_t verification)
        {
            if (verification != this->verification)
            {
                return GENERAL_ERROR_ARGS;
            }
            return 0;
        }

        //! \brief Add additional channel.
        //! \param name Name of channel.
        //! \param datasize Maximum size of Cosmos::PacketComm::data.
        //! \param rawsize Maximum size of associated hardware packet.
        //! \param byte_rate Speed of associated hardware.
        //! \param maximum Maximum size of ::channelstruc::quu.
        int32_t Channel::Add(string name, uint16_t datasize, uint16_t rawsize, float byte_rate, uint16_t maximum)
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
            channel.back().byte_rate = byte_rate;
            channel.back().maximum = maximum;
            channel.back().mtx = new std::recursive_mutex;
            return channel.size() - 1;
        }

        //! \brief Update existing channel.
        //! \param name Name of channel.
        //! \param datasize Maximum size of Cosmos::PacketComm::data.
        //! \param rawsize Maximum size of associated hardware packet.
        //! \param byte_rate Speed of associated hardware.
        //! \param maximum Maximum size of ::channelstruc::quu.
        int32_t Channel::Update(string name, uint16_t datasize, uint16_t rawsize, float byte_rate, uint16_t maximum)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                if (datasize)
                {
                    channel[iretn].datasize = datasize;
                }
                if (rawsize)
                {
                    channel[iretn].rawsize = rawsize;
                }
                if (byte_rate)
                {
                    channel[iretn].byte_rate = byte_rate;
                }
                if (maximum)
                {
                    channel[iretn].maximum = maximum;
                }
            }
            return iretn;
        }

        int32_t Channel::Update(uint8_t number, uint16_t datasize, uint16_t rawsize, float byte_rate, uint16_t maximum)
        {
            if (number < channel.size())
            {
                if (datasize)
                {
                    channel[number].datasize = datasize;
                }
                if (rawsize)
                {
                    channel[number].rawsize = rawsize;
                }
                if (byte_rate)
                {
                    channel[number].byte_rate = byte_rate;
                }
                if (maximum)
                {
                    channel[number].maximum = maximum;
                }
                return number;
            }
            else
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
        }

        //! \brief Find channel number from name.
        //! \param name Name of channel.
        //! \return Number found. Blank name will return 0 (SELF).
        int32_t Channel::Find(string name)
        {
            if (name.empty())
            {
                return 0;
            }
            for (uint8_t i=0; i<channel.size(); ++i)
            {
                if (channel[i].name == name)
                {
                    return i;
                }
            }
            return GENERAL_ERROR_NAME;
        }

        //! \brief Find channel name from number.
        //! \param number Number of channel.
        //! \return Name of channel.
        string Channel::Find(uint8_t number)
        {
            string result = "";
            if (number >= channel.size())
            {
                return result;
            }
            else
            {
                return channel[number].name;
            }
        }

        //! \brief Age from channel name.
        //! Time since ::channelstruc::quu was last modified by ::Push or ::Touch.
        //! \param name Name of channel.
        //! \return Time in seconds.
        double Channel::Age(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Age(iretn);
            }
            return -999999.;
        }

        //! \brief Age from channel number.
        //! Time since ::channelstruc::quu was last modified by ::Push or ::Touch.
        //! \param number Number of channel.
        //! \return Time in seconds.
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

        //! \brief Byte total from channel name.
        //! Number of bytes passed through channel from ::Push.
        //! \param name Name of channel.
        //! \return Total bytes.
        size_t Channel::Bytes(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Bytes(iretn);
            }
            return 0;
        }

        //! \brief Byte total from channel number.
        //! Number of bytes passed through channel from ::Push.
        //! \param number Number of channel.
        //! \return Total bytes.
        size_t Channel::Bytes(uint8_t number)
        {
            if (number >= channel.size())
            {
                return 0;
            }
            return channel[number].bytes;
        }

        //! \brief Byte rate from channel name.
        //! Speed to use when pulling from channel and sending to associated device.
        //! \param name Name of channel.
        //! \return Bytes per second.
        float Channel::ByteRate(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return ByteRate(iretn);
            }
            return 0;
        }

        //! \brief Byte rate from channel number.
        //! Speed to use when pulling from channel and sending to associated device.
        //! \param number Number of channel.
        //! \return Bytes per second.
        float Channel::ByteRate(uint8_t number)
        {
            if (number >= channel.size())
            {
                return 0;
            }
            return channel[number].byte_rate;
        }

        //! \brief Packet total from channel name.
        //! Number of packets passed through channel from ::Push.
        //! \param name Name of channel.
        //! \return Total packets.
        uint32_t Channel::Packets(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Packets(iretn);
            }
            return 0;
        }

        //! \brief Packet total from channel number.
        //! Number of packets passed through channel from ::Push.
        //! \param number Number of channel.
        //! \return Total packets.
        uint32_t Channel::Packets(uint8_t number)
        {
            if (number >= channel.size())
            {
                return 0;
            }
            return channel[number].packets;
        }

        //! \brief Touch channel by name.
        //! Update channel Age.
        //! \param name Name of channel.
        //! \return Previous age in seconds or -999999.
        double Channel::Touch(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Touch(iretn);
            }
            return -999999;
        }

        //! \brief Touch channel by number.
        //! Update channel Age.
        //! \param number Number of channel.
        //! \return Previous age in seconds or -999999.
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

        //! \brief Increment count totals by name.
        //! Add provided values to counters for ::Bytes and ::Packets.
        //! \param name Name of channel.
        //! \param byte_count Number of bytes to increment.
        //! \param packet_count Number of packets to increment.
        //! \return New byte total or negative error.
        ssize_t Channel::Increment(string name, size_t byte_count, uint32_t packet_count)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Increment(iretn, byte_count, packet_count);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Increment count totals by number.
        //! Add provided values to counters for ::Bytes and ::Packets.
        //! \param number Number of channel.
        //! \param byte_count Number of bytes to increment.
        //! \param packet_count Number of packets to increment.
        //! \return New byte total or negative error.
        ssize_t Channel::Increment(uint8_t number, size_t byte_count, uint32_t packet_count)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            channel[number].mtx->lock();
            channel[number].bytes += byte_count;
            channel[number].packets += packet_count;
            channel[number].mtx->unlock();
            return channel[number].bytes;
        }

        //! \brief Decrement count totals by name.
        //! Subtract provided values from counters for ::Cosmos::Support::Channel::Bytes and ::Cosmos::Support::Channel::Packets.
        //! \param name Name of channel.
        //! \param byte_count Number of bytes to decrement.
        //! \param packet_count Number of packets to decrement.
        //! \return New byte total or negative error.
        ssize_t Channel::Decrement(string name, size_t byte_count, uint32_t packet_count)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Decrement(iretn, byte_count, packet_count);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Decrement count totals by number.
        //! Subtract provided values from counters for ::Cosmos::Support::Channel::Bytes and ::Cosmos::Support::Channel::Packets.
        //! \param number Number of channel.
        //! \param byte_count Number of bytes to decrement.
        //! \param packet_count Number of packets to decrement.
        //! \return New byte total or negative error.
        ssize_t Channel::Decrement(uint8_t number, size_t byte_count, uint32_t packet_count)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            channel[number].mtx->lock();
            if (channel[number].bytes >= byte_count)
            {
                channel[number].bytes -= byte_count;
            }
            else
            {
                channel[number].bytes = 0;
            }
            if (channel[number].packets >= packet_count)
            {
                channel[number].packets -= packet_count;
            }
            else
            {
                channel[number].packets = 0;
            }
            channel[number].mtx->unlock();
            return channel[number].bytes;
        }

        //! \brief Push packet to ::Cosmos::Support::Channel::channelstruc::quu by name.
        //! Packet is assumed to be unpacketized, with information in ::Cosmos::Support::PacketComm:data
        //! and ::Cosmos::Support::PacketComm::header. Packet is submitted to ::Cosmos::Support::PacketComm::Wrap
        //! before it is added to ::Cosmos::Support::Channel::channelstruc::quu. Age and counter totals will be updated.
        //! \param name Name of channel.
        //! \param packet ::Cosmos::Support::PacketComm packet.
        //! \return New ::Cosmos::Support::Channel::channelstruc::quu size or negative error.
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

        //! \brief Push packet to ::Cosmos::Support::Channel::channelstruc::quu by number.
        //! Packet is assumed to be unpacketized, with information in ::Cosmos::Support::PacketComm:data
        //! and ::Cosmos::Support::PacketComm::header. Packet is submitted to ::Cosmos::Support::PacketComm::Wrap
        //! before it is added to ::Cosmos::Support::Channel::channelstruc::quu. Age and counter totals will be updated.
        //! \param number Number of channel.
        //! \param packet ::Cosmos::Support::PacketComm packet.
        //! \return New ::Cosmos::Support::Channel::channelstruc::quu size or negative error.
        int32_t Channel::Push(uint8_t number, PacketComm &packet)
        {
            int32_t iretn = 0;
            if (number >= channel.size())
            {
                iretn = GENERAL_ERROR_OUTOFRANGE;
            }
            else
            {
                packet.Wrap();
                channel[number].mtx->lock();
                channel[number].quu.push(packet);
                Increment(number, packet.wrapped.size());
                Touch(number);
                if (channel[number].quu.size() > channel[number].maximum)
                {
                    Decrement(number, channel[number].quu.front().wrapped.size());
                    channel[number].quu.pop();
                }
                iretn = channel[number].quu.size();
                channel[number].mtx->unlock();
                secondsleep(packet.wrapped.size() / channel[number].byte_rate);
            }
            return iretn;
        }

        //! \brief Pull packet from ::Cosmos::Support::Channel::channelstruc::quu by name.
        //! Packet is assumed to be unpacketized, with information in ::Cosmos::Support::PacketComm:data
        //! and ::Cosmos::Support::PacketComm::header. Packet is submitted to ::Cosmos::Support::PacketComm::Unwrap
        //! after it is pulled from ::Cosmos::Support::Channel::channelstruc::quu.
        //! \param name Name of channel.
        //! \param packet ::Cosmos::Support::PacketComm packet.
        //! \return New ::Cosmos::Support::Channel::channelstruc::quu size or negative error.
        int32_t Channel::Pull(string name, PacketComm &packet)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Pull(iretn, packet);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Pull packet from ::Cosmos::Support::Channel::channelstruc::quu by number.
        //! Packet is assumed to be unpacketized, with information in ::Cosmos::Support::PacketComm:data
        //! and ::Cosmos::Support::PacketComm::header. Packet is submitted to ::Cosmos::Support::PacketComm::Unwrap
        //! after it is pulled from ::Cosmos::Support::Channel::channelstruc::quu.
        //! \param number Number of channel.
        //! \param packet ::Cosmos::Support::PacketComm packet.
        //! \return Zero if nothing to pull, One if successful, or negative error.
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
                    iretn = packet.Unwrap();
                    if (iretn >= 0)
                    {
                        iretn = 1;
                    }
                    channel[number].quu.pop();
                }
                else
                {
                    iretn = 0;
                }
                channel[number].mtx->unlock();
            }
            return iretn;
        }

        //! \brief ::Cosmos::Support::Channel::channelstruc::quu size from channel name.
        //! \param name Name of channel.
        //! \return Size of ::Cosmos::Support::Channel::channelstruc::quu.
        int32_t Channel::Size(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Size(iretn);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief ::Cosmos::Support::Channel::channelstruc::quu size from channel name.
        //! \param number Number of channel.
        //! \return Size of ::Cosmos::Support::Channel::channelstruc::quu.
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

        //! \brief Clear ::Cosmos::Support::Channel::channelstruc::quu size by channel name.
        //! \param name Name of channel.
        //! \return Number of ::Cosmos::Support::Channel::channelstruc::quu or negative error.
        int32_t Channel::Clear(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Clear(iretn);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Clear ::Cosmos::Support::Channel::channelstruc::quu size by channel number.
        //! \param name Name of channel.
        //! \return Number of ::Cosmos::Support::Channel::channelstruc::quu or negative error.
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

        //! \brief Enable transmission over channel
        //! \param channel Channel name
        //! \param value 0 = disabled, 1 = active, 2 = suppressed
        //! \return Previous state, or negative error.
        int32_t Channel::Enable(string name, int8_t value)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Enable(iretn, value);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Enable transmission over channel
        //! \param channel Channel number
        //! \param value 0 = disabled, 1 = active, 2 = suppressed
        //! \return Previous state, or negative error.
        int32_t Channel::Enable(uint8_t number, int8_t value)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            int32_t iretn = 0;
            if (channel[number].enabled == value)
            {
                iretn = value;
            }
            else
            {
                iretn = channel[number].enabled;
                channel[number].enabled = value;
            }
            return iretn;
        }

        //! \brief Enabled State of transmission over channel
        //! \param channel Channel name
        //! \return Current state, or negative error.
        int32_t Channel::Enabled(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return Enabled(iretn);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Enabled State of transmission over channel
        //! \param channel Channel number
        //! \return Current state, or negative error.
        int32_t Channel::Enabled(uint8_t number)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            int32_t iretn = 0;
            iretn = channel[number].enabled;
            return iretn;
        }

        //! \brief Start channel performance test by channel name.
        //! \param name Name of channel.
        //! \param radio Name of radio.
        //! \param id Test ID.
        //! \param orig Node ID of origin.
        //! \param dest Node ID of destination.
        //! \param start First value of byte in sequence.
        //! \param step Amount to increment each byte in sequence.
        //! \param stop Value to reach before starting sequence again.
        //! \param total Total number of bytes to send in test.
        //! \return Number of ::Cosmos::Support::Channel::channelstruc::quu or negative error.
        int32_t Channel::TestStart(string name, string radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total)
        {
            int32_t number = Find(name);
            int32_t nradio = Find(radio);
            if (nradio >= 0 && number >= 0)
            {
                return TestStart(number, nradio, id, orig, dest, start, step, stop, total);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Start channel performance test by channel number.
        //! \param number Number of channel.
        //! \param radio Number of radio.
        //! \param id Test ID.
        //! \param orig Node ID of origin.
        //! \param dest Node ID of destination.
        //! \param start First value of byte in sequence.
        //! \param step Amount to increment each byte in sequence.
        //! \param stop Value to reach before starting sequence again.
        //! \param total Total number of bytes to send in test.
        //! \return Number of ::Cosmos::Support::Channel::channelstruc::quu or negative error.
        int32_t Channel::TestStart(uint8_t number, uint8_t radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total)
        {
            int32_t iretn = 0;
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            if (channel[number].testrunning == 1)
            {
                return GENERAL_ERROR_BUSY;
            }

            if (channel[number].testrunning == 2)
            {
                iretn = TestStop(number);
                if (iretn < 0)
                {
                    return iretn;
                }
            }

            channel[number].testthread = thread([=] { TestLoop(number, radio, id, orig, dest, start, step, stop, total); });
            return 0;
        }

        //! \brief Stop test by channel name.
        //! \param name Name of channel.
        //! \param seconds Seconds to wait for test thread to stop
        //! \return Zero or negative error.
        int32_t Channel::TestStop(string name, float seconds)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return TestStop(iretn, seconds);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Check progress of test by channel number.
        //! \param number Number of channel.
        //! \param seconds Seconds to wait for test thread to stop. Constrained to 0 - 60, otherwise set to 10.
        //! \return Zero or negative error.
        int32_t Channel::TestStop(uint8_t number, float seconds)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            if (!channel[number].testrunning)
            {
                return 0;
            }

            if (seconds < 1. || seconds > 10.)
            {
                seconds = 10.;
            }
            channel[number].testrunning = 2;
            ElapsedTime et;
            while (et.split() < seconds && !channel[number].testthread.joinable())
            {
                std::this_thread::yield();
            }
            if (channel[number].testthread.joinable())
            {
                channel[number].testthread.join();
            }

            return channel[number].remaining;
        }

        //! \brief Check progress of test by channel name.
        //! \param name Name of channel.
        //! \return Estimated seconds to completion or negative error.
        float Channel::TestRemaining(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return TestRemaining(iretn);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Check progress of test by channel number.
        //! \param number Number of channel.
        //! \return Estimated seconds to completion or negative error.
        float Channel::TestRemaining(uint8_t number)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            if (channel[number].testrunning == 0 || channel[number].testrunning == 2)
            {
                return 0.;
            }

            return channel[number].remaining;
        }


        //! \brief Check speed of test by channel name.
        //! \param name Name of channel.
        //! \return Estimated seconds to completion or negative error.
        float Channel::TestSpeed(string name)
        {
            int32_t iretn = Find(name);
            if (iretn >= 0)
            {
                return TestSpeed(iretn);
            }
            return GENERAL_ERROR_OUTOFRANGE;
        }

        //! \brief Check speed of test by channel number.
        //! \param number Number of channel.
        //! \return Speed of test or negative error.
        float Channel::TestSpeed(uint8_t number)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            if (channel[number].testrunning == 0 || channel[number].testrunning == 2)
            {
                return 0.;
            }

            return channel[number].speed;
        }

        //! \brief Loop for performing test.
        //! \param channel Number of channel.
        //! \param radio Number of radio.
        //! \param id Test ID.
        //! \param orig Node ID of origin.
        //! \param dest Node ID of destination.
        //! \param start First value of byte in sequence.
        //! \param step Amount to increment each byte in sequence.
        //! \param stop Value to reach before starting sequence again.
        //! \param total Total number of bytes to send in test.
        //! \return Number of ::Cosmos::Support::Channel::channelstruc::quu or negative error.
        uint32_t Channel::TestLoop(uint8_t number, uint8_t radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total)
        {
            if (number >= channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

//            int32_t iretn = 0;
            channel[number].testrunning = 1;
            channel[number].test_bytes = total;
            channel[number].test_id = id;
            channel[number].test_start = start;
            channel[number].test_step = step;
            channel[number].test_stop = stop;

            // Packet defaults
            PacketComm test_packet;
            test_packet.data.resize(channel[number].datasize);
            test_packet.header.type = PacketComm::TypeId::DataRadioTest;
            test_packet.header.nodeorig = orig;
            test_packet.header.nodedest = dest;
            test_packet.header.chanin = radio;

            PacketComm::TestHeader theader;
            theader.size = channel[number].datasize-(sizeof(theader)+2);
            theader.test_id = id;
            theader.packet_id = 0;

            uint8_t byte_value = start;
            size_t step_count = 0;
            for (size_t ib=0; ib<theader.size; ++ib)
            {
                test_packet.data[sizeof(PacketComm::TestHeader) + ib] = byte_value;
                if (step_count++ < stop)
                {
                    byte_value += step;
                }
                else
                {
                    step_count = 0;
                    byte_value = start;
                }
            }

            channel[number].testet.reset();
            channel[number].testcount = 0;
            channel[number].testseconds = 0.;
            while (channel[number].testrunning == 1 && channel[number].testcount < total && (channel[number].testseconds=channel[number].testet.split()) <= 60.)
            {
                memcpy(&test_packet.data[0], &theader, sizeof(theader));
                uint16_t crccalc = test_packet.calc_crc.calc(&test_packet.data[0], channel[number].datasize-2);
                test_packet.data[channel[number].datasize-1] = (crccalc>>8);
                test_packet.data[channel[number].datasize-2] = (crccalc&0xff);
                Push(number, test_packet);
                secondsleep(test_packet.packetized.size() / channel[number].byte_rate);
                theader.packet_id++;
                channel[number].testcount += theader.size;
                if (channel[number].testseconds > 0. && channel[number].testcount > 0)
                {
                    channel[number].speed = channel[number].testcount / channel[number].testseconds;
                    channel[number].remaining = (total - channel[number].testcount) / channel[number].speed;
                }
            }
            theader.packet_id = ((uint32_t)-1);
            memcpy(&test_packet.data[0], &theader, sizeof(theader));
            uint16_t crccalc = test_packet.calc_crc.calc(&test_packet.data[0], channel[number].datasize-2);
            test_packet.data[channel[number].datasize-1] = (crccalc>>8);
            test_packet.data[channel[number].datasize-2] = (crccalc&0xff);
            Push(number, test_packet);
            channel[number].testseconds = channel[number].testet.split();
            if (channel[number].testseconds > 0. && channel[number].testcount > 0)
            {
                channel[number].speed = channel[number].testcount / channel[number].testseconds;
                channel[number].remaining = (total - channel[number].testcount) / channel[number].speed;
            }
            channel[number].testrunning = 2;
            return channel[number].testcount;
        }

    }
}
