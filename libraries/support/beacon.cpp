#include "beacon.h"

namespace Cosmos {
    namespace Support {
        Beacon::Beacon()
        {

        }

        int32_t Beacon::Init(Agent* calling_agent, uint16_t short_beacon, uint16_t long_beacon) {
            // Beacon and SMS message byte size limits
            short_beacon_size = short_beacon;
            long_beacon_size = long_beacon;
            interval.store(1.);
            pattern_idx = 0;
            agent = calling_agent;
            return 0;
        }

        int32_t Beacon::add_beacon(const string& name, uint8_t type, size_t size) {
            beacon_size[name] = size;
            beacon_typeID[name] = type;
            // TODO: create invalid beacon size error code
            return 0;
        }

        int32_t Beacon::Generate(PacketComm& packet)
        {
            if (agent->cinfo == nullptr)
            {
                return GENERAL_ERROR_NULLPOINTER;
            }

            packet.data.clear();
            switch (packet.type)
            {
            case PacketComm::TypeId::CPU1BeaconS:
                if (agent->cinfo->devspec.cpu.size() && agent->cinfo->devspec.disk.size())
                {
                    cpu1_beacons beacon;
                    beacon.met = agent->cinfo->node.met;
                    beacon.load = agent->cinfo->devspec.cpu[0].load;
                    beacon.memory = agent->cinfo->devspec.cpu[0].gib;
                    beacon.disk = agent->cinfo->devspec.disk[0].gib;
                    packet.data.insert(packet.data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case PacketComm::TypeId::CPU2BeaconS:
                if (agent->cinfo->devspec.cpu.size() && agent->cinfo->devspec.disk.size())
                {
                    cpu2_beacons beacon;
                    beacon.met = agent->cinfo->node.met;
                    beacon.uptime = agent->cinfo->devspec.cpu[0].uptime;
                    beacon.bootcount = agent->cinfo->devspec.cpu[0].boot_count;
                    beacon.initialdate = agent->cinfo->node.utcstart;
                    packet.data.insert(packet.data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case PacketComm::TypeId::TempBeaconS:
                if (agent->cinfo->devspec.tsen.size() >= 3)
                {
                    temp_beacons beacon;
                    beacon.met = agent->cinfo->node.met;
                    for (uint16_t i=0; i<3; ++i)
                    {
                        beacon.temp[i] = agent->cinfo->devspec.tsen[i].temp;
                    }
                    packet.data.insert(packet.data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            default:
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return packet.data.size();
        }

        int32_t Beacon::Decode(PacketComm& packet, string& Contents)
        {
            if (packet.type >= PacketComm::TypeId::BeaconStart && packet.type <= PacketComm::TypeId::BeaconEnd)
            {
                Contents = "[" + packet.TypeString[packet.type] + "]";
                switch (packet.type)
                {
                case PacketComm::TypeId::CPU1BeaconS:
                    {
                        cpu1_beacons beacon;
                        memcpy(&beacon, packet.data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Load", beacon.load, 1);
                        Contents += to_label(" Memory", beacon.memory, 1);
                        Contents += to_label(" Disk", beacon.disk, 1);
                    }
                    break;
                case PacketComm::TypeId::CPU2BeaconS:
                    {
                        cpu2_beacons beacon;
                        memcpy(&beacon, packet.data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Uptime", beacon.uptime);
                        Contents += to_label(" BootCount", beacon.bootcount);
                        Contents += to_label(" InitialDate", beacon.initialdate);
                    }
                    break;
                case PacketComm::TypeId::TempBeaconS:
                    {
                        temp_beacons beacon;
                        memcpy(&beacon, packet.data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Temp1", beacon.temp[0], 1);
                        Contents += to_label(" Temp2", beacon.temp[1], 1);
                        Contents += to_label(" Temp3", beacon.temp[2], 1);
                    }
                    break;
                case PacketComm::TypeId::CPUBeaconL:
                    {
                        Contents = "[CPUBeaconL] ";
                    }
                    break;
                default:
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }
            else
            {
                Contents = "[Unknown Beacon]";
            }
        }

        double Beacon::get_interval() {
            return interval.load();
        }

        int32_t Beacon::set_pattern(const vector<string>& pattern) {
            std::lock_guard<mutex> lock(send_pattern_mtx);
            // TODO: add pattern verifier
            send_pattern = pattern;

            return 0;
        }

        PacketComm Beacon::get_from_name(const string& name) {
            PacketComm packet;
            // Update beacon struct of specified name
            update(name);
            size_t num_bytes = beacon_size[name];
            void* ptr = agent->cinfo->get_pointer(name);
            uint8_t* u_ptr = static_cast<uint8_t*>(ptr);
//            packet.type = beacon_typeID[name];
            packet.data = vector<uint8_t>(u_ptr, u_ptr+num_bytes);
            packet.SLIPPacketize();
            return packet;
        }

        PacketComm Beacon::get_next() {
            string name;
            size_t num_bytes;
            // Critical section
            {
                std::lock_guard<mutex> lock(send_pattern_mtx);
                // Get info of current beacon
                name = send_pattern[pattern_idx++];
                num_bytes = beacon_size[name];
                // Cycle through beacons to send in send_pattern
                if (pattern_idx >= send_pattern.size()) {
                    pattern_idx = 0;
                }
            }
            // Update the beacon struct
//            update(name);

            PacketComm packet;

            // Return beacon struct as SLIP-encoded PacketComm packet
            void* ptr = agent->cinfo->get_pointer(name);
            uint8_t* u_ptr = static_cast<uint8_t*>(ptr);
//            packet.type = beacon_typeID[name];
            packet.data = vector<uint8_t>(u_ptr, u_ptr+num_bytes);
            packet.SLIPPacketize();
            return packet;
        }
    }
}
