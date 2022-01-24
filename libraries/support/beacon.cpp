#include "beacon.h"

namespace Cosmos {
    namespace Support {
        int32_t Beacon::Init(Agent* calling_agent, uint16_t short_beacon, uint16_t long_beacon) {
            // Beacon and SMS message byte size limits
            short_beacon_size = short_beacon;
            long_beacon_size = long_beacon;
//            interval.store(1.);
            interval = 1.;
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

        int32_t Beacon::Encode(TypeId type)
        {
            if (agent->cinfo == nullptr)
            {
                return GENERAL_ERROR_NULLPOINTER;
            }

            data.clear();
            this->type = type;
            switch (type)
            {
            case TypeId::CPU1BeaconS:
                if (agent->cinfo->devspec.cpu.size() && agent->cinfo->devspec.disk.size())
                {
                    cpu1_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                    beacon.load = agent->cinfo->devspec.cpu[0].load;
                    beacon.memory = agent->cinfo->devspec.cpu[0].gib;
                    beacon.disk = agent->cinfo->devspec.disk[0].gib;
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::CPU2BeaconS:
                if (agent->cinfo->devspec.cpu.size() && agent->cinfo->devspec.disk.size())
                {
                    cpu2_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                    beacon.uptime = agent->cinfo->devspec.cpu[0].uptime;
                    beacon.bootcount = agent->cinfo->devspec.cpu[0].boot_count;
                    beacon.initialdate = agent->cinfo->node.utcstart;
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::TempBeaconS:
                if (agent->cinfo->devspec.tsen.size() >= 3)
                {
                    temp_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                    for (uint16_t i=0; i<3; ++i)
                    {
                        beacon.temp[i] = agent->cinfo->devspec.tsen[i].temp;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::EPSCPUBeaconS:
                for (uint16_t i=0; i<agent->cinfo->devspec.cpu.size(); ++i)
                {
                    if (agent->cinfo->devspec.cpu[i].name.find("eps") != string::npos)
                    {
                        epscpu_beacons beacon;
                        beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                        beacon.volt = agent->cinfo->devspec.cpu[i].volt;
                        beacon.amp = agent->cinfo->devspec.cpu[i].amp;
                        beacon.temp = agent->cinfo->devspec.cpu[i].temp;
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                        break;
                    }
                }
                break;
            case TypeId::EPSPVBeaconS:
                {
                    epspv_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                    for (uint16_t i=0; i<agent->cinfo->devspec.pvstrg.size(); ++i)
                    {
                        beacon.volt += agent->cinfo->devspec.pvstrg[i].volt;
                        beacon.amp += agent->cinfo->devspec.pvstrg[i].amp;
                        beacon.temp += agent->cinfo->devspec.pvstrg[i].temp;
                    }
                    if (agent->cinfo->devspec.pvstrg.size())
                    {
                        beacon.volt /= agent->cinfo->devspec.pvstrg.size();
                        beacon.temp /= agent->cinfo->devspec.pvstrg.size();
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                    }
                }
                break;
            case TypeId::EPSSWCHBeaconS:
                {
                    epsswch_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                    for (uint16_t i=0; i<agent->cinfo->devspec.swch.size(); ++i)
                    {
                        beacon.volt += agent->cinfo->devspec.swch[i].volt;
                        beacon.amp += agent->cinfo->devspec.swch[i].amp;
                        beacon.temp += agent->cinfo->devspec.swch[i].temp;
                    }
                    if (agent->cinfo->devspec.swch.size())
                    {
                        beacon.volt /= agent->cinfo->devspec.swch.size();
                        beacon.temp /= agent->cinfo->devspec.swch.size();
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                    }
                }
                break;
            case TypeId::EPSBATTBeaconS:
                {
                    epsbatt_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                    for (uint16_t i=0; i<agent->cinfo->devspec.batt.size(); ++i)
                    {
                        beacon.volt += agent->cinfo->devspec.batt[i].volt;
                        beacon.amp += agent->cinfo->devspec.batt[i].amp;
                        beacon.temp += agent->cinfo->devspec.batt[i].temp;
                    }
                    if (agent->cinfo->devspec.batt.size())
                    {
                        beacon.volt /= agent->cinfo->devspec.batt.size();
                        beacon.temp /= agent->cinfo->devspec.batt.size();
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                    }
                }
                break;
            case TypeId::CPUBeaconL:
                if (agent->cinfo->devspec.cpu.size() && agent->cinfo->devspec.disk.size())
                {
                    cpu1_beaconl beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
                    beacon.initialdate = agent->cinfo->node.utcstart;
                    for (uint16_t i=0; i<agent->cinfo->devspec.cpu.size(); ++i)
                    {
                        beacon.cpu[i].uptime = agent->cinfo->devspec.cpu[i].uptime;
                        beacon.cpu[i].bootcount = agent->cinfo->devspec.cpu[i].boot_count;
                        beacon.cpu[i].load = agent->cinfo->devspec.cpu[i].load;
                        beacon.cpu[i].memory = agent->cinfo->devspec.cpu[i].gib;
                        beacon.cpu[i].disk = agent->cinfo->devspec.cpu[i].storage;
                        beacon.cpu[i].temp = agent->cinfo->devspec.cpu[i].temp;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            default:
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return data.size();
        }

        int32_t Beacon::Decode(vector<uint8_t> data, string& Contents)
        {
            this->data = data;
            return Decode(Contents);
        }

        int32_t Beacon::Decode(string& Contents)
        {
            type = (TypeId) data[0];
            if (TypeString.find(type) != TypeString.end())
            {
                Contents = "[" + TypeString[type] + "]";
                switch (type)
                {
                case TypeId::CPU1BeaconS:
                    {
                        cpu1_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Load", beacon.load, 1);
                        Contents += to_label(" Memory", beacon.memory, 1);
                        Contents += to_label(" Disk", beacon.disk, 1);
                    }
                    break;
                case TypeId::CPU2BeaconS:
                    {
                        cpu2_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Uptime", beacon.uptime);
                        Contents += to_label(" BootCount", beacon.bootcount);
                        Contents += to_label(" InitialDate", beacon.initialdate);
                    }
                    break;
                case TypeId::TempBeaconS:
                    {
                        temp_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Temp1", beacon.temp[0], 1);
                        Contents += to_label(" Temp2", beacon.temp[1], 1);
                        Contents += to_label(" Temp3", beacon.temp[2], 1);
                    }
                    break;
                case TypeId::EPSCPUBeaconS:
                    {
                        epscpu_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Volt", beacon.volt, 1);
                        Contents += to_label(" Amp", beacon.amp, 1);
                        Contents += to_label(" Temp", beacon.temp, 1);
                    }
                    break;
                case TypeId::EPSPVBeaconS:
                    {
                        epspv_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Volt", beacon.volt, 1);
                        Contents += to_label(" Amp", beacon.amp, 1);
                        Contents += to_label(" Temp", beacon.temp, 1);
                    }
                    break;
                case TypeId::EPSSWCHBeaconS:
                    {
                        epsswch_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Volt", beacon.volt, 1);
                        Contents += to_label(" Amp", beacon.amp, 1);
                        Contents += to_label(" Temp", beacon.temp, 1);
                    }
                    break;
                case TypeId::EPSBATTBeaconS:
                    {
                        epsbatt_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" Volt", beacon.volt, 1);
                        Contents += to_label(" Amp", beacon.amp, 1);
                        Contents += to_label(" Temp", beacon.temp, 1);
                    }
                    break;
                case TypeId::CPUBeaconL:
                    {
                        cpu1_beaconl beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" MET", beacon.met, 1);
                        Contents += to_label(" InitialDate", beacon.initialdate, 1);
                        for (uint16_t i=0; i<6; ++i)
                        {
                            Contents += to_label(" CPU", i);
                            Contents += to_label(" Uptime", beacon.cpu[i].uptime);
                            Contents += to_label(" BootCount", beacon.cpu[i].bootcount);
                            Contents += to_label(" Load", beacon.cpu[i].load);
                            Contents += to_label(" Memory", beacon.cpu[i].memory);
                            Contents += to_label(" Disk", beacon.cpu[i].disk);
                            Contents += to_label(" Temp", beacon.cpu[i].temp);
                        }
                    }
                    break;
                default:
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }
            else
            {
                Contents = "[Unknown Beacon]";
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return 0;
        }

        double Beacon::get_interval() {
//            return interval.load();
            return interval;
        }

        int32_t Beacon::set_pattern(const vector<string>& pattern) {
            std::lock_guard<mutex> lock(send_pattern_mtx);
            // TODO: add pattern verifier
            send_pattern = pattern;

            return 0;
        }

//        PacketComm Beacon::get_from_name(const string& name) {
//            PacketComm packet;
//            // Update beacon struct of specified name
//            update(name);
//            size_t num_bytes = beacon_size[name];
//            void* ptr = agent->cinfo->get_pointer(name);
//            uint8_t* u_ptr = static_cast<uint8_t*>(ptr);
//            data = vector<uint8_t>(u_ptr, u_ptr+num_bytes);
//            SLIPPacketize();
//            return packet;
//        }

//        PacketComm Beacon::get_next() {
//            string name;
//            size_t num_bytes;
//            // Critical section
//            {
//                std::lock_guard<mutex> lock(send_pattern_mtx);
//                // Get info of current beacon
//                name = send_pattern[pattern_idx++];
//                num_bytes = beacon_size[name];
//                // Cycle through beacons to send in send_pattern
//                if (pattern_idx >= send_pattern.size()) {
//                    pattern_idx = 0;
//                }
//            }
//            // Update the beacon struct

//            PacketComm packet;

//            // Return beacon struct as SLIP-encoded PacketComm packet
//            void* ptr = agent->cinfo->get_pointer(name);
//            uint8_t* u_ptr = static_cast<uint8_t*>(ptr);
//            data = vector<uint8_t>(u_ptr, u_ptr+num_bytes);
//            SLIPPacketize();
//            return packet;
//        }
    }
}
