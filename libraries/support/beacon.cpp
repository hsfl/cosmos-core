#include "beacon.h"

namespace Cosmos {
    namespace Support {
        int32_t Beacon::Init(uint16_t short_beacon, uint16_t long_beacon) {
            // Beacon and SMS message byte size limits
            short_beacon_size = short_beacon;
            long_beacon_size = long_beacon;
            interval = 1.;
            pattern_idx = 0;
//            agent = calling_agent;
            return 0;
        }

        int32_t Beacon::add_beacon(const string& name, uint8_t type, size_t size) {
            beacon_size[name] = size;
            beacon_typeID[name] = type;
            // TODO: create invalid beacon size error code
            return 0;
        }

        int32_t Beacon::Encode(TypeId type, cosmosstruc* cinfo)
        {
            if (cinfo == nullptr)
            {
                return GENERAL_ERROR_NULLPOINTER;
            }

            data.clear();
            this->type = type;
            switch (type)
            {
            case TypeId::CPU1BeaconS:
                if (cinfo->devspec.cpu.size() && cinfo->devspec.disk.size())
                {
                    cpu1_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                    beacon.load = cinfo->devspec.cpu[0].load;
                    beacon.memory = cinfo->devspec.cpu[0].gib;
                    beacon.disk = cinfo->devspec.disk[0].gib;
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::CPU2BeaconS:
                if (cinfo->devspec.cpu.size() && cinfo->devspec.disk.size())
                {
                    cpu2_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                    beacon.uptime = cinfo->devspec.cpu[0].uptime;
                    beacon.bootcount = cinfo->devspec.cpu[0].boot_count;
                    beacon.initialdate = cinfo->node.utcstart;
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::TempBeaconS:
                if (cinfo->devspec.tsen.size() >= 3)
                {
                    temp_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                    for (uint16_t i=0; i<3; ++i)
                    {
                        beacon.temp[i] = cinfo->devspec.tsen[i].temp;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::EPSCPUBeaconS:
                for (uint16_t i=0; i<cinfo->devspec.cpu.size(); ++i)
                {
                    if (cinfo->devspec.cpu[i].name.find("eps") != string::npos)
                    {
                        epscpu_beacons beacon;
                        beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                        beacon.volt = cinfo->devspec.cpu[i].volt;
                        beacon.amp = cinfo->devspec.cpu[i].amp;
                        beacon.temp = cinfo->devspec.cpu[i].temp;
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                        break;
                    }
                }
                break;
            case TypeId::EPSPVBeaconS:
                {
                    epspv_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                    for (uint16_t i=0; i<cinfo->devspec.pvstrg.size(); ++i)
                    {
                        beacon.volt += cinfo->devspec.pvstrg[i].volt;
                        beacon.amp += cinfo->devspec.pvstrg[i].amp;
                        beacon.temp += cinfo->devspec.pvstrg[i].temp;
                    }
                    if (cinfo->devspec.pvstrg.size())
                    {
                        beacon.volt /= cinfo->devspec.pvstrg.size();
                        beacon.temp /= cinfo->devspec.pvstrg.size();
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                    }
                }
                break;
            case TypeId::EPSSWCHBeaconS:
                {
                    epsswch_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                    for (uint16_t i=0; i<cinfo->devspec.swch.size(); ++i)
                    {
                        beacon.volt += cinfo->devspec.swch[i].volt;
                        beacon.amp += cinfo->devspec.swch[i].amp;
                        beacon.temp += cinfo->devspec.swch[i].temp;
                    }
                    if (cinfo->devspec.swch.size())
                    {
                        beacon.volt /= cinfo->devspec.swch.size();
                        beacon.temp /= cinfo->devspec.swch.size();
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                    }
                }
                break;
            case TypeId::EPSBATTBeaconS:
                {
                    epsbatt_beacons beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                    for (uint16_t i=0; i<cinfo->devspec.batt.size(); ++i)
                    {
                        beacon.volt += cinfo->devspec.batt[i].volt;
                        beacon.amp += cinfo->devspec.batt[i].amp;
                        beacon.temp += cinfo->devspec.batt[i].temp;
                    }
                    if (cinfo->devspec.batt.size())
                    {
                        beacon.volt /= cinfo->devspec.batt.size();
                        beacon.temp /= cinfo->devspec.batt.size();
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                    }
                }
                break;
            case TypeId::CPUBeaconL:
                if (cinfo->devspec.cpu.size() && cinfo->devspec.disk.size())
                {
                    cpu1_beaconl beacon;
                    beacon.met = (utc2unixseconds(currentmjd()) - cinfo->node.utcstart);
                    beacon.initialdate = cinfo->node.utcstart;
                    for (uint16_t i=0; i<cinfo->devspec.cpu.size(); ++i)
                    {
                        beacon.cpu[i].uptime = cinfo->devspec.cpu[i].uptime;
                        beacon.cpu[i].bootcount = cinfo->devspec.cpu[i].boot_count;
                        beacon.cpu[i].load = cinfo->devspec.cpu[i].load;
                        beacon.cpu[i].memory = cinfo->devspec.cpu[i].gib;
                        beacon.cpu[i].disk = cinfo->devspec.cpu[i].storage;
                        beacon.cpu[i].temp = cinfo->devspec.cpu[i].temp;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            default:
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return data.size();
        }

        int32_t Beacon::Decode(vector<uint8_t> data, cosmosstruc *cinfo, string& Contents)
        {
            type = (TypeId) data[0];
            if (TypeString.find(type) != TypeString.end())
            {
                switch (type)
                {
                case TypeId::CPUBeaconL:
                    cpu1_beaconl beacon;
                    memcpy(&beacon, data.data(), sizeof(beacon));
                    Contents += to_label(" MET", beacon.met, 1);
                    Contents += to_label(" InitialDate", beacon.initialdate, 1);
                    for (uint16_t i=0; i<6; ++i)
                    {
                        Contents += cinfo->devspec.cpu[i].to_json().dump();
                    }
                    break;
                }
            }
            return 0;
        }

//        int32_t Beacon::Decode(vector<uint8_t> data, string& Contents)
        int32_t Beacon::Decode(const PacketComm& packet, string& Contents)
        {
            this->data = packet.data;
            type = (TypeId) data[0];
            json11::Json jobj;
            if (TypeString.find(type) != TypeString.end())
            {
                Contents = "[" + TypeString[type] + "]";
                switch (type)
                {
                case TypeId::CPU1BeaconS:
                    {
                        cpu1_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        jobj = json11::Json::object {
                            { "node_name", NodeData::lookup_node_id_name(packet.header.orig) },
                            { "beacon_type", TypeString[type] },
                            { "met", float(beacon.met) },
                            { "Load", float(beacon.load) },
                            { "Memory", float(beacon.memory) },
                            { "Disk", float(beacon.disk) },
                        };
                        Contents = jobj.dump();
                    }
                    break;
                case TypeId::CPU2BeaconS:
                    {
                        cpu2_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        jobj = json11::Json::object {
                            { "node_name", NodeData::lookup_node_id_name(packet.header.orig) },
                            { "beacon_type", TypeString[type] },
                            { "met", float(beacon.met) },
                            { "Uptime", int(beacon.uptime) },
                            { "BootCount", int(beacon.bootcount) },
                            { "InitialDate", int(beacon.initialdate) },
                        };
                        Contents = jobj.dump();
                    }
                    break;
                case TypeId::TempBeaconS:
                    {
                        temp_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        jobj = json11::Json::object {
                            { "node_name", NodeData::lookup_node_id_name(packet.header.orig) },
                            { "beacon_type", TypeString[type] },
                            { "met", float(beacon.met) },
                            { "Temp1", float(beacon.temp[0]) },
                            { "Temp2", float(beacon.temp[1]) },
                            { "Temp3", float(beacon.temp[2]) },
                        };
                        Contents = jobj.dump();
                    }
                    break;
                case TypeId::EPSCPUBeaconS:
                    {
                        epscpu_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        jobj = json11::Json::object {
                            { "node_name", NodeData::lookup_node_id_name(packet.header.orig) },
                            { "beacon_type", TypeString[type] },
                            { "met", float(beacon.met) },
                            { "Volt", float(beacon.volt) },
                            { "Amp", float(beacon.amp) },
                            { "Temp", float(beacon.temp) },
                        };
                        Contents = jobj.dump();
                    }
                    break;
                case TypeId::EPSPVBeaconS:
                    {
                        epspv_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        jobj = json11::Json::object {
                            { "node_name", NodeData::lookup_node_id_name(packet.header.orig) },
                            { "beacon_type", TypeString[type] },
                            { "met", float(beacon.met) },
                            { "Volt", float(beacon.volt) },
                            { "Amp", float(beacon.amp) },
                            { "Temp", float(beacon.temp) },
                        };
                        Contents = jobj.dump();
                    }
                    break;
                case TypeId::EPSSWCHBeaconS:
                    {
                        epsswch_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        jobj = json11::Json::object {
                            { "node_name", NodeData::lookup_node_id_name(packet.header.orig) },
                            { "beacon_type", TypeString[type] },
                            { "met", float(beacon.met) },
                            { "Volt", float(beacon.volt) },
                            { "Amp", float(beacon.amp) },
                            { "Temp", float(beacon.temp) },
                        };
                        Contents = jobj.dump();
                    }
                    break;
                case TypeId::EPSBATTBeaconS:
                    {
                        epsbatt_beacons beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        jobj = json11::Json::object {
                            { "node_name", NodeData::lookup_node_id_name(packet.header.orig) },
                            { "beacon_type", TypeString[type] },
                            { "met", float(beacon.met) },
                            { "Volt", float(beacon.volt) },
                            { "Amp", float(beacon.amp) },
                            { "Temp", float(beacon.temp) },
                        };
                        Contents = jobj.dump();
                    }
                    break;
                case TypeId::CPUBeaconL:
                    {
                        cpu1_beaconl beacon;
                        memcpy(&beacon, data.data(), sizeof(beacon));
                        Contents += to_label(" utc", beacon.met, 1);
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
            return interval;
        }

        int32_t Beacon::set_pattern(const vector<string>& pattern) {
            std::lock_guard<mutex> lock(send_pattern_mtx);
            // TODO: add pattern verifier
            send_pattern = pattern;

            return 0;
        }

    }
}
