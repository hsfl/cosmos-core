#include "beacon.h"

namespace Cosmos {
    namespace Support {
        int32_t Beacon::Init() {
            // Beacon and SMS message byte size limits
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

        int32_t Beacon::EncodeBinary(TypeId type, cosmosstruc* cinfo, vector<uint8_t> &data)
        {
            if (cinfo == nullptr)
            {
                return GENERAL_ERROR_NULLPOINTER;
            }

            cinfo->node.deci = decisec();
            data.clear();
            this->type = type;
            switch (type)
            {
            case TypeId::ADCSORBITBeaconS:
                if (cinfo->tle.size())
                {
                    adcsorbit_beacon beacon;
                    beacon.deci = cinfo->node.deci;

                    beacon.i = cinfo->tle[0].i;
                    beacon.e = cinfo->tle[0].e;
                    beacon.raan = cinfo->tle[0].raan;
                    beacon.ap = cinfo->tle[0].ap;
                    beacon.bstar = cinfo->tle[0].bstar;
                    beacon.mm = cinfo->tle[0].mm;
                    beacon.ma = cinfo->tle[0].ma;
                    //beacon.epoch = cinfo->tle[0].epoch;

                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::ADCSEXTRABeaconS:
                if (cinfo->devspec.mag.size())
                {
                    // this is junk for now
                    adcsextra_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    beacon.x = cinfo->devspec.mag[0].mag.col[0];
                    beacon.y = cinfo->devspec.mag[0].mag.col[1];
                    beacon.z = cinfo->devspec.mag[0].mag.col[2];
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::ADCSMAGBeaconS:
                if (cinfo->devspec.mag.size())
                {
                    adcsmag_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    beacon.magx = cinfo->devspec.mag[0].mag.col[0];
                    beacon.magy = cinfo->devspec.mag[0].mag.col[1];
                    beacon.magz = cinfo->devspec.mag[0].mag.col[2];
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::CPU1BeaconS:
                if (cinfo->devspec.cpu.size())
                {
                    cpu1_beacons beacon;
                    beacon.deci = cinfo->node.deci;
                    beacon.load = cinfo->devspec.cpu[0].load;
                    beacon.memory = cinfo->devspec.cpu[0].gib;
                    beacon.cdisk = cinfo->devspec.cpu[0].storage * 100. + .5;
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::CPU2BeaconS:
                if (cinfo->devspec.cpu.size())
                {
                    cpu2_beacons beacon;
                    beacon.deci = cinfo->node.deci;
                    beacon.uptime = cinfo->devspec.cpu[0].uptime;
                    beacon.bootcount = cinfo->devspec.cpu[0].boot_count;
                    beacon.initialdate = utc2unixseconds(cinfo->node.utcstart) + .5;
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::TimeBeaconS:
                if (cinfo->devspec.cpu.size())
                {
                    data.resize(16);
                    double mjd = currentmjd();
                    memcpy(&data[0], &mjd, 8);
                    double utcstart = cinfo->node.utcstart;
                    memcpy(&data[8], &utcstart, 8);
                }
                break;
            case TypeId::TsenBeaconS:
                //                if (cinfo->devspec.tsen.size() >= 6)
                {
                    tsen_beacons beacon;
                    beacon.deci = cinfo->node.deci;
                    for (uint16_t i=0; i<(cinfo->devspec.tsen.size()>=6?6:cinfo->devspec.tsen.size()); ++i)
                    {
                        beacon.ctemp[i] = cinfo->devspec.tsen[i].temp * 100. + .5;
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
                        beacon.deci = cinfo->node.deci;
                        beacon.volt = cinfo->devspec.cpu[i].volt;
                        beacon.amp = cinfo->devspec.cpu[i].amp;
                        beacon.temp = cinfo->devspec.cpu[i].temp;
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                        break;
                    }
                }
                break;
            case TypeId::EPSBCREGBeaconS:
                {
                    epsbcreg_beacons beacon;
                    beacon.deci = cinfo->node.deci;
                    for (uint16_t i=0; i<cinfo->devspec.bcreg.size(); ++i)
                    {
                        beacon.volt += cinfo->devspec.bcreg[i].volt;
                        beacon.amp += cinfo->devspec.bcreg[i].amp;
                        beacon.temp += cinfo->devspec.bcreg[i].temp / cinfo->devspec.bcreg.size();
                    }
                    if (cinfo->devspec.bcreg.size())
                    {
                        beacon.volt /= cinfo->devspec.bcreg.size();
                        beacon.temp /= cinfo->devspec.bcreg.size();
                        data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                    }
                }
                break;
            case TypeId::EPSSWCHBeaconS:
                {
                    epsswch_beacons beacon;
                    beacon.deci = cinfo->node.deci;
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
                    beacon.deci = cinfo->node.deci;
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
            case TypeId::RadioBeaconS:
                {
                    radio_beacons beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t radiocount = 0;
                    uint16_t rxrcount = cinfo->devspec.rxr.size();
                    for (uint16_t i=0; i<rxrcount; ++i)
                    {
                        if (cinfo->devspec.rxr[i].state)
                        {
                            beacon.active |= (1 << radiocount++);
                        }
                        if (decisec(cinfo->devspec.rxr[i].utcin) > beacon.lastdeciup)
                        {
                            beacon.lastdeciup = decisec(cinfo->devspec.rxr[i].utcin);
                        }
                    }
                    uint16_t txrcount = cinfo->devspec.txr.size();
                    for (uint16_t i=0; i<txrcount; ++i)
                    {
                        if (cinfo->devspec.txr[i].state)
                        {
                            beacon.active |= (1 << radiocount++);
                        }
                        if (decisec(cinfo->devspec.txr[i].utcout) > beacon.lastdecidown)
                        {
                            beacon.lastdecidown = decisec(cinfo->devspec.txr[i].utcout);
                        }
                    }
                }
                break;
            case TypeId::CPUBeacon:
                if (cinfo->devspec.cpu.size())
                {
                    cpus_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    beacon.initialdate = utc2unixseconds(cinfo->node.utcstart) + .5;
                    uint16_t cpucount = cinfo->devspec.cpu.size() < cpu_count?cinfo->devspec.cpu.size():cpu_count;
                    for (uint16_t i=0; i<cpucount; ++i)
                    {
                        beacon.cpu[i].uptime = cinfo->devspec.cpu[i].uptime;
                        beacon.cpu[i].bootcount = cinfo->devspec.cpu[i].boot_count;
                        beacon.cpu[i].mload = std::min(cinfo->devspec.cpu[i].load * 1000. + .5, 65535.);
                        beacon.cpu[i].mmemory = std::min(1000. * cinfo->devspec.cpu[i].gib + .5, 65535.);
                        beacon.cpu[i].mdisk = std::min(cinfo->devspec.cpu[i].storage * 1000. + .5, 65535.);
                        beacon.cpu[i].ctemp = std::min(cinfo->devspec.cpu[i].temp * 100. + .5, 65535.);
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+9+cpucount*sizeof(cpu_beacon));
                }
                break;
            case TypeId::TelemBeacon:
                if (cinfo->devspec.telem.size())
                {
                    telems_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    size_t offset = 0;
                    vector<uint8_t> bytes;
                    for (uint16_t i=0; i<cinfo->devspec.telem.size(); ++i)
                    {
                        bytes.clear();
                        switch (cinfo->devspec.telem[i].vtype)
                        {
                        case JSON_TYPE_UINT8:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vuint8, (uint8_t*)(&cinfo->devspec.telem[i].vuint8+1));
                            }
                            break;
                        case JSON_TYPE_INT8:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vint8, (uint8_t*)(&cinfo->devspec.telem[i].vint8+1));
                            }
                            break;
                        case JSON_TYPE_UINT16:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vuint16, (uint8_t*)(&cinfo->devspec.telem[i].vuint16+1));
                            }
                            break;
                        case JSON_TYPE_INT16:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vint16, (uint8_t*)(&cinfo->devspec.telem[i].vint16+1));
                            }
                            break;
                        case JSON_TYPE_UINT32:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vuint32, (uint8_t*)(&cinfo->devspec.telem[i].vuint32+1));
                            }
                            break;
                        case JSON_TYPE_INT32:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vint32, (uint8_t*)(&cinfo->devspec.telem[i].vint32+1));
                            }
                            break;
                        case JSON_TYPE_FLOAT:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vfloat, (uint8_t*)(&cinfo->devspec.telem[i].vfloat+1));
                            }
                            break;
                        case JSON_TYPE_DOUBLE:
                            {
                                bytes.assign((uint8_t*)&cinfo->devspec.telem[i].vdouble, (uint8_t*)(&cinfo->devspec.telem[i].vdouble+1));
                            }
                            break;
                        case JSON_TYPE_STRING:
                            {
                                // Note, currently supporting only up to string length 255 (not that telem_count is that long anyway)
                                if (cinfo->devspec.telem[i].vstring.size() > 255)
                                {
                                    continue;
                                }
                                bytes.push_back(cinfo->devspec.telem[i].vstring.size());
                                bytes.insert(bytes.end(), cinfo->devspec.telem[i].vstring.begin(), cinfo->devspec.telem[i].vstring.end());
                            }
                            break;
                        } // End switch
                        if (offset + bytes.size() > sizeof(beacon.content))
                        {
                            break;
                        }
                        std::copy_n(bytes.data(), bytes.size(), &beacon.content[offset]);
                        offset += bytes.size();
                    } // End for
                    data.assign((uint8_t*)(&beacon), (uint8_t*)(&beacon)+sizeof(beacon));
                }
                break;
            case TypeId::TsenBeacon:
                if (cinfo->devspec.tsen.size())
                {
                    tsen_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t tsencount = cinfo->devspec.tsen.size() < tsen_count?cinfo->devspec.tsen.size():tsen_count;
                    for (uint16_t i=0; i<tsencount; ++i)
                    {
                        beacon.ctemp[i] = cinfo->devspec.tsen[i].temp * 100. + .5;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+5+tsencount*2);
                }
                break;
            case TypeId::EPSSWCHBeacon:
                if (cinfo->devspec.swch.size())
                {
                    epsswchs_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t swchcount = cinfo->devspec.swch.size() < epsswch_count?cinfo->devspec.swch.size():epsswch_count;
                    for (uint16_t i=0; i<swchcount; ++i)
                    {
                        if (cinfo->devspec.swch[i].state)
                        {
                            beacon.swch[i].mamp = cinfo->devspec.swch[i].amp * 1000. + .5;
                        }
                        else
                        {
                            beacon.swch[i].mamp = 0;
                        }
                        beacon.swch[i].mvolt = cinfo->devspec.swch[i].volt * 1000. + .5;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+5+swchcount*sizeof(epsswch_beacon));
                }
                break;
            case TypeId::EPSBCREGBeacon:
                if (cinfo->devspec.swch.size())
                {
                    epsbcregs_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t pvcount = cinfo->devspec.bcreg.size() < epsbcreg_count?cinfo->devspec.bcreg.size():epsbcreg_count;
                    for (uint16_t i=0; i<pvcount; ++i)
                    {
                        beacon.bcreg[i].mamp = cinfo->devspec.bcreg[i].amp * 1000. + .5;
                        beacon.bcreg[i].mvolt = cinfo->devspec.bcreg[i].volt * 1000. + .5;
                        beacon.bcreg[i].mpptin_mamp = cinfo->devspec.bcreg[i].mpptin_amp * 1000. + .5;
                        beacon.bcreg[i].mpptin_mvolt = cinfo->devspec.bcreg[i].mpptin_volt * 1000. + .5;
                        beacon.bcreg[i].mpptout_mamp = cinfo->devspec.bcreg[i].mpptout_amp * 1000. + .5;
                        beacon.bcreg[i].mpptout_mvolt = cinfo->devspec.bcreg[i].mpptout_volt * 1000. + .5;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+5+pvcount*sizeof(epsbcreg_beacon));
                }
                break;
            case TypeId::EPSBATTBeacon:
                if (cinfo->devspec.swch.size())
                {
                    epsbatts_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t battcount = cinfo->devspec.batt.size() < epsbatt_count?cinfo->devspec.batt.size():epsbatt_count;
                    for (uint16_t i=0; i<battcount; ++i)
                    {
                        beacon.batt[i].mamp = cinfo->devspec.batt[i].amp * 1000. + .5;
                        beacon.batt[i].mvolt = cinfo->devspec.batt[i].volt * 1000. + .5;
                        beacon.batt[i].cpercent = cinfo->devspec.batt[i].percentage * 100. + .5;
                        beacon.batt[i].ctemp = cinfo->devspec.batt[i].temp * 100. + .5;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+5+battcount*sizeof(epsbatt_beacon));
                }
                break;
                //            case TypeId::EPSSUMBeacon:
                //                if (cinfo->devspec.swch.size())
                //                {
                //                    epsbatts_beacon beacon;
                //                    beacon.deci = cinfo->node.deci;
                //                    uint16_t battcount = cinfo->devspec.batt.size() < epsbatt_count?cinfo->devspec.batt.size():epsbatt_count;
                //                    for (uint16_t i=0; i<battcount; ++i)
                //                    {
                //                        beacon.batt[i].mamp = cinfo->devspec.batt[i].amp * 1000. + .5;
                //                        beacon.batt[i].mvolt = cinfo->devspec.batt[i].volt * 1000. + .5;
                //                        beacon.batt[i].cpercent = cinfo->devspec.batt[i].percentage * 100. + .5;
                //                        beacon.batt[i].ctemp = cinfo->devspec.batt[i].temp * 100. + .5;
                //                    }
                //                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+5+battcount*sizeof(epsbatt_beacon));
                //                }
                //                break;
            case TypeId::ADCSMTRBeacon:
                if (cinfo->devspec.mtr.size())
                {
                    adcsmtrs_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t mtrcount = cinfo->devspec.mtr.size() < adcsmtr_count?cinfo->devspec.mtr.size():adcsmtr_count;
                    for (uint16_t i=0; i<mtrcount; ++i)
                    {
                        beacon.mtr[i].mom = cinfo->devspec.mtr[i].mom;
                        beacon.mtr[i].align[0] = cinfo->devspec.mtr[i].align.w;
                        beacon.mtr[i].align[1] = cinfo->devspec.mtr[i].align.d.x;
                        beacon.mtr[i].align[2] = cinfo->devspec.mtr[i].align.d.y;
                        beacon.mtr[i].align[3] = cinfo->devspec.mtr[i].align.d.z;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+offsetof(adcsmtrs_beacon, adcsmtrs_beacon::mtr)+mtrcount*sizeof(adcsmtr_beacon));
                }
                break;
            case TypeId::ADCSGyroBeacon:
                if (cinfo->devspec.gyro.size())
                {
                    adcsgyros_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t gyrocount = cinfo->devspec.gyro.size() < adcsgyro_count?cinfo->devspec.gyro.size():adcsgyro_count;
                    for (uint16_t i=0; i<gyrocount; ++i)
                    {
                        beacon.gyro[i].omega = cinfo->devspec.gyro[i].omega;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+offsetof(adcsgyros_beacon, adcsgyros_beacon::gyro)+gyrocount*sizeof(adcsgyro_beacon));
                }
                break;
            case TypeId::ADCSRWBeacon:
                if (cinfo->devspec.rw.size())
                {
                    adcsrws_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t rwcount = cinfo->devspec.rw.size() < adcsrw_count?cinfo->devspec.rw.size():adcsrw_count;
                    for (uint16_t i=0; i<rwcount; ++i)
                    {
                        beacon.rw[i].amp = cinfo->devspec.rw[i].amp;
                        beacon.rw[i].omg = cinfo->devspec.rw[i].omg;
                        beacon.rw[i].romg = cinfo->devspec.rw[i].romg;
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+offsetof(adcsrws_beacon, adcsrws_beacon::rw)+rwcount*sizeof(adcsrw_beacon));
                }
                break;
            case TypeId::ADCSStateBeacon:
                {
                    adcsstate_beacon beacon;
                    beacon.deci = cinfo->node.deci;

                    beacon.x_eci = cinfo->node.loc.pos.eci.s.col[0];
                    beacon.y_eci = cinfo->node.loc.pos.eci.s.col[1];
                    beacon.z_eci = cinfo->node.loc.pos.eci.s.col[2];

                    beacon.vx_eci = cinfo->node.loc.pos.eci.v.col[0];
                    beacon.vy_eci = cinfo->node.loc.pos.eci.v.col[1];
                    beacon.vz_eci = cinfo->node.loc.pos.eci.v.col[2];

                    beacon.att_icrf_x = cinfo->node.loc.att.icrf.s.d.x;
                    beacon.att_icrf_y = cinfo->node.loc.att.icrf.s.d.y;
                    beacon.att_icrf_z = cinfo->node.loc.att.icrf.s.d.z;
                    beacon.att_icrf_w = cinfo->node.loc.att.icrf.s.w;

                    beacon.att_icrf_omega_x = cinfo->node.loc.att.icrf.v.col[0];
                    beacon.att_icrf_omega_y = cinfo->node.loc.att.icrf.v.col[1];
                    beacon.att_icrf_omega_z = cinfo->node.loc.att.icrf.v.col[2];

                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+sizeof(beacon));
                }
                break;
            case TypeId::RadioBeacon:
                {
                    radios_beacon beacon;
                    beacon.deci = cinfo->node.deci;
                    uint16_t radiocount = 0;
                    if (cinfo->devspec.rxr.size() || cinfo->devspec.txr.size())
                    {
                        //                        uint16_t rxrcount = cinfo->devspec.rxr.size() < rxrtxr_count?cinfo->devspec.rxr.size():rxrtxr_count;
                        for (uint16_t i=0; i<cinfo->devspec.rxr.size(); ++i)
                        {
                            if (radiocount >= rxrtxr_count)
                            {
                                break;
                            }
                            beacon.radio[radiocount].packet_size = cinfo->devspec.rxr[i].pktsize;
                            beacon.radio[radiocount].kbyte_rate = cinfo->devspec.rxr[i].byte_rate / 1000.;
                            beacon.radio[radiocount].uptime = cinfo->devspec.rxr[i].uptime;
                            beacon.radio[radiocount].ctemp = cinfo->devspec.rxr[i].temp / 100.;
                            beacon.radio[radiocount].kpower = cinfo->devspec.rxr[i].powerin / 1000.;
                            beacon.radio[radiocount].bytes = cinfo->devspec.rxr[i].bytesin;
                            beacon.radio[radiocount].lastdeci = decisec(cinfo->devspec.rxr[i].utcin);
                            ++radiocount;
                        }
                        //                        if (radiocount < rxrtxr_count)
                        //                        {
                        //                            uint16_t txr_count = rxrtxr_count - radiocount;
                        //                            uint16_t txrcount = cinfo->devspec.txr.size() < txr_count?cinfo->devspec.txr.size():txr_count;
                        for (uint16_t i=0; i<cinfo->devspec.txr.size(); ++i)
                        {
                            if (radiocount >= rxrtxr_count)
                            {
                                break;
                            }
                            beacon.radio[radiocount].packet_size = cinfo->devspec.txr[i].pktsize;
                            beacon.radio[radiocount].kbyte_rate = cinfo->devspec.txr[i].byte_rate / 1000.;
                            beacon.radio[radiocount].uptime = cinfo->devspec.txr[i].pktsize;
                            beacon.radio[radiocount].ctemp = cinfo->devspec.txr[i].temp / 100.;
                            beacon.radio[radiocount].kpower = cinfo->devspec.txr[i].powerout / 1000.;
                            beacon.radio[radiocount].bytes = cinfo->devspec.txr[i].bytesout;
                            beacon.radio[radiocount].lastdeci = decisec(cinfo->devspec.txr[i].utcout);
                            ++radiocount;
                        }
                        //                        }
                    }
                    data.insert(data.begin(), (uint8_t*)&beacon, (uint8_t*)&beacon+5+radiocount*sizeof(radio_beacon));
                }
                break;
            default:
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return data.size();
        }

        int32_t Beacon::Decode(vector<uint8_t> &data, cosmosstruc *cinfo)
        {
            if (data.size() > 5)
            {
                type = (TypeId) data[0];
                if (TypeString.find(type) != TypeString.end())
                {
                    switch (type)
                    {
                    case TypeId::ADCSStateBeacon:
                        {
                            adcsstate_beacon beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);

                            cinfo->node.loc.pos.eci.utc = mjd;
                            // JIMNOTE: need to update pass here?
                            // EP: 20221126 No, updating should be done elsewhere, depending on source of position
                            cinfo->node.loc.pos.eci.s.col[0] = beacon.x_eci;
                            cinfo->node.loc.pos.eci.s.col[1] = beacon.y_eci;
                            cinfo->node.loc.pos.eci.s.col[2] = beacon.z_eci;

                            cinfo->node.loc.pos.eci.v.col[0] = beacon.vx_eci;
                            cinfo->node.loc.pos.eci.v.col[1] = beacon.vy_eci;
                            cinfo->node.loc.pos.eci.v.col[2] = beacon.vz_eci;

                            cinfo->node.loc.att.icrf.utc = mjd;
                            // JIMNOTE: need to update pass here?
                            cinfo->node.loc.att.icrf.s.d.x = beacon.att_icrf_x;
                            cinfo->node.loc.att.icrf.s.d.y = beacon.att_icrf_y;
                            cinfo->node.loc.att.icrf.s.d.z = beacon.att_icrf_z;
                            cinfo->node.loc.att.icrf.s.w =   beacon.att_icrf_w;

                            cinfo->node.loc.att.icrf.v.col[0] = beacon.att_icrf_omega_x;
                            cinfo->node.loc.att.icrf.v.col[1] = beacon.att_icrf_omega_y;
                            cinfo->node.loc.att.icrf.v.col[2] = beacon.att_icrf_omega_z;
                        }
                        break;
                    case TypeId::ADCSORBITBeaconS:
                        {
                            adcsorbit_beacon beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            cinfo->tle[0].utc = mjd;

                            cinfo->tle[0].i = beacon.i;
                            cinfo->tle[0].e = beacon.e;
                            cinfo->tle[0].raan = beacon.raan;
                            cinfo->tle[0].ap = beacon.ap;
                            cinfo->tle[0].bstar = beacon.bstar;
                            cinfo->tle[0].mm = beacon.mm;
                            cinfo->tle[0].ma = beacon.ma;
                            //cinfo->tle[0].epoch = beacon.epoch;
                        }
                        break;
                    case TypeId::ADCSEXTRABeaconS:
                        {
                            adcsextra_beacon beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            //double mjd = decisec2mjd(beacon.deci);
                            //cinfo->devspec.mag[0].utc = mjd;
                            //cinfo->devspec.mag[0].mag.col[0] = beacon.x;
                            //cinfo->devspec.mag[0].mag.col[1] = beacon.y;
                            //cinfo->devspec.mag[0].mag.col[2] = beacon.z;
                        }
                        break;
                    case TypeId::ADCSMAGBeaconS:
                        {
                            adcsmag_beacon beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            cinfo->devspec.mag[0].utc = mjd;
                            cinfo->devspec.mag[0].mag.col[0] = beacon.magx;
                            cinfo->devspec.mag[0].mag.col[1] = beacon.magy;
                            cinfo->devspec.mag[0].mag.col[2] = beacon.magz;
                        }
                        break;
                    case TypeId::CPU1BeaconS:
                        {
                            cpu1_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            cinfo->devspec.cpu[0].utc = mjd;
                            cinfo->devspec.cpu[0].load = beacon.load;
                            cinfo->devspec.cpu[0].gib = beacon.memory;
                            cinfo->devspec.cpu[0].storage = beacon.cdisk / 100.;
                        }
                        break;
                    case TypeId::CPU2BeaconS:
                        {
                            cpu2_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            cinfo->devspec.cpu[0].utc = mjd;
                            cinfo->devspec.cpu[0].uptime = beacon.uptime;
                            cinfo->devspec.cpu[0].boot_count = beacon.bootcount;
                            cinfo->node.utcstart = unix2utc(beacon.initialdate);
                        }
                        break;
                    case TypeId::TimeBeaconS:
                        {
                            cinfo->node.utc = doublefrom(&data[0]);
                            cinfo->node.utcstart = doublefrom(&data[8]);
                        }
                        break;
                    case TypeId::TsenBeaconS:
                        {
                            tsen_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<(cinfo->devspec.tsen.size()>=6?6:cinfo->devspec.tsen.size()); ++i)
                            {
                                cinfo->devspec.tsen[i].utc = mjd;
                                cinfo->devspec.tsen[i].temp = beacon.ctemp[i] / 100.;
                            }
                        }
                        break;
                    case TypeId::EPSCPUBeaconS:
                        {
                            epscpu_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<cinfo->devspec.cpu.size(); ++i)
                            {
                                if (cinfo->devspec.cpu[i].name.find("eps") != string::npos)
                                {
                                    cinfo->devspec.cpu[i].utc = mjd;
                                    cinfo->devspec.cpu[i].volt = beacon.volt;
                                    cinfo->devspec.cpu[i].amp = beacon.amp;
                                    cinfo->devspec.cpu[i].temp = beacon.temp;
                                    break;
                                }
                            }
                        }
                        break;
                    case TypeId::EPSBCREGBeaconS:
                        {
                            epsbcreg_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            beacon.deci = cinfo->node.deci;
                            for (uint16_t i=0; i<cinfo->devspec.bcreg.size(); ++i)
                            {
                                cinfo->devspec.bcreg[i].utc = mjd;
                                cinfo->devspec.bcreg[i].volt = beacon.volt;
                                cinfo->devspec.bcreg[i].amp = beacon.amp;
                                cinfo->devspec.bcreg[i].temp = beacon.temp;
                            }
                        }
                        break;
                    case TypeId::EPSSWCHBeaconS:
                        {
                            epsswch_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<cinfo->devspec.swch.size(); ++i)
                            {
                                cinfo->devspec.swch[i].utc = mjd;
                                cinfo->devspec.swch[i].volt = beacon.volt;
                                cinfo->devspec.swch[i].amp = beacon.amp;
                                cinfo->devspec.swch[i].temp = beacon.temp;
                            }
                        }
                        break;
                    case TypeId::EPSBATTBeaconS:
                        {
                            epsbatt_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<cinfo->devspec.batt.size(); ++i)
                            {
                                cinfo->devspec.batt[i].utc = mjd;
                                cinfo->devspec.batt[i].volt = beacon.volt;
                                cinfo->devspec.batt[i].amp = beacon.amp;
                                cinfo->devspec.batt[i].temp = beacon.temp;
                            }
                        }
                        break;
                    case TypeId::RadioBeaconS:
                        {
                            radio_beacons beacon;
                            if (data.size() <= sizeof(beacon)) {
                                memcpy(&beacon, data.data(), data.size());
                            } else {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            uint16_t radiocount = 0;
                            for (uint16_t i=0; i<cinfo->devspec.rxr.size(); ++i)
                            {
                                if (radiocount == rxrtxr_count)
                                {
                                    break;
                                }
                                cinfo->devspec.rxr[i].utc = mjd;
                                if (beacon.active & (1 << radiocount++))
                                {
                                    cinfo->devspec.rxr[i].state = 1;
                                }
                                else
                                {
                                    cinfo->devspec.rxr[i].state = 0;
                                }
                                cinfo->devspec.rxr[i].utcin = decisec2mjd(beacon.lastdeciup);
                            }
                            for (uint16_t i=0; i<cinfo->devspec.txr.size(); ++i)
                            {
                                if (radiocount == rxrtxr_count)
                                {
                                    break;
                                }
                                cinfo->devspec.txr[i].utc = mjd;
                                if (beacon.active & (1 << radiocount++))
                                {
                                    cinfo->devspec.txr[i].state = 1;
                                }
                                else
                                {
                                    cinfo->devspec.txr[i].state = 0;
                                }
                                cinfo->devspec.txr[i].utcout = decisec2mjd(beacon.lastdecidown);
                            }
                        }
                        break;
                    case TypeId::CPUBeacon:
                        {
                            cpus_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            cinfo->node.utcstart = unix2utc(beacon.initialdate);
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<(data.size()-9)/COSMOS_SIZEOF(cpu_beacon); ++i)
                            {
                                if (beacon.cpu[i].uptime > 0.)
                                {
                                    cinfo->devspec.cpu[i].utc = mjd;
                                    cinfo->devspec.cpu[i].uptime = beacon.cpu[i].uptime;
                                    cinfo->devspec.cpu[i].boot_count = beacon.cpu[i].bootcount;
                                    cinfo->devspec.cpu[i].load = beacon.cpu[i].mload / 1000.;
                                    cinfo->devspec.cpu[i].gib = beacon.cpu[i].mmemory / 1000.;
                                    cinfo->devspec.cpu[i].storage = beacon.cpu[i].mdisk / 1000.;
                                    cinfo->devspec.cpu[i].temp = beacon.cpu[i].ctemp / 100.;
                                }
                            }
                        }
                        break;
                    case TypeId::TelemBeacon:
                        {
                            telems_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            size_t offset = 0;
                            bool break_for = false;
                            for (size_t i=0; i<cinfo->devspec.telem.size(); ++i)
                            {
                                switch (cinfo->devspec.telem[i].vtype)
                                {
                                case JSON_TYPE_UINT8:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vuint8) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vuint8 = beacon.content[offset];
                                        offset += sizeof(cinfo->devspec.telem[i].vuint8);
                                    }
                                    break;
                                case JSON_TYPE_INT8:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vint8) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vint8 = beacon.content[offset];
                                        offset += sizeof(cinfo->devspec.telem[i].vint8);
                                    }
                                    break;
                                case JSON_TYPE_UINT16:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vuint16) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vuint16 = uint16from(&beacon.content[offset]);
                                        offset += sizeof(cinfo->devspec.telem[i].vuint16);
                                    }
                                    break;
                                case JSON_TYPE_INT16:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vint16) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vint16 = int16from(&beacon.content[offset]);
                                        offset += sizeof(cinfo->devspec.telem[i].vint16);
                                    }
                                    break;
                                case JSON_TYPE_UINT32:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vuint32) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vuint32 = uint32from(&beacon.content[offset]);
                                        offset += sizeof(cinfo->devspec.telem[i].vuint32);
                                    }
                                    break;
                                case JSON_TYPE_INT32:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vint32) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vint32 = int32from(&beacon.content[offset]);
                                        offset += sizeof(cinfo->devspec.telem[i].vint32);
                                    }
                                    break;
                                case JSON_TYPE_FLOAT:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vfloat) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vfloat = floatfrom(&beacon.content[offset]); 
                                        offset += sizeof(cinfo->devspec.telem[i].vfloat);
                                    }
                                    break;
                                case JSON_TYPE_DOUBLE:
                                    {
                                        if (offset + sizeof(cinfo->devspec.telem[i].vdouble) > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vdouble = doublefrom(&beacon.content[offset]);
                                        offset += sizeof(cinfo->devspec.telem[i].vdouble);
                                    }
                                    break;
                                case JSON_TYPE_STRING:
                                    {
                                        // First byte of string is the string size
                                        uint8_t str_len = beacon.content[offset];
                                        if (offset + 1 + str_len > sizeof(beacon.content))
                                        {
                                            break_for = true;
                                            break;
                                        }
                                        cinfo->devspec.telem[i].vstring.clear();
                                        cinfo->devspec.telem[i].vstring.insert(cinfo->devspec.telem[i].vstring.end(), &beacon.content[offset+1], &beacon.content[offset+1+str_len]);
                                        offset += 1 + str_len;
                                    }
                                    break;
                                default:
                                    return GENERAL_ERROR_MISMATCH;
                                }
                                if (break_for)
                                {
                                    break;
                                }
                                cinfo->devspec.telem[i].utc = mjd;
                            }
                        }
                        break;
                    case TypeId::TsenBeacon:
                        {
                            tsen_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<(data.size()-5)/2; ++i)
                            {
                                if (beacon.ctemp[i])
                                {
                                    cinfo->devspec.tsen[i].utc = mjd;
                                    cinfo->devspec.tsen[i].temp = beacon.ctemp[i] / 100.;
                                }
                            }
                        }
                        break;
                    case TypeId::EPSSWCHBeacon:
                        {
                            epsswchs_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<(data.size()-5)/COSMOS_SIZEOF(epsswch_beacon); ++i)
                            {
                                cinfo->devspec.swch[i].utc = mjd;
                                cinfo->devspec.swch[i].volt = beacon.swch[i].mvolt / 1000.;
                                if (beacon.swch[i].mamp)
                                {
                                    cinfo->devspec.swch[i].amp = beacon.swch[i].mamp / 1000.;
                                    cinfo->devspec.swch[i].state = 1;
                                }
                                else
                                {
                                    cinfo->devspec.swch[i].amp = 0.;
                                    cinfo->devspec.swch[i].state = 0;
                                }
                                cinfo->devspec.swch[i].power = cinfo->devspec.swch[i].volt * cinfo->devspec.swch[i].amp;
                            }
                        }
                        break;
                    case TypeId::EPSBCREGBeacon:
                        {
                            epsbcregs_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<(data.size()-5)/COSMOS_SIZEOF(epsbcreg_beacon); ++i)
                            {
                                cinfo->devspec.bcreg[i].utc = mjd;
                                cinfo->devspec.bcreg[i].volt = beacon.bcreg[i].mvolt / 1000.;
                                cinfo->devspec.bcreg[i].amp = beacon.bcreg[i].mamp / 1000.;
                                cinfo->devspec.bcreg[i].power = (beacon.bcreg[i].mvolt / 1000.) * (beacon.bcreg[i].mamp / 1000.);
                                cinfo->devspec.bcreg[i].mpptin_volt = beacon.bcreg[i].mpptin_mvolt / 1000.;
                                cinfo->devspec.bcreg[i].mpptin_amp = beacon.bcreg[i].mpptin_mamp / 1000.;
                                cinfo->devspec.bcreg[i].mpptout_volt = beacon.bcreg[i].mpptout_mvolt / 1000.;
                                cinfo->devspec.bcreg[i].mpptout_amp = beacon.bcreg[i].mpptout_mamp / 1000.;
                            }
                        }
                        break;
                    case TypeId::EPSBATTBeacon:
                        {
                            epsbatts_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<(data.size()-5)/COSMOS_SIZEOF(epsbatt_beacon); ++i)
                            {
                                cinfo->devspec.batt[i].utc = mjd;
                                cinfo->devspec.batt[i].volt = beacon.batt[i].mvolt / 1000.;
                                cinfo->devspec.batt[i].amp = beacon.batt[i].mamp / 1000.;
                                cinfo->devspec.batt[i].power = cinfo->devspec.batt[i].volt * cinfo->devspec.batt[i].amp;
                                cinfo->devspec.batt[i].temp = beacon.batt[i].ctemp / 100.;
                                cinfo->devspec.batt[i].percentage = beacon.batt[i].cpercent / 100.;
                            }
                        }
                        break;
                    case TypeId::ADCSMTRBeacon:
                        {
                            adcsmtrs_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<cinfo->devspec.mtr.size(); ++i)
                            {
                                if (i >= adcsmtr_count)
                                {
                                    break;
                                }
                                cinfo->devspec.mtr[i].utc = mjd;
                                cinfo->devspec.mtr[i].mom = beacon.mtr[i].mom;
                                cinfo->devspec.mtr[i].align.w = beacon.mtr[i].align[0];
                                cinfo->devspec.mtr[i].align.d.x = beacon.mtr[i].align[1];
                                cinfo->devspec.mtr[i].align.d.y = beacon.mtr[i].align[2];
                                cinfo->devspec.mtr[i].align.d.z = beacon.mtr[i].align[3];
                            }
                        }
                        break;
                    case TypeId::ADCSGyroBeacon:
                        {
                            adcsgyros_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<cinfo->devspec.gyro.size(); ++i)
                            {
                                if (i >= adcsgyro_count)
                                {
                                    break;
                                }
                                cinfo->devspec.gyro[i].utc = mjd;
                                cinfo->devspec.gyro[i].omega = beacon.gyro[i].omega;
                            }
                        }
                        break;
                    case TypeId::ADCSRWBeacon:
                        {
                            adcsrws_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            double mjd = decisec2mjd(beacon.deci);
                            for (uint16_t i=0; i<cinfo->devspec.rw.size(); ++i)
                            {
                                if (i >= adcsrw_count)
                                {
                                    break;
                                }
                                cinfo->devspec.rw[i].utc = mjd;
                                cinfo->devspec.rw[i].amp = beacon.rw[i].amp;
                                cinfo->devspec.rw[i].omg = beacon.rw[i].omg;
                                cinfo->devspec.rw[i].romg = beacon.rw[i].romg;
                            }
                        }
                        break;
                    case TypeId::RadioBeacon:
                        {
                            radios_beacon beacon;
                            if (data.size() <= sizeof(beacon))
                            {
                                memcpy(&beacon, data.data(), data.size());
                            }
                            else
                            {
                                return GENERAL_ERROR_BAD_SIZE;
                            }
                            //                            double mjd = decisec2mjd(beacon.deci);
                            uint16_t radiocount = 0;
                            //                            uint16_t rxrcount = cinfo->devspec.rxr.size() < rxrtxr_count?cinfo->devspec.rxr.size():rxrtxr_count;
                            for (uint16_t i=0; i<cinfo->devspec.rxr.size(); ++i)
                            {
                                if (radiocount >= rxrtxr_count)
                                {
                                    break;
                                }
                                cinfo->devspec.rxr[i].pktsize = beacon.radio[radiocount].packet_size;
                                cinfo->devspec.rxr[i].byte_rate = beacon.radio[radiocount].kbyte_rate * 1000.;
                                cinfo->devspec.rxr[i].uptime = beacon.radio[radiocount].uptime;
                                cinfo->devspec.rxr[i].temp = beacon.radio[radiocount].ctemp * 100.;
                                cinfo->devspec.rxr[i].powerin = beacon.radio[radiocount].kpower * 1000.;
                                cinfo->devspec.rxr[i].bytesin = beacon.radio[radiocount].bytes;
                                cinfo->devspec.rxr[i].utcin = decisec2mjd(beacon.radio[radiocount].lastdeci);
                                ++radiocount;
                            }
                            for (uint16_t i=0; i<cinfo->devspec.txr.size(); ++i)
                            {
                                if (radiocount >= rxrtxr_count)
                                {
                                    break;
                                }
                                cinfo->devspec.txr[i].pktsize = beacon.radio[radiocount].packet_size;
                                cinfo->devspec.txr[i].byte_rate = beacon.radio[radiocount].kbyte_rate * 1000.;
                                cinfo->devspec.txr[i].uptime = beacon.radio[radiocount].uptime;
                                cinfo->devspec.txr[i].temp = beacon.radio[radiocount].ctemp * 100.;
                                cinfo->devspec.txr[i].powerout = beacon.radio[radiocount].kpower * 1000.;
                                cinfo->devspec.txr[i].bytesout = beacon.radio[radiocount].bytes;
                                cinfo->devspec.txr[i].utcout = decisec2mjd(beacon.radio[radiocount].lastdeci);
                                ++radiocount;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
                return 0;
            }
            else
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
        }

        int32_t Beacon::EncodeJson(TypeId type, cosmosstruc *cinfo, vector<uint8_t>& Contents)
        {
            string scontents;
            int32_t iretn = EncodeJson(type, cinfo, scontents);
            Contents.clear();
            Contents.insert(Contents.begin(), scontents.begin(), scontents.end());
            return iretn;
        }

        int32_t Beacon::EncodeJson(TypeId type, cosmosstruc *cinfo, string& Contents)
        {
            Contents.clear();
            switch (type)
            {
            case TypeId::ADCSStateBeacon:
                {
                    // JIMNOTE: should not this be mjd to deci?
                    json_out(Contents, "node_loc_pos_eci_utc", cinfo);

                    json_out(Contents, "node_loc_pos_eci_s", cinfo);
                    json_out(Contents, "node_loc_pos_eci_v", cinfo);

                    json_out(Contents, "node_loc_att_icrf_s", cinfo);
                    json_out(Contents, "node_loc_att_icrf_v", cinfo);
                }
                break;
            case TypeId::ADCSORBITBeaconS:
                {
                    json_out_1d(Contents, "tle_utc", 0, cinfo);
                    json_out_1d(Contents, "tle_i", 0, cinfo);
                    json_out_1d(Contents, "tle_e", 0, cinfo);
                    json_out_1d(Contents, "tle_raan", 0, cinfo);
                    json_out_1d(Contents, "tle_ap", 0, cinfo);
                    json_out_1d(Contents, "tle_bstar", 0, cinfo);
                    json_out_1d(Contents, "tle_mm", 0, cinfo);
                    json_out_1d(Contents, "tle_ma", 0, cinfo);
                    //json_out_1d(Contents, "tle_epoch", 0, cinfo);
                }
                break;
            case TypeId::ADCSEXTRABeaconS:
                {
                    //json_out_1d(Contents, "device_mag_utc", 0, cinfo);
                    //json_out_1d(Contents, "device_mag_mag", 0, cinfo);
                }
                break;
            case TypeId::ADCSMAGBeaconS:
                {
                    // does this need mag utc?
                    // JIMNOTE double check the JSON output (vector or not?)
                    json_out_1d(Contents, "device_mag_utc", 0, cinfo);
                    json_out_1d(Contents, "device_mag_mag_x", 0, cinfo);
                    json_out_1d(Contents, "device_mag_mag_y", 0, cinfo);
                    json_out_1d(Contents, "device_mag_mag_z", 0, cinfo);
                }
                break;
            case TypeId::CPU1BeaconS:
                {
                    json_out_1d(Contents, "device_cpu_utc", 0, cinfo);
                    json_out_1d(Contents, "device_cpu_load", 0, cinfo);
                    json_out_1d(Contents, "device_cpu_gib", 0, cinfo);
                    json_out_1d(Contents, "device_disk_gib", 0, cinfo);
                }
                break;
            case TypeId::CPU2BeaconS:
                {
                    json_out_1d(Contents, "device_cpu_utc", 0, cinfo);
                    json_out_1d(Contents, "device_cpu_uptime", 0, cinfo);
                    json_out_1d(Contents, "device_cpu_boot_count", 0, cinfo);
                    json_out(Contents, "node_utcstart", cinfo);
                }
                break;
            case TypeId::TimeBeaconS:
                {
                    json_out(Contents, "node_utc", cinfo);
                    json_out(Contents, "node_utcstart", cinfo);
                }
                break;
            case TypeId::TsenBeaconS:
                {
                    for (uint16_t i=0; i<std::min(static_cast<size_t>(6), cinfo->devspec.tsen.size()); ++i)
                    {
                        json_out_1d(Contents, "device_tsen_utc", i, cinfo);
                        json_out_1d(Contents, "device_tsen_temp", i, cinfo);
                    }
                }
                break;
            case TypeId::EPSCPUBeaconS:
                {
                    for (uint16_t i=0; i<cinfo->devspec.cpu.size(); ++i)
                    {
                        if (cinfo->devspec.cpu[i].name.find("eps") != string::npos)
                        {
                            json_out_1d(Contents, "device_cpu_utc", i, cinfo);
                            json_out_1d(Contents, "device_cpu_volt", i, cinfo);
                            json_out_1d(Contents, "device_cpu_amp", i, cinfo);
                            json_out_1d(Contents, "device_cpu_temp", i, cinfo);
                            break;
                        }
                    }
                }
                break;
            case TypeId::EPSBCREGBeaconS:
                {
                    for (uint16_t i=0; i<cinfo->devspec.bcreg.size(); ++i)
                    {
                        json_out_1d(Contents, "device_bcreg_utc", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_volt", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_amp", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_temp", i, cinfo);
                    }
                }
                break;
            case TypeId::EPSSWCHBeaconS:
                {
                    for (uint16_t i=0; i<cinfo->devspec.swch.size(); ++i)
                    {
                        json_out_1d(Contents, "device_swch_utc", i, cinfo);
                        json_out_1d(Contents, "device_swch_volt", i, cinfo);
                        json_out_1d(Contents, "device_swch_amp", i, cinfo);
                        json_out_1d(Contents, "device_swch_temp", i, cinfo);
                    }
                }
                break;
            case TypeId::EPSBATTBeaconS:
                {
                    for (uint16_t i=0; i<cinfo->devspec.batt.size(); ++i)
                    {
                        json_out_1d(Contents, "device_batt_utc", i, cinfo);
                        json_out_1d(Contents, "device_batt_volt", i, cinfo);
                        json_out_1d(Contents, "device_batt_amp", i, cinfo);
                        json_out_1d(Contents, "device_batt_temp", i, cinfo);
                    }
                }
                break;
            case TypeId::CPUBeacon:
                {
                    //json_out(Contents, "node_utc", cinfo);
                    json_out(Contents, "node_utcstart", cinfo);
                    for (uint16_t i=0; i<cinfo->devspec.cpu.size(); ++i)
                    {
                        json_out_1d(Contents, "device_cpu_utc", i, cinfo);
                        json_out_1d(Contents, "device_cpu_uptime", i, cinfo);
                        json_out_1d(Contents, "device_cpu_boot_count", i, cinfo);
                        json_out_1d(Contents, "device_cpu_load", i, cinfo);
                        json_out_1d(Contents, "device_cpu_gib", i, cinfo);
                        json_out_1d(Contents, "device_cpu_maxgib", i, cinfo);
                        json_out_1d(Contents, "device_cpu_storage", i, cinfo);
                        json_out_1d(Contents, "device_cpu_temp", i, cinfo);
                    }
                }
                break;
            case TypeId::TelemBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.telem.size(); ++i)
                    {
                        json_out_1d(Contents, "device_telem_utc", i, cinfo);
                        json_out_1d(Contents, "device_telem_name", i, cinfo);
                        switch (cinfo->devspec.telem[i].vtype)
                        {
                        case JSON_TYPE_UINT8:
                            {
                                json_out_1d(Contents, "device_telem_vuint8", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_INT8:
                            {
                                json_out_1d(Contents, "device_telem_vint8", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_UINT16:
                            {
                                json_out_1d(Contents, "device_telem_vuint16", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_INT16:
                            {
                                json_out_1d(Contents, "device_telem_vint16", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_UINT32:
                            {
                                json_out_1d(Contents, "device_telem_vuint32", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_INT32:
                            {
                                json_out_1d(Contents, "device_telem_vint32", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_FLOAT:
                            {
                                json_out_1d(Contents, "device_telem_vfloat", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_DOUBLE:
                            {
                                json_out_1d(Contents, "device_telem_vdouble", i, cinfo);
                            }
                            break;
                        case JSON_TYPE_STRING:
                            {
                                json_out_1d(Contents, "device_telem_vstring", i, cinfo);
                            }
                            break;
                        default:
                            return GENERAL_ERROR_MISMATCH;
                        }
                    }
                }
                break;
            case TypeId::TsenBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.tsen.size(); ++i)
                    {
                        json_out_1d(Contents, "device_tsen_utc", i, cinfo);
                        json_out_1d(Contents, "device_tsen_temp", i, cinfo);
                    }
                }
                break;
            case TypeId::EPSSWCHBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.swch.size(); ++i)
                    {
                        json_out_1d(Contents, "device_swch_utc", i, cinfo);
                        json_out_1d(Contents, "device_swch_amp", i, cinfo);
                        json_out_1d(Contents, "device_swch_volt", i, cinfo);
                        json_out_1d(Contents, "device_swch_power", i, cinfo);
                    }
                }
                break;
            case TypeId::EPSBCREGBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.bcreg.size(); ++i)
                    {
                        json_out_1d(Contents, "device_bcreg_utc", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_amp", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_volt", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_power", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_mpptin_amp", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_mpptin_volt", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_mpptout_amp", i, cinfo);
                        json_out_1d(Contents, "device_bcreg_mpptout_volt", i, cinfo);
                    }
                }
                break;
            case TypeId::EPSBATTBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.batt.size(); ++i)
                    {
                        json_out_1d(Contents, "device_batt_utc", i, cinfo);
                        json_out_1d(Contents, "device_batt_amp", i, cinfo);
                        json_out_1d(Contents, "device_batt_volt", i, cinfo);
                        json_out_1d(Contents, "device_batt_power", i, cinfo);
                        json_out_1d(Contents, "device_batt_temp", i, cinfo);
                        json_out_1d(Contents, "device_batt_percentage", i, cinfo);
                    }
                }
                break;
            case TypeId::ADCSMTRBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.mtr.size(); ++i)
                    {
                        json_out_1d(Contents, "device_mtr_utc", i, cinfo);
                        json_out_1d(Contents, "device_mtr_name", i, cinfo);
                        json_out_1d(Contents, "device_mtr_mom", i, cinfo);
                        json_out_1d(Contents, "device_mtr_align", i, cinfo);
                    }
                }
                break;
            case TypeId::ADCSGyroBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.gyro.size(); ++i)
                    {
                        json_out_1d(Contents, "device_gyro_utc", i, cinfo);
                        json_out_1d(Contents, "device_gyro_name", i, cinfo);
                        json_out_1d(Contents, "device_gyro_omega", i, cinfo);
                    }
                }
                break;
            case TypeId::ADCSRWBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.rw.size(); ++i)
                    {
                        json_out_1d(Contents, "device_rw_utc", i, cinfo);
                        json_out_1d(Contents, "device_rw_name", i, cinfo);
                        json_out_1d(Contents, "device_rw_amp", i, cinfo);
                        json_out_1d(Contents, "device_rw_omg", i, cinfo);
                        json_out_1d(Contents, "device_rw_romg", i, cinfo);
                    }
                }
                break;
            case TypeId::RadioBeacon:
                {
                    for (uint16_t i=0; i<cinfo->devspec.rxr.size(); ++i)
                    {
                        json_out_1d(Contents, "device_rxr_utc", i, cinfo);
                        json_out_1d(Contents, "device_rxr_amp", i, cinfo);
                        json_out_1d(Contents, "device_rxr_volt", i, cinfo);
                        json_out_1d(Contents, "device_rxr_power", i, cinfo);
                        json_out_1d(Contents, "device_rxr_temp", i, cinfo);
                        json_out_1d(Contents, "device_rxr_pktsize", i, cinfo);
                        json_out_1d(Contents, "device_rxr_byte_rate", i, cinfo);
                        json_out_1d(Contents, "device_rxr_uptime", i, cinfo);
                        json_out_1d(Contents, "device_rxr_bytesin", i, cinfo);
                        json_out_1d(Contents, "device_rxr_powerin", i, cinfo);
                        json_out_1d(Contents, "device_rxr_utcin", i, cinfo);
                    }
                    for (uint16_t i=0; i<cinfo->devspec.txr.size(); ++i)
                    {
                        json_out_1d(Contents, "device_txr_amp", i, cinfo);
                        json_out_1d(Contents, "device_txr_volt", i, cinfo);
                        json_out_1d(Contents, "device_txr_power", i, cinfo);
                        json_out_1d(Contents, "device_txr_temp", i, cinfo);
                        json_out_1d(Contents, "device_txr_pktsize", i, cinfo);
                        json_out_1d(Contents, "device_txr_byte_rate", i, cinfo);
                        json_out_1d(Contents, "device_txr_uptime", i, cinfo);
                        json_out_1d(Contents, "device_txr_bytesout", i, cinfo);
                        json_out_1d(Contents, "device_txr_powerout", i, cinfo);
                        json_out_1d(Contents, "device_txr_utcout", i, cinfo);
                    }
                }
                break;
            default:
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
