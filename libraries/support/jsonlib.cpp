/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence->
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

/*! \file jsonlib.cpp
    \brief JSON support source file
*/

#include "support/cosmos-errno.h"
#include "support/jsonlib.h"
#include "support/convertlib.h"
#include "support/timelib.h"
#include "support/ephemlib.h"
#include "support/jsonclass.h"
#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"

#include <sys/stat.h>
#include <iostream>
#include <limits>
#include <fstream>
#include <malloc.h>

static vector <string> device_type_string;

//! \ingroup jsonlib
//! \defgroup jsonlib_namespace JSON Name Space
//! @{
//! A hierarchical set of variable names meant to describe everything in COSMOS. Each name
//! maps to a variable of a type specified in \ref jsonlib_type. Names are restricted to
//! alphabetic characters, and can be no longer than ::COSMOS_MAX_NAME. Once mapped through use
//! of ::json_setup_node, these names will be tied to elements of the ::cosmosstruc.

//! @}


/*! \ingroup jsonlib
* \defgroup jsonlib_packet JSON Packet
* @{
* A JSON object constructed only of members present in the \ref jsonlib_namespace. Each \ref jsonlib_type in the JSON
* packet is a separate JSON Object to aid in parsing.
*/

//! @}

//! \addtogroup jsonlib_functions JSON functions
//! @{



// function to initialize the vector<vector<unitstruc>> unit
void json_init_unit(cosmosstruc* cinfo)	{

    // Create JSON Map unit table
    cinfo->unit.resize(JSON_UNIT_COUNT);
    for (uint16_t i=0; i<JSON_UNIT_COUNT; ++i)
    {
        // SI Units
//        cinfo->unit.push_back(vector<unitstruc>());
        unitstruc tunit;

        switch (i)
        {
        case JSON_UNIT_NONE:
            tunit.name = "";
            break;
        case JSON_UNIT_ACCELERATION:
            tunit.name = "m/s2";
            break;
        case JSON_UNIT_ANGLE:
            tunit.name = "rad";
            break;
        case JSON_UNIT_ANGULAR_RATE:
            tunit.name = "rad/s";
            break;
        case JSON_UNIT_AREA:
            tunit.name = "m2";
            break;
        case JSON_UNIT_BYTES:
            tunit.name = "GiB";
            break;
        case JSON_UNIT_CAPACITANCE:
            tunit.name = "F";
            break;
        case JSON_UNIT_CHARGE:
            tunit.name = "C";
            break;
        case JSON_UNIT_CURRENT:
            tunit.name = "A";
            break;
        case JSON_UNIT_DATE:
            tunit.name = "MJD";
            break;
        case JSON_UNIT_DENSITY:
            tunit.name = "kg/m3";
            break;
        case JSON_UNIT_ENERGY:
            tunit.name = "j";
            break;
        case JSON_UNIT_FORCE:
            tunit.name = "N";
            break;
        case JSON_UNIT_FRACTION:
            tunit.name = "";
            break;
        case JSON_UNIT_FREQUENCY:
            tunit.name = "Hz";
            break;
        case JSON_UNIT_INTENSITY:
            tunit.name = "Ca";
            break;
        case JSON_UNIT_ISP:
            tunit.name = "s";
            break;
        case JSON_UNIT_LENGTH:
            tunit.name = "m";
            break;
        case JSON_UNIT_LUMINANCE:
            tunit.name = "Cd/m2";
            break;
        case JSON_UNIT_MAGFIELD:
            tunit.name = "A/m";
            break;
        case JSON_UNIT_MAGFLUX:
            tunit.name = "Wb";
            break;
        case JSON_UNIT_MAGDENSITY:
            tunit.name = "T";
            break;
        case JSON_UNIT_MAGMOMENT:
            tunit.name = "Am2";
            break;
        case JSON_UNIT_MASS:
            tunit.name = "kg";
            break;
        case JSON_UNIT_MOI:
            tunit.name = "kgm2";
            break;
        case JSON_UNIT_POWER:
            tunit.name = "watt";
            break;
        case JSON_UNIT_PRESSURE:
            tunit.name = "Pa";
            break;
        case JSON_UNIT_RESISTANCE:
            tunit.name = "ohm";
            break;
        case JSON_UNIT_SOLIDANGLE:
            tunit.name = "sr";
            break;
        case JSON_UNIT_SPEED:
            tunit.name = "m/s";
            break;
        case JSON_UNIT_TEMPERATURE:
            tunit.name = "K";
            break;
        case JSON_UNIT_TIME:
            tunit.name = "s";
            break;
        case JSON_UNIT_TORQUE:
            tunit.name = "Nm";
            break;
        case JSON_UNIT_VOLTAGE:
            tunit.name = "V";
            break;
        case JSON_UNIT_VOLUME:
            tunit.name = "m3";
            break;
        }
        cinfo->unit[i].push_back(tunit);

        // Alternate Units
        switch (i)
        {
        case JSON_UNIT_NONE:
            break;
        case JSON_UNIT_ACCELERATION:
            tunit.name = "g";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/9.80665f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_ANGLE:
            tunit.name = "deg";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 =  static_cast<float>(RTOD);
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_ANGULAR_RATE:
            tunit.name = "deg/s";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 =  static_cast<float>(RTOD);
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_AREA:
            tunit.name = "ft2";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 10.76391f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_BYTES:
            tunit.name = "KiB";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/1024.f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_CAPACITANCE:
            break;
        case JSON_UNIT_CHARGE:
            tunit.name = "Ahr";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 3600.f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_CURRENT:
            break;
        case JSON_UNIT_DATE:
            tunit.name = "min";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1440.f;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "hr";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 24.f;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "day";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_DENSITY:
            break;
        case JSON_UNIT_ENERGY:
            tunit.name = "erg";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e7;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_FORCE:
            tunit.name = "dyn";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e5;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "lb";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = .22481f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_FRACTION:
            tunit.name = "%";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 100.f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_FREQUENCY:
            break;
        case JSON_UNIT_INTENSITY:
            break;
        case JSON_UNIT_ISP:
            break;
        case JSON_UNIT_LENGTH:
            tunit.name = "km";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e-3f;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "cm";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e2;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "ft";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 3.280833f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_LUMINANCE:
            break;
        case JSON_UNIT_MAGFIELD:
            break;
        case JSON_UNIT_MAGFLUX:
            break;
        case JSON_UNIT_MAGDENSITY:
            tunit.name = "uT";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e6;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "nT";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e9;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_MAGMOMENT:
            break;
        case JSON_UNIT_MASS:
            tunit.name = "g";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e3;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_PRESSURE:
            break;
        case JSON_UNIT_RESISTANCE:
            break;
        case JSON_UNIT_SOLIDANGLE:
            break;
        case JSON_UNIT_SPEED:
            tunit.name = "km/s";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e-3f;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "cm/s";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e2;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_TEMPERATURE:
            tunit.name = "\260C";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p0 = -273.15f;
            tunit.p1 = 1.;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_TIME:
            tunit.name = "min";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/60.f;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "hr";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/3600.f;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "day";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/86400.f;
            cinfo->unit[i].push_back(tunit);
            tunit.name = "MJD";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/86400.f;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_VOLTAGE:
            break;
        case JSON_UNIT_VOLUME:
            break;
        }
    }
}

void json_init_device_type_string()	{
    // Create device component names
    device_type_string.clear();
    device_type_string.resize(DeviceType::COUNT);
    device_type_string[DeviceType::ANT] = "ant";
    device_type_string[DeviceType::BATT] = "batt";
    device_type_string[DeviceType::BCREG] = "bcreg";
    device_type_string[DeviceType::BUS] = "bus";
    device_type_string[DeviceType::CAM] = "cam";
    device_type_string[DeviceType::CPU] = "cpu";
    device_type_string[DeviceType::DISK] = "disk";
    device_type_string[DeviceType::GPS] = "gps";
    device_type_string[DeviceType::GYRO] = "gyro";
    device_type_string[DeviceType::HTR] = "htr";
    device_type_string[DeviceType::IMU] = "imu";
    device_type_string[DeviceType::MAG] = "mag";
    device_type_string[DeviceType::MCC] = "mcc";
    device_type_string[DeviceType::MOTR] = "motr";
    device_type_string[DeviceType::MTR] = "mtr";
    device_type_string[DeviceType::PLOAD] = "pload";
    device_type_string[DeviceType::PROP] = "prop";
    device_type_string[DeviceType::PSEN] = "psen";
    device_type_string[DeviceType::PVSTRG] = "pvstrg";
    device_type_string[DeviceType::ROT] = "rot";
    device_type_string[DeviceType::RW] = "rw";
    device_type_string[DeviceType::RXR] = "rxr";
    device_type_string[DeviceType::SSEN] = "ssen";
    device_type_string[DeviceType::STT] = "stt";
    device_type_string[DeviceType::SUCHI] = "suchi";
    device_type_string[DeviceType::SWCH] = "swch";
    device_type_string[DeviceType::TCU] = "tcu";
    device_type_string[DeviceType::TCV] = "tcv";
    device_type_string[DeviceType::TELEM] = "telem";
    device_type_string[DeviceType::THST] = "thst";
    device_type_string[DeviceType::TNC] = "tnc";
    device_type_string[DeviceType::TSEN] = "tsen";
    device_type_string[DeviceType::TXR] = "txr";
    device_type_string[DeviceType::XYZSEN] = "xyzsen";
    return;
}

void json_init_node(cosmosstruc* cinfo)	{
    // JIMNOTE: how hard to change to string?
    cinfo->node.name.clear();
}

void json_init_reserve(cosmosstruc* cinfo) {
    // JIMNOTE: change all to reserve
    // reserve/resize fixed-sized vectors
    cinfo->unit.clear();
//    cinfo->unit.reserve(JSON_UNIT_COUNT);

    cinfo->jmap.resize(JSON_MAX_HASH);
    cinfo->ujmap.clear();
    cinfo->emap.resize(JSON_MAX_HASH);

//    cinfo->node.name.reserve(COSMOS_MAX_NAME+1);
//    cinfo->node.agent.reserve(COSMOS_MAX_NAME+1);
//    cinfo->node.lastevent.reserve(COSMOS_MAX_NAME+1);

    // TODO: enforce this maximum for all vert/tri.push_back calls
    cinfo->node.phys.vertices.clear();
//    cinfo->node.phys.vertices.reserve(MAX_NUMBER_OF_VERTICES);
    cinfo->node.phys.triangles.clear();
//    cinfo->node.phys.triangles.reserve(MAX_NUMBER_OF_TRIANGLES);

    cinfo->vertexs.clear();
//    cinfo->vertexs.reserve(MAX_NUMBER_OF_VERTEXS);
    cinfo->normals.clear();
//    cinfo->normals.reserve(MAX_NUMBER_OF_NORMALS);

//    cinfo->user.reserve(MAX_NUMBER_OF_USERS);
//    cinfo->user.resize(MAX_NUMBER_OF_USERS);

//    cinfo->agent.reserve(MAX_NUMBER_OF_AGENTS);
    cinfo->agent.resize(1);

    cinfo->equation.clear();
//    cinfo->equation.reserve(MAX_NUMBER_OF_EQUATIONS);
    cinfo->pieces.clear();
//    cinfo->pieces.reserve(MAX_NUMBER_OF_PIECES);
    cinfo->device.clear();
//    cinfo->device.reserve(MAX_NUMBER_OF_DEVICES);
    cinfo->target.clear();
//    cinfo->target.reserve(MAX_NUMBER_OF_TARGETS);

    cinfo->sim_states.clear();
//    cinfo->sim_states.reserve(MAX_NUMBER_OF_SATELLITES);

    // init sim_states
    sim_state s;
    s.node_name = "daughter_01";
    //s.agent_name = "allison";
    s.agent_name = "daughter";
    cinfo->sim_states.push_back(s);
    s.node_name = "daughter_02";
    //s.agent_name = "becky";
    s.agent_name = "daughter";
    cinfo->sim_states.push_back(s);
    s.node_name = "daughter_03";
    //s.agent_name = "cecilia";
    s.agent_name = "daughter";
    cinfo->sim_states.push_back(s);
    s.node_name = "daughter_04";
    //s.agent_name = "delilah";
    s.agent_name = "daughter";
    cinfo->sim_states.push_back(s);
    s.node_name = "mothership";
    //s.agent_name = "mother";
    s.agent_name = "mother";
    cinfo->sim_states.push_back(s);

//    cinfo->event.reserve(MAX_NUMBER_OF_EVENTS);
    cinfo->event.resize(1);

    cinfo->port.clear();
//    cinfo->port.reserve(MAX_NUMBER_OF_PORTS);

    cinfo->tle.clear();
//    cinfo->tle.reserve(MAX_NUMBER_OF_TLES);
    return;
}

//! Initialize JSON pointer map
/*! Create a ::cosmosstruc and use it to assign storage for each of the groups and entries
 * for each of the non Node based elements to the JSON Name Map.
    \return Pointer to new ::cosmosstruc or nullptr.
*/

// this should completely reserve all memory used up to MAX_NUMBER_OF_######
// add_default_names at the very end
cosmosstruc* json_init(cosmosstruc *cinfo)
{
    json_init_device_type_string();
    // would be nice to have unit test for these three guys
    //SCOTTNOTE: reserve capacity for all vectors in these strucs
//    cinfo->node.name = "";
//    cinfo->node.agent = "";
//    cinfo->node.lastevent = "";
    //	cinfo->node = nodestruc();
    //    cinfo->node.phys = physicsstruc();
    //	cinfo->devspec = devspecstruc();

    if (cinfo == nullptr)
    {
        return nullptr;
    }

    json_init_reserve(cinfo);
    json_init_unit(cinfo);
    json_init_node(cinfo);

    cinfo->timestamp = currentmjd();

    // NS1 check
//    int32_t iretn = json_mapbaseentries(cinfo);
    int32_t iretn = json_updatecosmosstruc(cinfo);
    if (iretn < 0) {
        delete cinfo;
        cinfo = nullptr;
    }

    // NS2
//    cinfo->add_default_names();



    return cinfo;
}

cosmosstruc* json_init()
{
    cosmosstruc* cinfo = nullptr;
    if ((cinfo = new cosmosstruc()) == nullptr)
    {
        return nullptr;
    }

    return json_init(cinfo);
}

//! Remove JSON pointer map
/*! Frees up all space assigned to JSON pointer map. Includes any space allocated
 * through ::json_addentry.
*/
void json_destroy(cosmosstruc* cinfo)
{
    if (cinfo == nullptr) { return; }
    delete cinfo;
    cinfo = nullptr;
}

//! Calculate JSON HASH
/*! Simple hash function (TCPL Section 6.6 Table Lookup)
    \param hstring String to calculate the hash for.
    \return The hash, as an unsigned 16 bit number.
*/

uint16_t json_hash(string hstring)
{
    uint16_t hashval;

    hashval = 0;
    for (uint8_t val : hstring)
    {
        hashval *= 31;
        hashval += val;
    }
    return (hashval % JSON_MAX_HASH);
}

int32_t json_create_node(cosmosstruc *cinfo, string &node_name, uint16_t node_type)
{
    if (node_name.empty())
    {
        DeviceCpu deviceCpu;
        node_name = deviceCpu.getHostName();
    }

    if (!get_nodedir(node_name, true).empty())
    {
        //        strncpy(cinfo->node.name, node_name.c_str(), COSMOS_MAX_NAME);
        cinfo->node.name = node_name;
        cinfo->node.type = node_type;

//        int32_t iretn = json_dump_node(cinfo);
//        return iretn;
        return 0;
    }
    else
    {
        return DATA_ERROR_NODES_FOLDER;
    }
}

int32_t json_create_cpu(string &node_name)
{
    cosmosstruc *cinfo = nullptr;
    DeviceCpu deviceCpu;
    int32_t iretn = 0;

    if (node_name.empty())
    {
        node_name = deviceCpu.getHostName();
    }

    if (get_nodedir(node_name).empty())
    {
        if (get_nodedir(node_name, true).empty())
        {
            return 1;
        }

        cinfo = json_init();
        //        strncpy(cinfo->node.name, node_name.c_str(), COSMOS_MAX_NAME);
        cinfo->node.name = node_name;
        iretn = json_createpiece(cinfo, node_name + "_cpu", DeviceType::CPU);
        if (iretn >= 0)
        {
            uint16_t cpu_cidx = cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            uint16_t cpu_didx = cinfo->device[cpu_cidx]->didx;
            cinfo->devspec.cpu[cpu_didx].load = static_cast <float>(deviceCpu.getLoad());
            cinfo->devspec.cpu[cpu_didx].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
            cinfo->devspec.cpu[cpu_didx].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
            cinfo->devspec.cpu[cpu_didx].maxload = deviceCpu.getCpuCount();
        }

        DeviceDisk deviceDisk;
        vector <DeviceDisk::info> dinfo = deviceDisk.getInfo();
        for (uint16_t i=0; i<dinfo.size(); ++i)
        {
            string name = "disk_" + to_unsigned(i, 2, true);
            iretn = json_createpiece(cinfo, name, DeviceType::DISK);
            if (iretn < 0)
            {
                continue;
            }
            uint16_t cidx = cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            uint16_t didx = cinfo->device[cidx]->didx;
            if (dinfo[i].mount.size() > COSMOS_MAX_NAME)
            {
                dinfo[i].mount.resize(COSMOS_MAX_NAME);
            }
            cinfo->devspec.disk[didx].path = dinfo[i].mount;
        }
        json_addpiece(cinfo, "main_drive", DeviceType::DISK);
        json_mappieceentry(cinfo->pieces.size()-1, cinfo);
        json_togglepieceentry(cinfo->pieces.size()-1, cinfo, true);

        cinfo->node.device_cnt = cinfo->node.piece_cnt;
        cinfo->device.resize(cinfo->node.device_cnt);
        cinfo->devspec.cpu_cnt = 1;
        cinfo->devspec.disk_cnt = 1;
        cinfo->node.port_cnt = 1;
        cinfo->port.resize(cinfo->node.port_cnt);

//        int32_t iretn = json_dump_node(cinfo);
        json_destroy(cinfo);
//        return iretn;
        return 0;
    }
    else
    {
        return 0;
    }
}

int32_t json_create_mcc(string &node_name)
{
    cosmosstruc *cinfo = nullptr;

    if (node_name.empty())
    {
        DeviceCpu deviceCpu;
        node_name = deviceCpu.getHostName();
    }

    if (get_nodedir(node_name).empty())
    {
        if (get_nodedir(node_name, true).empty())
        {
            return 1;
        }

        cinfo = json_init();
        //        json_mapbaseentries(cinfo);
        //        strncpy(cinfo->node.name, node_name.c_str(), COSMOS_MAX_NAME);
        cinfo->node.name = node_name;
        cinfo->node.type = NODE_TYPE_TESTBED;

        json_addpiece(cinfo, "motion_capture_unit", DeviceType::MCC);
        json_mappieceentry(cinfo->pieces.size()-1, cinfo);
        json_togglepieceentry(cinfo->pieces.size()-1, cinfo, true);

        cinfo->node.device_cnt = cinfo->node.piece_cnt;
        cinfo->device.resize(cinfo->node.device_cnt);
        cinfo->devspec.mcc_cnt = 1;
        cinfo->node.port_cnt = 1;
        cinfo->port.resize(cinfo->node.port_cnt);

        for (size_t i=0; i<cinfo->node.piece_cnt; ++i)
        {

            cinfo->device[i]->pidx = i;
            cinfo->device[i]->cidx = i;
            switch (i)
            {
            case 0:
                cinfo->device[i]->type = DeviceType::MCC;
                cinfo->device[i]->didx = 0;
                cinfo->device[i]->portidx = 0;
                cinfo->devspec.mcc[cinfo->device[i]->didx].align = {{0., 0., 0.}, 1.};
                json_mapdeviceentry(cinfo->device[i], cinfo);
                json_toggledeviceentry(0, DeviceType::MCC, cinfo, true);
                json_addentry("mcc_utilization", "(\"device_mcc_load_000\"/\"device_mcc_maxload_000\")", cinfo);
                cinfo->port[0].type = PORT_TYPE_ETHERNET;
                json_mapportentry(cinfo->device[i]->portidx, cinfo);
                json_toggleportentry(cinfo->device[i]->portidx, cinfo, true);
                break;
            default:
                cinfo->devspec.disk[cinfo->device[i]->didx].maxgib = 1000.;
                cinfo->device[i]->type = DeviceType::DISK;
                cinfo->device[i]->didx = i-1;
                cinfo->device[i]->portidx = cinfo->device[i]->didx;
                cinfo->port[cinfo->device[i]->didx].type = PORT_TYPE_DRIVE;
                json_mapdeviceentry(cinfo->device[i], cinfo);
                json_toggledeviceentry(i-1, DeviceType::DISK, cinfo, true);
#ifdef COSMOS_WIN_OS
                cinfo->port[cinfo->device[i]->didx].name = "c:/";
#else
                cinfo->port[cinfo->device[i]->didx].name = "/";
#endif
                break;
            }
            json_mapcompentry(i, cinfo);
            json_togglecompentry(i, cinfo, true);
            cinfo->device[i]->enabled = true;
        }

//        int32_t iretn = json_dump_node(cinfo);
        json_destroy(cinfo);
//        return iretn;
        return 0;
    }
    else
    {
        return 0;
    }
}

//! Create new piece
/*! Use ::json_addpiece to add a new piecestruc and its attendant device information
 * and then map it.
 * \param type JSON PIECE_TYPE
 * \param emi Emissivity
 * \param abs Absorbtivity
 * \param hcap Heat capacity
 * \param hcon Heat conductivity
 * \param density Density
 * \return Index of piece, or negative error
 */
int32_t json_createpiece(cosmosstruc *cinfo, string name, DeviceType ctype, double emi, double abs, double hcap, double hcon, double density)
{
    int32_t iretn = 0;
    if (name.size() > COSMOS_MAX_NAME)
    {
        name.resize(COSMOS_MAX_NAME);
    }

    iretn = json_addpiece(cinfo, name, ctype, emi, abs, hcap, hcon, density);
    if (iretn < 0)
    {
        return iretn;
    }
    uint16_t pidx = static_cast <uint16_t>(iretn);

    iretn = json_mappieceentry(pidx, cinfo);
    if (iretn < 0)
    {
        return iretn;
    }
    iretn = json_togglepieceentry(pidx, cinfo, true);
    if (iretn < 0)
    {
        return iretn;
    }

    if (ctype < DeviceType::COUNT)
    {
        uint16_t cidx = cinfo->pieces[pidx].cidx;
        json_mapcompentry(cidx, cinfo);
        json_togglecompentry(cidx, cinfo, true);
        json_mapdeviceentry(cinfo->device[cidx], cinfo);
        json_toggledeviceentry(cinfo->device[cidx]->didx, ctype, cinfo, true);
    }
    return pidx;
}

int32_t json_finddev(cosmosstruc *cinfo, string name)
{
    if (name.size() > COSMOS_MAX_NAME)
    {
        name.resize(COSMOS_MAX_NAME);
    }

    int32_t iretn = 0;

    iretn = json_findpiece(cinfo, name);
    if (iretn >= 0)
    {
        iretn = cinfo->pieces[static_cast <size_t>(iretn)].cidx;
        if (iretn >= 0)
        {
            iretn = cinfo->device[static_cast <size_t>(iretn)]->didx;
        }
    }

    return iretn;
}

int32_t json_findcomp(cosmosstruc *cinfo, string name)
{
    if (name.size() > COSMOS_MAX_NAME)
    {
        name.resize(COSMOS_MAX_NAME);
    }

    int32_t iretn = 0;

    iretn = json_findpiece(cinfo, name);
    if (iretn >= 0)
    {
        iretn = cinfo->pieces[static_cast <size_t>(iretn)].cidx;
    }

    return iretn;
}

int32_t json_findpiece(cosmosstruc *cinfo, string name)
{
    if (name.size() > COSMOS_MAX_NAME)
    {
        name.resize(COSMOS_MAX_NAME);
    }

    for (size_t i=0; i<cinfo->pieces.size(); ++i)
    {
        if (name == cinfo->pieces[i].name)
        {
            return i;
        }
    }
    return JSON_ERROR_NOJMAP;
}

//! Add new piece
/*! Take an empty ::piecestruc and fill it with the provided information, generating the vertexs for
     * the indicated type.
     * \param type JSON PIECE_TYPE
     * \param emi Emissivity
     * \param abs Absorbtivity
     * \param hcap Heat capacity
     * \param hcon Heat conductivity
     * \param density Density
     * \return Zero, or negative error
     */
int32_t json_addpiece(cosmosstruc *cinfo, string name, DeviceType ctype, double emi, double abs, double hcap, double hcon, double density)
{
    int32_t iretn = 0;
    if (name.size() > COSMOS_MAX_NAME)
    {
        name.resize(COSMOS_MAX_NAME);
    }

    // look for existing piece
    for (size_t i=0; i<cinfo->pieces.size(); ++i)
    {
        if (name == cinfo->pieces[i].name)
        {
            return i;
        }
    }

    // otherwise make a new piece
    piecestruc piece;
    piece.name = name;
    piece.emi = emi;
    piece.abs = abs;
    piece.density = density;
    piece.volume = .001 / density;
    piece.hcap = hcap;
    piece.hcon = hcon;
    if (ctype < DeviceType::COUNT)
    {
        iretn = json_adddevice(cinfo, static_cast <uint16_t>(cinfo->pieces.size()) - 1, ctype);
        if (iretn < 0)
        {
            return iretn;
        }
        piece.cidx = iretn;
        cinfo->device.back()->name = piece.name;
    }
    else
    {
        piece.cidx = DeviceType::NONE;
    }
    piece.enabled = true;
    piece.face_cnt = 0;
    cinfo->pieces.push_back(piece);
    cinfo->node.piece_cnt = static_cast <uint16_t>(cinfo->pieces.size());

    return (static_cast <int32_t>(cinfo->pieces.size() - 1));
}

//! Add new port entry
//! Extend the existing port list with an entry for the given name and piece
//! \param cinfo Pointer to cosmosstruc
//! \param name Name information for port
//! \param type Port type listed in ::PORT_TYPE.
int32_t json_createport(cosmosstruc *cinfo, string name, PORT_TYPE type)
{
    if (name.size() > COSMOS_MAX_NAME)
    {
        name.resize(COSMOS_MAX_NAME);
    }

    for (uint16_t i=0; i<cinfo->port.size(); ++i)
    {
        if (name == cinfo->port[i].name)
        {
            if (type != cinfo->port[i].type)
            {
                return GENERAL_ERROR_MISMATCH;
            }
            return i;
        }
    }

    portstruc port;
    port.name = name;
    port.type = type;
    cinfo->port.push_back(port);
    cinfo->node.port_cnt = static_cast <uint16_t>(cinfo->port.size());
    json_mapportentry(static_cast <uint16_t>(cinfo->port.size()) - 1, cinfo);
    json_toggleportentry(static_cast <uint16_t>(cinfo->port.size()) - 1, cinfo, true);

    return (static_cast <int32_t>(cinfo->port.size() - 1));
}

//! Add new device
/*! Take an empty ::devicestruc and fill it with the provided information.
     * \param type JSON_DEVICE_TYPE
     * \return Zero, or negative error
     */
int32_t json_adddevice(cosmosstruc *cinfo, uint16_t pidx, DeviceType ctype)
{
    if (ctype < DeviceType::COUNT)
    {
        switch(ctype)
        {
        //! Antennae
        case DeviceType::ANT:
            cinfo->devspec.ant.push_back(antstruc());
            cinfo->devspec.ant.back().didx = cinfo->devspec.ant.size() - 1;
            cinfo->devspec.ant_cnt = cinfo->devspec.ant.size();
            cinfo->device.push_back(&cinfo->devspec.ant.back());
            break;
            //! Battery
        case DeviceType::BATT:
            cinfo->devspec.batt.push_back(battstruc());
            cinfo->devspec.batt.back().didx = cinfo->devspec.batt.size() - 1;
            cinfo->devspec.batt_cnt = cinfo->devspec.batt.size();
            cinfo->device.push_back(&cinfo->devspec.batt.back());
            break;
            //! BCREG
        case DeviceType::BCREG:
            cinfo->devspec.bcreg.push_back(bcregstruc());
            cinfo->devspec.bcreg.back().didx = cinfo->devspec.bcreg.size() - 1;
            cinfo->devspec.bcreg_cnt = cinfo->devspec.bcreg.size();
            cinfo->device.push_back(&cinfo->devspec.bcreg.back());
            break;
            //! Bus
        case DeviceType::BUS:
            cinfo->devspec.bus.push_back(busstruc());
            cinfo->devspec.bus.back().didx = cinfo->devspec.bus.size() - 1;
            cinfo->devspec.bus_cnt = cinfo->devspec.bus.size();
            cinfo->device.push_back(&cinfo->devspec.bus.back());
            break;
            //! Camera
        case DeviceType::CAM:
            cinfo->devspec.cam.push_back(camstruc());
            cinfo->devspec.cam.back().didx = cinfo->devspec.cam.size() - 1;
            cinfo->devspec.cam_cnt = cinfo->devspec.cam.size();
            cinfo->device.push_back(&cinfo->devspec.cam.back());
            break;
            //! Processing Unit
        case DeviceType::CPU:
            {
            cinfo->devspec.cpu.push_back(cpustruc{});
            cinfo->devspec.cpu.back().didx = cinfo->devspec.cpu.size() - 1;
            cinfo->devspec.cpu_cnt = cinfo->devspec.cpu.size();
            cinfo->device.push_back(&cinfo->devspec.cpu.back());
            }
            break;
            //! Disk
        case DeviceType::DISK:
            cinfo->devspec.disk.push_back(diskstruc());
            cinfo->devspec.disk.back().didx = cinfo->devspec.disk.size() - 1;
            cinfo->devspec.disk_cnt = cinfo->devspec.disk.size();
            cinfo->device.push_back(&cinfo->devspec.disk.back());
            break;
            //! GPS Unit
        case DeviceType::GPS:
            cinfo->devspec.gps.push_back(gpsstruc());
            cinfo->devspec.gps.back().didx = cinfo->devspec.gps.size() - 1;
            cinfo->devspec.gps_cnt = cinfo->devspec.gps.size();
            cinfo->device.push_back(&cinfo->devspec.gps.back());
            break;
            //! Gyroscope
        case DeviceType::GYRO:
            cinfo->devspec.gyro.push_back(gyrostruc());
            cinfo->devspec.gyro.back().didx = cinfo->devspec.gyro.size() - 1;
            cinfo->devspec.gyro_cnt = cinfo->devspec.gyro.size();
            cinfo->device.push_back(&cinfo->devspec.gyro.back());
            break;
            //! Heater
        case DeviceType::HTR:
            cinfo->devspec.htr.push_back(htrstruc());
            cinfo->devspec.htr.back().didx = cinfo->devspec.htr.size() - 1;
            cinfo->devspec.htr_cnt = cinfo->devspec.htr.size();
            cinfo->device.push_back(&cinfo->devspec.htr.back());
            break;
            //! Inertial Measurement Unit
        case DeviceType::IMU:
            cinfo->devspec.imu.push_back(imustruc());
            cinfo->devspec.imu.back().didx = cinfo->devspec.imu.size() - 1;
            cinfo->devspec.imu_cnt = cinfo->devspec.imu.size();
            cinfo->device.push_back(&cinfo->devspec.imu.back());
            break;
            //! Magnetometer
        case DeviceType::MAG:
            cinfo->devspec.mag.push_back(magstruc());
            cinfo->devspec.mag.back().didx = cinfo->devspec.mag.size() - 1;
            cinfo->devspec.mag_cnt = cinfo->devspec.mag.size();
            cinfo->device.push_back(&cinfo->devspec.mag.back());
            break;
            //! Motion Capture Camera
        case DeviceType::MCC:
            cinfo->devspec.mcc.push_back(mccstruc());
            cinfo->devspec.mcc.back().didx = cinfo->devspec.mcc.size() - 1;
            cinfo->devspec.mcc_cnt = cinfo->devspec.mcc.size();
            cinfo->device.push_back(&cinfo->devspec.mcc.back());
            break;
            //! Motor
        case DeviceType::MOTR:
            cinfo->devspec.motr.push_back(motrstruc());
            cinfo->devspec.motr.back().didx = cinfo->devspec.motr.size() - 1;
            cinfo->devspec.motr_cnt = cinfo->devspec.motr.size();
            cinfo->device.push_back(&cinfo->devspec.motr.back());
            break;
            //! Magnetic Torque Rod
        case DeviceType::MTR:
            cinfo->devspec.mtr.push_back(mtrstruc());
            cinfo->devspec.mtr.back().didx = cinfo->devspec.mtr.size() - 1;
            cinfo->devspec.mtr_cnt = cinfo->devspec.mtr.size();
            cinfo->device.push_back(&cinfo->devspec.mtr.back());
            break;
            //! Payload
        case DeviceType::PLOAD:
            cinfo->devspec.pload.push_back(ploadstruc());
            cinfo->devspec.pload.back().didx = cinfo->devspec.pload.size() - 1;
            cinfo->devspec.pload_cnt = cinfo->devspec.pload.size();
            cinfo->device.push_back(&cinfo->devspec.pload.back());
            break;
            //! Propellant Tank
        case DeviceType::PROP:
            cinfo->devspec.prop.push_back(propstruc());
            cinfo->devspec.prop.back().didx = cinfo->devspec.prop.size() - 1;
            cinfo->devspec.prop_cnt = cinfo->devspec.prop.size();
            cinfo->device.push_back(&cinfo->devspec.prop.back());
            break;
            //! Pressure Sensor
        case DeviceType::PSEN:
            cinfo->devspec.psen.push_back(psenstruc());
            cinfo->devspec.psen.back().didx = cinfo->devspec.psen.size() - 1;
            cinfo->devspec.psen_cnt = cinfo->devspec.psen.size();
            cinfo->device.push_back(&cinfo->devspec.psen.back());
            break;
            //! Photo Voltaic String
        case DeviceType::PVSTRG:
            cinfo->devspec.pvstrg.push_back(pvstrgstruc());
            cinfo->devspec.pvstrg.back().didx = cinfo->devspec.pvstrg.size() - 1;
            cinfo->devspec.pvstrg_cnt = cinfo->devspec.pvstrg.size();
            cinfo->device.push_back(&cinfo->devspec.pvstrg.back());
            break;
            //! Rotor
        case DeviceType::ROT:
            cinfo->devspec.rot.push_back(rotstruc());
            cinfo->devspec.rot.back().didx = cinfo->devspec.rot.size() - 1;
            cinfo->devspec.rot_cnt = cinfo->devspec.rot.size();
            cinfo->device.push_back(&cinfo->devspec.rot.back());
            break;
            //! Reaction Wheel
        case DeviceType::RW:
            cinfo->devspec.rw.push_back(rwstruc());
            cinfo->devspec.rw.back().didx = cinfo->devspec.rw.size() - 1;
            cinfo->devspec.rw_cnt = cinfo->devspec.rw.size();
            cinfo->device.push_back(&cinfo->devspec.rw.back());
            break;
            //! Radio Receiver
        case DeviceType::RXR:
            cinfo->devspec.rxr.push_back(rxrstruc());
            cinfo->devspec.rxr.back().didx = cinfo->devspec.rxr.size() - 1;
            cinfo->devspec.rxr_cnt = cinfo->devspec.rxr.size();
            cinfo->device.push_back(&cinfo->devspec.rxr.back());
            break;
            //! Elevation and Azimuth Sun Sensor
        case DeviceType::SSEN:
            cinfo->devspec.ssen.push_back(ssenstruc());
            cinfo->devspec.ssen.back().didx = cinfo->devspec.ssen.size() - 1;
            cinfo->devspec.ssen_cnt = cinfo->devspec.ssen.size();
            cinfo->device.push_back(&cinfo->devspec.ssen.back());
            break;
            //! Star Tracker
        case DeviceType::STT:
            cinfo->devspec.stt.push_back(sttstruc());
            cinfo->devspec.stt.back().didx = cinfo->devspec.stt.size() - 1;
            cinfo->devspec.stt_cnt = cinfo->devspec.stt.size();
            cinfo->device.push_back(&cinfo->devspec.stt.back());
            break;
            //! SUCHI
        case DeviceType::SUCHI:
            cinfo->devspec.suchi.push_back(suchistruc());
            cinfo->devspec.suchi.back().didx = cinfo->devspec.suchi.size() - 1;
            cinfo->devspec.suchi_cnt = cinfo->devspec.suchi.size();
            cinfo->device.push_back(&cinfo->devspec.suchi.back());
            break;
            //! Switch
        case DeviceType::SWCH:
            cinfo->devspec.swch.push_back(swchstruc());
            cinfo->devspec.swch.back().didx = cinfo->devspec.swch.size() - 1;
            cinfo->devspec.swch_cnt = cinfo->devspec.swch.size();
            cinfo->device.push_back(&cinfo->devspec.swch.back());
            break;
            //! Texas Christian University
        case DeviceType::TCU:
            cinfo->devspec.tcu.push_back(tcustruc());
            cinfo->devspec.tcu.back().didx = cinfo->devspec.tcu.size() - 1;
            cinfo->devspec.tcu_cnt = cinfo->devspec.tcu.size();
            cinfo->device.push_back(&cinfo->devspec.tcu.back());
            break;
            //! Radio Transceiver
        case DeviceType::TCV:
            cinfo->devspec.tcv.push_back(tcvstruc());
            cinfo->devspec.tcv.back().didx = cinfo->devspec.tcv.size() - 1;
            cinfo->devspec.tcv_cnt = cinfo->devspec.tcv.size();
            cinfo->device.push_back(&cinfo->devspec.tcv.back());
            break;
            //! Telemetry
        case DeviceType::TELEM:
            cinfo->devspec.telem.push_back(telemstruc());
            cinfo->devspec.telem.back().didx = cinfo->devspec.telem.size() - 1;
            cinfo->devspec.telem_cnt = cinfo->devspec.telem.size();
            cinfo->device.push_back(&cinfo->devspec.telem.back());
            break;
            //! Thruster
        case DeviceType::THST:
            cinfo->devspec.thst.push_back(thststruc());
            cinfo->devspec.thst.back().didx = cinfo->devspec.thst.size() - 1;
            cinfo->devspec.thst_cnt = cinfo->devspec.thst.size();
            cinfo->device.push_back(&cinfo->devspec.thst.back());
            break;
            //! Temperature Sensor
        case DeviceType::TSEN:
            cinfo->devspec.tsen.push_back(tsenstruc());
            cinfo->devspec.tsen.back().didx = cinfo->devspec.tsen.size() - 1;
            cinfo->devspec.tsen_cnt = cinfo->devspec.tsen.size();
            cinfo->device.push_back(&cinfo->devspec.tsen.back());
            break;
            //! TNC
        case DeviceType::TNC:
            cinfo->devspec.tnc.push_back(tncstruc());
            cinfo->devspec.tnc.back().didx = cinfo->devspec.tnc.size() - 1;
            cinfo->devspec.tnc_cnt = cinfo->devspec.tnc.size();
            cinfo->device.push_back(&cinfo->devspec.tnc.back());
            break;
            //! Radio Transmitter
        case DeviceType::TXR:
            cinfo->devspec.txr.push_back(txrstruc());
            cinfo->devspec.txr.back().didx = cinfo->devspec.txr.size() - 1;
            cinfo->devspec.txr_cnt = cinfo->devspec.txr.size();
            cinfo->device.push_back(&cinfo->devspec.txr.back());
            break;
        case DeviceType::XYZSEN:
            cinfo->devspec.xyzsen.push_back(xyzsenstruc());
            cinfo->devspec.xyzsen.back().didx = cinfo->devspec.xyzsen.size() - 1;
            cinfo->devspec.xyzsen_cnt = cinfo->devspec.xyzsen.size();
            cinfo->device.push_back(&cinfo->devspec.xyzsen.back());
            break;
        case DeviceType::COUNT:
        case DeviceType::NONE:
        default:
            return (static_cast <int32_t>(cinfo->pieces.size() - 1));
            break;
        }
        cinfo->device.back()->pidx = pidx;
        cinfo->device.back()->cidx = static_cast <int32_t>(cinfo->device.size() - 1);
        cinfo->device.back()->type = static_cast<uint16_t>(ctype);
        cinfo->node.device_cnt = cinfo->device.size();
    }
    return (static_cast <int32_t>(cinfo->device.size() - 1));
}

//! Enter an alias into the JSON Namespace.
/*! See if the provided name is in the Namespace. If so, add an entry
 * for the provided alias that points to the same location.
 * \param alias Name to add as an alias.
 * \param value Either the contents of an equation, a constant, or a Namespace name that
 * should already exist in the Namespace
 * \param cmeta Reference to ::cosmosstruc to use.
 * \return The current number of entries, if successful, otherwise negative error.
*/
int32_t json_addentry(string alias, string value, cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    jsonhandle handle;
    //    uint16_t count = 0;
    // Add this alias only if it is not already in the map
    if ((iretn = json_name_map(alias, cinfo, handle)))
    {
        jsonentry tentry;
        tentry.name = alias;
        aliasstruc talias;
        talias.name = alias;
        string val_added = value;

        switch (value[0])
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '+':
        case '-':
        case '.':
        case '%':
            // Add a constant as an equation multiplied by 1 (kind of hack-ish).
            val_added = "(" + value + "*1.)";

            // If it begins with ( then it is an equation, otherwise treat as name
        case '(':
            // Add new equation
            iretn = json_equation_map(val_added, cinfo, &handle);
            if (iretn < 0)
            {
                return iretn;
            }
            talias.handle = handle;
            talias.type = JSON_TYPE_EQUATION;
            break;

            //        case '"': /// TODO: Aliases for just namespace members are not working.
            //            // Strip the quotes away.
            //            val_added.clear();
            //            for (size_t i = 0; i < value.size(); ++i) {
            //                if (value[i] != '"') { val_added += value[i]; }
            //            }
            //        default:
            //            // It is a Namespace name which should only be added if it is in the map
            //            if ((iretn = json_name_map(val_added, cinfo, handle)))
            //            {
            //                return iretn;
            //            }
            //            // Add new alias
            //            talias.handle = handle;
            //            talias.type = cinfo->jmap[handle.hash][handle.index].type;
            //            break;
        default:
            return GENERAL_ERROR_UNIMPLEMENTED;
        }
        // Place it in the Alias vector and point to it in the map
        cinfo->alias.push_back(talias);
        tentry.type = JSON_TYPE_ALIAS;
        tentry.group = JSON_STRUCT_ALIAS;
        //tentry.group = JSON_STRUCT_PTR; // Unsure why this change was made...
        tentry.offset = cinfo->alias.size() - 1;
        tentry.ptr = (uint8_t *)&cinfo->alias[tentry.offset];
        iretn = json_addentry(tentry, cinfo);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    return cinfo->jmapped;
}

//! Enter an entry into the JSON Namespace.
/*! Enters a ::jsonentry in the JSON Data Name Space.
    \param entry The entry to be entered.

    \return The current number of entries, if successful, negative error if the entry could not be
    added.
*/
int32_t json_addentry(jsonentry entry, cosmosstruc *cinfo)
{
    uint16_t hash = json_hash(entry.name);
    size_t csize = cinfo->jmap[hash].size();

    for (size_t i=0; i<csize; ++i)
    {
        if (entry.name == cinfo->jmap[hash][i].name)
        {
            cinfo->jmap[hash][i] = entry;
            return cinfo->jmapped;
        }
    }

    cinfo->jmap[hash].push_back(entry);
    if (cinfo->jmap[hash].size() != csize+1)
    {
        return JSON_ERROR_NOENTRY;
    }
    cinfo->ujmap[entry.name] = entry;

    ++cinfo->jmapped;
    cinfo->ujmapped = cinfo->ujmap.size();
    return (cinfo->jmapped);
}

//! Add an entry to the JSON Namespace map with units.
/*! Allocates the space for a new ::jsonentry and then enters the information
 * associating a pointer with an entry in the name table. The name will be of form "name" if it is a
 * scalar, "name_iii" if it is a first level array, "name_iii_iii" if it is second
 * level, where "iii" is the zero filled index for the appropriate level.
    \param name variable name from the JSON Data Name Space
    \param d1 array index for first level, otherwise -1
    \param d2 array index for second level, otherwise -1
    \param offset Offset to the data from the beginning of its group.
    \param size Number of bytes ::jsonentry take up.
    \param type COSMOS JSON Data Type.
    \param group COSMOS JSON Data Group.

    \param unit Index into JMAP unit table.
    \return The current number of entries, if successful, 0 if the entry could not be
    added, or if enough memory could not be allocated to hold the JSON stream.
*/
int32_t json_addentry(string name, uint16_t d1, uint16_t d2, ptrdiff_t offset, uint16_t type, uint16_t group, cosmosstruc *cinfo, uint16_t unit)
{
    jsonentry tentry;
    char ename[COSMOS_MAX_NAME+1];

    // Determine extended name
    strcpy(ename,name.c_str());
    if (d1 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d1);
    if (d2 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d2);

    // Populate the entry
    tentry.alarm_index = 0;
    tentry.alert_index = 0;
    tentry.maximum_index = 0;
    tentry.minimum_index = 0;
    tentry.unit_index = unit;
    tentry.type = type;
    tentry.group = group;
    tentry.name = ename;
    tentry.offset = offset;
    tentry.ptr = nullptr;
    //    tentry.size = size;

    return json_addentry(tentry, cinfo);
}

int32_t json_addentry(string name, uint16_t d1, uint16_t d2, uint8_t* ptr, uint16_t type, cosmosstruc *cinfo, uint16_t unit)
{
    jsonentry tentry;
    char ename[COSMOS_MAX_NAME+1];

    // Determine extended name
    strcpy(ename,name.c_str());
    if (d1 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d1);
    if (d2 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d2);

    // Populate the entry
    tentry.alarm_index = 0;
    tentry.alert_index = 0;
    tentry.maximum_index = 0;
    tentry.minimum_index = 0;
    tentry.unit_index = unit;
    tentry.type = type;
    tentry.group = JSON_STRUCT_PTR;
    tentry.name = ename;
    tentry.offset = 0;
    tentry.ptr = ptr;
    //    tentry.size = size;

    return json_addentry(tentry, cinfo);
}

//! Toggle the enable state of an entry in the JSON Namespace map.
/*! Sets or clears the enabled flag for an entry in the name table.
 * The name will be of form "name" if it is a
 * scalar, "name_iii" if it is a first level array, "name_iii_iii" if it is second
 * level, where "iii" is the zero filled index for the appropriate level.
    \param name variable name from the JSON Data Name Space
    \param d1 array index for first level, otherwise -1
    \param d2 array index for second level, otherwise -1

    \param state Enable state to toggle to.
    \return 0 or a negative error.
*/
int32_t json_toggleentry(string name, uint16_t d1, uint16_t d2, cosmosstruc *cinfo, bool state)
{
    char ename[COSMOS_MAX_NAME+1];

    // Determine extended name
    strcpy(ename,name.c_str());
    //  JIMNOTE:  this scheme really only works up to 999, not MAX.  Also should error if out of bounds
    if (d1 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d1);
    if (d2 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d2);

    // Find the name in the map
    jsonentry *tentry = json_entry_of(ename, cinfo);
    if (tentry!= nullptr)
    {
        tentry->enabled = state;
        return 0;
    } else {
        return JSON_ERROR_NOENTRY;
    }
}

//! Check the enable state of an entry in the JSON Namespace map.
/*! Returns the enabled flag for an entry in the name table.
 * The name will be of form "name" if it is a
 * scalar, "name_iii" if it is a first level array, "name_iii_iii" if it is second
 * level, where "iii" is the zero filled index for the appropriate level.
    \param name variable name from the JSON Data Name Space
    \param d1 array index for first level, otherwise -1
    \param d2 array index for second level, otherwise -1

    \return 0 or a negative error.
*/
bool json_checkentry(string name, uint16_t d1, uint16_t d2, cosmosstruc *cinfo)
{
    char ename[COSMOS_MAX_NAME+1];

    // Determine extended name
    strcpy(ename,name.c_str());
    //  JIMNOTE:  this scheme really only works up to 999, not MAX.  Also should error if out of bounds
    if (d1 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d1);
    if (d2 < UINT16_MAX)
        sprintf(&ename[strlen(ename)],"_%03u",d2);

    // Find the name in the map
    jsonentry *tentry = json_entry_of(ename, cinfo);
    if (tentry!= nullptr)
    {
        return tentry->enabled;
    } else {
        return false;
    }
}

//! Number of items in current JSON map with a specific hash
/*! Returns the number of JSON items currently mapped.
    \param hash JSON HASH value.

    \return Number of JSON items mapped, otherwise zero.
*/
size_t json_count_hash(uint16_t hash, cosmosstruc *cinfo)
{
    return (cinfo->jmap[hash].size());
}

//! Number of items in the current JSON map
/*! Returns the number of JSON items currently mapped.

    \return total number of JSON items mapped, otherwise zero.
*/

size_t json_count_total(cosmosstruc *cinfo)
{
    size_t i = 0;
    for(uint32_t j = 0; j < cinfo->jmap.size(); ++j)
        i += json_count_hash((uint16_t)j, cinfo);
    return i;
}

//! Perform JSON output for a JSON item by handle
/*! Appends the indicated JSON object to the supplied JSON Stream.
    \param jstring Reference to JSON stream.
    \param handle The ::jsonhandle of the desired object.

    \param cinfo Reference to ::cosmosstruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_handle(string &jstring, jsonhandle handle, cosmosstruc *cinfo)
{
    if (!cinfo->jmapped)
        return (JSON_ERROR_NOJMAP);
    return json_out_entry(jstring, cinfo->jmap[handle.hash][handle.index], cinfo);
}

//! Perform JSON output for a single JSON entry
/*! Populates the jstring for the indicated ::jsonmap by ::jsonentry.
    \param jstring Reference to JSON stream.
    \param entry The ::jsonentry of the desired variable.
    \param cinfo Reference to ::cosmosstruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_entry(string &jstring, const jsonentry &entry, cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    uint8_t *data;

    jstring.erase(jstring.find_last_not_of(" \n\r\t")+1);
    if (!jstring.empty() && jstring.back() =='}')       //replaces '}' with ',' to allow the continuation of the json object (if node json is to remain separate, input ' ' at end?
    {
        jstring.back()=',';
    }
    else if ((iretn=json_out_character(jstring,'{')) != 0) //still inputs '{' if it is the first entry into the string
        return iretn;

    data = json_ptr_of_entry(entry, cinfo);
    if ((iretn=json_out_value(jstring, entry.name, data, entry.type, cinfo)) != 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) != 0)       //closes json object. if json continues it will be overwritten with ','
        return iretn;

    return iretn;
}

//! Output JSON Pair
/*! Append a name:value pair to the provided JSON String for a variable in
 * the JSON Namespace, based on the provided data and type.
 * \param jstring Reference to JSON String to append to.
 * \param name Namespace name of variable.
 * \param data Pointer to location in ::cosmosstruc of variable.
 * \param type Type of variable.

    \param cinfo Reference to ::cosmosstruc to use.
 * \return Zero or negative error.
*/
int32_t json_out_value(string &jstring, string name, uint8_t *data, uint16_t type, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    if (!cinfo->jmapped)
    {
        return JSON_ERROR_NOJMAP;
    }

    if ((iretn=json_out_name(jstring ,name)) != 0)
    {
        return iretn;
    }

    if ((iretn=json_out_type(jstring, data, type, cinfo)) != 0)
    {
        return iretn;
    }

    return 0;
}

//! Output JSON Value
/*! Append the value of a name:value pair to the provided JSON String for a variable in
 * the JSON Namespace, based on the provided data and type.
 * \param jstring Reference to JSON String to append to.
 * \param data Pointer to location of variable.
 * \param type Type of variable.

    \param cinfo Reference to ::cosmosstruc to use.
 * \return Zero or negative error.
*/
int32_t json_out_type(string &jstring, uint8_t *data, uint16_t type, cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    if (data == 0)
    {
        return GENERAL_ERROR_EMPTY;
    }

    switch (type)
    {
    case JSON_TYPE_BOOL:
        if ((iretn=json_out_uint8(jstring,*(uint8_t *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_UINT8:
        if ((iretn=json_out_uint8(jstring,*(uint8_t *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_UINT16:
        if ((iretn=json_out_uint16(jstring,*(uint16_t *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_UINT32:
        if ((iretn=json_out_uint32(jstring,*(uint32_t *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_INT8:
        if ((iretn=json_out_int8(jstring,*(int8_t *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_INT16:
        if ((iretn=json_out_int16(jstring,*(int16_t *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_INT32:
        if ((iretn=json_out_int32(jstring,*(int32_t *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_FLOAT:
        if ((iretn=json_out_float(jstring,*(float *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_DOUBLE:
    case JSON_TYPE_TIMESTAMP:
        if ((iretn=json_out_double(jstring,*(double *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_STRING:
        if (!(*(string *)data)[0])
        {
            iretn = json_out_string(jstring,"",COSMOS_MAX_DATA);
        }
        else
        {
            iretn = json_out_string(jstring,*(string *)data,COSMOS_MAX_DATA);
        }
        if (iretn != 0)
        {
            return iretn;
        }
        break;
    case JSON_TYPE_NAME:
        if ((iretn=json_out_string(jstring,(char *)data,COSMOS_MAX_NAME)) != 0)
            return iretn;
        break;
    case JSON_TYPE_VECTOR:
        if ((iretn=json_out_vector(jstring,*(Vector *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_GVECTOR:
        if ((iretn=json_out_gvector(jstring,*(gvector *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_CVECTOR:
        if ((iretn=json_out_cvector(jstring,*(cvector *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_AVECTOR:
        if ((iretn=json_out_avector(jstring,*(avector *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_QUATERNION:
        if ((iretn=json_out_quaternion(jstring,*(quaternion *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_RMATRIX:
        if ((iretn=json_out_rmatrix(jstring,*(rmatrix *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_RVECTOR:
        //    case JSON_TYPE_TVECTOR:
        if ((iretn=json_out_rvector(jstring,*(rvector *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_GEOIDPOS:
    case JSON_TYPE_POS_SELG:
    case JSON_TYPE_POS_GEOD:
        if ((iretn=json_out_geoidpos(jstring,*(Convert::geoidpos *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_SPHERPOS:
    case JSON_TYPE_POS_GEOS:
        if ((iretn=json_out_spherpos(jstring,*(Convert::spherpos *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_CARTPOS:
    case JSON_TYPE_POS_GEOC:
    case JSON_TYPE_POS_SELC:
    case JSON_TYPE_POS_ECI:
    case JSON_TYPE_POS_SCI:
    case JSON_TYPE_POS_ICRF:
        if ((iretn=json_out_cartpos(jstring,*(Convert::cartpos *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_DCMATT:
        if ((iretn=json_out_dcmatt(jstring,*(Convert::dcmatt *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_QATT:
    case JSON_TYPE_QATT_TOPO:
    case JSON_TYPE_QATT_GEOC:
    case JSON_TYPE_QATT_LVLH:
    case JSON_TYPE_QATT_ICRF:
    case JSON_TYPE_QATT_SELC:
        if ((iretn=json_out_qatt(jstring,*(Convert::qatt *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_HBEAT:
        if ((iretn=json_out_beatstruc(jstring,*(beatstruc *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_LOC_POS:
        if ((iretn=json_out_posstruc(jstring,*(Convert::posstruc *)data)) != 0)
            return iretn;
        break;
    case JSON_TYPE_LOC_ATT:
        {
            if ((iretn=json_out_attstruc(jstring,*(Convert::attstruc *)data)) != 0)
                return iretn;
            break;
        }
    case JSON_TYPE_LOCSTRUC:
        {
            if ((iretn=json_out_locstruc(jstring,*(Convert::locstruc *)data)) != 0)
                return iretn;
            break;
        }
    case JSON_TYPE_ALIAS:
        {
            aliasstruc *aptr = (aliasstruc *)data;
            switch (aptr->type)
            {
            case JSON_TYPE_EQUATION:
                {
                    jsonequation *eptr = &cinfo->emap[aptr->handle.hash][aptr->handle.index];
                    if ((iretn=json_out_double(jstring, json_equation(eptr, cinfo))) != 0)
                    {
                        return iretn;
                    }
                }
                break;
            default:
                {
                    jsonentry *eptr = &cinfo->jmap[aptr->handle.hash][aptr->handle.index];
                    if ((iretn=json_out_type(jstring, eptr->data.data(), eptr->type, cinfo)) != 0)
                    {
                        return iretn;
                    }
                }
                break;
            }
            break;
        }
    case JSON_TYPE_EQUATION:
        {
            const char *ptr = (char *)data;
            if ((iretn=json_out_double(jstring, json_equation(ptr, cinfo))) != 0)
            {
                return iretn;
            }
            break;
        }
    }

    return iretn;
}

//! Extend JSON stream
/*! Append the indicated string to the current JSON stream, extending it if necessary.
 * \param jstring JSON stting to be appended to.
    \param tstring String to be appended.
    \return 0 if successful, negative error number otherwise.
*/
int32_t json_append(string &jstring, const char *tstring)
{
    jstring.append(tstring);
    return 0;
}

//! Single character to JSON
/*! Appends an entry for the single character to the current JSON stream.
    \param jstring Reference to JSON stream.
    \param character Character to be added to string in the raw.
    \return 0 if successful, otherwise negative error.
*/
int32_t json_out_character(string &jstring,char character)
{
    char tstring[2] = {0,0};
    int32_t iretn = 0;

    tstring[0] = character;
    if ((iretn=json_append(jstring,tstring)) < 0)
        return iretn;

    return 0;
}

//! Object name to JSON
/*! Appends an entry for the name piece of a JSON object (including ":") to the current JSON
 * string.
    \param jstring Reference to JSON stream.
    \param name The Object Name
    \return  0 if successful, otherwise negative error.
*/
int32_t json_out_name(string &jstring, string name)
{
    int32_t iretn = 0;

    if ((iretn=json_out_string(jstring, name, COSMOS_MAX_NAME)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,':')) < 0)
        return iretn;
    return 0;
}

//! Boolean to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 8 bit boolean.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_bool(string &jstring,bool value)
{
    int32_t iretn = 0;
    char tstring[15];

    if (value)
    {
        sprintf(tstring, "true");
    }
    else
    {
        sprintf(tstring, "false");
    }

    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Signed 8 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 8 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int8(string &jstring,int8_t value)
{
    int32_t iretn = 0;
    char tstring[15];

    sprintf(tstring,"%" PRIi8,value);

    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Signed 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int16(string &jstring,int16_t value)
{
    int32_t iretn = 0;
    char tstring[15];

    sprintf(tstring,"%d",value);

    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Signed 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int32(string &jstring,int32_t value)
{
    int32_t iretn = 0;
    char tstring[15];

    sprintf(tstring,"%d",value);

    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Unsigned 8 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 8 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint8(string &jstring, uint8_t value)
{
    int32_t iretn = 0;
    char tstring[15];

    sprintf(tstring, "%u",value);

    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Unsigned 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint16(string &jstring,uint16_t value)
{
    int32_t iretn = 0;
    char tstring[15];

    sprintf(tstring,"%u",value);

    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Unsigned 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint32(string &jstring,uint32_t value)
{
    int32_t iretn = 0;
    char tstring[15];

    sprintf(tstring,"%u",value);

    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Single precision floating vertex32_t to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated float.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_float(string &jstring,float value)
{
    int32_t iretn = 0;
    char tstring[15];

    if (!isfinite(value))             //CT:11/07/2017: changing invalid floats to 0.
    {
        value=0.0;
    }

    sprintf(tstring,"%.8g",value);
    iretn = json_append(jstring,tstring);
    return iretn;
}

//! Perform JSON output for a single nonindexed double
/*! Appends a JSON entry to the current JSON stream for the indicated double.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_double(string &jstring,double value)
{
    int32_t iretn = 0;
    char tstring[30];

    if (!isfinite(value))           //CT:11/07/2017: changing invalid doubles to 0.
    {
        value=0.0;
    }

    sprintf(tstring,"%.17g",value);
    iretn = json_append(jstring,tstring);

    return iretn;
}

//! String to JSON
/*! Appends a JSON entry to the current JSON stream for the string variable.
    \param jstring Reference to JSON stream.
    \param ostring String to append.
    \param len Maximum allowed size.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_string(string &jstring, string ostring, uint16_t len)
{
    int32_t iretn = 0;
    uint16_t i;
    char tstring[5];

    if (len > JSON_MAX_DATA)
        len = JSON_MAX_DATA;

    if ((iretn=json_out_character(jstring,'"')) < 0)
    {
        return iretn;
    }

    for (i=0; i<ostring.size(); i++)
    {
        if (i > len)
        {
            break;
        }

        if (ostring[i] == '"' || ostring[i] == '\\' || ostring[i] == '/')
        {
            if ((iretn=json_out_character(jstring,'\\')) < 0)
                return iretn;
            if ((iretn=json_out_character(jstring, ostring[i])) < 0)
                return iretn;
        }
        else if (ostring[i] < 32)
        {
            if ((iretn=json_out_character(jstring,'\\')) < 0)
                return iretn;
            switch (ostring[i])
            {
            case '\b':
                if ((iretn=json_out_character(jstring,'b')) < 0)
                    return iretn;
                break;
            case '\f':
                if ((iretn=json_out_character(jstring,'f')) < 0)
                    return iretn;
                break;
            case '\n':
                if ((iretn=json_out_character(jstring,'n')) < 0)
                    return iretn;
                break;
            case '\r':
                if ((iretn=json_out_character(jstring,'r')) < 0)
                    return iretn;
                break;
            case '\t':
                if ((iretn=json_out_character(jstring,'t')) < 0)
                    return iretn;
                break;
            default:
                if ((iretn=json_out_character(jstring,'u')) < 0)
                    return iretn;
                sprintf(tstring,"%04x", ostring[i]);
                if ((iretn=json_out_string(jstring,tstring,4)) < 0)
                    return iretn;
            }
        }
        else if (ostring[i] < 127)
        {
            if ((iretn=json_out_character(jstring, ostring[i])) < 0)
                return iretn;
        }
        else
        {
            if ((iretn=json_out_character(jstring,'\\')) < 0)
                return iretn;
            if ((iretn=json_out_character(jstring,'u')) < 0)
                return iretn;
            sprintf(tstring,"%04x", ostring[i]);
            if ((iretn=json_out_string(jstring,tstring,4)) < 0)
                return iretn;
        }
    }
    if ((iretn=json_out_character(jstring,'"')) < 0)
        return iretn;

    return iretn;
}

//! ::gvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::gvector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_gvector(string &jstring,gvector value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Latitude
    if ((iretn=json_out_name(jstring, "lat")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.lat)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Longitude
    if ((iretn=json_out_name(jstring, "lon")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.lon)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Height
    if ((iretn=json_out_name(jstring, "h")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.h)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::svector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::svector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_svector(string &jstring,svector value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Latitude
    if ((iretn=json_out_name(jstring, "phi")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.phi)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Longitude
    if ((iretn=json_out_name(jstring, "lambda")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.lambda)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Height
    if ((iretn=json_out_name(jstring, "r")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.r)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::avector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::avector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_avector(string &jstring, avector value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Bank
    if ((iretn=json_out_name(jstring, "b")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.b)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Elevation
    if ((iretn=json_out_name(jstring, "e")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.e)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Heading
    if ((iretn=json_out_name(jstring, "h")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.h)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::Vector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::Vector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_vector(string &jstring,Vector value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return iretn;

    // Output x
    if ((iretn=json_out_double(jstring,value.x)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output y
    if ((iretn=json_out_double(jstring,value.y)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output z
    if ((iretn=json_out_double(jstring,value.z)) < 0)
        return iretn;

    // Optionally output w
    if (value.w != 0.)
    {
        if ((iretn=json_out_character(jstring,',')) < 0)
            return iretn;
        if ((iretn=json_out_double(jstring,value.w)) < 0)
            return iretn;
    }

    if ((iretn=json_out_character(jstring,']')) < 0)
        return iretn;

    return 0;
}

//! ::rvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::rvector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_rvector(string &jstring,rvector value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return iretn;

    // Output Col[0]
    if ((iretn=json_out_double(jstring,value.col[0])) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Col[1]
    if ((iretn=json_out_double(jstring,value.col[1])) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Col[2]
    if ((iretn=json_out_double(jstring,value.col[2])) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,']')) < 0)
        return iretn;

    return 0;
}

//! ::quaternion to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::quaternion.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_quaternion(string &jstring,quaternion value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Orientation
    if ((iretn=json_out_name(jstring, "d")) < 0)
        return iretn;
    if ((iretn=json_out_cvector(jstring,value.d)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Rotation
    if ((iretn=json_out_name(jstring, "w")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.w)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::cvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::cvector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_cvector(string &jstring,cvector value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output X
    if ((iretn=json_out_name(jstring, "x")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.x)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Y
    if ((iretn=json_out_name(jstring, "y")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.y)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Z
    if ((iretn=json_out_name(jstring, "z")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.z)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::cartpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::cartpos.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_cartpos(string &jstring, Convert::cartpos value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Time
    if ((iretn=json_out_name(jstring, "utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Position
    if ((iretn=json_out_name(jstring, "pos")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.s)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Velocity
    if ((iretn=json_out_name(jstring, "vel")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.v)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Acceleration
    if ((iretn=json_out_name(jstring, "acc")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.a)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::geoidpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::geoidpos.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_geoidpos(string &jstring, Convert::geoidpos value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Time
    if ((iretn=json_out_name(jstring, "utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Position
    if ((iretn=json_out_name(jstring, "pos")) < 0)
        return iretn;
    if ((iretn=json_out_gvector(jstring,value.s)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Velocity
    if ((iretn=json_out_name(jstring, "vel")) < 0)
        return iretn;
    if ((iretn=json_out_gvector(jstring,value.v)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Acceleration
    if ((iretn=json_out_name(jstring, "acc")) < 0)
        return iretn;
    if ((iretn=json_out_gvector(jstring,value.a)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::spherpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::spherpos.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_spherpos(string &jstring, Convert::spherpos value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Time
    if ((iretn=json_out_name(jstring, "utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Position
    if ((iretn=json_out_name(jstring, "pos")) < 0)
        return iretn;
    if ((iretn=json_out_svector(jstring,value.s)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Velocity
    if ((iretn=json_out_name(jstring, "vel")) < 0)
        return iretn;
    if ((iretn=json_out_svector(jstring,value.v)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Acceleration
    if ((iretn=json_out_name(jstring, "acc")) < 0)
        return iretn;
    if ((iretn=json_out_svector(jstring,value.a)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! Node name to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * Node name.
 * \param jstring JSON stream to append to
 * \param value The JSON data of the desired variable
 * \return  0 if successful, negative error otherwise
*/

int32_t json_out_node(string &jstring, string value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return iretn;

    if ((iretn=json_out_name(jstring, "node_name")) < 0)
        return iretn;
    if ((iretn=json_out_string(jstring, value, COSMOS_MAX_NAME)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! UTC Start to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * UTC start.
 * \param jstring JSON stream to append to
 * \param value The JSON data of the desired variable
 * \return  0 if successful, negative error otherwise
*/

int32_t json_out_utcstart(string &jstring, double value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    if ((iretn=json_out_name(jstring, "node_utcstart")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ECI position to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ECI based position.
 * \param jstring JSON stream to append to
 * \param value The JSON data of the desired variable
 * \return  0 if successful, negative error otherwise
*/

int32_t json_out_ecipos(string &jstring, Convert::cartpos value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Earth Centered Inertial
    if ((iretn=json_out_name(jstring, "node_loc_pos_eci")) < 0)
        return iretn;
    if ((iretn=json_out_cartpos(jstring,value)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::posstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::posstruc.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_posstruc(string &jstring, Convert::posstruc value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Time
    if ((iretn=json_out_name(jstring, "utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Earth Magnetic Field
    if ((iretn=json_out_name(jstring, "bearth")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.bearth)) < 0)
        return iretn;

    // Output Barycentric
    if ((iretn=json_out_name(jstring, "icrf")) < 0)
        return iretn;
    if ((iretn=json_out_cartpos(jstring,value.icrf)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Earth Centered Inertial
    if ((iretn=json_out_name(jstring, "eci")) < 0)
        return iretn;
    if ((iretn=json_out_cartpos(jstring,value.eci)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Selene Centered Inertial
    if ((iretn=json_out_name(jstring, "sci")) < 0)
        return iretn;
    if ((iretn=json_out_cartpos(jstring,value.sci)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Geocentric
    if ((iretn=json_out_name(jstring, "geoc")) < 0)
        return iretn;
    if ((iretn=json_out_cartpos(jstring,value.geoc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Selenocentric
    if ((iretn=json_out_name(jstring, "selc")) < 0)
        return iretn;
    if ((iretn=json_out_cartpos(jstring,value.selc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Geodetic
    if ((iretn=json_out_name(jstring, "geod")) < 0)
        return iretn;
    if ((iretn=json_out_geoidpos(jstring,value.geod)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Selenographic
    if ((iretn=json_out_name(jstring, "selg")) < 0)
        return iretn;
    if ((iretn=json_out_geoidpos(jstring,value.selg)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Geocentric Spherical
    if ((iretn=json_out_name(jstring, "geos")) < 0)
        return iretn;
    if ((iretn=json_out_spherpos(jstring,value.geos)) < 0)
        return iretn;
    //	if ((iretn=json_out_character(jstring,',')) < 0)
    //		return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::attstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::attstruc.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_attstruc(string &jstring, Convert::attstruc value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Time
    if ((iretn=json_out_name(jstring, "utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Topocentric
    if ((iretn=json_out_name(jstring, "topo")) < 0)
        return iretn;
    if ((iretn=json_out_qatt(jstring,value.topo)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output LVLH
    if ((iretn=json_out_name(jstring, "lvlh")) < 0)
        return iretn;
    if ((iretn=json_out_qatt(jstring,value.lvlh)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Geocentric
    if ((iretn=json_out_name(jstring, "geoc")) < 0)
        return iretn;
    if ((iretn=json_out_qatt(jstring,value.geoc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Selenocentric
    if ((iretn=json_out_name(jstring, "selc")) < 0)
        return iretn;
    if ((iretn=json_out_qatt(jstring,value.selc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output ICRF
    if ((iretn=json_out_name(jstring, "icrf")) < 0)
        return iretn;
    if ((iretn=json_out_qatt(jstring,value.icrf)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::locstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::locstruc.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_locstruc(string &jstring, Convert::locstruc value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Time
    if ((iretn=json_out_name(jstring, "utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Position
    if ((iretn=json_out_name(jstring, "pos")) < 0)
        return iretn;
    if ((iretn=json_out_posstruc(jstring,value.pos)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Attitude
    if ((iretn=json_out_name(jstring, "att")) < 0)
        return iretn;
    if ((iretn=json_out_attstruc(jstring,value.att)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! Command event to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::eventstruc specific to a command event.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_commandevent(string &jstring,eventstruc value)
{
    int32_t iretn = 0;

    //CT-JSON: keep as a separate object? or have it combine?
    if ((iretn=json_out_character(jstring, '{')) < 0)
        return iretn;
    if ((iretn=json_out_name(jstring, (char *)"event_utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring, value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return iretn;

    if (value.utcexec != 0.)
    {
        if ((iretn=json_out_character(jstring, '{')) < 0)
            return iretn;
        if ((iretn=json_out_name(jstring, (char *)"event_utcexec")) < 0)
            return iretn;
        if ((iretn=json_out_double(jstring, value.utcexec)) < 0)
            return iretn;
        if ((iretn=json_out_character(jstring, '}')) < 0)
            return iretn;
    }

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return iretn;
    if ((iretn=json_out_name(jstring, (char *)"event_name")) < 0)
        return iretn;
    if ((iretn=json_out_string(jstring, value.name, COSMOS_MAX_NAME)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return iretn;
    if ((iretn=json_out_name(jstring, (char *)"event_type")) < 0)
        return iretn;
    if ((iretn=json_out_uint32(jstring, value.type)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return iretn;
    if ((iretn=json_out_name(jstring, (char *)"event_flag")) < 0)
        return iretn;
    if ((iretn=json_out_uint32(jstring, value.flag)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return iretn;
    if ((iretn=json_out_name(jstring, (char *)"event_data")) < 0)
        return iretn;
    if ((iretn=json_out_string(jstring, value.data, COSMOS_MAX_DATA)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return iretn;

    if (value.flag & EVENT_FLAG_CONDITIONAL)
    {
        if ((iretn=json_out_character(jstring, '{')) < 0)
            return iretn;
        if ((iretn=json_out_name(jstring, (char *)"event_condition")) < 0)
            return iretn;
        if ((iretn=json_out_string(jstring, value.condition, COSMOS_MAX_DATA)) < 0)
            return iretn;
        if ((iretn=json_out_character(jstring, '}')) < 0)
            return iretn;
    }

    return 0;
}

/*! Appends a JSON entry to the current JSON stream for the indicated ::dcmatt.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_dcmatt(string &jstring, Convert::dcmatt value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Position
    if ((iretn=json_out_name(jstring, "pos")) < 0)
        return iretn;
    if ((iretn=json_out_dcm(jstring,value.s)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Velocity
    if ((iretn=json_out_name(jstring, "vel")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.v)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Acceleration
    if ((iretn=json_out_name(jstring, "acc")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.a)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::qatt to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::qatt.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_qatt(string &jstring, Convert::qatt value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Time
    if ((iretn=json_out_name(jstring, "utc")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Position
    if ((iretn=json_out_name(jstring, "pos")) < 0)
        return iretn;
    if ((iretn=json_out_quaternion(jstring,value.s)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Velocity
    if ((iretn=json_out_name(jstring, "vel")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.v)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Acceleration
    if ((iretn=json_out_name(jstring, "acc")) < 0)
        return iretn;
    if ((iretn=json_out_rvector(jstring,value.a)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! ::rmatrix to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::rmatrix.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_dcm(string &jstring,rmatrix value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return iretn;

    // Output Row[0]
    if ((iretn=json_out_rvector(jstring,value.row[0])) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Row[1]
    if ((iretn=json_out_rvector(jstring,value.row[1])) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Row[2]
    if ((iretn=json_out_rvector(jstring,value.row[2])) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,']')) < 0)
        return iretn;
    return 0;
}

//! ::rmatrix to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::rmatrix.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_rmatrix(string &jstring,rmatrix value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return iretn;

    // Output Row[0]
    if ((iretn=json_out_rvector(jstring,value.row[0])) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Row[1]
    if ((iretn=json_out_rvector(jstring,value.row[1])) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Row[2]
    if ((iretn=json_out_rvector(jstring,value.row[2])) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,']')) < 0)
        return iretn;
    return 0;
}

//! ::beatstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::beatstruc.
    \param jstring Reference to JSON stream.
   \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_beatstruc(string &jstring,beatstruc value)
{
    int32_t iretn = 0;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return iretn;

    // Output Ntype
    if ((iretn=json_out_name(jstring, "ntype")) < 0)
        return iretn;
    if ((iretn=json_out_uint16(jstring,(uint16_t)value.ntype)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output IP Address
    if ((iretn=json_out_name(jstring, "addr")) < 0)
        return iretn;
    if ((iretn=json_out_string(jstring,value.addr,16)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output IP Port
    if ((iretn=json_out_name(jstring, "port")) < 0)
        return iretn;
    if ((iretn=json_out_int32(jstring,value.port)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Buffer Size
    if ((iretn=json_out_name(jstring, "bsz")) < 0)
        return iretn;
    if ((iretn=json_out_int32(jstring,value.bsz)) < 0)
        return iretn;
    if ((iretn=json_out_character(jstring,',')) < 0)
        return iretn;

    // Output Beat Period
    if ((iretn=json_out_name(jstring, "bprd")) < 0)
        return iretn;
    if ((iretn=json_out_double(jstring,value.bprd)) < 0)
        return iretn;

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return iretn;
    return 0;
}

//! Perform JSON output for a single element of a 1D named JSON item.
/*! Calls ::json_out for the indexed element of the named JSON vector.
    \param jstring Reference to JSON stream.
    \param token The JSON name for the desired variable
    \param index The desired element number

    \param cinfo Reference to ::cosmosstruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_1d(string &jstring, const char *token, uint16_t index, cosmosstruc *cinfo)
{
    char name[COSMOS_MAX_NAME+1];
    int32_t iretn = 0;

    if (strlen(token) > COSMOS_MAX_NAME+4)
        return (JSON_ERROR_NAME_LENGTH);

    if (index > 999)
        return (JSON_ERROR_INDEX_SIZE);

    sprintf(name,"%s_%03u",token,index);
    iretn = json_out(jstring, name, cinfo);
    return iretn;
}

//! Perform JSON output for a single element of a 2D named JSON item.
/*! Calls ::json_out for the indexed element of the named JSON matrix.
    \param jstring Reference to JSON stream.
    \param token The JSON name for the desired variable
    \param row The desired row number
    \param col The desired column number

    \param cinfo Reference to ::cosmosstruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_2d(string &jstring, const char *token, uint16_t row, uint16_t col, cosmosstruc *cinfo)
{
    char name[COSMOS_MAX_NAME+1];
    int32_t iretn = 0;

    if (strlen(token) > COSMOS_MAX_NAME+8)
        return (JSON_ERROR_NAME_LENGTH);

    if (row > 999 || col > 999)
        return (JSON_ERROR_INDEX_SIZE);

    sprintf(name,"%s_%03u_%03u",token,row,col);
    iretn = json_out(jstring, name, cinfo);
    return iretn;
}

//! Perform JSON output for a single named JSON item
/*! Populates the jstring for the indicated ::jsonmap through reference to JSON name.
    \param jstring The jstring into which to store the result.
    \param token The JSON name for the desired variable.

    \param cinfo Reference to ::cosmosstruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out(string &jstring, string token, cosmosstruc *cinfo)
{
    jsonhandle h;

    if (!cinfo->jmapped)
        return (JSON_ERROR_NOJMAP);

    h.hash = json_hash(token);

    if (cinfo->jmap.size() == 0)
        return (JSON_ERROR_NOJMAP);

    for (h.index=0; h.index < cinfo->jmap[h.hash].size(); ++h.index)
        //		if (!strcmp(token.c_str(), cinfo->jmap[h.hash][h.index].name))
        if (token == cinfo->jmap[h.hash][h.index].name)
        {
            return (json_out_handle(jstring, h, cinfo));
        }

    return (JSON_ERROR_NOENTRY);
}

//! Output a list of named JSON items.
/*! Populates the jstring for the indicated ::jsonmap through reference to a list of JSON
 * names.
    \param jstring The jstring into which to store the result.
    \param tokens The comma separated list of JSON names for the desired variables.

    \param cinfo Reference to ::cosmosstruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_list(string &jstring,string tokens, cosmosstruc *cinfo)
{
    string tstring;
    const char* ptr;
    int32_t iretn = 0;

    ptr = &tokens[0];
    while (ptr[0] != 0 && ptr[0] != '{')
    {
        ptr++;
    }
    if ((iretn=json_skip_character(ptr,'{')) != 0)
    {
        return iretn;
    }
    do
    {
        if ((iretn=json_extract_string(ptr, tstring)) != 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        json_out(jstring, tstring.c_str(), cinfo);
    } while (!json_skip_character(ptr,','));
    if ((iretn = json_skip_white(ptr)) < 0)
        return iretn;
    if ((iretn=json_skip_character(ptr,'}')) != 0 && iretn!=JSON_ERROR_EOS)
        return iretn;
    if ((iretn = json_skip_white(ptr)) < 0 && iretn!=JSON_ERROR_EOS)
        return iretn;
    return 0;
}


//! Output JSON items from wildcard.
/*! Populates the jstring for the indicated ::jsonmap through reference to a regular expression matching of JSON
 * names.
    \param jstring The jstring into which to store the result.
    \param wildcard The regular expression to match against.

    \param cinfo Reference to ::cosmosstruc to use.
    \return  0 if successful, negative error otherwise
*/

int32_t json_out_wildcard(string &jstring, string wildcard, cosmosstruc *cinfo)
{
    int32_t iretn=0;
    jsonhandle h;

    for (h.hash=0; h.hash<cinfo->jmap.size(); ++h.hash)
    {
        for (h.index=0; h.index<cinfo->jmap[h.hash].size(); ++h.index)
        {
            if (string_cmp(wildcard.c_str(), cinfo->jmap[h.hash][h.index].name.c_str()))
            {
                iretn = json_out_handle(jstring, h, cinfo);
            }
        }
    }
    return iretn;
}

//! Address from entry
/*! Calculate the actual address of a value from its entry in the ::jmap.
 \param entry A ::jsonentry

    \param cinfo Reference to ::cosmosstruc to use.
 \return A pointer, castable into any desired type.
*/
uint8_t *json_ptr_of_entry(const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *data=nullptr;

    if (entry.group == JSON_STRUCT_PTR)
    {
        data = entry.ptr;
    }
    else
    {
        data = json_ptr_of_offset(entry.offset, entry.group, cinfo);
    }

    return data;
}

//! Address from offset
/*! Calculate the actual address of an offset into either static or
 * dynamic space, using the provide cdata static and dynamic addresses.
 \param offset An offset taken from a ::jsonentry
 \param group The structure group from which the offset is measured.

    \param cinfo Reference to ::cosmosstruc to use.
 \return A pointer, castable into any desired type.
*/
uint8_t *json_ptr_of_offset(ptrdiff_t offset, uint16_t group, cosmosstruc *cinfo)
{
    uint8_t *data=nullptr;

    switch (group)
    {
    case JSON_STRUCT_ABSOLUTE:
        data = offset + (uint8_t *)cinfo;
        break;
    case JSON_STRUCT_AGENT:
        data =  offset + (uint8_t *)cinfo->agent.data();
        break;
    case JSON_STRUCT_PHYSICS:
        data =  offset + (uint8_t *)&(cinfo->node.phys);
        break;
    case JSON_STRUCT_EVENT:
        data =  offset + (uint8_t *)cinfo->event.data();
        break;
    case JSON_STRUCT_NODE:
        data =  offset + (uint8_t *)&(cinfo->node);
        break;
    case JSON_STRUCT_DEVICE:
        data =  offset + (uint8_t *)cinfo->device.data();
        break;
    case JSON_STRUCT_DEVSPEC:
        data =  offset + (uint8_t *)&(cinfo->devspec);
        break;
    case JSON_STRUCT_PIECE:
        data = offset + (uint8_t *)cinfo->pieces.data();
        break;
    case JSON_STRUCT_TARGET:
        data = offset + (uint8_t *)cinfo->target.data();
        break;
    case JSON_STRUCT_USER:
        data = offset + (uint8_t *)cinfo->user.data();
        break;
    case JSON_STRUCT_PORT:
        data =  offset + (uint8_t *)cinfo->port.data();
        break;
    case JSON_STRUCT_TLE:
        data =  offset + (uint8_t *)cinfo->tle.data();
        break;
    case JSON_STRUCT_ALIAS:
        data = (uint8_t *)&cinfo->alias[(size_t)offset];
        break;
    case JSON_STRUCT_EQUATION:
        data = (uint8_t *)&cinfo->equation[offset];
        break;
    }
    return (data);
}

//! Output a vector of JSON entries.
/*! Populates the provided vector for the indicated ::jsonmap through reference to a list of JSON
 * names.
    \param table The vector of ::jsonentry into which to store the result.
    \param tokens The comma separated list of JSON names for the desired variables.

    \return  0 if successful, negative error otherwise
*/
int32_t json_table_of_list(vector<jsonentry*> &table, string tokens, cosmosstruc *cinfo)
{
    string tstring;
    const char *ptr;
    int32_t iretn = 0;
    jsonentry* tentry;

    ptr = &tokens[0];
    while (ptr[0] != 0 && ptr[0] != '{') //move forward until '{'
        ptr++;
    if ((iretn=json_skip_character(ptr,'{')) != 0) //skip over '{'
        return iretn;
    do
    {
        if ((iretn=json_extract_string(ptr, tstring)) != 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        tentry = json_entry_of(tstring, cinfo);
        table.push_back(tentry);
    } while (!json_skip_character(ptr,','));
    if ((iretn = json_skip_white(ptr)) < 0)
        return iretn;
    if ((iretn=json_skip_character(ptr,'}')) != 0 && iretn!=JSON_ERROR_EOS)
        return iretn;
    if ((iretn = json_skip_white(ptr)) < 0 && iretn!=JSON_ERROR_EOS)
        return iretn;
    return 0;
}

//! Info on Namespace address
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a the namespace value that matches a given memory address.
 \param ptr Address of a variable that may match a namespace name.

    \param cinfo Reference to ::cosmosstruc to use.
 \return Pointer to the ::jsonentry for the token, or nullptr.
*/
jsonentry *json_entry_of(uint8_t *ptr, cosmosstruc *cinfo)
{
    uint16_t m;
    uint16_t group = UINT16_MAX;
    ptrdiff_t offset;

    if (!cinfo->jmapped)
        return nullptr;

    offset = -1;

    if ((offset = (uint8_t *)ptr - (uint8_t *)&(cinfo->node)) > 0 && offset < (ptrdiff_t)sizeof(nodestruc))
    {
        group = JSON_STRUCT_NODE;
    }
    else
    {
        offset = -1;
        //#undef max;
        offset = (numeric_limits<ptrdiff_t>::max)();

    }
    if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cinfo->node)) > 0 && offset < (ptrdiff_t)sizeof(agentstruc))
    {
        group = JSON_STRUCT_AGENT;
    }
    else
    {
        offset = -1;
    }
    if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cinfo->node)) > 0 && offset < (ptrdiff_t)sizeof(physicsstruc))
    {
        group = JSON_STRUCT_PHYSICS;
    }
    else
    {
        offset = -1;
    }
    if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cinfo->node)) > 0 && offset < (ptrdiff_t)sizeof(eventstruc))
    {
        group = JSON_STRUCT_EVENT;
    }
    else
    {
        offset = -1;
    }

    if (offset == -1)
        return ((jsonentry *)nullptr);

    for (m=0; m<cinfo->jmap.size(); m++)
    {
        for (size_t n=0; n<cinfo->jmap[m].size(); n++)
        {
            if (cinfo->jmap[m][n].group == group && cinfo->jmap[m][n].offset == offset)
            {
                return ((jsonentry *)&cinfo->jmap[m][n]);
            }
        }
    }
    return ((jsonentry *)nullptr);
}

//! Info on Namespace name
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a given name.
 \param token Namespace name to look up

 \return Pointer to the ::jsonentry for the token, or nullptr.
*/
jsonentry *json_entry_of(string token, cosmosstruc *cinfo)
{
    int16_t hash;
    //    uint16_t n;

    if (!cinfo->jmapped)
        return nullptr;

    hash = json_hash(token);

    if (cinfo->jmap[hash].size() == 0)
        return ((jsonentry *)nullptr);

    for (size_t n=0; n<cinfo->jmap[hash].size(); n++)
    {
        if (token == cinfo->jmap[hash][n].name)
        {
            return ((jsonentry *)&cinfo->jmap[hash][n]);
        }
    }
    return ((jsonentry *)nullptr);
}

//! Info on Namespace equation
/*! Return a pointer to the Namespace Equation structure containing the
 * information for a given equation.
 \param handle ::jsonhandle for the entry in the global emap

 \return Pointer to the ::jsonequation for the token, or nullptr.
*/
jsonequation *json_equation_of(jsonhandle handle, cosmosstruc *cinfo)
{
    if (!cinfo->jmapped || handle.hash >= cinfo->emap.size())
        return nullptr;

    if (cinfo->emap[handle.hash].size() > handle.index)
    {
        return ((jsonequation *)&cinfo->emap[handle.hash][handle.index]);
    }
    return ((jsonequation *)nullptr);
}

//! Info on Namespace name
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a given name.
 \param handle ::jsonhandle for the entry in the global jmap

 \return Pointer to the ::jsonentry for the token, or nullptr.
*/
jsonentry *json_entry_of(jsonhandle handle, cosmosstruc *cinfo)
{
    if (!cinfo->jmapped || handle.hash >= cinfo->jmap.size())
        return nullptr;

    if (cinfo->jmap[handle.hash].size() > handle.index)
    {
        return ((jsonentry *)&cinfo->jmap[handle.hash][handle.index]);
    }
    return ((jsonentry *)nullptr);
}

//! Type of namespace name.
/*! Return the \ref jsonlib_type of the token in the ::jsonmap.
 \param token the JSON name for the desired variable

 \return The \ref jsonlib_type, otherwise 0.
*/
uint16_t json_type_of_name(string token, cosmosstruc *cinfo)
{
    jsonentry *entry;

    if (token[0] == '(')
        return (JSON_TYPE_EQUATION);

    if ((entry=json_entry_of(token, cinfo)) != nullptr)
    {
        return (entry->type);
    }
    return 0;
}

//! Return the data pointer that matches a JSON name
/*! Look up the provided JSON data name in the indicated ::jsonmap and return the
 * associated data pointer.
 \param token the JSON name for the desired variable

    \param cinfo Reference to ::cosmosstruc to use.
 \return The associated data pointer, if succesful, otherwise nullptr
*/
uint8_t *json_ptrto(string token, cosmosstruc *cinfo)
{
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cinfo)) == nullptr)
    {
        return ((uint8_t *)nullptr);
    }
    else
    {
        if (ptr->group == JSON_STRUCT_PTR)
        {
            return ptr->ptr;
        }
        else
        {
            return json_ptr_of_entry(*ptr, cinfo);
        }
    }
}

//! Return the data pointer that matches a singly indexed JSON name
/*! Look up the provided JSON data name with the indicated index in the current ::jsonmap
 * and return the associated data pointer.
    \param token the JSON name for the desired variable
    \param index1 Primary index.

    \param cinfo Reference to ::cosmosstruc to use.
    \return The associated data pointer, if succesful, otherwise nullptr
*/
uint8_t *json_ptrto_1d(string token, uint16_t index1, cosmosstruc *cinfo)
{
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return ((uint8_t *)nullptr);

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(),index1);

    return (json_ptrto(token, cinfo));

}

//! Return the data pointer that matches a doubly indexed JSON name
/*! Look up the provided JSON data name with the indicated indices in the current ::jsonmap
 * and return the associated data pointer.
    \param token the JSON name for the desired variable
    \param index1 Primary index.
    \param index2 Secondary index.

    \param cinfo Reference to ::cosmosstruc to use.
    \return The associated data pointer, if succesful, otherwise nullptr
*/
uint8_t *json_ptrto_2d(string token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo)
{
    char tstring[9+COSMOS_MAX_NAME];

    // Return error if index too large
    if (index1 > 999 || index2 > 999)
        return ((uint8_t *)nullptr);

    // Create extended name, shortening if necessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);

    return (json_ptrto(token, cinfo));

}

//! Return integer from handle.
/*! If the value at this ::jsonhandle can in any way be interepreted as a number,
 * return it as an int32_t.
 \param handle ::jsonhandle for a valid COSMOS Namespace entry.

    \param cinfo Reference to ::cosmosstruc to use.
 \return Value cast as an int32_t, or 0.
*/
int32_t json_get_int(jsonhandle &handle, cosmosstruc *cinfo)
{
    int32_t value=0;

    if (cinfo->jmap[handle.hash].size() <= handle.index)
    {
        return 0;
    }

    value = json_get_int(cinfo->jmap[handle.hash][handle.index], cinfo);
    return value;
}

//! Return integer from entry.
/*! If the value stored in this ::jsonentry can in any way be interepreted as a number,
 * return it as an int32_t.
 \param entry A valid COSMOS Namespace entry.

    \param cinfo Reference to ::cosmosstruc to use.
 \return Value cast as an int32_t, or 0.
*/
int32_t json_get_int(const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *dptr=nullptr;
    int32_t value=0;

    dptr = json_ptr_of_entry(entry, cinfo);
    if (dptr == nullptr)
    {
        return 0;
    }
    else
    {
        switch (entry.type)
        {
        case JSON_TYPE_UINT16:
            value = (int32_t)(*(uint16_t *)(dptr));
            break;
        case JSON_TYPE_UINT32:
            value = (int32_t)(*(uint32_t *)(dptr));
            break;
        case JSON_TYPE_INT16:
            value = (int32_t)(*(int16_t *)(dptr));
            break;
        case JSON_TYPE_INT32:
            value = (int32_t)(*(int32_t *)(dptr));
            break;
        case JSON_TYPE_FLOAT:
            value = (int32_t)(*(float *)(dptr));
            break;
        case JSON_TYPE_DOUBLE:
        case JSON_TYPE_TIMESTAMP:
            value = (int32_t)(*(double *)(dptr) + .5);
            break;
        case JSON_TYPE_STRING:
            value = stoi(*(string *)(dptr));
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value = (int32_t)json_equation(tpointer, cinfo);
            }
            break;
        case JSON_TYPE_ALIAS:
            {
                jsonentry *eptr;
                if ((eptr=json_entry_of((*(jsonhandle *)(dptr)), cinfo)) == nullptr)
                {
                    value =  0;
                }
                else
                {
                    value = json_get_int(*eptr, cinfo);
                }
            }
            break;
        }
        return value;
    }
}

//! Return integer from name.
/*! If the named value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.

    \param cinfo Reference to ::cosmosstruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int(string token, cosmosstruc *cinfo)
{
    int32_t value=0;
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cinfo)) == nullptr)
    {
        return 0;
    }

    value = json_get_int(*ptr, cinfo);
    return (value);

}

//! Return integer from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.

    \param cinfo Reference to ::cosmosstruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int(string token, uint16_t index1, cosmosstruc *cinfo)
{
    int32_t value;
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(), index1);

    value = json_get_int(tstring, cinfo);

    return (value);
}

//! Return integer from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.

    \param cinfo Reference to ::cosmosstruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int(string token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo)
{
    int32_t value;
    char tstring[9+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);

    value = json_get_int(tstring, cinfo);

    return (value);
}

//! Return unsigned integer from handle.
/*! If the value at this ::jsonhandle can in any way be interepreted as a number,
 * return it as an uint32_t.
 \param handle ::jsonhandle for a valid COSMOS Namespace entry.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as an uint32_t, or 0.
*/
uint32_t json_get_uint(jsonhandle &handle, cosmosstruc *cinfo)
{
    uint32_t value=0;

    if (cinfo->jmap[handle.hash].size() <= handle.index)
    {
        return 0;
    }

    value = json_get_uint(cinfo->jmap[handle.hash][handle.index], cinfo);
    return value;
}

//! Return unsigned integer from entry.
/*! If the value stored in this ::jsonentry can in any way be interepreted as a number,
 * return it as an uint32_t.
 \param entry A valid COSMOS Namespace entry.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as an uint32_t, or 0.
*/
uint32_t json_get_uint(const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *dptr=nullptr;
    uint32_t value=0;

    dptr = json_ptr_of_entry(entry, cinfo);
    if (dptr == nullptr)
    {
        return 0;
    }
    else
    {
        switch (entry.type)
        {
        case JSON_TYPE_UINT16:
            value = (uint32_t)(*(uint16_t *)(dptr));
            break;
        case JSON_TYPE_UINT32:
            value = (uint32_t)(*(uint32_t *)(dptr));
            break;
        case JSON_TYPE_INT16:
            value = (uint32_t)(*(int16_t *)(dptr));
            break;
        case JSON_TYPE_INT32:
            value = (uint32_t)(*(int32_t *)(dptr));
            break;
        case JSON_TYPE_FLOAT:
            value = (uint32_t)(*(float *)(dptr));
            break;
        case JSON_TYPE_DOUBLE:
        case JSON_TYPE_TIMESTAMP:
            value = (uint32_t)(*(double *)(dptr) + .5);
            break;
        case JSON_TYPE_STRING:
            value = stoi(*(string *)(dptr));
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value = (uint32_t)json_equation(tpointer, cinfo);
            }
            break;
        case JSON_TYPE_ALIAS:
            {
                aliasstruc *aptr = (aliasstruc *)dptr;
                switch (aptr->type)
                {
                case JSON_TYPE_EQUATION:
                    {
                        jsonequation *eptr;
                        if ((eptr=json_equation_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_equation(eptr, cinfo);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_get_uint(*eptr, cinfo);
                        }
                    }
                    break;
                }
            }
            break;
        }
        return value;
    }
}

//! Return unsigned integer from name.
/*! If the named value can in any way be interepreted as a number,
 * return it as a signed 32 bit unsigned integer.
 \param token Valid COSMOS Namespace name.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a signed 32 bit unsigned integer, or 0.
*/
uint32_t json_get_uint(string token, cosmosstruc *cinfo)
{
    uint32_t value=0;
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cinfo)) == nullptr)
    {
        return 0;
    }

    value = json_get_uint(*ptr, cinfo);
    return (value);

}

//! Return unsigned integer from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit unsigned integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a signed 32 bit unsigned integer, or 0.
*/
uint32_t json_get_uint(string token, uint16_t index1, cosmosstruc *cinfo)
{
    uint32_t value;
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(), index1);

    value = json_get_uint(tstring, cinfo);

    return (value);
}

//! Return unsigned integer from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit unsigned integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a signed 32 bit unsigned integer, or 0.
*/
uint32_t json_get_uint(string token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo)
{
    uint32_t value;
    char tstring[9+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);

    value = json_get_uint(tstring, cinfo);

    return (value);
}

//! Return double from handle.
/*! If the value at this ::jsonhandle can in any way be interepreted as a number,
 * return it as a double.
 \param handle ::jsonhandle for a valid COSMOS Namespace entry.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as an double, or 0.
*/
double json_get_double(jsonhandle &handle, cosmosstruc *cinfo)
{
    double value=0.;

    if (cinfo->jmap[handle.hash].size() <= handle.index)
    {
        return 0.;
    }

    value = json_get_double(cinfo->jmap[handle.hash][handle.index], cinfo);
    return value;
}

//! Return double from name.
/*! If the named value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(string token, cosmosstruc *cinfo)
{
    double value=0.;
    jsonentry *entry;
    const char* tokenp = &token[0];

    if (!std::isnan(value=json_equation(tokenp, cinfo)))
        return (value);

    if ((entry=json_entry_of(token, cinfo)) == nullptr)
    {
        return (NAN);
    }

    if (!std::isnan(value=json_get_double(*entry, cinfo)))
        return (value);

    return (NAN);
}

//! Return double from entry.
/*! If the named value can in any way be interepreted as a number,
 * return it as a double.
 \param entry Pointer to a valid ::jsonentry.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *dptr=nullptr;
    double value=0.;

    dptr = json_ptr_of_entry(entry, cinfo);
    if (dptr == nullptr)
    {
        return 0.;
    }
    else
    {
        switch (entry.type)
        {
        case JSON_TYPE_UINT16:
            value = (double)(*(uint16_t *)(dptr));
            break;
        case JSON_TYPE_UINT32:
            value = (double)(*(uint32_t *)(dptr));
            break;
        case JSON_TYPE_INT16:
            value = (double)(*(int16_t *)(dptr));
            break;
        case JSON_TYPE_INT32:
            value = (double)(*(int32_t *)(dptr));
            break;
        case JSON_TYPE_FLOAT:
            value = (double)(*(float *)(dptr));
            break;
        case JSON_TYPE_DOUBLE:
        case JSON_TYPE_TIMESTAMP:
            value = (double)(*(double *)(dptr));
            break;
        case JSON_TYPE_STRING:
            value = stof(*(string *)dptr);
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value = (double)json_equation(tpointer, cinfo);
            }
            break;
        case JSON_TYPE_ALIAS:
            {
                aliasstruc *aptr = (aliasstruc *)dptr;
                switch (aptr->type)
                {
                case JSON_TYPE_EQUATION:
                    {
                        jsonequation *eptr;
                        if ((eptr=json_equation_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_equation(eptr, cinfo);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_get_double(*eptr, cinfo);
                        }
                    }
                    break;
                }
            }
            break;
        default:
            value = 0;
        }

        return value;
    }
}

//! Return rvector from entry.
/*! If the named value can in any way be interepreted as three numbers,
 * return it as an rvector.
 \param entry Pointer to a valid ::jsonentry..

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as an rvector, or 0.
*/
rvector json_get_rvector(const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *dptr=nullptr;
    rvector value;

    dptr = json_ptr_of_entry(entry, cinfo);
    if (dptr == nullptr)
    {
        return value;
    }
    else
    {
        switch (entry.type)
        {
        case JSON_TYPE_SPHERPOS:
        case JSON_TYPE_POS_GEOS:
            {
                Convert::spherpos tpos = (Convert::spherpos)(*(Convert::spherpos *)(dptr));
                value.col[0] = tpos.s.phi;
                value.col[1] = tpos.s.lambda;
                value.col[2] = tpos.s.r;
            }
            break;
        case JSON_TYPE_GEOIDPOS:
        case JSON_TYPE_POS_GEOD:
        case JSON_TYPE_POS_SELG:
            {
                Convert::geoidpos tpos = (Convert::geoidpos)(*(Convert::geoidpos *)(dptr));
                value.col[0] = tpos.s.lat;
                value.col[1] = tpos.s.lon;
                value.col[2] = tpos.s.h;
            }
            break;
        case JSON_TYPE_CARTPOS:
        case JSON_TYPE_POS_GEOC:
        case JSON_TYPE_POS_SELC:
        case JSON_TYPE_POS_ECI:
        case JSON_TYPE_POS_SCI:
        case JSON_TYPE_POS_ICRF:
            {
                Convert::cartpos tpos = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
                value = tpos.s;
            }
            break;
        case JSON_TYPE_VECTOR:
        case JSON_TYPE_RVECTOR:
            //        case JSON_TYPE_TVECTOR:
        case JSON_TYPE_CVECTOR:
        case JSON_TYPE_SVECTOR:
        case JSON_TYPE_AVECTOR:
            value = (rvector)(*(rvector *)(dptr));
            break;
        case JSON_TYPE_UINT16:
            value.col[0] = (double)(*(uint16_t *)(dptr));
            break;
        case JSON_TYPE_UINT32:
            value.col[0] = (double)(*(uint32_t *)(dptr));
            break;
        case JSON_TYPE_INT16:
            value.col[0] = (double)(*(int16_t *)(dptr));
            break;
        case JSON_TYPE_INT32:
            value.col[0] = (double)(*(int32_t *)(dptr));
            break;
        case JSON_TYPE_FLOAT:
            value.col[0] = (double)(*(float *)(dptr));
            break;
        case JSON_TYPE_DOUBLE:
        case JSON_TYPE_TIMESTAMP:
            value.col[0] = (double)(*(double *)(dptr));
            break;
        case JSON_TYPE_STRING:
            value.col[0] = stof(*(string *)dptr);
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value.col[0] = (double)json_equation(tpointer, cinfo);
            }
            break;
        case JSON_TYPE_ALIAS:
            {
                aliasstruc *aptr = (aliasstruc *)dptr;
                switch (aptr->type)
                {
                case JSON_TYPE_EQUATION:
                    {
                        jsonequation *eptr;
                        if ((eptr=json_equation_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value.col[0] =  0;
                        }
                        else
                        {
                            value.col[0] = json_equation(eptr, cinfo);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value.col[0] =  0;
                        }
                        else
                        {
                            value.col[0] = json_get_double(*eptr, cinfo);
                        }
                    }
                    break;
                }
            }
            break;
        default:
            value.col[0] = 0.;
        }

        return value;
    }
}

//! Return quaternion from entry.
/*! If the named value can in any way be interepreted as three numbers,
 * return it as an quaternion.
 \param entry Pointer to a valid ::jsonentry.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as an quaternion, or 0.
*/
quaternion json_get_quaternion(const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *dptr=nullptr;
    quaternion value={{0., 0., 0.}, 0.};

    dptr = json_ptr_of_entry(entry, cinfo);
    if (dptr == nullptr)
    {
        return value;
    }
    else
    {
        switch (entry.type)
        {
        case JSON_TYPE_QATT:
        case JSON_TYPE_QATT_GEOC:
        case JSON_TYPE_QATT_SELC:
        case JSON_TYPE_QATT_ICRF:
        case JSON_TYPE_QATT_LVLH:
        case JSON_TYPE_QATT_TOPO:
            {
                value = (quaternion)(*(quaternion *)(dptr));
            }
            break;
        case JSON_TYPE_QUATERNION:
            value = (quaternion)(*(quaternion *)(dptr));
            break;
        case JSON_TYPE_RVECTOR:
            //        case JSON_TYPE_TVECTOR:
            {
                rvector tvalue = (rvector)(*(rvector *)(dptr));
                value.d.x = tvalue.col[0];
                value.d.y = tvalue.col[1];
                value.d.z = tvalue.col[2];
            }
            break;
        case JSON_TYPE_GVECTOR:
            {
                gvector tvalue = (gvector)(*(gvector *)(dptr));
                value.d.x = tvalue.lat;
                value.d.y = tvalue.lon;
                value.d.z = tvalue.h;
            }
            break;
        case JSON_TYPE_SVECTOR:
            {
                svector tvalue = (svector)(*(svector *)(dptr));
                value.d.x = tvalue.phi;
                value.d.y = tvalue.lambda;
                value.d.z = tvalue.r;
            }
            break;
        case JSON_TYPE_AVECTOR:
            {
                avector tvalue = (avector)(*(avector *)(dptr));
                value.d.x = tvalue.h;
                value.d.y = tvalue.e;
                value.d.z = tvalue.b;
            }
            break;
        case JSON_TYPE_UINT16:
            value.d.x = (double)(*(uint16_t *)(dptr));
            break;
        case JSON_TYPE_UINT32:
            value.d.x = (double)(*(uint32_t *)(dptr));
            break;
        case JSON_TYPE_INT16:
            value.d.x = (double)(*(int16_t *)(dptr));
            break;
        case JSON_TYPE_INT32:
            value.d.x = (double)(*(int32_t *)(dptr));
            break;
        case JSON_TYPE_FLOAT:
            value.d.x = (double)(*(float *)(dptr));
            break;
        case JSON_TYPE_DOUBLE:
        case JSON_TYPE_TIMESTAMP:
            value.d.x = (double)(*(double *)(dptr));
            break;
        case JSON_TYPE_STRING:
            value.d.x = stof(*(string *)dptr);
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value.d.x = (double)json_equation(tpointer, cinfo);
            }
            break;
        case JSON_TYPE_ALIAS:
            {
                aliasstruc *aptr = (aliasstruc *)dptr;
                switch (aptr->type)
                {
                case JSON_TYPE_EQUATION:
                    {
                        jsonequation *eptr;
                        if ((eptr=json_equation_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value.d.x =  0;
                        }
                        else
                        {
                            value.d.x = json_equation(eptr, cinfo);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cinfo)) == nullptr)
                        {
                            value.d.x =  0;
                        }
                        else
                        {
                            value.d.x = json_get_double(*eptr, cinfo);
                        }
                    }
                    break;
                }
            }
            break;
        default:
            value.d.x = 0.;
        }

        return value;
    }
}

//! Return double from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(string token, uint16_t index1, cosmosstruc *cinfo)
{
    double value;
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(), index1);


    value = json_get_double(tstring, cinfo);

    return (value);
}

//! Return double from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(string token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo)
{
    double value;
    char tstring[9+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);


    value = json_get_double(tstring, cinfo);

    return (value);
}

//! Return string from name.
/*! If the named value is a string, just copy it. Otherwise, print
whatever numerical value as a string. Return a pointer to an internal
storage buffer for the string. Note: this value is changed each time
you call this function.
    \param token Valid COSMOS Namespace name.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Pointer to a char buffer containing the string.
*/
string json_get_string(string token, cosmosstruc *cinfo)
{
    jsonentry *ptr;
    string tstring;

    if ((ptr=json_entry_of(token, cinfo)) != nullptr)
    {
        tstring = json_get_string(*ptr, cinfo);
    }

    return tstring;
}

//! Return string from entry.
/*! If the named value is a string, just copy it. Otherwise, print
whatever numerical value as a string. Return a pointer to an internal
storage buffer for the string. Note: this value is changed each time
you call this function.
    \param ptr Pointer to a valid ::jsonentry..

    \param cinfo Reference to ::cosmosstruc to use.
    \return Pointer to a char buffer containing the string.
*/
string json_get_string(const jsonentry &entry, cosmosstruc *cinfo)
{
    string tstring;
    //char tbuf[200];
    char tbuf[232];

    //    if (entry == nullptr)
    //    {
    //        return (tstring);
    //    }

    switch (entry.type)
    {
    case JSON_TYPE_UINT16:
        sprintf(tbuf,"%u",(*(uint16_t *)(json_ptr_of_entry(entry, cinfo))));
        tstring = tbuf;
        break;
    case JSON_TYPE_UINT32:
        sprintf(tbuf,"%u",(*(uint32_t *)(json_ptr_of_entry(entry, cinfo))));
        tstring = tbuf;
        break;
    case JSON_TYPE_INT16:
        sprintf(tbuf,"%d",(*(int16_t *)(json_ptr_of_entry(entry, cinfo))));
        tstring = tbuf;
        break;
    case JSON_TYPE_INT32:
        sprintf(tbuf,"%d",(*(int32_t *)(json_ptr_of_entry(entry, cinfo))));
        tstring = tbuf;
        break;
    case JSON_TYPE_FLOAT:
        sprintf(tbuf,"%.8g",(*(float *)(json_ptr_of_entry(entry, cinfo))));
        tstring = tbuf;
        break;
    case JSON_TYPE_DOUBLE:
    case JSON_TYPE_TIMESTAMP:
        sprintf(tbuf,"%.17g",(*(double *)(json_ptr_of_entry(entry, cinfo))) + .5);
        tstring = tbuf;
        break;
    case JSON_TYPE_STRING:
        tstring = *(string *)(json_ptr_of_entry(entry, cinfo));
        break;
    case JSON_TYPE_NAME:
        tstring = (char *)(json_ptr_of_entry(entry, cinfo));
        break;
    case JSON_TYPE_POS_ECI:
        Convert::cartpos tval = (*(Convert::cartpos *)(json_ptr_of_entry(entry, cinfo)));
        sprintf(tbuf, "[%.17g %.17g %.17g] [%.17g %.17g %.17g] [%.17g %.17g %.17g]",
                tval.s.col[0], tval.s.col[1], tval.s.col[2],
                tval.v.col[0], tval.v.col[1], tval.v.col[2],
                tval.a.col[0], tval.a.col[1], tval.a.col[2]);
        tstring = tbuf;
        break;
    }

    return (tstring);
}

//! Return ::posstruc from entry.
/*! If the named value can in any way be interepreted as a ::posstruc,
 * return it as a Convert::posstruc.
 \param entry Pointer to a valid ::jsonentry..

    \param cinfo Reference to ::cosmosstruc to use.

 \return Value cast as a ::posstruc, or 0.
*/
Convert::posstruc json_get_posstruc(const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *dptr=nullptr;
    Convert::locstruc value;

    memset((void *) &value, 0, COSMOS_SIZEOF(Convert::locstruc));

    dptr = json_ptr_of_entry(entry, cinfo);
    if (dptr == nullptr)
    {
        return value.pos;
    }
    else
    {
        switch (entry.type)
        {
        case JSON_TYPE_POSSTRUC:
            {
                value.pos = (Convert::posstruc)(*(Convert::posstruc *)(dptr));
            }
            break;
        case JSON_TYPE_RVECTOR:
            {
                value.pos.eci.s = (rvector)(*(rvector *)(dptr));
                value.pos.eci.v = rv_zero();
                value.pos.eci.a = rv_zero();
                ++value.pos.eci.pass;
                Convert::pos_eci(&value);
            }
            break;
        case JSON_TYPE_CARTPOS:
            {
                value.pos.eci = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
                ++value.pos.eci.pass;
                Convert::pos_eci(&value);
            }
            break;
        case JSON_TYPE_POS_GEOC:
            {
                value.pos.geoc = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
                ++value.pos.geoc.pass;
                Convert::pos_geoc(&value);
            }
            break;
        case JSON_TYPE_POS_GEOD:
            {
                value.pos.geod = (Convert::geoidpos)(*(Convert::geoidpos *)(dptr));
                ++value.pos.geod.pass;
                Convert::pos_geod(&value);
            }
            break;
        case JSON_TYPE_POS_SELC:
            {
                value.pos.selc = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
                ++value.pos.selc.pass;
                Convert::pos_selc(&value);
            }
            break;
        case JSON_TYPE_POS_SELG:
            {
                value.pos.selg = (Convert::geoidpos)(*(Convert::geoidpos *)(dptr));
                ++value.pos.selg.pass;
                Convert::pos_selg(&value);
            }
            break;
        case JSON_TYPE_POS_ECI:
            {
                value.pos.eci = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
                ++value.pos.eci.pass;
                Convert::pos_eci(&value);
            }
            break;
        case JSON_TYPE_POS_SCI:
            {
                value.pos.sci = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
                ++value.pos.sci.pass;
                Convert::pos_sci(&value);
            }
            break;
        case JSON_TYPE_POS_ICRF:
            {
                value.pos.icrf = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
                ++value.pos.icrf.pass;
                Convert::pos_icrf(&value);
            }
            break;
        }

        return value.pos;
    }
}

//! Set name from double.
/*! If the provided double can in any way be placed in the name it
 * will.
 \param value Double precision value to be stored in the name space.
 \param token Valid COSMOS Namespace name.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Negative error or zero.
*/
int32_t json_set_double_name(double value, char *token, cosmosstruc *cinfo)
{
    utype *nval;
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cinfo)) == nullptr)
        return 0;

    nval = (utype *)(json_ptr_of_entry(*ptr, cinfo));
    switch (ptr->type)
    {
    case JSON_TYPE_UINT16:
        nval->u16 = (uint16_t)(value + .5);
        break;
    case JSON_TYPE_UINT32:
        nval->u32 = (uint32_t)(value + .5);
        break;
    case JSON_TYPE_INT16:
        nval->i16 = (int16_t)(value + .5);
        break;
    case JSON_TYPE_INT32:
        nval->i32 = (int32_t)(value + .5);
        break;
    case JSON_TYPE_FLOAT:
        nval->f = (float)value;
        break;
    case JSON_TYPE_DOUBLE:
    case JSON_TYPE_TIMESTAMP:
        nval->d = value;
        break;
    }
    return 0;
}

//! Return the results of a JSON equation.
/*! At its minimum, a JSON equation will be defined as any two JSON
 * names separated by an operation. Equations bracketed by '()' will
 * be treated as names in their own right and will therefore require a
 * matching operation, and name or bracketed equation. The result will
 * always be assumed to be double precision, even in the case of
 * booleans. Valid operations are:
    - '!': logical NOT
    - '~': bitwise complement
    - '+': addition
    - '-': subtraction
    - '*': multiplication
    - '/': division
    - '%': modulo
    - '&': logical AND
    - '|': logical OR
    - '>': logical Greater Than
    - '<': logical Less Than
    - '=': logical Equal
    - '^': power
  \param ptr Pointer to a pointer to a JSON stream.

    \param cinfo Reference to ::cosmosstruc to use.

  \return Result of the equation, or NAN.
*/
double json_equation(const char* &ptr, cosmosstruc *cinfo)
{
    string equation;
    int32_t iretn = 0;
    jsonhandle handle;

    if ((iretn=json_parse_equation(ptr, equation)) < 0) {
        return (NAN);
    }

    if (cinfo->emap.size() == 0) {
        return (NAN);
    }

    if ((iretn=json_equation_map(equation, cinfo, &handle)) < 0) {
        return (NAN);
    }

    return(json_equation(&cinfo->emap[handle.hash][handle.index], cinfo));
}

//! Return the results of a known JSON equation handle
/*! Calculate a ::json_equation using already looked up values for the hash and index.
    \param handle Values for hash and index that point to an entry in the map.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Result of the equation, or NAN.
*/
double json_equation(jsonhandle *handle, cosmosstruc *cinfo)
{
    return(json_equation(&cinfo->emap[handle->hash][handle->index], cinfo));
}

//! Return the results of a known JSON equation entry
/*! Calculate a ::json_equation using already looked up entry from the map.
    \param ptr Pointer to a ::jsonequation from the map.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Result of the equation, or NAN.
*/
double json_equation(jsonequation *ptr, cosmosstruc *cinfo)
{
    double a[2]={0.,0.}, c=NAN;

    for (uint16_t i=0; i<2; ++i)
    {
        switch(ptr->operand[i].type)
        {
        case JSON_OPERAND_NULL:
            break;
        case JSON_OPERAND_CONSTANT:
            a[i] = ptr->operand[i].value;
            break;
        case JSON_OPERAND_EQUATION:
            a[i] = json_equation(&cinfo->emap[ptr->operand[i].data.hash][ptr->operand[i].data.index], cinfo);
            break;
        case JSON_OPERAND_NAME:
            a[i] = json_get_double(cinfo->jmap[ptr->operand[i].data.hash][ptr->operand[i].data.index], cinfo);
            break;
        }
    }

    switch(ptr->operation)
    {
    case JSON_OPERATION_NOT:
        c = !static_cast<bool>(a[0]);
        break;
    case JSON_OPERATION_COMPLEMENT:
        c = ~static_cast<uint32_t>(a[0]);
        break;
    case JSON_OPERATION_ADD:
        c = a[0] + a[1];
        break;
    case JSON_OPERATION_SUBTRACT:
        c = a[0] - a[1];
        break;
    case JSON_OPERATION_MULTIPLY:
        c = a[0] * a[1];
        break;
    case JSON_OPERATION_DIVIDE:
        c = a[0] / a[1];
        break;
    case JSON_OPERATION_MOD:
        c = fmod(a[0], a[1]);
        break;
    case JSON_OPERATION_AND:
        c = static_cast<int>(a[0]) && static_cast<int>(a[1]);
        break;
    case JSON_OPERATION_OR:
        c = static_cast<int>(a[0]) || static_cast<int>(a[1]);
        break;
    case JSON_OPERATION_GT:
        c = a[0] > a[1];
        break;
    case JSON_OPERATION_LT:
        c = a[0] < a[1];
        break;
    case JSON_OPERATION_EQ:
        c = fabs(a[0] - a[1]) < std::numeric_limits<double>::epsilon();
        break;
    case JSON_OPERATION_POWER:
        c = pow(a[0], a[1]);
        break;
    case JSON_OPERATION_BITWISEAND:
        c = static_cast<int>(a[0]) & static_cast<int>(a[1]);
        break;
    case JSON_OPERATION_BITWISEOR:
        c = static_cast<int>(a[0]) | static_cast<int>(a[1]);
    }

    return (c);
}

//! Extract JSON value matching name.
/*! Scan through the provided JSON stream looking for the supplied
    Namespace name. If it is found, return its value as a character
    string.
    \param json A string of JSON data
    \param token The Namespace name to be extracted.
    \return A character string representing the extracted value, otherwise nullptr.
*/
string json_extract_namedmember(string json, string token)
{
    string tstring;
    const char *ptr;
    int32_t iretn = 0;

    // Look for namespace name in string
    if ((ptr=(char*)strstr(json.c_str(), token.c_str())) == nullptr)
    {
        return tstring;
    }
    ptr += token.length();

    // Skip over " (which will be there if name was supplied without " "'s around it)
    if (ptr[0] == '"')
    {
        iretn = json_skip_character(ptr, '"');
        if (iretn < 0)
        {
            return tstring;
        }
    }

    // Skip over :
    if ((iretn = json_skip_white(ptr)) < 0)
        return tstring;
    iretn = json_skip_character(ptr, ':');
    if (iretn < 0)
    {
        return tstring;
    }

    //	while (ptr[0] != 0 && ptr[0] != ':') ptr++;

    //	if ((ptr)[0] == 0) return (tstring);

    // Skip over any white space
    iretn = json_skip_white(ptr);
    if (iretn < 0)
    {
        return tstring;
    }

    //	ptr++;
    //	while (ptr[0] != 0 && isspace(ptr[0])) ptr++;

    //	if (ptr[0] == 0) return (tstring);

    // Extract next JSON value
    iretn = json_extract_value(ptr, tstring);
    return tstring;
}

//! Extract next JSON value.
/*! Extract the next JSON value from the provided string. If it is found,
 * return its value as a string.
    \param ptr Pointer to a pointer to a JSON stream.
    \param value Reference to extracted string.
    \return The length of the returned string.
*/
int32_t json_extract_value(const char *&ptr, string &value)
{
    const char *bptr, *eptr;
    uint32_t count;
    int32_t iretn = 0;

    bptr = ptr;
    switch (ptr[0])
    {
    case '{':
        count = 1;
        do
        {
            ++ptr;
            switch (ptr[0])
            {
            case '}':
                --count;
                break;
            case '{':
                ++count;
                break;
            case 0:
                return (iretn=JSON_ERROR_EOS);
            }
        } while (count);
        eptr = ptr;
        ptr++;
        break;
    case '[':
        count = 1;
        do
        {
            ++ptr;
            switch (ptr[0])
            {
            case ']':
                --count;
                break;
            case '[':
                ++count;
                break;
            case 0:
                return (iretn=JSON_ERROR_EOS);
            }
        } while (count);
        eptr = ptr;
        ptr++;
        break;
    case '"':
        do
        {
            ++ptr;
            if (ptr[0]==0) return (iretn=JSON_ERROR_EOS);
        } while (ptr[0] != '"');
        eptr = ptr;
        ptr++;
        break;
    default:
        do
        {
            ++ptr;
        } while ((ptr[0]>='0'&&ptr[0]<='9')||ptr[0]=='e'||ptr[0]=='E'||ptr[0]=='.'||ptr[0]=='-');
        eptr = ptr-1;
        break;
    }

    value = bptr;
    value.resize(eptr-bptr+1);

    return iretn;
}

//! Convert JSON to string.
/*! Convert the supplied JSON Object to a string representation. If the supplied
 * Object does not have the proper format for a JSON string, an empty string will
 * be returned.
 * \param object Fully formed JSON object.
 * \return Object as string, or empty string.
 */
string json_convert_string(string object)
{
    string result;
    const char *ptr = &object[0];

    json_extract_string(ptr, result);

    return result;
}

//! Convert JSON to double.
/*! Convert the supplied JSON Object to a double representation. If the supplied
 * Object does not have the proper format for a double, zero
 * be returned.
 * \param object Fully formed JSON object.
 * \return Object as double, or zero.
 */
double json_convert_double(string object)
{
    double dresult;

    if (!object.empty())
    {
        dresult = atof(object.c_str());
        return dresult;
    }
    else
    {
        dresult = 0;
    }

    return dresult;
}

//! Tokenize using JSON Name Space.
/*! Scan through the provided JSON stream, matching names to the ::jsonmap.
 * for each match that is found, create a ::jsontoken entry and add it to a vector
 * of tokens.
 * \param jstring string containing JSON stream.
 * \param tokens vector of ::jsontoken.

 * \return Zero or negative error.
 */
int32_t json_tokenize(string jstring, cosmosstruc *cinfo, vector<jsontoken> &tokens)
{
    const char *cpoint;
    size_t length;
    int32_t iretn = 0;
    jsontoken ttoken;

    string val = json_extract_namedmember(jstring, "node_utc");
    if (val.length()!=0) ttoken.utc = json_convert_double(val);
    else
    {
        //Some older sets of telemetry do not include "node_utc" so the utc must be found elsewhere:
        if ((val = json_extract_namedmember(jstring, "node_loc_pos_eci")).length()!=0)
        {
            if ((val=json_extract_namedmember(val, "utc")).length()!=0) ttoken.utc = json_convert_double(val);
        }
    }

    length = jstring.size();
    cpoint = &jstring[0];
    while (*cpoint != 0 && *cpoint != '{')
        cpoint++;
    tokens.clear();
    do
    {
        if (*cpoint != 0)
        {
            if ((iretn = json_tokenize_namedmember(cpoint, cinfo, ttoken)) < 0)
            {
                if (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP)
                    iretn = 0;
            }
            else
            {
                tokens.push_back(ttoken);
            }
        }
        else
            iretn = JSON_ERROR_EOS;
    } while (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP && *cpoint != 0 && (size_t)(cpoint-&jstring[0]) <= length);

    if (!iretn) iretn = (int32_t)tokens.size();
    return iretn;
}

//! Tokenize next JSON Named Pair
/*! Extract the next Named Pair from the provided JSON stream and place it in a ::jsontoken.
 * Leave pointer at the next Object in the string.
 * \param ptr Pointer to a pointer to a JSON stream.
 *
 * \param token ::jsontoken to return.
 * \return Zero, or a negative error.
*/
int32_t json_tokenize_namedmember(const char* &ptr, cosmosstruc *cinfo, jsontoken &token)
{
    int32_t iretn=0;
    string ostring;
    uint16_t hash, index;

    if (!(cinfo->jmapped))
    {
        return (JSON_ERROR_NOJMAP);
    }

    // Skip over opening brace
    if (ptr[0] != '{')
    {
        if ((iretn = json_skip_to_next_member(ptr)) < 0)
            return iretn;
        else
            return (JSON_ERROR_SCAN);
    }

    ptr++;

    // Extract string that should hold name of this object.
    if ((iretn = json_extract_string(ptr, ostring)) < 0)
    {
        if (iretn != JSON_ERROR_EOS)
        {
            if ((iretn = json_skip_to_next_member(ptr)) < 0)
                return iretn;
            else
                return (JSON_ERROR_SCAN);
        }
        else
            return iretn;
    }
    // Calculate hash
    hash = json_hash(ostring);

    // See if there is a match in the ::jsonmap.
    for (index=0; index<cinfo->jmap[hash].size(); ++index)	{
        if (ostring == cinfo->jmap[hash][index].name)
        {
            break;
        }
    }

    if (index == cinfo->jmap[hash].size())
    {
        if ((iretn = json_skip_to_next_member(ptr)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return iretn;
        }
        else
            return (JSON_ERROR_NOENTRY);
    }
    else
    {
        // Skip white space before separator
        if ((iretn = json_skip_white(ptr)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_to_next_member(ptr)) < 0)
                    return iretn;
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return iretn;
        }
        // Skip separator
        if ((iretn = json_skip_character(ptr,':')) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_to_next_member(ptr)) < 0)
                    return iretn;
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return iretn;
        }
        // Skip white space before value
        if ((iretn = json_skip_white(ptr)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_to_next_member(ptr)) < 0)
                    return iretn;
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return iretn;
        }
        // Read value
        string input;
        if ((iretn = json_extract_value(ptr, input)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_to_next_member(ptr)) < 0)
                    return iretn;
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return iretn;
        }
        if (input.size())
        {
            token.value = input;
            token.handle.hash = hash;
            token.handle.index = index;
        }
        //Skip whitespace after value
        if ((iretn = json_skip_white(ptr)) < 0)
        {
            return iretn;
        }
        // Skip over closing brace
        if ((iretn = json_skip_character(ptr,'}')) < 0)
        {
            if ((iretn = json_skip_to_next_member(ptr)) < 0)
                return iretn;
            else
                return (JSON_ERROR_SCAN);
        }
    }

    //	ptr++;
    json_skip_white(ptr);
    return iretn;
}

//! Parse JSON using Name Space.
/*! Scan through the provided JSON stream, matching names to the ::jsonmap. For
 * each match that is found, load the associated data item with the accompanying data.
 * This function supports complex data types.
    \param jstring A string of JSON data

    \param cinfo Reference to ::cosmosstruc to use.

    \return Zero or negative error.
*/
int32_t json_parse(string jstring, cosmosstruc *cinfo)
{
    const char *cpoint;
    size_t length;
    int32_t iretn = 0;
    uint32_t count = 0;

    length = jstring.size();
    cpoint = &jstring[0];
    while (*cpoint != 0 && *cpoint != '{' && *cpoint != ',' )  //advance until you find a '{' or ','
        cpoint++;
    do
    {
        // is this the only reference to endlines?
        if (*cpoint != 0)// && *cpoint != '\r' && *cpoint != '\n')
            //if (*cpoint != 0 && *cpoint != '\r' && *cpoint != '\n')
        {
            if ((iretn = json_parse_namedmember(cpoint, cinfo)) < 0)
            {
                if (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP)
                    iretn = 0;
            }
            else
            {
                ++count;
            }
        }
        else
            iretn = JSON_ERROR_EOS;
    } while (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP && *cpoint != 0 && (size_t)(cpoint-&jstring[0]) <= length);

    if (iretn >= 0)
    {

        cinfo->timestamp = currentmjd();
        iretn = count;
    }
    return iretn;
}

//! Parse next JSON Named Pair
/*! Extract the next Named Pair from the provided JSON stream. Return a pointer to the next
 * Object in the string, and an error flag.
    \param ptr Pointer to a pointer to a JSON stream.

    \param cinfo Reference to ::cosmosstruc to use.
    \return Zero, or a negative error.
*/
int32_t json_parse_namedmember(const char* &ptr, cosmosstruc *cinfo)
{
    uint32_t hash;
    int32_t iretn=0;
    string ostring;

    if (!(cinfo->jmapped))
    {
        return (JSON_ERROR_NOJMAP);
    }

    if (ptr[0] != '{' && ptr[0] != ',')      //verify that there is a '{' or ','
    {
        if ((iretn = json_skip_to_next_member(ptr)) < 0)
            return iretn;
        else
            return (JSON_ERROR_SCAN);
    }

    ptr++;      //move forward from '{' or ',' to '"' ?

    // Extract string that should hold name of this member.
    if ((iretn = json_extract_string(ptr, ostring)) < 0)
    {
        if (iretn != JSON_ERROR_EOS)
        {
            if ((iretn = json_skip_to_next_member(ptr)) < 0)
                return iretn;
            else
                return (JSON_ERROR_SCAN);
        }
        else
            return iretn;
    }

    if(ostring.find("$$_") == 0)
    {
        ostring = ostring.substr(3);
    }


    // Calculate hash of Namespace name
    hash = json_hash(ostring);

    // See if there is a match in the ::jsonmap.
    size_t n;
    for (n=0; n<cinfo->jmap[hash].size(); ++n)	//check through every column in the row (using hash) exit loop if the name matches
    {
        if (ostring == cinfo->jmap[hash][n].name)
        {
            break;
        }
    }

    if (n == cinfo->jmap[hash].size())       //if there was no match
    {
        if ((iretn = json_skip_to_next_member(ptr)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return iretn;
        }
        else
            return (JSON_ERROR_NOENTRY);
    }
    else                            //if a match was found
    {
        if ((iretn = json_skip_white(ptr)) < 0)     //skip the white space
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_to_next_member(ptr)) < 0)
                    return iretn;
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return iretn;
        }
        if ((iretn = json_skip_character(ptr,':')) < 0)   //skip the ':' before the value
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_to_next_member(ptr)) < 0)
                    return iretn;
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return iretn;
        }
        // enter value into the structs using pointer
        if ((iretn = json_parse_value(ptr, cinfo->jmap[hash][n], cinfo)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_to_next_member(ptr)) < 0)
                    return iretn;
                else
                    return (JSON_ERROR_SCAN);
            }
            else
            {
                return iretn;
            }
        }
    }

    //    ptr++;
    json_skip_white(ptr);
    json_skip_character(ptr, '}');
    json_skip_white(ptr);
    if (iretn >= 0)
    {
        cinfo->jmap[hash][n].enabled = true;
    }
    return iretn;
}

//! Skip over a specific character in a JSON stream
/*! Look for the specified character in the provided JSON stream and
 * flag an error if it's not there. Otherwise, increment the pointer
 * to the next byte in the string.
 \param ptr Pointer to a pointer to a JSON stream.
 \param character The character to look for.
 \return Zero or a negative error.
*/
int32_t json_skip_character(const char* &ptr, const char character)
{
    int32_t iretn = 0;

    iretn = json_skip_white(ptr);
    if (iretn < 0)
    {
        return iretn;
    }

    if (ptr[0] == 0)
    {
        return (JSON_ERROR_EOS);
    }

    if (ptr[0] != character)
    {
        return (JSON_ERROR_SCAN);
    }
    ptr++;

    return (0);
}

//! Parse the next variable name out of a JSON stream.
/*! Look for a valid JSON string, followed by a ':' and copy that
 * name to the provided buffer, otherwise flag an error.
 \param ptr Pointer to a pointer to a JSON stream.
 \param ostring Pointer to a location to copy the string.
 \return Zero, otherwise negative error.
*/
int32_t json_extract_name(const char* &ptr, string& ostring)
{
    int32_t iretn = 0;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    //Skip whitespace before name
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return iretn;
    }

    //Parse name
    if ((iretn = json_extract_string(ptr, ostring)) < 0)
        return iretn;

    //Skip whitespace after name
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return iretn;
    }

    if ((iretn = json_skip_character(ptr, ':')) < 0)
        return iretn;

    //Skip whitespace after seperator
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return iretn;
    }

    return iretn;
}

//! Parse the next JSON equation out of a JSON stream.
/*! Look for a valid JSON equation in the provided JSON stream and copy
 * it to the provided buffer. Otherwise flag an error.
 \param ptr Pointer to a pointer to a JSON stream.
 \param equation Reference to a location to copy the equation.
 \return Zero, otherwise negative error.
*/
int32_t json_parse_equation(const char* &ptr, string& equation)
{
    int32_t iretn = 0;
    uint16_t i2;
    uint16_t index, depth=1;
    size_t ilen;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    // Skip white space before '('
    if ((iretn=json_skip_white(ptr)) < 0)
    {
        return(JSON_ERROR_SCAN);
    }

    // Check if it's an equation. All equations must begin with '(' and end with ')'
    if (ptr[0] != '(')
    {
        return(JSON_ERROR_SCAN);
    }

    ilen = strlen(ptr);

    // Start of object, get equation
    index = 0;
    equation.push_back(ptr[0]);
    for (i2=1; i2<ilen; i2++)
    {
        if (index == JSON_MAX_DATA-1)
            break;
        switch(ptr[i2])
        {
        case ' ':
        case '\t':
        case '\f':
        case '\n':
        case '\r':
        case '\v':
            break;
        case ')':
            --depth;
            equation.push_back(ptr[i2]);
            break;
        case '(':
            ++depth;
            equation.push_back(ptr[i2]);
            break;
        default:
            equation.push_back(ptr[i2]);
            break;
        }
        if (!depth)
            break;
    }

    if (i2 >= ilen)
    {
        ptr = &ptr[ilen-1];
        return(JSON_ERROR_SCAN);
    }

    ptr = &ptr[i2+1];

    return iretn;
}

//! Parse the next JSON equation operand out of a JSON stream.
/*! Look for a valid JSON equation operand in the provided JSON stream and
 * copy it to a ::jsonoperand.
 \param ptr Pointer to a pointer to a JSON stream.
 \param operand Pointer to a ::jsonoperand.

 \return Zero, otherwise negative error.
*/
int32_t json_parse_operand(const char* &ptr, jsonoperand *operand, cosmosstruc *cinfo)
{
    string tstring;
    int32_t iretn = 0;

    json_skip_white(ptr);
    switch(ptr[0])
    {
    case '(':
        // It's an equation
        if ((iretn=json_parse_equation(ptr, tstring)) < 0)
        {
            return (JSON_ERROR_SCAN);
        }
        if ((iretn=json_equation_map(tstring, cinfo, &operand->data)) < 0)
        {
            return (JSON_ERROR_SCAN);
        }
        operand->type = JSON_OPERAND_EQUATION;
        break;
    case '"':
        // It's a string
        if ((iretn=json_extract_string(ptr, tstring)) < 0)
        {
            return (JSON_ERROR_SCAN);
        }
        if ((iretn=json_name_map(tstring, cinfo, operand->data)) < 0)
        {
            return (JSON_ERROR_SCAN);
        }
        operand->type = JSON_OPERAND_NAME;
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '+':
    case '-':
    case '.':
    case '%':
        // It's a constant
        operand->type = JSON_OPERAND_CONSTANT;
        if ((iretn=json_parse_number(ptr,&operand->value)) < 0)
            return (JSON_ERROR_SCAN);
        break;
    default:
        operand->type = JSON_OPERAND_NULL;
        break;
    }

    return 0;
}

//! Parse the next JSON string out of a JSON stream.
/*! Look for a valid JSON string in the provided JSON stream and copy
 * it to the provided buffer. Otherwise flag an error.
 \param ptr Pointer to a pointer to a JSON stream.
 \param ostring Pointer to a location to copy the string.
 \return Zero, otherwise negative error.
*/
int32_t json_extract_string(const char* &ptr, string &ostring)
{
    int32_t iretn = 0;
    uint32_t i2;
    size_t ilen;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    //Skip whitespace before string
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return iretn;
    }

    ilen = strlen(ptr);

    // Start of object, get string
    ostring.clear();
    for (i2=1; i2<ilen; i2++) //start from ptr[1] rather than ptr[0] which is a '"'?
    {
        if (ptr[i2] == '"')
            break;  //exits for loop?
        if (ptr[i2] == '\\')
        {
            switch (ptr[i2+1])
            {
            case '"':
            case '\\':
            case '/':
                ostring.push_back(ptr[i2+1]);
                break;
            case 'b':
                ostring.push_back('\b');
                break;
            case 'f':
                ostring.push_back('\f');
                break;
            case 'n':
                ostring.push_back('\n');
                break;
            case 'r':
                ostring.push_back('\r');
                break;
            case 't':
                ostring.push_back('\t');
                break;
            default:
                i2 += 3;
            }
            i2++;
        }
        else
        {
            ostring.push_back(ptr[i2]);
        }
    }

    if (i2 >= ilen)
    {
        ptr = &ptr[ilen-1];
        return(JSON_ERROR_SCAN);
    }

    // i2 is last character in string +1, index is length of extracted string
    if (ostring.size() >= JSON_MAX_DATA)
    {
        ostring.resize(JSON_MAX_DATA-1);
    }

    ptr = &ptr[i2+1];

    return iretn;
}


//! Parse the next number out of a JSON stream.
/*! Extract a valid number out of the provided JSON stream, otherwise
 * flag an error.
 \param ptr Pointer to a pointer to a JSON stream.
 \param number Extracted number in double precission.
 \return Zero or negative error.
*/
int32_t json_parse_number(const char* &ptr, double *number)
{
    int32_t iretn = 0;
    uint32_t i1;
    size_t ilen;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    //Skip whitespace before number
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return iretn;
    }
    ilen = strlen(ptr);

    // First, check for integer: series of digits
    i1 = 0;
    if (ptr[i1] == '-' || ptr[i1] == '+')
    {
        ++i1;
    }
    while (i1 < ilen && ptr[i1] >= '0' && ptr[i1] <= '9')
    {
        ++i1;
    }

    // Second, check for fraction: . followed by series of digits
    if (ptr[i1] == '.')
    {
        ++i1;
        while (i1 < ilen && ptr[i1] >= '0' && ptr[i1] <= '9')
        {
            ++i1;
        }
    }

    // Third, check for exponent: e or E followed by optional +/- and series of digits
    if (ptr[i1] == 'e' || ptr[i1] == 'E')
    {
        ++i1;
        if (ptr[i1] == '-' || ptr[i1] == '+')
        {
            ++i1;
        }
        if (ptr[i1] == '+')
        {
            ++i1;
        }
        while (i1 < ilen && ptr[i1] >= '0' && ptr[i1] <= '9')
        {
            ++i1;
        }
    }

    // Finally, scan resulting string and move pointer to new location: i1 equals first position after number
    sscanf(ptr,"%lf",number);
    ptr = &ptr[i1];
    return iretn;
}

//! Skip white space in JSON string.
/*! Skip over any white space characters, leaving pointer at next non white
 * space.
    \param ptr Double pointer to the JSON string
    \return Zero, or negative error.
*/
int32_t json_skip_white(const char* &ptr)
{
    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    while (ptr[0] != 0 && isspace(ptr[0])) ptr++;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);
    else
        return 0;
}

//! Skip to next COSMOS name in JSON string
/*! Skip over characters until you reach the next COSMOS Namespace name in a JSON string.
    \param ptr Double pointer to the JSON string
    \return Zero, or negative error.
*/
int32_t json_skip_to_next_member(const char* &ptr)
{
    int32_t iretn = 0;

    if (ptr[0] == 0)
    {
        return (JSON_ERROR_EOS);
    }

    do
    {
        if (ptr[0] == '\\')
        {
            ptr++;
            if (ptr[0] == 0)
            {
                break;
            }
        }
        else if (ptr[0] == '{' || ptr[0] == ',')
        {
            if (ptr[1] == 0)
            {
                break;
            }
            if (ptr[1] == '"')
            {
                string tstring;
                const char * cptr = ptr+1;
                iretn = json_extract_string(cptr, tstring);
                if (iretn < 0)
                {
                    ptr = cptr;
                    return iretn;
                }
                if (cptr[0] == ':')
                {
                    return 0;
                }
            }
        }
        ptr++;
    } while (ptr[0] != 0);

    return (JSON_ERROR_EOS);
}

int32_t json_set_string(string val, const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *data;

    data = json_ptr_of_entry(entry, cinfo);
    if (data == nullptr)
    {
        return JSON_ERROR_NAN;
    }

    switch (entry.type)
    {
    case JSON_TYPE_UINT8:
        *(uint8_t *)data = stoi(val);
        break;
    case JSON_TYPE_INT8:
        *(int8_t *)data = stoi(val);
        break;
    case JSON_TYPE_UINT16:
        *(uint16_t *)data = stoi(val);
        break;
    case JSON_TYPE_UINT32:
        *(uint32_t *)data = stol(val);
        break;
    case JSON_TYPE_INT16:
        *(int16_t *)data = stoi(val);
        break;
    case JSON_TYPE_INT32:
        *(int32_t *)data = stol(val);
        break;
    case JSON_TYPE_FLOAT:
        *(float *)data = stof(val);
        break;
    case JSON_TYPE_TIMESTAMP:
    case JSON_TYPE_DOUBLE:
        *(double *)data = stod(val);
        break;
    }
    return 0;
}

int32_t json_set_number(double val, const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *data;
    int32_t iretn = 0;

    data = json_ptr_of_entry(entry, cinfo);

    switch (entry.type)
    {
    case JSON_TYPE_UINT8:
        *(uint8_t *)data = (uint8_t)val;
        break;
    case JSON_TYPE_INT8:
        *(int8_t *)data = (int8_t)val;
        break;
    case JSON_TYPE_UINT16:
        *(uint16_t *)data = (uint16_t)val;
        break;
    case JSON_TYPE_UINT32:
        *(uint32_t *)data = (uint32_t)val;
        break;
    case JSON_TYPE_INT16:
        *(int16_t *)data = (int16_t)val;
        break;
    case JSON_TYPE_INT32:
        *(int32_t *)data = (int32_t)val;
        break;
    case JSON_TYPE_FLOAT:
        *(float *)data = (float)val;
        break;
    case JSON_TYPE_TIMESTAMP:
    case JSON_TYPE_DOUBLE:
        *(double *)data = (double)val;
        break;
    }
    return iretn;
}

int32_t json_parse_value(const char* &ptr, const jsonentry &entry, cosmosstruc *cinfo)
{
    uint8_t *data;
    int32_t iretn=0;

    data = json_ptr_of_entry(entry, cinfo);

    iretn = json_parse_value(ptr, entry.type, data, cinfo);
    return iretn;
}

int32_t json_parse_value(const char* &ptr, uint16_t type, ptrdiff_t offset, uint16_t group, cosmosstruc *cinfo)
{
    uint8_t *data;
    int32_t iretn = 0;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    data = json_ptr_of_offset(offset, group, cinfo);

    iretn = json_parse_value(ptr, type, data, cinfo);
    return iretn;
}

int32_t json_parse_value(const char *&ptr, uint16_t type, uint8_t *data, cosmosstruc *cinfo)
{
    int32_t iretn=0;
    string input;
    double val;
    Convert::cartpos *rp;
    Convert::geoidpos *gp;
    Convert::spherpos *sp;
    Convert::dcmatt *ra;
    Convert::qatt *qa;
    string empty;

    //Skip whitespace before value
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return iretn;
    }

    switch (type)
    {
    case JSON_TYPE_UINT8:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(uint8_t *)data = (uint8_t)val;
        break;

    case JSON_TYPE_INT8:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(int8_t *)data = (int8_t)val;
        break;

    case JSON_TYPE_UINT16:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(uint16_t *)data = (uint16_t)val;
        break;
    case JSON_TYPE_UINT32:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(uint32_t *)data = (uint32_t)val;
        break;
    case JSON_TYPE_INT16:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(int16_t *)data = (int16_t)val;
        break;
    case JSON_TYPE_INT32:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(int32_t *)data = (int32_t)val;
        break;
    case JSON_TYPE_FLOAT:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(float *)data = (float)val;
        break;
    case JSON_TYPE_TIMESTAMP:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(double *)data = (double)val;
        break;
    case JSON_TYPE_DOUBLE:
        if (std::isnan(val=json_equation(ptr, cinfo)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return iretn;
        }
        *(double *)data = (double)val;
        break;
    case JSON_TYPE_STRING:
        if ((iretn = json_extract_string(ptr, input)) < 0)
            return iretn;
        if (input.size())
        {
            if (input.size() > COSMOS_MAX_DATA)
            {
                input.resize(COSMOS_MAX_DATA-1);
            }
            *(string *)data = input;
        }
        break;
    case JSON_TYPE_NAME:
        if ((iretn = json_extract_string(ptr, input)) < 0)
            return iretn;
        if (input.size())
        {
            if (input.size() > COSMOS_MAX_NAME)
            {
                input.resize(COSMOS_MAX_NAME-1);
            }
            strcpy((char *)data, (char*)&input[0]);
        }
        break;
    case JSON_TYPE_GVECTOR:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(gvector,lat), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(gvector,lon), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(gvector,h), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        break;
    case JSON_TYPE_AVECTOR:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(avector,h), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(avector,e), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(avector,b), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        break;
    case JSON_TYPE_CVECTOR:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(cvector,x), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(cvector,y), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(cvector,z), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        break;
    case JSON_TYPE_VECTOR:
        if ((iretn = json_skip_character(ptr,'[')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Vector,x), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Vector,y), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Vector,z), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,']')) < 0)
        {
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return iretn;
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Vector,w), cinfo)) < 0)
                return iretn;
            if ((iretn = json_skip_character(ptr,']')) < 0)
            {
                return iretn;
            }
        }
        break;
    case JSON_TYPE_RVECTOR:
        //    case JSON_TYPE_TVECTOR:
        if ((iretn = json_skip_character(ptr,'[')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(rvector,col[0]), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(rvector,col[1]), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(rvector,col[2]), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,']')) < 0)
            return iretn;
        break;
    case JSON_TYPE_QUATERNION:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CVECTOR, data+(ptrdiff_t)offsetof(quaternion,d), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(quaternion,w), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        break;
    case JSON_TYPE_RMATRIX:
        if ((iretn = json_skip_character(ptr,'[')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(rmatrix,row[0]), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(rmatrix,row[1]), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(rmatrix,row[2]), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(rmatrix,row[3]), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,']')) < 0)
            return iretn;
        break;
        //    case JSON_TYPE_DCM:
        //        if ((iretn = json_skip_character(ptr,'[')) < 0)
        //            return iretn;
        //        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(rmatrix,row[0]), cinfo)) < 0)
        //            return iretn;
        //        if ((iretn = json_skip_character(ptr,',')) < 0)
        //            return iretn;
        //        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(rmatrix,row[1]), cinfo)) < 0)
        //            return iretn;
        //        if ((iretn = json_skip_character(ptr,',')) < 0)
        //            return iretn;
        //        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(rmatrix,row[2]), cinfo)) < 0)
        //            return iretn;
        //        if ((iretn = json_skip_character(ptr,']')) < 0)
        //            return iretn;
        //        break;
    case JSON_TYPE_GEOIDPOS:
    case JSON_TYPE_POS_SELG:
    case JSON_TYPE_POS_GEOD:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr,input)) < 0)
            return iretn;

        gp = (Convert::geoidpos *)(data+(ptrdiff_t)offsetof(Convert::geoidpos,utc));
        gp->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Convert::cartpos,utc), cinfo)) < 0)
                return iretn;
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return iretn;
            if ((iretn = json_extract_name(ptr, empty)) < 0)
                return iretn;
        }
        if (gp->utc == 0.)
        {
            gp->utc = currentmjd(cinfo->node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_GVECTOR, data+(ptrdiff_t)offsetof(Convert::geoidpos,s), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_GVECTOR, data+(ptrdiff_t)offsetof(Convert::geoidpos,v), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_GVECTOR, data+(ptrdiff_t)offsetof(Convert::geoidpos,a), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        gp->pass++;
        switch (type)
        {
        case JSON_TYPE_POS_SELG:
            Convert::pos_selg(&cinfo->node.loc);
            break;
        case JSON_TYPE_POS_GEOD:
            Convert::pos_geod(&cinfo->node.loc);
            break;
        }
        break;
    case JSON_TYPE_SPHERPOS:
    case JSON_TYPE_POS_GEOS:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr,input)) < 0)
            return iretn;

        sp = (Convert::spherpos *)(data+(ptrdiff_t)offsetof(Convert::spherpos,utc));
        sp->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Convert::cartpos,utc), cinfo)) < 0)
                return iretn;
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return iretn;
            if ((iretn = json_extract_name(ptr, empty)) < 0)
                return iretn;
        }
        if (sp->utc == 0.)
        {
            sp->utc = currentmjd(cinfo->node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_SVECTOR, data+(ptrdiff_t)offsetof(Convert::spherpos,s), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_SVECTOR, data+(ptrdiff_t)offsetof(Convert::spherpos,v), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_SVECTOR, data+(ptrdiff_t)offsetof(Convert::spherpos,a), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        sp->pass++;
        Convert::pos_geos(&cinfo->node.loc);
        break;
    case JSON_TYPE_CARTPOS:
    case JSON_TYPE_POS_GEOC:
    case JSON_TYPE_POS_SELC:
    case JSON_TYPE_POS_ECI:
    case JSON_TYPE_POS_SCI:
    case JSON_TYPE_POS_ICRF:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr,input)) < 0)
            return iretn;

        rp = (Convert::cartpos *)(data+(ptrdiff_t)offsetof(Convert::cartpos,utc));
        rp->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Convert::cartpos,utc), cinfo)) < 0)
                return iretn;
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return iretn;
            if ((iretn = json_extract_name(ptr, empty)) < 0)
                return iretn;
        }
        if (rp->utc == 0.)
        {
            rp->utc = currentmjd(cinfo->node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::cartpos,s), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::cartpos,v), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::cartpos,a), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        rp->pass++;
        switch (type)
        {
        case JSON_TYPE_POS_SELC:
            Convert::pos_selc(&cinfo->node.loc);
            break;
        case JSON_TYPE_POS_GEOC:
            Convert::pos_geoc(&cinfo->node.loc);
            break;
        case JSON_TYPE_POS_ECI:
            Convert::pos_eci(&cinfo->node.loc);
            break;
        case JSON_TYPE_POS_SCI:
            Convert::pos_sci(&cinfo->node.loc);
            break;
        case JSON_TYPE_POS_ICRF:
            Convert::pos_icrf(&cinfo->node.loc);
            break;
        }
        break;
    case JSON_TYPE_DCMATT:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RMATRIX, data+(ptrdiff_t)offsetof(Convert::dcmatt,s), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::dcmatt,v), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::dcmatt,a), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        ra = (Convert::dcmatt *)(data+(ptrdiff_t)offsetof(Convert::dcmatt,utc));
        ra->utc = *(double *)json_ptrto((char *)"node_utc", cinfo);
        break;
    case JSON_TYPE_QATT:
    case JSON_TYPE_QATT_TOPO:
    case JSON_TYPE_QATT_GEOC:
    case JSON_TYPE_QATT_ICRF:
    case JSON_TYPE_QATT_SELC:
    case JSON_TYPE_QATT_LVLH:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr,input)) < 0)
            return iretn;

        qa = (Convert::qatt *)(data+(ptrdiff_t)offsetof(Convert::qatt,utc));
        qa->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Convert::qatt,utc), cinfo)) < 0)
                return iretn;
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return iretn;
            if ((iretn = json_extract_name(ptr, empty)) < 0)
                return iretn;
        }
        if (qa->utc == 0.)
        {
            qa->utc = currentmjd(cinfo->node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_QUATERNION, data+(ptrdiff_t)offsetof(Convert::qatt,s), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::qatt,v), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_name(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::qatt,a), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        qa->pass++;
        switch (type)
        {
        case JSON_TYPE_QATT_GEOC:
            att_geoc(&cinfo->node.loc);
            break;
        case JSON_TYPE_QATT_ICRF:
            Convert::att_icrf(&cinfo->node.loc);
            break;
        case JSON_TYPE_QATT_LVLH:
            Convert::att_lvlh(&cinfo->node.loc);
            break;
        case JSON_TYPE_QATT_SELC:
            att_selc(&cinfo->node.loc);
            break;
        case JSON_TYPE_QATT_TOPO:
            att_topo(&cinfo->node.loc);
            break;
        }
        break;
    case JSON_TYPE_HBEAT:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(beatstruc,utc), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_STRING, data+(ptrdiff_t)offsetof(beatstruc,node), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_STRING, data+(ptrdiff_t)offsetof(beatstruc,proc), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_INT32, data+(ptrdiff_t)offsetof(beatstruc,ntype), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_STRING, data+(ptrdiff_t)offsetof(beatstruc,addr), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_INT32, data+(ptrdiff_t)offsetof(beatstruc,port), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_INT32, data+(ptrdiff_t)offsetof(beatstruc,bsz), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(beatstruc,bprd), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        break;
    case JSON_TYPE_LOC_POS:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Convert::posstruc,utc), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS, data+(ptrdiff_t)offsetof(Convert::posstruc,icrf), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS, data+(ptrdiff_t)offsetof(Convert::posstruc,eci), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS, data+(ptrdiff_t)offsetof(Convert::posstruc,sci), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS, data+(ptrdiff_t)offsetof(Convert::posstruc,geoc), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS, data+(ptrdiff_t)offsetof(Convert::posstruc,selc), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POS_GEOD, data+(ptrdiff_t)offsetof(Convert::posstruc,geod), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POS_GEOD, data+(ptrdiff_t)offsetof(Convert::posstruc,selg), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POS_GEOS, data+(ptrdiff_t)offsetof(Convert::posstruc,geos), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR, data+(ptrdiff_t)offsetof(Convert::posstruc,bearth), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        break;
    case JSON_TYPE_LOCSTRUC:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE, data+(ptrdiff_t)offsetof(Convert::locstruc,utc), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POSSTRUC, data+(ptrdiff_t)offsetof(Convert::locstruc,pos), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return iretn;
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return iretn;
        if ((iretn = json_skip_white(ptr)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return iretn;
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_ATTSTRUC, data+(ptrdiff_t)offsetof(Convert::locstruc,att), cinfo)) < 0)
            return iretn;
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return iretn;
        break;
    }

    //Skip whitespace after value
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return iretn;
    }

    return (type);
}

//! Clear global data structure
/*! Zero out elements of the ::cosmosstruc. The entire structure can be cleared, or the
 * clearing can be confined to either the Dynamic or Static piece. This allows you to remove
 * the effects of previous calls to ::json_parse.
    \param type Instance of JSON_STRUCT_* enumeration.

    \param cinfo Reference to ::cosmosstruc to use.

    \return 0, or a negative \ref error.
*/
int32_t json_clear_cosmosstruc(int32_t type, cosmosstruc *cinfo)
{
    switch (type)
    {
    case JSON_STRUCT_NODE:
        cinfo->node = nodestruc();
        break;
    case JSON_STRUCT_EVENT:
        cinfo->event.clear();
        break;
    case JSON_STRUCT_PIECE:
        cinfo->pieces.clear();
        break;
    case JSON_STRUCT_DEVICE:
        cinfo->device.clear();
        break;
    case JSON_STRUCT_DEVSPEC:
        cinfo->devspec = devspecstruc();
        break;
    case JSON_STRUCT_PHYSICS:
        cinfo->node.phys = physicsstruc();
        break;
    case JSON_STRUCT_AGENT:
        cinfo->agent.clear();
        break;
    case JSON_STRUCT_USER:
        cinfo->user.clear();
        break;
    case JSON_STRUCT_PORT:
        cinfo->port.clear();
        break;
    case JSON_STRUCT_TARGET:
        cinfo->target.clear();
        break;
    case JSON_STRUCT_TLE:
        cinfo->tle.clear();
        break;
    case JSON_STRUCT_ALIAS:
        cinfo->alias.clear();
        break;
    case JSON_STRUCT_EQUATION:
        cinfo->equation.clear();
        break;
    case JSON_STRUCT_ALL:
        for (int32_t i=1; i<(int32_t)JSON_STRUCT_ALL; ++i)
        {
            json_clear_cosmosstruc(i, cinfo);
        }
        break;
    }
    return 0;
}

//! Map Name Space to global data structure components and pieces
/*! Create an entry in the JSON mapping tables between each name in the Name Space and the
 * \ref cosmosstruc.
 *	\param node Name and/or path of node directory. If name, then a path will be created
 * based on nodedir setting. If path, then name will be extracted from the end.
 *	\param json ::jsonnode for storing JSON.
    \return 0, or a negative value from \ref error_defines.
*/
int32_t json_load_node(string node, jsonnode &json)
{
    int32_t iretn = 0;
    struct stat fstat;
    ifstream ifs;
    char *ibuf;
    string fname;
    string nodepath;

    size_t nodestart;
    if ((nodestart = node.rfind('/')) == string::npos)
    {
        nodepath = get_nodedir(node);
    }
    else
    {
        nodepath = node;
        node = node.substr(nodestart+1, string::npos);
    }

    // First: parse data for summary information - includes piece_cnt, device_cnt and port_cnt
    fname = nodepath + "/node.ini";

    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        //        cerr << "error " << DATA_ERROR_NODES_FOLDER << ": could not find cosmos/nodes folder" << endl;
        return (DATA_ERROR_NODES_FOLDER);
        //return (NODE_ERROR_NODE);
    }

    if (fstat.st_size)
    {
        ifs.open(fname);
        if (!ifs.is_open())
        {
            return (NODE_ERROR_NODE);
        }

        ibuf = (char *)calloc(1,fstat.st_size+1);
        ifs.read(ibuf, fstat.st_size);
        ifs.close();
        ibuf[fstat.st_size] = 0;
        json_out_node(json.node, node);
        json.node += ibuf;
        free(ibuf);
    }

    // 1A: load state vector, if it is present
    fname = nodepath + "/state.ini";

    if (!stat(fname.c_str(),&fstat) && fstat.st_size)
    {
        ifs.open(fname);
        if (ifs.is_open())
        {
            ibuf = (char *)calloc(1,fstat.st_size+1);
            ifs.read(ibuf, fstat.st_size);
            ifs.close();
            ibuf[fstat.st_size] = 0;
            json.state = ibuf;
            free(ibuf);
        }
    }
    // If not, use TLE if it is present
    else {
        fname = nodepath + "/state.tle";

        if (!stat(fname.c_str(),&fstat) && fstat.st_size)
        {
            int32_t iretn = 0;
            Convert::cartpos eci;
            vector <Convert::tlestruc> tles;
            iretn = load_lines(fname, tles);
            if (iretn > 0)
            {
                if ((iretn=lines2eci(currentmjd()-10./1440., tles, eci)) == 0)
                {
                    json_out_ecipos(json.state, eci);
                }
            }
        }
    }

    // Set node_utcstart
    fname = nodepath + "/node_utcstart.dat";
    double utcstart;
    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        // First time, so write it
        utcstart = currentmjd();
        printf("jsonlib: create utcstart %f\n", utcstart);
        FILE *ifp = fopen(fname.c_str(), "w");
        if (ifp == nullptr)
        {
            return (NODE_ERROR_NODE);
        }
        fprintf(ifp, "%.15g", utcstart);
        fclose(ifp);
    }
    else
    {
        // Already set, so read it
        FILE *ifp = fopen(fname.c_str(), "r");
        if (ifp == nullptr)
        {
            // Still some problem, so just set it to current time
            utcstart = currentmjd();
            printf("jsonlib: fix utcstart %f\n", utcstart);
        }
        else
        {
            iretn = fscanf(ifp, "%lg", &utcstart);
            if (iretn != 1)
            {
                utcstart = currentmjd();
                printf("jsonlib: read utcstart %f\n", utcstart);
            }
            fclose(ifp);
        }
    }

    // Second: enter information for pieces
    fname = nodepath + "/vertices.ini";
    if (!stat(fname.c_str(),&fstat) && fstat.st_size)
    {
        ifs.open(fname);
        if (!ifs.is_open()) { return (NODE_ERROR_NODE); }

        ibuf = (char *)calloc(1,fstat.st_size+1);
        ifs.read(ibuf, fstat.st_size);
        ifs.close();
        ibuf[fstat.st_size] = 0;
        json.vertexs = ibuf;
        free(ibuf);
    }

    fname = nodepath + "/faces.ini";
    if (!stat(fname.c_str(),&fstat) && fstat.st_size)
    {
        ifs.open(fname);
        if (!ifs.is_open()) { return (NODE_ERROR_NODE); }

        ibuf = (char *)calloc(1,fstat.st_size+1);
        ifs.read(ibuf, fstat.st_size);
        ifs.close();
        ibuf[fstat.st_size] = 0;
        json.faces = ibuf;
        free(ibuf);
    }


    fname = nodepath + "/pieces.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1) { return (NODE_ERROR_NODE); }

    ifs.open(fname);
    if (!ifs.is_open()) { return (NODE_ERROR_NODE); }

    ibuf = (char *)calloc(1,fstat.st_size+1);
    ifs.read(ibuf, fstat.st_size);
    ifs.close();
    ibuf[fstat.st_size] = 0;
    json.pieces = ibuf;
    free(ibuf);

    // Third: enter information for all devices
    fname = nodepath + "/devices_general.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1) { return (NODE_ERROR_NODE); }

    ifs.open(fname);
    if (!ifs.is_open()) { return (NODE_ERROR_NODE); }

    ibuf = (char *)calloc(1,fstat.st_size+1);
    ifs.read(ibuf, fstat.st_size);
    ifs.close();
    ibuf[fstat.st_size] = 0;
    json.devgen = ibuf;
    free(ibuf);

    // Fourth: enter information for specific devices
    fname = nodepath + "/devices_specific.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1) { return (NODE_ERROR_NODE); }

    ifs.open(fname);
    if (!ifs.is_open()) { return (NODE_ERROR_NODE); }

    ibuf = (char *)calloc(1,fstat.st_size+1);
    ifs.read(ibuf, fstat.st_size);
    ifs.close();
    ibuf[fstat.st_size] = 0;
    json.devspec = ibuf;
    free(ibuf);

    // Fifth: enter information for ports
    // Resize, then add names for ports
    fname = nodepath + "/ports.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1) { return (NODE_ERROR_NODE); }

    ifs.open(fname);
    if (!ifs.is_open()) { return (NODE_ERROR_NODE); }

    ibuf = (char *)calloc(1,fstat.st_size+1);
    ifs.read(ibuf, fstat.st_size);
    ifs.close();
    ibuf[fstat.st_size] = 0;
    json.ports = ibuf;
    free(ibuf);

    // Load targeting information
    fname = nodepath + "/target.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == 0)
    {
        ifs.open(fname);
        if (ifs.is_open())
        {
            ibuf = (char *)calloc(1,fstat.st_size+1);
            ifs.read(ibuf, fstat.st_size);
            ifs.close();
            ibuf[fstat.st_size] = 0;
            json.targets = ibuf;
            free(ibuf);
        }
    }

    //! Load alias map
    fname = nodepath + "/aliases.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == 0)
    {
        ifs.open(fname);
        if (ifs.is_open())
        {
            ibuf = (char *)calloc(1,fstat.st_size+1);
            ifs.read(ibuf, fstat.st_size);
            ifs.close();
            ibuf[fstat.st_size] = 0;
            json.aliases=ibuf;
            free(ibuf);
        }
    }
    return 0;
}

int32_t json_recenter_node(cosmosstruc *cinfo)
{
    // Calculate centroid, normal and area for each face
    for (size_t i=0; i<cinfo->faces.size(); ++i)
    {
        if (cinfo->faces[i].vertex_cnt)
        {
            Vector fcentroid = cinfo->vertexs[cinfo->faces[i].vertex_idx[0]];
            fcentroid += cinfo->vertexs[cinfo->faces[i].vertex_idx[1]];
            Vector v1 = cinfo->vertexs[cinfo->faces[i].vertex_idx[0]] - cinfo->vertexs[cinfo->faces[i].vertex_idx[cinfo->faces[i].vertex_cnt-1]];
            Vector v2 = cinfo->vertexs[cinfo->faces[i].vertex_idx[1]] - cinfo->vertexs[cinfo->faces[i].vertex_idx[0]];
            Vector fnormal = v1.cross(v2);
            for (size_t j=2; j<cinfo->faces[i].vertex_cnt; ++j)
            {
                fcentroid += cinfo->vertexs[cinfo->faces[i].vertex_idx[j]];
                v1 = v2;
                v2 = cinfo->vertexs[cinfo->faces[i].vertex_idx[j]] - cinfo->vertexs[cinfo->faces[i].vertex_idx[j-1]];
                fnormal += v1.cross(v2);
            }
            v1 = v2;
            v2 = cinfo->vertexs[cinfo->faces[i].vertex_idx[0]] - cinfo->vertexs[cinfo->faces[i].vertex_idx[cinfo->faces[i].vertex_cnt-1]];
            fnormal += v1.cross(v2);

            fcentroid /= cinfo->faces[i].vertex_cnt;
            fnormal.normalize();
            cinfo->faces[i].normal = fnormal;

            cinfo->faces[i].com = Vector();
            cinfo->faces[i].area = 0.;
            v1 = cinfo->vertexs[cinfo->faces[i].vertex_idx[cinfo->faces[i].vertex_cnt-1]] - fcentroid;
            for (size_t j=0; j<cinfo->faces[i].vertex_cnt; ++j)
            {
                v2 = cinfo->vertexs[cinfo->faces[i].vertex_idx[j]] - fcentroid;
                // Area of triangle made by v1, v2 and Face centroid
                double tarea = v1.area(v2);
                // Sum
                cinfo->faces[i].area += tarea;
                // Centroid of triangle made by v1, v2 amd Face centroid
                Vector tcentroid = (v1 + v2 + fcentroid) / 3.;
                // Weighted sum
                cinfo->faces[i].com += tcentroid * tarea;
                v1 = v2;
            }
            // Divide by summed weights
            if (cinfo->faces[i].area)
            {
                cinfo->faces[i].com /= cinfo->faces[i].area;
            }
        }
    }

    Vector tcom = Vector();
    double tvolume = 0.;
    for (size_t i=0; i<cinfo->pieces.size(); ++i)
    {
        // Clean up any missing faces and calculate center of mass for each Piece using Faces
        cinfo->pieces[i].com = Vector ();
        for (size_t j=0; j<cinfo->pieces[i].face_cnt; ++j)
        {
            if (cinfo->faces.size() <= cinfo->pieces[i].face_idx[j])
            {
                cinfo->faces.resize(cinfo->pieces[i].face_idx[j]+1);
                cinfo->faces[cinfo->pieces[i].face_idx[j]].com = Vector ();
                cinfo->faces[cinfo->pieces[i].face_idx[j]].area = 0.;
                cinfo->faces[cinfo->pieces[i].face_idx[j]].normal = Vector ();
            }
            cinfo->pieces[i].com += cinfo->faces[cinfo->pieces[i].face_idx[j]].com;
        }
        if (cinfo->pieces[i].face_cnt)
        {
            cinfo->pieces[i].com /= cinfo->pieces[i].face_cnt;
        }

        // Calculate volume for each Piece using center of mass for each Face
        // Calculate normal for each Face that faces away from center of piece, or center of object
        cinfo->pieces[i].volume = 0.;
        for (size_t j=0; j<cinfo->pieces[i].face_cnt; ++j)
        {
            Vector dv = cinfo->faces[(cinfo->pieces[i].face_idx[j])].com - cinfo->pieces[i].com;
            if (dv.norm() != 0.)
            {
                cinfo->pieces[i].volume += cinfo->faces[(cinfo->pieces[i].face_idx[j])].area * dv.norm() / 3.;
            }
        }
        cinfo->node.face_cnt = cinfo->faces.size();
        tvolume += cinfo->pieces[i].volume;
        tcom +=  cinfo->pieces[i].com * cinfo->pieces[i].volume;
    }
    if (tvolume)
    {
        tcom /= tvolume;
    }

    // Calculate special norm for external panels
    for (size_t i=0; i<cinfo->pieces.size(); ++i)
    {
        if (cinfo->pieces[i].face_cnt == 1)
        {
            Vector dv = cinfo->faces[cinfo->pieces[i].face_idx[0]].com - tcom;
            if (dv.separation(cinfo->faces[cinfo->pieces[i].face_idx[0]].normal) > DPI2)
            {
                cinfo->normals.push_back(-cinfo->faces[cinfo->pieces[i].face_idx[0]].normal);
                cinfo->pieces[i].face_idx[0] = cinfo->normals.size() - 1;
            }
        }
    }

    // Recenter all vectors to object center of mass
    for (size_t i=0; i<cinfo->vertexs.size(); ++i)
    {
        cinfo->vertexs[i] -= tcom;
    }

    for (size_t i=0; i<cinfo->faces.size(); ++i)
    {
        cinfo->faces[i].com -= tcom;
        cinfo->faces[i].normal -= tcom;
    }

    for (size_t i=0; i<cinfo->pieces.size(); ++i)
    {
        cinfo->pieces[i].com -= tcom;
    }
    return 0;
}

int32_t json_updatecosmosstruc(cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    size_t count = 0;

    for (auto &dev : cinfo->devspec.ant)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.batt)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.bcreg)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.bus)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.cam)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.cpu)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.disk)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.gps)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.gyro)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.htr)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.imu)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.mag)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.mcc)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.motr)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.mtr)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.pload)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.prop)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.psen)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.pvstrg)
    {
        cinfo->device[dev.cidx] = &dev;
//        json_mapcompentry(dev.cidx, cinfo);
//        json_mapdeviceentry(&dev, cinfo);
        ++count;
    }
    for (auto &dev : cinfo->devspec.rot)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.rw)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.rxr)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.ssen)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.stt)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.suchi)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.swch)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.tcu)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.tcv)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.telem)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.thst)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.tnc)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.tsen)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.txr)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }
    for (auto &dev : cinfo->devspec.xyzsen)
    {
        cinfo->device[dev.cidx] = &dev;
        ++count;
    }

    cinfo->node.vertex_cnt = cinfo->vertexs.size();
    cinfo->node.normal_cnt = cinfo->normals.size();
    cinfo->node.face_cnt = cinfo->faces.size();
    cinfo->node.piece_cnt = cinfo->pieces.size();
    cinfo->node.device_cnt = cinfo->device.size();
    cinfo->node.port_cnt = cinfo->port.size();
    cinfo->node.agent_cnt = cinfo->agent.size();
    cinfo->node.event_cnt = cinfo->event.size();
    cinfo->node.target_cnt = cinfo->target.size();
    cinfo->node.user_cnt = cinfo->user.size();
    cinfo->node.tle_cnt = cinfo->tle.size();

    iretn = json_mapentries(cinfo);
    if (iretn >= 0)
    {
        cinfo->add_default_names();
    }

    return iretn;
}

//! Setup JSON Namespace using Node description JSON
/*! Create an entry in the JSON mapping tables between each name in the Name Space and the
 * \ref cosmosstruc. Load descriptive information from a structure of JSON descriptions.
 * \param json Structure containing JSON descriptions.
 * \param cinfo Pointer to cinfo ::cosmosstruc.
 * \param create_flag Whether or not to create node directory if it doesn't already exist.
   \return 0, or a negative \ref error_defines
*/
int32_t json_setup_node(jsonnode json, cosmosstruc *cinfo, bool create_flag)
{
    int32_t iretn = 0;
    struct stat fstat;
    ifstream ifs;
    string fname;

    if (!cinfo->jmapped)
    {
        return (JSON_ERROR_NOJMAP);
    }

    cinfo->node.utcoffset = 0.;
    // First: parse data for summary information - includes piece_cnt, device_cnt and port_cnt
    if (!json.node.empty())
    {
        // NS1
        // if ((iretn = json_parse(json.node, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
        // NS2 maybe use set_json?  do we even need this?  what is the purpose of jsonnode?
        if ((iretn = json_parse(json.node, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return iretn;
        }
    }
    else
    {
        return NODE_ERROR_NODE;
    }

    string nodepath;
    //    cinfo->name = cinfo->node.name;
    bool dump_flag = false;
    if (create_flag)
    {
        if ((nodepath = get_nodedir(cinfo->node.name)).empty())
        {
            dump_flag = true;
        }
    }
    nodepath = get_nodedir(cinfo->node.name, create_flag);

    // 1A: load state vector, if it is present
    if (!json.state.empty())
    {
        // NS1
        // if ((iretn = json_parse(json.state, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
        // NS2 maybe use set_json?  do we even need this?  what is the purpose of jsonnode?
        if ((iretn = json_parse(json.state, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return iretn;
        }
        //        loc_update(&cinfo->node.loc);
    }

    // Second: enter information for pieces
    // Vertices
    cinfo->vertexs.clear();
    if (cinfo->node.vertex_cnt)
    {
        // be careful about resizing vertex past MAX_NUMBER_VERTEXS
        cinfo->vertexs.resize(cinfo->node.vertex_cnt);
        if (cinfo->vertexs.size() != cinfo->node.vertex_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }
        for (uint16_t i=0; i<cinfo->node.vertex_cnt; i++)
        {
            //Add relevant names to namespace
            json_mapvertexentry(i, cinfo);
        }

        // Parse data for vertex information
        if (!json.vertexs.empty())
        {
            if ((iretn = json_parse(json.vertexs, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }
    }


    // Faces
    cinfo->faces.clear();
    if (cinfo->node.face_cnt)
    {
        cinfo->faces.resize(cinfo->node.face_cnt);
        if (cinfo->faces.size() != cinfo->node.face_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }

        for (uint16_t i=0; i<cinfo->node.face_cnt; i++)
        {
            //Add relevant names to namespace
            json_mapfaceentry(i, cinfo);
        }

        // Parse data for face information
        if (!json.faces.empty())
        {
            if ((iretn = json_parse(json.faces, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }

        // Do it a second time because now we know how many vertices in each face.
        for (uint16_t i=0; i<cinfo->node.face_cnt; i++)
        {
            //Add relevant names to namespace
            cinfo->faces[i].vertex_idx.resize(cinfo->faces[i].vertex_cnt);
            json_mapfaceentry(i, cinfo);
        }

        // Parse data for face information
        if (!json.faces.empty())
        {
            if ((iretn = json_parse(json.faces, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }
    }

    // Resize, then add entries to map for pieces
    cinfo->pieces.clear();
    if (cinfo->node.piece_cnt)
    {
        cinfo->pieces.resize(cinfo->node.piece_cnt);
        if (cinfo->pieces.size() != cinfo->node.piece_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }

        for (uint16_t i=0; i<cinfo->node.piece_cnt; i++)
        {
            // Initialize to disabled
            cinfo->pieces[i].enabled = false;
            cinfo->pieces[i].face_cnt = 0;
            //Add relevant names to namespace
            json_mappieceentry(i, cinfo);
            // Initialize to no component
            cinfo->pieces[i].cidx = DeviceType::NONE;
        }

        // Parse data for piece information
        if (!json.pieces.empty())
        {
            if ((iretn = json_parse(json.pieces, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }

        // Do it a second time, now that we know how many faces in each piece
        for (uint16_t i=0; i<cinfo->node.piece_cnt; i++)
        {
            cinfo->pieces[i].face_idx.resize(cinfo->pieces[i].face_cnt);
            json_mappieceentry(i, cinfo);
        }

        // Parse data for piece information
        if (!json.pieces.empty())
        {
            if ((iretn = json_parse(json.pieces, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }

        // Work through jmap, enabling each piece for which piece_type has been enabled
        for (size_t i=0; i<cinfo->node.piece_cnt; i++)
        {
            cinfo->pieces[i].enabled = json_checkentry("piece_name", i, UINT16_MAX, cinfo);
        }

        // Third: enter information for all devices
        // Aadd entries to map for devices
        // Add entries to map for Devices and set pointers in nodestruc for comp and devspec
        // Parse data for general device information
        if (!json.devgen.empty())
        {
            Json devgen;
            devgen.extract_contents(json.devgen);
            Json::Object jobj = devgen.ObjectContents;
            vector<size_t> pidx;
            for (auto &dev : jobj)
            {
                size_t cidx = stol(dev.first.substr(dev.first.find_last_of('_')+1));
                if (dev.first.find("all_pidx") != string::npos)
                {
                    pidx.push_back(dev.second.nvalue);
                }
                else if (dev.first.find("all_type") != string::npos)
                {
                    iretn = json_adddevice(cinfo, pidx[cidx], (DeviceType)(dev.second.nvalue));
                }
            }

            json_updatecosmosstruc(cinfo);

            for (uint16_t i=0; i< cinfo->node.device_cnt; i++)
            {
                // Initialize to disabled
                cinfo->device[i]->enabled = false;
                // Add relevant names for generic device to namespace
                json_mapcompentry(i, cinfo);
                // Initialize to no port
                cinfo->device[i]->portidx = PORT_TYPE_NONE;
            }

            if ((iretn = json_parse(json.devgen, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }

        // Fix any mis registered pieces
        for (size_t i=0; i < cinfo->node.piece_cnt; ++i)
        {
            if (cinfo->pieces[i].cidx != UINT16_MAX)
            {
                if (cinfo->pieces[i].cidx >= cinfo->device.size())
                {
                    cinfo->pieces[i].cidx = UINT16_MAX;
                } else {
                    cinfo->device[cinfo->pieces[i].cidx]->pidx = i;
                }
            }
        }

        // Work through jmap, enabling each device for which device_type has been enabled
        for (size_t i=0; i<cinfo->node.device_cnt; i++)
        {
            cinfo->device[i]->enabled = json_checkentry("device_type", i, UINT16_MAX, cinfo);
        }

        // Fourth: enter information for specific devices
        // Add entries to map for Devices specific information
        for (uint16_t i=0; i< cinfo->node.device_cnt; i++)
        {
            json_mapdeviceentry(cinfo->device[i], cinfo);
            //            json_pushdevspec(i, cinfo);
        }

        // Parse data for specific device information
        if (!json.devspec.empty())
        {
            if ((iretn = json_parse(json.devspec, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }

        // Clean up any errors and perform some initializations
        for (uint16_t i=0; i< cinfo->node.device_cnt; i++)
        {
            cinfo->device[i]->cidx = i;
            cinfo->device[i]->amp = cinfo->device[i]->namp;
            cinfo->device[i]->volt = cinfo->device[i]->nvolt;
        }

        // Fifth: enter information for ports
        // Resize, then add names for ports
        cinfo->port.resize(cinfo->node.port_cnt);
        if (cinfo->port.size() != cinfo->node.port_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }

        for (uint16_t i=0; i<cinfo->node.port_cnt; i++)
        {
            json_mapportentry(i, cinfo);
        }

        // Parse data for port information
        if (!json.ports.empty())
        {
            if ((iretn = json_parse(json.ports, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }

        node_calc(cinfo);

        //! Load targeting information
        if (!json.targets.empty())
        {
            if ((iretn = json_parse(json.targets, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return iretn;
            }
        }

        //! Load alias map
        fname = nodepath + "/aliases.ini";
        if ((iretn=stat(fname.c_str(),&fstat)) == 0)
        {
            ifs.open(fname);
            if (ifs.is_open())
            {
                string alias;
                while (getline(ifs, alias, ' '))
                {
                    string cname;
                    getline(ifs, cname);
                    json_addentry(alias, cname, cinfo);
                } ;
            }
        }

    }

    cinfo->json = json;
    double mjd = currentmjd(cinfo->node.utcoffset);

    if (cinfo->node.type == NODE_TYPE_SUN)
    {
        Convert::jplpos(JPL_EARTH, JPL_SUN, Convert::utc2tt(mjd), &cinfo->node.loc.pos.eci);
        cinfo->node.loc.pos.eci.utc = mjd;
        cinfo->node.loc.pos.eci.pass++;
        Convert::pos_eci(&cinfo->node.loc);
    }

    if (cinfo->node.type == NODE_TYPE_MOON)
    {
        Convert::jplpos(JPL_EARTH, JPL_MOON, Convert::utc2tt(mjd), &cinfo->node.loc.pos.eci);
        cinfo->node.loc.pos.eci.utc = mjd;
        cinfo->node.loc.pos.eci.pass++;
        Convert::pos_eci(&cinfo->node.loc);
    }

    if (cinfo->node.type == NODE_TYPE_MARS)
    {
        Convert::jplpos(JPL_EARTH, JPL_MARS, Convert::utc2tt(mjd), &cinfo->node.loc.pos.eci);
        cinfo->node.loc.pos.eci.utc = mjd;
        cinfo->node.loc.pos.eci.pass++;
        Convert::pos_eci(&cinfo->node.loc);
    }

    if (dump_flag && !nodepath.empty()) { json_dump_node(cinfo); }

    return 0;
}

//! Setup JSON Namespace using file.
/*! Create an entry in the JSON mapping tables between each name in the Name Space and the
 * \ref cosmosstruc. Load descriptive information from files in a Node directory of the goven name.
 *	\param node Name and/or path of node directory. If name, then a path will be created
 * based on nodedir setting. If path, then name will be extracted from the end.
 *	\param cinfo Pointer to cinfo ::cosmosstruc.
    \return 0, or a negative value from \ref error_defines.
*/
int32_t json_setup_node(string &node, cosmosstruc *cinfo)
{
    if (cinfo == nullptr) { return (JSON_ERROR_NOJMAP); }

    if (node.empty())
    {
        DeviceCpu deviceCpu;
        node = deviceCpu.getHostName();
        if (node.empty()) { return JSON_ERROR_NAME_LENGTH; }
    }

    if (node.length() > COSMOS_MAX_NAME) { node = node.substr(0, COSMOS_MAX_NAME); }

    jsonnode json;
    int32_t iretn = json_load_node(node, json);
    if (iretn < 0 || json.node.empty())
    {
        iretn = json_create_node(cinfo, node);
        return iretn;
    }

    iretn = json_setup_node(json, cinfo);
    if (iretn < 0) { return iretn; }

    return 0;
}

//! Save Node entries to disk
/*! Create all of the initialization files that represent the Node in the provided
 * ::cosmosstruc.
 * \param cinfo Reference to ::cosmosstruc to use.
 * \return Zero if successful, otherwise negative error.
 */
int32_t json_dump_node(cosmosstruc *cinfo)
{
    string jst;
    string filename;
//    json_mapentries(cinfo);
    json_updatecosmosstruc(cinfo);

    // Node
    string output = json_node(jst, cinfo);
    string fileloc = get_nodedir(cinfo->node.name, true);
    if (fileloc.empty()) { return DATA_ERROR_NODES_FOLDER; }
    rename((fileloc+"/node.ini").c_str(), (fileloc+"/node.ini.old").c_str());
    //    string filename = fileloc + "/node.ini";
    FILE *file = fopen((fileloc+"/node.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Vertices
    output = json_vertices(jst, cinfo);
    rename((fileloc+"/vertices.ini").c_str(), (fileloc+"/vertices.ini.old").c_str());
    //    filename = fileloc + "/vertices.ini";
    file = fopen((fileloc + "/vertices.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // cinfo->faces
    output = json_faces(jst, cinfo);
    rename((fileloc+"/faces.ini").c_str(), (fileloc+"/faces.ini.old").c_str());
    //    filename = fileloc + "/faces.ini";
    file = fopen((fileloc + "/faces.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Pieces
    output = json_pieces(jst, cinfo);
    rename((fileloc+"/pieces.ini").c_str(), (fileloc+"/pieces.ini.old").c_str());
    //    filename = fileloc + "/pieces.ini";
    file = fopen((fileloc + "/pieces.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // General Devices
    output = json_devices_general(jst, cinfo);
    rename((fileloc+"/devices_general.ini").c_str(), (fileloc+"/devices_general.ini.old").c_str());
    //    filename = fileloc + "/devices_general.ini";
    file = fopen((fileloc + "/devices_general.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Specific Devices
    output = json_devices_specific(jst, cinfo);
    rename((fileloc+"/devices_specific.ini").c_str(), (fileloc+"/devices_specific.ini.old").c_str());
    //    filename = fileloc + "/devices_specific.ini";
    file = fopen((fileloc + "/devices_specific.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Ports
    output = json_ports(jst, cinfo);
    rename((fileloc+"/ports.ini").c_str(), (fileloc+"/ports.ini.old").c_str());
    //    filename = fileloc + "/ports.ini";
    file = fopen((fileloc + "/ports.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Aliases
    if (cinfo->alias.size() || cinfo->equation.size())
    {
        rename((fileloc+"/aliases.ini").c_str(), (fileloc+"/aliases.ini.old").c_str());
        file = fopen((fileloc+"/aliases.ini").c_str(), "w");
        if (file == nullptr) { return -errno; }
        for (aliasstruc &alias : cinfo->alias) {
            //            fprintf(file, "%s %s\n", alias.name.c_str(), cinfo->emap[alias.handle.hash][alias.handle.index].text);
            fprintf(file, "%s %s\n", alias.name.c_str(), cinfo->emap[alias.handle.hash][alias.handle.index].text.c_str());
        }
        for (equationstruc &equation : cinfo->equation) {
            fprintf(file, "%s %s\n", equation.name.c_str(), equation.value.c_str());
        }
        fclose(file);
    }
    return 0;
}

//! Map all entries in JMAP
//! Add or update all possible entries in the Namespace map.
//! \param cinfo Reference to ::cosmosstruc to use.
//! \return The current number of entries, if successful, 0 if the entry could not be mapped.

// similar to cosmosstruc::add_default_names() for Namespace 2.0
int32_t json_mapentries(cosmosstruc *cinfo)
{
    json_mapbaseentries(cinfo);

//    for (uint16_t i=0; i<cinfo->node.vertex_cnt; i++)
//    {
//        json_mapvertexentry(i, cinfo);
//    }

//    for (uint16_t i=0; i<cinfo->node.face_cnt; i++)
//    {
//        json_mapfaceentry(i, cinfo);
//    }

    for (uint16_t i=0; i<cinfo->node.piece_cnt; i++)
    {
        json_mappieceentry(i, cinfo);
    }

    for (uint16_t i=0; i<cinfo->node.device_cnt; i++)
    {
        json_mapcompentry(i, cinfo);
    }

    for (uint16_t i=0; i<cinfo->node.device_cnt; i++)
    {
        json_mapdeviceentry(cinfo->device[i], cinfo);
    }

    for (uint16_t i=0; i<cinfo->node.port_cnt; i++)
    {
        json_mapportentry(i, cinfo);
    }

    return cinfo->jmapped;
}

//! Add base entries to JMAP
/*! Add all of the base entries to the Namespace map.
*	\param cinfo Reference to ::cosmosstruc to use.
    \return The current number of entries, if successful, 0 if the entry could not be
 */

// similar to cosmosstruc::add_default_names() for Namespace 2.0
int32_t json_mapbaseentries(cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    // User structure
    iretn = json_addentry("user_node", UINT16_MAX, UINT16_MAX,offsetof(userstruc,node), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_USER, cinfo);
    json_addentry("user_name", UINT16_MAX, UINT16_MAX,offsetof(userstruc,name), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_USER, cinfo);
    json_addentry("user_tool", UINT16_MAX, UINT16_MAX,offsetof(userstruc,tool), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_USER, cinfo);
    json_addentry("user_cpu", UINT16_MAX, UINT16_MAX,offsetof(userstruc,cpu), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_USER, cinfo);


    // Agent structure
    json_addentry("agent_addr", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,addr), (uint16_t)JSON_TYPE_NAME, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_aprd", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,aprd), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_AGENT, cinfo, JSON_UNIT_TIME);
    json_addentry("agent_beat", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat), (uint16_t)JSON_TYPE_HBEAT, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_bprd", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,bprd), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_AGENT, cinfo, JSON_UNIT_TIME);
    json_addentry("agent_bsz", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,bsz), (uint16_t)JSON_TYPE_UINT16, JSON_STRUCT_AGENT, cinfo, JSON_UNIT_BYTES);
    json_addentry("agent_node", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,node), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_ntype", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,ntype), (uint16_t)JSON_TYPE_UINT16, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_pid", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,pid), (uint16_t)JSON_TYPE_INT32, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_port", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,port), (uint16_t)JSON_TYPE_UINT16, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_proc", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,proc), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_stateflag", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,stateflag), (uint16_t)JSON_TYPE_UINT16, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_user", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,user), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_utc", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,utc), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_AGENT, cinfo, JSON_UNIT_DATE);
    json_addentry("agent_cpu", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,cpu), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_memory", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,memory), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_AGENT, cinfo, JSON_UNIT_BYTES);
    json_addentry("agent_jitter", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,jitter), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_AGENT, cinfo);
    json_addentry("agent_dcycle", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,dcycle), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_AGENT, cinfo);

    // Event structure
    json_addentry("event_cbytes", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,cbytes), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_BYTES);
    json_addentry("event_cenergy", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,cenergy), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_ENERGY);
    json_addentry("event_cmass", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,cmass), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_MASS);
    json_addentry("event_condition", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,condition), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_EVENT, cinfo);
    json_addentry("event_ctime", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,ctime), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_TIME);
    json_addentry("event_data", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,data), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_BYTES);
    json_addentry("event_dbytes", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,dbytes), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_BYTES);
    json_addentry("event_denergy", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,denergy), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_ENERGY);
    json_addentry("event_dmass", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,dmass), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_MASS);
    json_addentry("event_dtime", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,dtime), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_TIME);
    json_addentry("event_flag", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,flag), (uint16_t)JSON_TYPE_UINT32, JSON_STRUCT_EVENT, cinfo);
    json_addentry("event_name", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,name), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_EVENT, cinfo);
    json_addentry("event_node", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,node), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_EVENT, cinfo);
    json_addentry("event_type", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,type), (uint16_t)JSON_TYPE_UINT32, JSON_STRUCT_EVENT, cinfo);
    json_addentry("event_user", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,user), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_EVENT, cinfo);
    json_addentry("event_utc", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,utc), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_DATE);
    json_addentry("event_utcexec", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,utcexec), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_EVENT, cinfo, JSON_UNIT_DATE);
    json_addentry("event_value", UINT16_MAX, UINT16_MAX,offsetof(eventstruc,value), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_EVENT, cinfo);

    // Physics structure
    json_addentry("physics_dt", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,dt), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TIME);
    json_addentry("physics_dtj", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,dtj), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_DATE);
    json_addentry("physics_mjdbase", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,utc), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_DATE);
    json_addentry("physics_mjdaccel", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mjdaccel), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_DATE);
    json_addentry("physics_mjddiff", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mjddiff), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_DATE);
    json_addentry("physics_mode", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mode), (uint16_t)JSON_TYPE_INT32, JSON_STRUCT_PHYSICS, cinfo);
    json_addentry("physics_thrust", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,thrust), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_FORCE);
    json_addentry("physics_adrag", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,adrag), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_FORCE);
    json_addentry("physics_rdrag", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,rdrag), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_FORCE);
    json_addentry("physics_atorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,atorque), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_rtorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,rtorque), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_gtorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,gtorque), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_htorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,htorque), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_ctorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ctorque), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_ftorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_ftorque_x", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque.x), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_ftorque_y", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque.y), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_ftorque_z", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque.z), (uint16_t)JSON_TYPE_DOUBLE, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_TORQUE);
    json_addentry("physics_hcap", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,hcap), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_PHYSICS, cinfo);
    json_addentry("physics_area", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,area), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_AREA);
    json_addentry("physics_moi", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,moi), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_MOI);
    json_addentry("physics_com", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,com), (uint16_t)JSON_TYPE_RVECTOR, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_LENGTH);
    json_addentry("physics_mass", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mass), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_PHYSICS, cinfo, JSON_UNIT_MASS);
    json_addentry("physics_heat", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,heat), (uint16_t)JSON_TYPE_FLOAT, JSON_STRUCT_PHYSICS, cinfo);

    // Node Structure
    json_addentry("node_utcoffset", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.utcoffset, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_DATE);
    json_addentry("node_name", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.name, (uint16_t)JSON_TYPE_STRING, cinfo);
    json_addentry("node_agent", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.agent, (uint16_t)JSON_TYPE_STRING, cinfo);
    json_addentry("node_lastevent", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.lastevent, (uint16_t)JSON_TYPE_STRING, cinfo);
    json_addentry("node_lasteventutc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.lasteventutc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
    json_addentry("node_type", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.type, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_state", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.state, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_hcap", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.hcap, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("node_mass", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.mass, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_MASS);
    json_addentry("node_area", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.area, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_AREA);
    json_addentry("node_moi", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.moi, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_MOI);
    json_addentry("node_battcap", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.battcap, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CHARGE);
    json_addentry("node_flags", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.flags, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_powmode", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.powmode, (uint16_t)JSON_TYPE_INT16, cinfo);
    json_addentry("node_powgen", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.powgen, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_POWER);
    json_addentry("node_powuse", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.powuse, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_POWER);
    json_addentry("node_battlev", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.phys.battlev, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CHARGE);
    json_addentry("node_downtime", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.downtime, (uint16_t)JSON_TYPE_UINT32, cinfo, JSON_UNIT_CHARGE);
    json_addentry("node_utc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_DATE);
    json_addentry("node_utcstart", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.utcstart, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_DATE);
    json_addentry("node_deci", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.deci, (uint16_t)JSON_TYPE_UINT32, cinfo, JSON_UNIT_DATE);

    json_addentry("node_loc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc, (uint16_t)JSON_TYPE_LOCSTRUC, cinfo);
    json_addentry("node_loc_utc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
    json_addentry("node_loc_pos", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos, (uint16_t)JSON_TYPE_POSSTRUC, cinfo);
    json_addentry("node_loc_pos_geod", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geod, (uint16_t)JSON_TYPE_POS_GEOD, cinfo);
    json_addentry("node_loc_pos_geod_v_lat", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geod.s.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_pos_geod_v_lon", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geod.s.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_pos_geod_v_h", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geod.s.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_pos_geod_s_lat", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geod.s.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_pos_geod_s_lon", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geod.s.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_pos_geod_s_h", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geod.s.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_pos_geoc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geoc, (uint16_t)JSON_TYPE_POS_GEOC, cinfo);
    json_addentry("node_loc_pos_geoc_v_x", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geoc.v.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_pos_geoc_v_y", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geoc.v.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_pos_geoc_v_z", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geoc.v.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_pos_geoc_s_x", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geoc.s.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_pos_geoc_s_y", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geoc.s.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_pos_geoc_s_z", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geoc.s.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_pos_geos", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.geos, (uint16_t)JSON_TYPE_POS_GEOS, cinfo);
    json_addentry("node_loc_pos_eci", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.eci, (uint16_t)JSON_TYPE_POS_ECI, cinfo);
    json_addentry("node_loc_pos_eci_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.eci.s, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_pos_eci_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.eci.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_pos_eci_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.eci.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ACCELERATION);
    json_addentry("node_loc_pos_sci", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.sci, (uint16_t)JSON_TYPE_POS_SCI, cinfo);
    json_addentry("node_loc_pos_selc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.selc, (uint16_t)JSON_TYPE_POS_SELC, cinfo);
    json_addentry("node_loc_pos_selg", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.selg, (uint16_t)JSON_TYPE_POS_SELG, cinfo);
    json_addentry("node_loc_pos_icrf", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.icrf, (uint16_t)JSON_TYPE_POS_ICRF, cinfo);
    json_addentry("node_loc_pos_sunsize", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.sunsize, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_pos_sunradiance", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.sunradiance, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_pos_earthsep", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.earthsep, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_pos_moonsep", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.moonsep, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("node_loc_att", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att, (uint16_t)JSON_TYPE_ATTSTRUC, cinfo);
    json_addentry("node_loc_att_icrf", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.icrf, (uint16_t)JSON_TYPE_QATT_ICRF, cinfo);
    json_addentry("node_loc_att_icrf_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.icrf.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_att_icrf_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.icrf.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_att_icrf_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.icrf.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_att_topo", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.topo, (uint16_t)JSON_TYPE_QATT_TOPO, cinfo);
    json_addentry("node_loc_att_topo_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.topo.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_att_topo_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.topo.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_att_topo_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.topo.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_att_geoc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.geoc, (uint16_t)JSON_TYPE_QATT_GEOC, cinfo);
    json_addentry("node_loc_att_geoc_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.geoc.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_att_geoc_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.geoc.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_att_geoc_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.geoc.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_att_lvlh", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.lvlh, (uint16_t)JSON_TYPE_QATT_LVLH, cinfo);
    json_addentry("node_loc_att_lvlh_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.lvlh.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_att_lvlh_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.lvlh.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_att_lvlh_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.lvlh.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_att_selc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.selc, (uint16_t)JSON_TYPE_QATT_SELC, cinfo);
    json_addentry("node_loc_att_selc_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.selc.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_att_selc_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.selc.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_att_selc_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.att.selc.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_bearth", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.bearth, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_orbit", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc.pos.orbit, (uint16_t)JSON_TYPE_DOUBLE, cinfo);

    json_addentry("node_loc_est", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est, (uint16_t)JSON_TYPE_LOCSTRUC, cinfo);
    json_addentry("node_loc_est_utc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
    json_addentry("node_loc_est_pos", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos, (uint16_t)JSON_TYPE_POSSTRUC, cinfo);
    json_addentry("node_loc_est_pos_geod", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geod, (uint16_t)JSON_TYPE_POS_GEOD, cinfo);
    json_addentry("node_loc_est_pos_geod_v_lat", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geod.s.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_est_pos_geod_v_lon", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geod.s.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_est_pos_geod_v_h", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geod.s.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_est_pos_geod_s_lat", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geod.s.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_est_pos_geod_s_lon", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geod.s.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_est_pos_geod_s_h", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geod.s.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_est_pos_geoc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geoc, (uint16_t)JSON_TYPE_POS_GEOC, cinfo);
    json_addentry("node_loc_est_pos_geoc_v_x", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geoc.v.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_est_pos_geoc_v_y", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geoc.v.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_est_pos_geoc_v_z", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geoc.v.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_est_pos_geoc_s_x", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geoc.s.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_est_pos_geoc_s_y", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geoc.s.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_est_pos_geoc_s_z", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geoc.s.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_est_pos_geos", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.geos, (uint16_t)JSON_TYPE_POS_GEOS, cinfo);
    json_addentry("node_loc_est_pos_eci", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.eci, (uint16_t)JSON_TYPE_POS_ECI, cinfo);
    json_addentry("node_loc_est_pos_eci_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.eci.s, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_est_pos_eci_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.eci.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_est_pos_eci_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.eci.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ACCELERATION);
    json_addentry("node_loc_est_pos_sci", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.sci, (uint16_t)JSON_TYPE_POS_SCI, cinfo);
    json_addentry("node_loc_est_pos_selc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.selc, (uint16_t)JSON_TYPE_POS_SELC, cinfo);
    json_addentry("node_loc_est_pos_selg", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.selg, (uint16_t)JSON_TYPE_POS_SELG, cinfo);
    json_addentry("node_loc_est_pos_icrf", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.icrf, (uint16_t)JSON_TYPE_POS_ICRF, cinfo);
    json_addentry("node_loc_est_pos_sunsize", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.sunsize, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_est_pos_sunradiance", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.sunradiance, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_est_pos_earthsep", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.earthsep, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_est_pos_moonsep", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.moonsep, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("node_loc_est_att", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att, (uint16_t)JSON_TYPE_ATTSTRUC, cinfo);
    json_addentry("node_loc_est_att_icrf", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.icrf, (uint16_t)JSON_TYPE_QATT_ICRF, cinfo);
    json_addentry("node_loc_est_att_icrf_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.icrf.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_est_att_icrf_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.icrf.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_est_att_icrf_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.icrf.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_est_att_topo", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.topo, (uint16_t)JSON_TYPE_QATT_TOPO, cinfo);
    json_addentry("node_loc_est_att_topo_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.topo.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_est_att_topo_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.topo.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_est_att_topo_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.topo.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_est_att_geoc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.geoc, (uint16_t)JSON_TYPE_QATT_GEOC, cinfo);
    json_addentry("node_loc_est_att_geoc_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.geoc.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_est_att_geoc_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.geoc.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_est_att_geoc_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.geoc.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_est_att_lvlh", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.lvlh, (uint16_t)JSON_TYPE_QATT_LVLH, cinfo);
    json_addentry("node_loc_est_att_lvlh_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.lvlh.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_est_att_lvlh_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.lvlh.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_est_att_lvlh_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.lvlh.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_est_att_selc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.selc, (uint16_t)JSON_TYPE_QATT_SELC, cinfo);
    json_addentry("node_loc_est_att_selc_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.selc.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_est_att_selc_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.selc.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_est_att_selc_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.att.selc.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_est_bearth", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.bearth, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_est_orbit", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_est.pos.orbit, (uint16_t)JSON_TYPE_DOUBLE, cinfo);

    json_addentry("node_loc_std", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std, (uint16_t)JSON_TYPE_LOCSTRUC, cinfo);
    json_addentry("node_loc_std_utc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
    json_addentry("node_loc_std_pos", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos, (uint16_t)JSON_TYPE_POSSTRUC, cinfo);
    json_addentry("node_loc_std_pos_geod", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geod, (uint16_t)JSON_TYPE_POS_GEOD, cinfo);
    json_addentry("node_loc_std_pos_geod_v_lat", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geod.s.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_std_pos_geod_v_lon", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geod.s.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_std_pos_geod_v_h", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geod.s.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_std_pos_geod_s_lat", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geod.s.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_std_pos_geod_s_lon", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geod.s.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_std_pos_geod_s_h", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geod.s.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_std_pos_geoc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geoc, (uint16_t)JSON_TYPE_POS_GEOC, cinfo);
    json_addentry("node_loc_std_pos_geoc_v_x", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geoc.v.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_std_pos_geoc_v_y", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geoc.v.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_std_pos_geoc_v_z", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geoc.v.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_std_pos_geoc_s_x", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geoc.s.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_std_pos_geoc_s_y", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geoc.s.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_std_pos_geoc_s_z", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geoc.s.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_std_pos_geos", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.geos, (uint16_t)JSON_TYPE_POS_GEOS, cinfo);
    json_addentry("node_loc_std_pos_eci", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.eci, (uint16_t)JSON_TYPE_POS_ECI, cinfo);
    json_addentry("node_loc_std_pos_eci_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.eci.s, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_loc_std_pos_eci_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.eci.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_SPEED);
    json_addentry("node_loc_std_pos_eci_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.eci.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ACCELERATION);
    json_addentry("node_loc_std_pos_sci", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.sci, (uint16_t)JSON_TYPE_POS_SCI, cinfo);
    json_addentry("node_loc_std_pos_selc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.selc, (uint16_t)JSON_TYPE_POS_SELC, cinfo);
    json_addentry("node_loc_std_pos_selg", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.selg, (uint16_t)JSON_TYPE_POS_SELG, cinfo);
    json_addentry("node_loc_std_pos_icrf", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.icrf, (uint16_t)JSON_TYPE_POS_ICRF, cinfo);
    json_addentry("node_loc_std_pos_sunsize", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.sunsize, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_std_pos_sunradiance", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.sunradiance, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_std_pos_earthsep", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.earthsep, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_loc_std_pos_moonsep", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.moonsep, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("node_loc_std_att", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att, (uint16_t)JSON_TYPE_ATTSTRUC, cinfo);
    json_addentry("node_loc_std_att_icrf", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.icrf, (uint16_t)JSON_TYPE_QATT_ICRF, cinfo);
    json_addentry("node_loc_std_att_icrf_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.icrf.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_std_att_icrf_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.icrf.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_std_att_icrf_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.icrf.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_std_att_topo", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.topo, (uint16_t)JSON_TYPE_QATT_TOPO, cinfo);
    json_addentry("node_loc_std_att_topo_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.topo.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_std_att_topo_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.topo.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_std_att_topo_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.topo.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_std_att_geoc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.geoc, (uint16_t)JSON_TYPE_QATT_GEOC, cinfo);
    json_addentry("node_loc_std_att_geoc_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.geoc.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_std_att_geoc_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.geoc.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_std_att_geoc_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.geoc.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_std_att_lvlh", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.lvlh, (uint16_t)JSON_TYPE_QATT_LVLH, cinfo);
    json_addentry("node_loc_std_att_lvlh_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.lvlh.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_std_att_lvlh_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.lvlh.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_std_att_lvlh_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.lvlh.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_std_att_selc", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.selc, (uint16_t)JSON_TYPE_QATT_SELC, cinfo);
    json_addentry("node_loc_std_att_selc_s", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.selc.s, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
    json_addentry("node_loc_std_att_selc_v", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.selc.v, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
    json_addentry("node_loc_std_att_selc_a", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.att.selc.a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_std_bearth", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.bearth, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
    json_addentry("node_loc_std_orbit", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.loc_std.pos.orbit, (uint16_t)JSON_TYPE_DOUBLE, cinfo);

    json_addentry("node_azfrom", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.azfrom, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_azto", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.azto, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_elfrom", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.elfrom, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_elto", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.elto, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGLE);
    json_addentry("node_range", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.range, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_LENGTH);
    json_addentry("node_device_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.device_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_vertex_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.vertex_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_normal_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.normal_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_face_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.face_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_piece_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.piece_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_port_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.port_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_target_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.target_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_agent_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.agent_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_event_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.event_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("node_user_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->node.user_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_ant_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.ant_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_batt_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.batt_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_bus_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.bus_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_cam_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.cam_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_cpu_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.cpu_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_disk_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.disk_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_gps_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.gps_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_gyro_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.gyro_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_htr_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.htr_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_imu_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.imu_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_mag_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.mag_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_mcc_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.mcc_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_motr_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.motr_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_mtr_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.mtr_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_pload_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.pload_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_prop_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.prop_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_psen_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.psen_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_bcreg_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.bcreg_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_rot_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.rot_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_rw_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.rw_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_rxr_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.rxr_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_ssen_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.ssen_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_pvstrg_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.pvstrg_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_stt_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.stt_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_suchi_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.suchi_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_swch_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.swch_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_tcu_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.tcu_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_tcv_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.tcv_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_telem_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.telem_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_thst_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.thst_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_tnc_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.tnc_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_tsen_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.tsen_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_txr_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.txr_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_xyzsen_cnt", UINT16_MAX, UINT16_MAX, (uint8_t *)&cinfo->devspec.xyzsen_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);

    if (iretn >= 0)
    {
        iretn = cinfo->jmapped;
    }
    return iretn;
}

//! Add vertex entry.
/*! Add an entry for vertex number vidx to the JSON Namespace map.
 \param vidx Piece number.
*	\param cmeta Reference to ::cosmosstruc to use.
    \return The current number of entries, if successful, negative error if the entry could not be
 */

// this really just adds Namespace 1.0 names
// with Namespace 2.0 default names should already be added from MAX_NUMBER_OF_VERTEXS
int32_t json_mapvertexentry(uint16_t vidx, cosmosstruc *cinfo)
{
    int32_t iretn=0;

    iretn = json_addentry("vertex", vidx, UINT16_MAX, (uint8_t *)&cinfo->vertexs[vidx], (uint16_t)JSON_TYPE_VECTOR, cinfo);
    iretn = json_addentry("vertex_x", vidx, UINT16_MAX, (uint8_t *)&cinfo->vertexs[vidx].x, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
    iretn = json_addentry("vertex_y", vidx, UINT16_MAX, (uint8_t *)&cinfo->vertexs[vidx].y, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
    iretn = json_addentry("vertex_z", vidx, UINT16_MAX, (uint8_t *)&cinfo->vertexs[vidx].z, (uint16_t)JSON_TYPE_DOUBLE, cinfo);

    if (iretn >= 0)
    {
        iretn = cinfo->jmapped;
    }
    return iretn;
}

//! Add face entry.
/*! Add an entry for face number fidx to the JSON Namespace map.
 \param fidx Piece number.
*	\param cmeta Reference to ::cosmosstruc to use.
    \return The current number of entries, if successful, negative error if the entry could not be
 */

// this really just adds Namespace 1.0 names
// with Namespace 2.0 default names should already be added from MAX_NUMBER_OF_FACES
int32_t json_mapfaceentry(uint16_t fidx, cosmosstruc *cinfo)
{
    int32_t iretn=0;

    iretn = json_addentry("face", fidx, UINT16_MAX, (uint8_t *)&cinfo->faces[fidx], (uint16_t)JSON_TYPE_FACESTRUC, cinfo);
    iretn = json_addentry("face_com", fidx, UINT16_MAX, (uint8_t *)&cinfo->faces[fidx].com, (uint16_t)JSON_TYPE_VECTOR, cinfo);
    iretn = json_addentry("face_normal", fidx, UINT16_MAX, (uint8_t *)&cinfo->faces[fidx].normal, (uint16_t)JSON_TYPE_VECTOR, cinfo);
    iretn = json_addentry("face_area", fidx, UINT16_MAX, (uint8_t *)&cinfo->faces[fidx].area, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
    iretn = json_addentry("face_vertex_cnt", fidx, UINT16_MAX, (uint8_t *)&cinfo->faces[fidx].vertex_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    for (uint16_t j=0; j<cinfo->faces[fidx].vertex_cnt; ++j)
    {
        iretn = json_addentry("face_vertex_idx", fidx,j,(uint8_t *)&cinfo->faces[fidx].vertex_idx[j], (uint16_t)JSON_TYPE_UINT16, cinfo);
    }

    if (iretn >= 0)
    {
        iretn = cinfo->jmapped;
    }
    return iretn;
}

//! Add piece entry.
/*! Add an entry for piece number pidx to the JSON Namespace map.
 \param pidx Piece number.
*	\param cmeta Reference to ::cosmosstruc to use.
    \return The current number of entries, if successful, negative error if the entry could not be
 */

// this really just adds Namespace 1.0 names
// with Namespace 2.0 default names should already be added from MAX_NUMBER_OF_PIECES
int32_t json_mappieceentry(uint16_t pidx, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    iretn = json_addentry("piece_name", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].name, (uint16_t)JSON_TYPE_STRING, cinfo);
    //    json_addentry("piece_type", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].type, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("piece_cidx", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("piece_mass", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].mass, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_density", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].density, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_emi", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].emi, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_abs", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].abs, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_dim", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].dim, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_hcap", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].hcap, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_hcon", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].hcon, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_area", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].area, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_volume", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].volume, (uint16_t)JSON_TYPE_FLOAT, cinfo);
    json_addentry("piece_com", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].com, (uint16_t)JSON_TYPE_VECTOR, cinfo);
    json_addentry("piece_twist", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].twist, (uint16_t)JSON_TYPE_VECTOR, cinfo);
    json_addentry("piece_shove", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].shove, (uint16_t)JSON_TYPE_VECTOR, cinfo);
    json_addentry("piece_face_cnt", pidx, UINT16_MAX, (uint8_t *)&cinfo->pieces[pidx].face_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
    for (uint16_t j=0; j<cinfo->pieces[pidx].face_cnt; ++j)
    {
        json_addentry("piece_face_idx", pidx,j,(uint8_t *)&cinfo->pieces[pidx].face_idx[j], (uint16_t)JSON_TYPE_UINT16, cinfo);
    }

    if (iretn >= 0)
    {
        iretn = cinfo->jmapped;
    }
    return iretn;
}

//! Toggle piece entry.
/*! Toggle the enable state of an entry for piece number pidx in the JSON Namespace map.
 \param pidx Piece number.
*	\param cmeta Reference to ::cosmosstruc to use.
* \param state Desired enable state.
    \return 0 or negative error.
 */
int32_t json_togglepieceentry(uint16_t pidx, cosmosstruc *cinfo, bool state)
{
    int32_t iretn = 0;

    iretn = json_toggleentry("piece_name", pidx, UINT16_MAX, cinfo, state);
    //    json_toggleentry("piece_type", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_cidx", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_mass", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_density", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_emi", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_abs", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_dim", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_hcap", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_hcon", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_area", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_com", pidx, UINT16_MAX, cinfo, state);
    json_toggleentry("piece_pnt_cnt", pidx, UINT16_MAX, cinfo, state);
    for (uint16_t j=0; j<MAXPNT; ++j)
    {
        json_toggleentry("piece_pnt", pidx,j, cinfo, state);
    }

    return iretn;
}

//! Add component entry.
/*! Add an entry for component number cidx to the JSON Namespace map.
 \param cidx Component number.
*	\param cmeta Reference to ::cosmosstruc to use.
    \return The current number of entries, if successful, 0 if the entry could not be
 */

// this really just adds Namespace 1.0 names
// with Namespace 2.0 default names should already be added from MAX_NUMBER_OF_DEVICES
int32_t json_mapcompentry(uint16_t cidx, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    iretn = json_addentry("device_all_type",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->type, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_model",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->model, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_flag",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->flag, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_addr",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->addr, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_cidx",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_didx",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->didx, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_pidx",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->pidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_bidx",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->bidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_portidx",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->portidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
    json_addentry("device_all_namp",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->namp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CURRENT);
    json_addentry("device_all_nvolt",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->nvolt, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
    json_addentry("device_all_amp",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CURRENT);
    json_addentry("device_all_volt",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
    json_addentry("device_all_power",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->power, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
    json_addentry("device_all_drate",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->drate, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
    json_addentry("device_all_temp",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_TEMPERATURE);
    json_addentry("device_all_utc",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_DATE);
    json_addentry("device_all_expiration",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->expiration, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_DATE);
    json_addentry("device_all_name",cidx, UINT16_MAX, (uint8_t *)&cinfo->device[cidx]->name, (uint16_t)JSON_TYPE_STRING, cinfo);

    if (iretn >= 0)
    {
        iretn = cinfo->jmapped;
    }
    return iretn;
}

//! Toggle component entry.
/*! Toggle the enable state of an entry for component number cidx in the JSON Namespace map.
 \param cidx Component number.
*	\param cmeta Reference to ::cosmosstruc to use.
* \param state Desired enable state.
    \return 0 or negative error.
 */
int32_t json_togglecompentry(uint16_t cidx, cosmosstruc *cinfo, bool state)
{
    int32_t iretn = 0;

    iretn = json_toggleentry("device_all_type",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_model",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_flag",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_addr",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_cidx",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_didx",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_pidx",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_bidx",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_portidx",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_namp",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_nvolt",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_amp",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_volt",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_power",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_drate",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_temp",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_utc",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_expiration",cidx, UINT16_MAX, cinfo, state);
    json_toggleentry("device_all_name",cidx, UINT16_MAX, cinfo, state);

    return iretn;
}

//! Add device entry.
/*! Add entries specific to device number i to the JSON Namespace map.
 * \param cidx Component number.
 \param didx Device number.
 \param type Device type number taken from ::DeviceType.

    \return The current number of entries, if successful, 0 if the entry could not be added.
 */
uint16_t json_mapdeviceentry(devicestruc* devicein, cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    uint16_t didx = devicein->didx;
//    uint16_t cidx = devicein->cidx;
//    uint16_t pidx = cinfo->device[cidx]->pidx;
//    piecestruc *pptr = &cinfo->pieces[pidx];

    switch (static_cast <DeviceType>(devicein->type))
    {
    //! Antenna
    case DeviceType::ANT:
        {
            antstruc* device = reinterpret_cast<antstruc*>(devicein);
            iretn = json_addentry("device_ant_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_ant_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_ant_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_ant_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ant_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_ant_azim",didx, UINT16_MAX, (uint8_t *)&device->azim, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ant_elev",didx, UINT16_MAX, (uint8_t *)&device->elev, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ant_minelev",didx, UINT16_MAX, (uint8_t *)&device->minelev, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Battery
    case DeviceType::BATT:
        {
            battstruc* device = reinterpret_cast<battstruc*>(devicein);
            iretn = json_addentry("device_batt_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_batt_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_batt_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_batt_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_cap",didx, UINT16_MAX, (uint8_t *)&device->capacity, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_eff",didx, UINT16_MAX, (uint8_t *)&device->efficiency, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_charge",didx, UINT16_MAX, (uint8_t *)&device->charge, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_nvolt",didx, UINT16_MAX, (uint8_t *)&device->nvolt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_r_in",didx, UINT16_MAX, (uint8_t *)&device->r_in, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_r_out",didx, UINT16_MAX, (uint8_t *)&device->r_out, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_percentage",didx, UINT16_MAX, (uint8_t *)&device->percentage, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_batt_time_remaining",didx, UINT16_MAX, (uint8_t *)&device->time_remaining, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
    case DeviceType::BCREG:
        {
            bcregstruc* device = reinterpret_cast<bcregstruc*>(devicein);
            iretn = json_addentry("device_bcreg_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_bcreg_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_bcreg_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_bcreg_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_energy",didx, UINT16_MAX, (uint8_t *)&device->energy, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_mpptin_volt",didx, UINT16_MAX, (uint8_t *)&device->mpptin_volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_mpptin_amp",didx, UINT16_MAX, (uint8_t *)&device->mpptin_amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_mpptout_volt",didx, UINT16_MAX, (uint8_t *)&device->mpptout_volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bcreg_mpptout_amp",didx, UINT16_MAX, (uint8_t *)&device->mpptout_amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
    case DeviceType::BUS:
        {
            busstruc* device = reinterpret_cast<busstruc*>(devicein);
            iretn = json_addentry("device_bus_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_bus_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_bus_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_bus_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bus_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bus_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bus_nvolt",didx, UINT16_MAX, (uint8_t *)&device->nvolt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bus_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bus_energy",didx, UINT16_MAX, (uint8_t *)&device->energy, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_bus_wdt",didx, UINT16_MAX, (uint8_t *)&device->wdt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Camera
    case DeviceType::CAM:
        {
            camstruc* device = reinterpret_cast<camstruc*>(devicein);
            iretn = json_addentry("device_cam_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_cam_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_cam_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_cam_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cam_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cam_drate",didx, UINT16_MAX, (uint8_t *)&device->drate, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cam_pwidth",didx, UINT16_MAX, (uint8_t *)&device->pwidth, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_cam_pheight",didx, UINT16_MAX, (uint8_t *)&device->pheight, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_cam_width",didx, UINT16_MAX, (uint8_t *)&device->width, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cam_height",didx, UINT16_MAX, (uint8_t *)&device->height, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cam_flength",didx, UINT16_MAX, (uint8_t *)&device->flength, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Processing Unit
    case DeviceType::CPU:
        {
            cpustruc* device = reinterpret_cast<cpustruc*>(devicein);
            iretn = json_addentry("device_cpu_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_cpu_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_cpu_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_cpu_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_uptime",didx, UINT16_MAX, (uint8_t *)&device->uptime, (uint16_t)JSON_TYPE_UINT32, cinfo);
            json_addentry("device_cpu_maxgib",didx, UINT16_MAX, (uint8_t *)&device->maxgib, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_maxload",didx, UINT16_MAX, (uint8_t *)&device->maxload, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_load",didx, UINT16_MAX, (uint8_t *)&device->load, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_gib",didx, UINT16_MAX, (uint8_t *)&device->gib, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_storage",didx, UINT16_MAX, (uint8_t *)&device->storage, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_cpu_boot_count",didx, UINT16_MAX, (uint8_t *)&device->boot_count, (uint16_t)JSON_TYPE_UINT32, cinfo);
            char tempbuf1[100];
            char tempbuf2[100];
            sprintf(tempbuf1, "cpu_utilization_%03u", didx);
            sprintf(tempbuf2, "(\"device_cpu_load_%03u\"/\"device_cpu_maxload_%03u\")", didx, didx);
            json_addentry(tempbuf1, tempbuf2, cinfo);
            sprintf(tempbuf1, "memory_utilization_%03u", didx);
            sprintf(tempbuf2, "(\"device_cpu_gib_%03u\"/\"device_cpu_maxgib_%03u\")", didx, didx);
            json_addentry(tempbuf1, tempbuf2, cinfo);
        }
        break;
    case DeviceType::DISK:
        {
            diskstruc* device = reinterpret_cast<diskstruc*>(devicein);
            iretn = json_addentry("device_disk_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_disk_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_disk_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_disk_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_disk_maxgib",didx, UINT16_MAX, (uint8_t *)&device->maxgib, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_disk_gib",didx, UINT16_MAX, (uint8_t *)&device->gib, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_disk_path",didx, UINT16_MAX, (uint8_t *)&device->path, (uint16_t)JSON_TYPE_STRING, cinfo);
            char tempbuf1[100];
            char tempbuf2[100];
            sprintf(tempbuf1, "disk_utilization_%03u", didx);
            sprintf(tempbuf2, "(\"device_disk_gib_%03u\"/\"device_disk_maxgib_%03u\")", didx, didx);
            json_addentry(tempbuf1, tempbuf2, cinfo);
            break;
        }
        //! GPS Unit
    case DeviceType::GPS:
        {
            gpsstruc* device = reinterpret_cast<gpsstruc*>(devicein);
            iretn = json_addentry("device_gps_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_gps_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_gps_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_gps_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CURRENT);
            json_addentry("device_gps_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
            json_addentry("device_gps_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_POWER);
            json_addentry("device_gps_dutc",didx, UINT16_MAX, (uint8_t *)&device->dutc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geocs",didx, UINT16_MAX, (uint8_t *)&device->geocs, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_gps_geocs_x",didx, UINT16_MAX, (uint8_t *)&device->geocs.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geocs_y",didx, UINT16_MAX, (uint8_t *)&device->geocs.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geocs_z",didx, UINT16_MAX, (uint8_t *)&device->geocs.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geocv",didx, UINT16_MAX, (uint8_t *)&device->geocv, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_gps_geocv_x",didx, UINT16_MAX, (uint8_t *)&device->geocv.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geocv_y",didx, UINT16_MAX, (uint8_t *)&device->geocv.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geocv_z",didx, UINT16_MAX, (uint8_t *)&device->geocv.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeocs",didx, UINT16_MAX, (uint8_t *)&device->dgeocs, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_gps_dgeocs_x",didx, UINT16_MAX, (uint8_t *)&device->dgeocs.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeocs_y",didx, UINT16_MAX, (uint8_t *)&device->dgeocs.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeocs_z",didx, UINT16_MAX, (uint8_t *)&device->dgeocs.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeocv",didx, UINT16_MAX, (uint8_t *)&device->dgeocv, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_gps_dgeocv_x",didx, UINT16_MAX, (uint8_t *)&device->dgeocv.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeocv_y",didx, UINT16_MAX, (uint8_t *)&device->dgeocv.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeocv_z",didx, UINT16_MAX, (uint8_t *)&device->dgeocv.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geods",didx, UINT16_MAX, (uint8_t *)&device->geods, (uint16_t)JSON_TYPE_GVECTOR, cinfo);
            json_addentry("device_gps_geods_lat",didx, UINT16_MAX, (uint8_t *)&device->geods.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geods_lon",didx, UINT16_MAX, (uint8_t *)&device->geods.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geods_h",didx, UINT16_MAX, (uint8_t *)&device->geods.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geodv",didx, UINT16_MAX, (uint8_t *)&device->geodv, (uint16_t)JSON_TYPE_GVECTOR, cinfo);
            json_addentry("device_gps_geodv_lat",didx, UINT16_MAX, (uint8_t *)&device->geodv.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geodv_lon",didx, UINT16_MAX, (uint8_t *)&device->geodv.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_geodv_h",didx, UINT16_MAX, (uint8_t *)&device->geodv.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeods",didx, UINT16_MAX, (uint8_t *)&device->dgeods, (uint16_t)JSON_TYPE_GVECTOR, cinfo);
            json_addentry("device_gps_dgeods_lat",didx, UINT16_MAX, (uint8_t *)&device->dgeods.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeods_lon",didx, UINT16_MAX, (uint8_t *)&device->dgeods.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeods_h",didx, UINT16_MAX, (uint8_t *)&device->dgeods.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeodv",didx, UINT16_MAX, (uint8_t *)&device->dgeodv, (uint16_t)JSON_TYPE_GVECTOR, cinfo);
            json_addentry("device_gps_dgeodv_lat",didx, UINT16_MAX, (uint8_t *)&device->dgeodv.lat, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeodv_lon",didx, UINT16_MAX, (uint8_t *)&device->dgeodv.lon, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_dgeodv_h",didx, UINT16_MAX, (uint8_t *)&device->dgeodv.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gps_heading",didx, UINT16_MAX, (uint8_t *)&device->heading, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_gps_sats_used",didx, UINT16_MAX, (uint8_t *)&device->sats_used, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_gps_sats_visible",didx, UINT16_MAX, (uint8_t *)&device->sats_visible, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_gps_time_status",didx, UINT16_MAX, (uint8_t *)&device->time_status, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_gps_position_type",didx, UINT16_MAX, (uint8_t *)&device->position_type, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_gps_solution_status",didx, UINT16_MAX, (uint8_t *)&device->solution_status, (uint16_t)JSON_TYPE_UINT16, cinfo);
            break;
        }
        //! Heater
    case DeviceType::HTR:
        {
            htrstruc* device = reinterpret_cast<htrstruc*>(devicein);
            iretn = json_addentry("device_htr_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_htr_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_htr_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_htr_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_htr_state",didx, UINT16_MAX, (uint8_t *)&device->state, (uint16_t)JSON_TYPE_BOOL, cinfo);
            break;
        }
        //! Gyroscope
    case DeviceType::GYRO:
        {
            gyrostruc* device = reinterpret_cast<gyrostruc*>(devicein);
            iretn = json_addentry("device_gyro_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_gyro_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_gyro_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_gyro_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_TEMPERATURE);
            json_addentry("device_gyro_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CURRENT);
            json_addentry("device_gyro_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
            json_addentry("device_gyro_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_POWER);
            json_addentry("device_gyro_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_gyro_omega",didx, UINT16_MAX, (uint8_t *)&device->omega, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGULAR_RATE);
            json_addentry("device_gyro_alpha",didx, UINT16_MAX, (uint8_t *)&device->alpha, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGULAR_ACCELERATION);
            break;
        }
        //! Inertial Measurement Unit
    case DeviceType::IMU:
        {
            imustruc* device = reinterpret_cast<imustruc*>(devicein);
            iretn = json_addentry("device_imu_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_imu_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_imu_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_imu_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_TEMPERATURE);
            json_addentry("device_imu_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CURRENT);
            json_addentry("device_imu_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
            json_addentry("device_imu_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_POWER);
            json_addentry("device_imu_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_imu_accel",didx, UINT16_MAX, (uint8_t *)&device->accel, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ACCELERATION);
            json_addentry("device_imu_accel_x",didx, UINT16_MAX, (uint8_t *)&device->accel.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ACCELERATION);
            json_addentry("device_imu_accel_y",didx, UINT16_MAX, (uint8_t *)&device->accel.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ACCELERATION);
            json_addentry("device_imu_accel_z",didx, UINT16_MAX, (uint8_t *)&device->accel.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ACCELERATION);
            json_addentry("device_imu_theta",didx, UINT16_MAX, (uint8_t *)&device->theta, (uint16_t)JSON_TYPE_QUATERNION, cinfo, JSON_UNIT_NONE);
            json_addentry("device_imu_euler",didx, UINT16_MAX, (uint8_t *)&device->euler, (uint16_t)JSON_TYPE_AVECTOR, cinfo, JSON_UNIT_ANGLE);
            json_addentry("device_imu_euler_h",didx, UINT16_MAX, (uint8_t *)&device->euler.h, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
            json_addentry("device_imu_euler_e",didx, UINT16_MAX, (uint8_t *)&device->euler.e, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
            json_addentry("device_imu_euler_b",didx, UINT16_MAX, (uint8_t *)&device->euler.b, (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_ANGLE);
            json_addentry("device_imu_omega",didx, UINT16_MAX, (uint8_t *)&device->omega, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
            json_addentry("device_imu_alpha",didx, UINT16_MAX, (uint8_t *)&device->alpha, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_imu_mag",didx, UINT16_MAX, (uint8_t *)&device->mag, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_imu_mag_x",didx, UINT16_MAX, (uint8_t *)&device->mag.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_imu_mag_y",didx, UINT16_MAX, (uint8_t *)&device->mag.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_imu_mag_z",didx, UINT16_MAX, (uint8_t *)&device->mag.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_imu_bdot",didx, UINT16_MAX, (uint8_t *)&device->bdot, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            break;
        }
        //! Magnetometer
    case DeviceType::MAG:
        {
            magstruc* device = reinterpret_cast<magstruc*>(devicein);
            iretn = json_addentry("device_mag_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_mag_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_mag_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_mag_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_TEMPERATURE);
            json_addentry("device_mag_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CURRENT);
            json_addentry("device_mag_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
            json_addentry("device_mag_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_POWER);
            json_addentry("device_mag_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
//            json_addentry("device_mag_omega",didx, UINT16_MAX, (uint8_t *)&device->omega, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_ANGULAR_RATE);
//            json_addentry("device_mag_alpha",didx, UINT16_MAX, (uint8_t *)&device->alpha, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_mag_mag",didx, UINT16_MAX, (uint8_t *)&device->mag, (uint16_t)JSON_TYPE_RVECTOR, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_mag_mag_x",didx, UINT16_MAX, (uint8_t *)&device->mag.col[0], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_mag_mag_y",didx, UINT16_MAX, (uint8_t *)&device->mag.col[1], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_mag_mag_z",didx, UINT16_MAX, (uint8_t *)&device->mag.col[2], (uint16_t)JSON_TYPE_DOUBLE, cinfo, JSON_UNIT_MAGFIELD);
            json_addentry("device_mag_bdot",didx, UINT16_MAX, (uint8_t *)&device->bdot, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            break;
        }
        //! Motion Capture Camera
    case DeviceType::MCC:
        {
            mccstruc* device = reinterpret_cast<mccstruc*>(devicein);
            iretn = json_addentry("device_mcc_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_mcc_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_mcc_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_mcc_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_mcc_q",didx, UINT16_MAX, (uint8_t *)&device->q, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_mcc_o",didx, UINT16_MAX, (uint8_t *)&device->o, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_mcc_a",didx, UINT16_MAX, (uint8_t *)&device->a, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_mcc_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            break;
        }
        //! Motor
    case DeviceType::MOTR:
        {
            motrstruc* device = reinterpret_cast<motrstruc*>(devicein);
            iretn = json_addentry("device_motr_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_motr_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_motr_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_motr_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_motr_rat",didx, UINT16_MAX, (uint8_t *)&device->rat, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_motr_max",didx, UINT16_MAX, (uint8_t *)&device->max, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_motr_spd",didx, UINT16_MAX, (uint8_t *)&device->spd, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Magnetic Torque Rod
    case DeviceType::MTR:
        {
            mtrstruc* device = reinterpret_cast<mtrstruc*>(devicein);
            iretn = json_addentry("device_mtr_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_mtr_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_mtr_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_mtr_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_mtr_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_mtr_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_mtr_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_mtr_mxmom",didx, UINT16_MAX, (uint8_t *)&device->mxmom, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_mtr_tc",didx, UINT16_MAX, (uint8_t *)&device->tc, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_mtr_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            for (uint16_t j=0; j<7; j++)
            {
                json_addentry("device_mtr_npoly",didx,j,(uint8_t *)&device->npoly+j*sizeof(float), (uint16_t)JSON_TYPE_FLOAT, cinfo);
                json_addentry("device_mtr_ppoly",didx,j,(uint8_t *)&device->ppoly+j*sizeof(float), (uint16_t)JSON_TYPE_FLOAT, cinfo);
            }
            json_addentry("device_mtr_mom",didx, UINT16_MAX, (uint8_t *)&device->mom, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_MAGMOMENT);
            json_addentry("device_mtr_rmom",didx, UINT16_MAX, (uint8_t *)&device->rmom, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
    case DeviceType::PLOAD:
        {
            ploadstruc* device = reinterpret_cast<ploadstruc*>(devicein);
            iretn = json_addentry("device_pload_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_pload_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_pload_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_pload_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pload_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pload_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pload_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pload_drate",didx, UINT16_MAX, (uint8_t *)&device->drate, (uint16_t)JSON_TYPE_UINT32, cinfo);
            json_addentry("device_pload_key_cnt",didx, UINT16_MAX, (uint8_t *)&device->key_cnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
            for (uint16_t j=0; j<MAXPLOADKEYCNT; j++)
            {
                json_addentry("device_pload_keyidx",didx,j,(uint8_t *)&device->keyidx[j], (uint16_t)JSON_TYPE_UINT16, cinfo);
                json_addentry("device_pload_keyval",didx,j,(uint8_t *)&device->keyval[j], (uint16_t)JSON_TYPE_FLOAT, cinfo);
            }
            break;
        }
        //! Propellant Tank
    case DeviceType::PROP:
        {
            propstruc* device = reinterpret_cast<propstruc*>(devicein);
            iretn = json_addentry("device_prop_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_prop_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_prop_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_prop_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_prop_cap",didx, UINT16_MAX, (uint8_t *)&device->cap, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_prop_lev",didx, UINT16_MAX, (uint8_t *)&device->lev, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Pressure Sensor
    case DeviceType::PSEN:
        {
            psenstruc* device = reinterpret_cast<psenstruc*>(devicein);
            iretn = json_addentry("device_psen_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_psen_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_psen_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_psen_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_psen_press",didx, UINT16_MAX, (uint8_t *)&device->press, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Photo Voltaic String
    case DeviceType::PVSTRG:
        {
            pvstrgstruc* device = reinterpret_cast<pvstrgstruc*>(devicein);
            iretn = json_addentry("device_pvstrg_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_pvstrg_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_pvstrg_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_pvstrg_bcidx",didx, UINT16_MAX, (uint8_t *)&device->bcidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_pvstrg_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pvstrg_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pvstrg_efi",didx, UINT16_MAX, (uint8_t *)&device->effbase, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pvstrg_efs",didx, UINT16_MAX, (uint8_t *)&device->effslope, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_pvstrg_max",didx, UINT16_MAX, (uint8_t *)&device->maxpower, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Rotor
    case DeviceType::ROT:
        {
            rotstruc* device = reinterpret_cast<rotstruc*>(devicein);
            iretn = json_addentry("device_rot_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_rot_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rot_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_rot_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rot_angle",didx, UINT16_MAX, (uint8_t *)&device->angle, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Reaction Wheel
    case DeviceType::RW:
        {
            rwstruc* device = reinterpret_cast<rwstruc*>(devicein);
            iretn = json_addentry("device_rw_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_rw_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rw_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_rw_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_TEMPERATURE);
            json_addentry("device_rw_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_CURRENT);
            json_addentry("device_rw_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_VOLTAGE);
            json_addentry("device_rw_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_POWER);
            json_addentry("device_rw_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_rw_mom",didx, UINT16_MAX, (uint8_t *)&device->mom, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_rw_mxomg",didx, UINT16_MAX, (uint8_t *)&device->mxomg, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rw_mxalp",didx, UINT16_MAX, (uint8_t *)&device->mxalp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rw_tc",didx, UINT16_MAX, (uint8_t *)&device->tc, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rw_omg",didx, UINT16_MAX, (uint8_t *)&device->omg, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGULAR_RATE);
            json_addentry("device_rw_alp",didx, UINT16_MAX, (uint8_t *)&device->alp, (uint16_t)JSON_TYPE_FLOAT, cinfo, JSON_UNIT_ANGULAR_RATE);
            json_addentry("device_rw_romg",didx, UINT16_MAX, (uint8_t *)&device->romg, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rw_ralp",didx, UINT16_MAX, (uint8_t *)&device->ralp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Radio Receiver
    case DeviceType::RXR:
        {
            rxrstruc* device = reinterpret_cast<rxrstruc*>(devicein);
            iretn = json_addentry("device_rxr_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            json_addentry("device_rxr_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rxr_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_rxr_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rxr_opmode",didx, UINT16_MAX, (uint8_t *)&device->opmode, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_rxr_modulation",didx, UINT16_MAX, (uint8_t *)&device->modulation, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_rxr_flag",didx, UINT16_MAX, (uint8_t *)&device->flag, (uint16_t)JSON_TYPE_UINT32, cinfo);
            json_addentry("device_rxr_rssi",didx, UINT16_MAX, (uint8_t *)&device->rssi, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_rxr_pktsize",didx, UINT16_MAX, (uint8_t *)&device->pktsize, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_rxr_freq",didx, UINT16_MAX, (uint8_t *)&device->freq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rxr_maxfreq",didx, UINT16_MAX, (uint8_t *)&device->maxfreq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rxr_minfreq",didx, UINT16_MAX, (uint8_t *)&device->minfreq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rxr_byte_rate",didx, UINT16_MAX, (uint8_t *)&device->byte_rate, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rxr_powerin",didx, UINT16_MAX, (uint8_t *)&device->powerin, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rxr_powerout",didx, UINT16_MAX, (uint8_t *)&device->powerout, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rxr_band",didx, UINT16_MAX, (uint8_t *)&device->band, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_rxr_goodratio",didx, UINT16_MAX, (uint8_t *)&device->goodratio, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rxr_utcin",didx, UINT16_MAX, (uint8_t *)&device->utcin, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_rxr_uptime",didx, UINT16_MAX, (uint8_t *)&device->uptime, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            break;
        }
        //! Elevation and Azimuth Sun Sensor
    case DeviceType::SSEN:
        {
            ssenstruc* device = reinterpret_cast<ssenstruc*>(devicein);
            iretn = json_addentry("device_ssen_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_ssen_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_ssen_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_ssen_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ssen_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_ssen_qva",didx, UINT16_MAX, (uint8_t *)&device->qva, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ssen_qvb",didx, UINT16_MAX, (uint8_t *)&device->qvb, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ssen_qvc",didx, UINT16_MAX, (uint8_t *)&device->qvc, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ssen_qvd",didx, UINT16_MAX, (uint8_t *)&device->qvd, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ssen_azimuth",didx, UINT16_MAX, (uint8_t *)&device->azimuth, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_ssen_elevation",didx, UINT16_MAX, (uint8_t *)&device->elevation, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Star Tracker
    case DeviceType::STT:
        {
            sttstruc* device = reinterpret_cast<sttstruc*>(devicein);
            iretn = json_addentry("device_stt_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_stt_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_stt_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_stt_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_stt_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_stt_att",didx, UINT16_MAX, (uint8_t *)&device->att, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_stt_omega",didx, UINT16_MAX, (uint8_t *)&device->omega, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            json_addentry("device_stt_retcode",didx, UINT16_MAX, (uint8_t *)&device->retcode, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_stt_status",didx, UINT16_MAX, (uint8_t *)&device->status, (uint16_t)JSON_TYPE_UINT32, cinfo);
            break;
        }
        //! SUCHI
    case DeviceType::SUCHI:
        {
            suchistruc* device = reinterpret_cast<suchistruc*>(devicein);
            iretn = json_addentry("device_suchi_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_suchi_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_suchi_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_suchi_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_suchi_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_suchi_press",didx, UINT16_MAX, (uint8_t *)&device->press, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            for (uint16_t j=0; j<8; j++)
            {
                json_addentry("device_suchi_temps",didx,j,(uint8_t *)&device->temps+j*sizeof(float), (uint16_t)JSON_TYPE_FLOAT, cinfo);
            }
        }
        break;
        //! Switch
    case DeviceType::SWCH:
        {
            swchstruc* device = reinterpret_cast<swchstruc*>(devicein);
            iretn = json_addentry("device_swch_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_swch_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_swch_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_swch_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_swch_volt",didx, UINT16_MAX, (uint8_t *)&device->volt, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_swch_amp",didx, UINT16_MAX, (uint8_t *)&device->amp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_swch_power",didx, UINT16_MAX, (uint8_t *)&device->power, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_swch_energy",didx, UINT16_MAX, (uint8_t *)&device->energy, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
    case DeviceType::TCU:
        {
            tcustruc* device = reinterpret_cast<tcustruc*>(devicein);
            iretn = json_addentry("device_tcu_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_tcu_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcu_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tcu_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_tcu_mcnt",didx, UINT16_MAX, (uint8_t *)&device->mcnt, (uint16_t)JSON_TYPE_UINT16, cinfo);
            for (uint16_t j=0; j<3; j++)
            {
                json_addentry("device_tcu_mcidx",didx,j,(uint8_t *)&device->mcidx+j*sizeof(int16_t), (uint16_t)JSON_TYPE_UINT16, cinfo);
            }
            break;
        }
        //! Radio Transceiver
    case DeviceType::TCV:
        {
            tcvstruc* device = reinterpret_cast<tcvstruc*>(devicein);
            iretn = json_addentry("device_tcv_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_tcv_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcv_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tcv_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_tcv_opmode",didx, UINT16_MAX, (uint8_t *)&device->opmode, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tcv_modulation",didx, UINT16_MAX, (uint8_t *)&device->modulation, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tcv_flag",didx, UINT16_MAX, (uint8_t *)&device->flag, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tcv_rssi",didx, UINT16_MAX, (uint8_t *)&device->rssi, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tcv_pktsize",didx, UINT16_MAX, (uint8_t *)&device->pktsize, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tcv_freq",didx, UINT16_MAX, (uint8_t *)&device->freq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcv_maxfreq",didx, UINT16_MAX, (uint8_t *)&device->maxfreq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcv_minfreq",didx, UINT16_MAX, (uint8_t *)&device->minfreq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcv_byte_rate",didx, UINT16_MAX, (uint8_t *)&device->byte_rate, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_tcv_powerin",didx, UINT16_MAX, (uint8_t *)&device->powerin, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_tcv_powerout",didx, UINT16_MAX, (uint8_t *)&device->powerout, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_tcv_maxpower",didx, UINT16_MAX, (uint8_t *)&device->maxpower, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_tcv_band",didx, UINT16_MAX, (uint8_t *)&device->band, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_tcv_goodratio",didx, UINT16_MAX, (uint8_t *)&device->goodratio, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcv_utcout",didx, UINT16_MAX, (uint8_t *)&device->utcout, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcv_utcin",didx, UINT16_MAX, (uint8_t *)&device->utcin, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tcv_uptime",didx, UINT16_MAX, (uint8_t *)&device->uptime, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            break;
        }
    case DeviceType::TELEM:
        {
            telemstruc* device = reinterpret_cast<telemstruc*>(devicein);
            iretn = json_addentry("device_telem_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_telem_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_telem_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_telem_type",didx, UINT16_MAX, (uint8_t *)&device->type, (uint16_t)JSON_TYPE_UINT32, cinfo);
            json_addentry("device_telem_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            json_addentry("device_telem_vuint8",didx, UINT16_MAX, (uint8_t *)&device->vuint8, (uint16_t)JSON_TYPE_UINT8, cinfo);
            json_addentry("device_telem_vint8",didx, UINT16_MAX, (uint8_t *)&device->vint8, (uint16_t)JSON_TYPE_INT8, cinfo);
            json_addentry("device_telem_vuint16",didx, UINT16_MAX, (uint8_t *)&device->vuint16, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_telem_vint16",didx, UINT16_MAX, (uint8_t *)&device->vint16, (uint16_t)JSON_TYPE_INT16, cinfo);
            json_addentry("device_telem_vuint32",didx, UINT16_MAX, (uint8_t *)&device->vuint32, (uint16_t)JSON_TYPE_UINT32, cinfo);
            json_addentry("device_telem_vint32",didx, UINT16_MAX, (uint8_t *)&device->vint32, (uint16_t)JSON_TYPE_INT32, cinfo);
            json_addentry("device_telem_vfloat",didx, UINT16_MAX, (uint8_t *)&device->vfloat, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_telem_vdouble",didx, UINT16_MAX, (uint8_t *)&device->vdouble, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_telem_vstring",didx, UINT16_MAX, (uint8_t *)&device->vstring, (uint16_t)JSON_TYPE_STRING, cinfo);
            break;
        }
        //! Thruster
    case DeviceType::THST:
        {
            thststruc* device = reinterpret_cast<thststruc*>(devicein);
            iretn = json_addentry("device_thst_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_thst_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_thst_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_thst_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_thst_isp",didx, UINT16_MAX, (uint8_t *)&device->isp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_thst_align",didx, UINT16_MAX, (uint8_t *)&device->align, (uint16_t)JSON_TYPE_QUATERNION, cinfo);
            json_addentry("device_thst_flw",didx, UINT16_MAX, (uint8_t *)&device->flw, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
    case DeviceType::TNC:
        {
            tncstruc* device = reinterpret_cast<tncstruc*>(devicein);
            iretn = json_addentry("device_tnc_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_tnc_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tnc_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tnc_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Temperature Sensor
    case DeviceType::TSEN:
        {
            tsenstruc* device = reinterpret_cast<tsenstruc*>(devicein);
            iretn = json_addentry("device_tsen_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_tsen_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_tsen_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_tsen_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            break;
        }
        //! Radio Transmitter
    case DeviceType::TXR:
        {
            txrstruc* device = reinterpret_cast<txrstruc*>(devicein);
            iretn = json_addentry("device_txr_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_txr_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_txr_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_txr_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_txr_opmode",didx, UINT16_MAX, (uint8_t *)&device->opmode, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_txr_modulation",didx, UINT16_MAX, (uint8_t *)&device->modulation, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_txr_flag",didx, UINT16_MAX, (uint8_t *)&device->flag, (uint16_t)JSON_TYPE_UINT32, cinfo);
            json_addentry("device_txr_rssi",didx, UINT16_MAX, (uint8_t *)&device->rssi, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_txr_pktsize",didx, UINT16_MAX, (uint8_t *)&device->pktsize, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_txr_freq",didx, UINT16_MAX, (uint8_t *)&device->freq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_txr_maxfreq",didx, UINT16_MAX, (uint8_t *)&device->maxfreq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_txr_minfreq",didx, UINT16_MAX, (uint8_t *)&device->minfreq, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_txr_byte_rate",didx, UINT16_MAX, (uint8_t *)&device->byte_rate, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_txr_powerin",didx, UINT16_MAX, (uint8_t *)&device->powerin, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_txr_powerout",didx, UINT16_MAX, (uint8_t *)&device->powerout, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_txr_maxpower",didx, UINT16_MAX, (uint8_t *)&device->maxpower, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_txr_goodratio",didx, UINT16_MAX, (uint8_t *)&device->goodratio, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_txr_utcout",didx, UINT16_MAX, (uint8_t *)&device->utcout, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_txr_uptime",didx, UINT16_MAX, (uint8_t *)&device->uptime, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            break;
        }
    case DeviceType::XYZSEN:
        {
            xyzsenstruc* device = reinterpret_cast<xyzsenstruc*>(devicein);
            iretn = json_addentry("device_xyzsen_name",didx, UINT16_MAX, (uint8_t *)&device->name, (uint16_t)JSON_TYPE_STRING, cinfo);
            iretn = json_addentry("device_xyzsen_utc",didx, UINT16_MAX, (uint8_t *)&device->utc, (uint16_t)JSON_TYPE_DOUBLE, cinfo);
            json_addentry("device_xyzsen_cidx",didx, UINT16_MAX, (uint8_t *)&device->cidx, (uint16_t)JSON_TYPE_UINT16, cinfo);
            json_addentry("device_xyzsen_temp",didx, UINT16_MAX, (uint8_t *)&device->temp, (uint16_t)JSON_TYPE_FLOAT, cinfo);
            json_addentry("device_xyzsen_direction",didx, UINT16_MAX, (uint8_t *)&device->direction, (uint16_t)JSON_TYPE_RVECTOR, cinfo);
            break;
        }
    default:
        iretn = JSON_ERROR_NOENTRY;
        break;
    }

    if (iretn >= 0)
    {
        iretn = cinfo->jmapped;
    }
    return iretn;
}

//! Toggle device entry.
/*! Toggle the enable state of the entries specific to device number i in the JSON Namespace map.
 * \param type Device type number taken from ::DeviceType.
 \param didx Device number.

    \param state Desired enable state.
    \return 0 or negative error.
 */
int32_t json_toggledeviceentry(uint16_t didx, DeviceType type, cosmosstruc *cinfo, bool state)
{
    switch (type)
    {
    case DeviceType::TELEM:
        json_toggleentry("device_telem_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_type",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vuint8",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vint8",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vuint16",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vint16",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vuint32",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vint32",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vfloat",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vdouble",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_telem_vstring",didx, UINT16_MAX, cinfo, state);
        break;
    case DeviceType::PLOAD:
        json_toggleentry("device_pload_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pload_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pload_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pload_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pload_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pload_drate",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pload_key_cnt",didx, UINT16_MAX, cinfo, state);
        for (uint16_t j=0; j<MAXPLOADKEYCNT; j++)
        {
            json_toggleentry("device_pload_keyidx",didx,j, cinfo, state);
            json_toggleentry("device_pload_keyval",didx,j, cinfo, state);
        }
        break;
        //! Elevation and Azimuth Sun Sensor
    case DeviceType::SSEN:
        json_toggleentry("device_ssen_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_qva",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_qvb",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_qvc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_qvd",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_azimuth",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ssen_elevation",didx, UINT16_MAX, cinfo, state);
        break;
        //! Gyroscope
    case DeviceType::GYRO:
        json_toggleentry("device_gyro_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_omega",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gyro_alpha",didx, UINT16_MAX, cinfo, state);
        break;
        //! Inertial Measurement Unit
    case DeviceType::IMU:
        json_toggleentry("device_imu_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_accel",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_accel_x",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_accel_y",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_accel_z",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_omega",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_alpha",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_mag",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_mag_x",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_mag_y",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_mag_z",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_imu_bdot",didx, UINT16_MAX, cinfo, state);
        break;
    case DeviceType::MAG:
        json_toggleentry("device_mag_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_mag",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_mag_x",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_mag_y",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_mag_z",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mag_bdot",didx, UINT16_MAX, cinfo, state);
        break;
        //! Reaction Wheel
    case DeviceType::RW:
        json_toggleentry("device_rw_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_mom",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_mxomg",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_mxalp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_tc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_omg",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_alp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_romg",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rw_ralp",didx, UINT16_MAX, cinfo, state);
        break;
        //! Magnetic Torque Rod
    case DeviceType::MTR:
        json_toggleentry("device_mtr_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_mxmom",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_tc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_align",didx, UINT16_MAX, cinfo, state);
        for (uint16_t j=0; j<7; j++)
        {
            json_toggleentry("device_mtr_npoly",didx,j, cinfo, state);
            json_toggleentry("device_mtr_ppoly",didx,j, cinfo, state);
        }
        json_toggleentry("device_mtr_mom",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mtr_rmom",didx, UINT16_MAX, cinfo, state);
        break;
        //! Camera
    case DeviceType::CAM:
        json_toggleentry("device_cam_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_drate",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_pwidth",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_pheight",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_width",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_height",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cam_flength",didx, UINT16_MAX, cinfo, state);
        break;
        //! Processing Unit
    case DeviceType::CPU:
        json_toggleentry("device_cpu_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_uptime",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_downtime",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_maxgib",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_maxload",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_load",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_gib",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_storage",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_cpu_boot_count",didx, UINT16_MAX, cinfo, state);
        break;
    case DeviceType::DISK:
        json_toggleentry("device_disk_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_disk_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_disk_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_disk_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_disk_maxgib",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_disk_gib",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_disk_path",didx, UINT16_MAX, cinfo, state);
        break;
        //! GPS Unit
    case DeviceType::GPS:
        json_toggleentry("device_gps_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dutc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocs",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocs_x",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocs_y",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocs_z",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocv",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocv_x",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocv_y",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geocv_z",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocs",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocs_x",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocs_y",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocs_z",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocv",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocv_x",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocv_y",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeocv_z",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geods",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geods_lat",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geods_lon",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geods_h",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geodv",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geodv_lat",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geodv_lon",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_geodv_h",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeods",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeods_lat",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeods_lon",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeods_h",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeodv",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeodv_lat",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeodv_lon",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_dgeodv_h",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_heading",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_sats_used",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_sats_visible",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_time_status",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_position_type",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_gps_solution_status",didx, UINT16_MAX, cinfo, state);
        break;
        //! Antenna
    case DeviceType::ANT:
        json_toggleentry("device_ant_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ant_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ant_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ant_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ant_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ant_azim",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_ant_elev",didx, UINT16_MAX, cinfo, state);;
        json_toggleentry("device_ant_minelev",didx, UINT16_MAX, cinfo, state);
        break;
        //! Radio Receiver
    case DeviceType::RXR:
        json_toggleentry("device_rxr_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_opmode",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_modulation",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_flag",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_rssi",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_pktsize",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_freq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_maxfreq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_minfreq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_byte_rate",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_powerin",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_powerout",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_maxpower",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_band",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_goodratio",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_utcin",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_uptime",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rxr_bytesin",didx, UINT16_MAX, cinfo, state);
        break;
        //! Radio Transmitter
    case DeviceType::TXR:
        json_toggleentry("device_txr_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_opmode",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_modulation",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_flag",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_rssi",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_pktsize",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_freq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_maxfreq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_minfreq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_byte_rate",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_powerin",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_powerout",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_maxpower",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_band",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_goodratio",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_utcout",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_uptime",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_txr_bytesout",didx, UINT16_MAX, cinfo, state);
        break;
        //! Radio Transceiver
    case DeviceType::TCV:
        json_toggleentry("device_tcv_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_opmode",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_modulation",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_flag",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_rssi",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_pktsize",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_freq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_maxfreq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_minfreq",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_byte_rate",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_powerin",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_powerout",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_maxpower",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_band",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_goodratio",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_utcout",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_utcin",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_uptime",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_bytesin",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcv_bytesout",didx, UINT16_MAX, cinfo, state);
        break;
        //! Photo Voltaic String
    case DeviceType::PVSTRG:
        json_toggleentry("device_pvstrg_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pvstrg_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pvstrg_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pvstrg_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pvstrg_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pvstrg_efi",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pvstrg_efs",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_pvstrg_max",didx, UINT16_MAX, cinfo, state);
        break;
        //! Battery
    case DeviceType::BATT:
        json_toggleentry("device_batt_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_batt_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_batt_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_batt_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_batt_cap",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_batt_eff",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_batt_charge",didx, UINT16_MAX, cinfo, state);
        break;
        //! Heater
    case DeviceType::HTR:
        json_toggleentry("device_htr_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_htr_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_htr_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_htr_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_htr_state",didx, UINT16_MAX, cinfo, state);
        break;
        //! Motor
    case DeviceType::MOTR:
        json_toggleentry("device_motr_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_motr_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_motr_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_motr_rat",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_motr_max",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_motr_spd",didx, UINT16_MAX, cinfo, state);
        break;
        //! Pressure Sensor
    case DeviceType::PSEN:
        json_toggleentry("device_psen_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_psen_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_psen_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_psen_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_psen_press",didx, UINT16_MAX, cinfo, state);;
        break;
        //! Temperature Sensor
    case DeviceType::TSEN:
        json_toggleentry("device_tsen_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tsen_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tsen_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tsen_temp",didx, UINT16_MAX, cinfo, state);
        break;
        //! Thruster
    case DeviceType::THST:
        json_toggleentry("device_thst_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_thst_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_thst_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_thst_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_thst_isp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_thst_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_thst_flw",didx, UINT16_MAX, cinfo, state);
        break;
        //! Propellant Tank
    case DeviceType::PROP:
        json_toggleentry("device_prop_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_prop_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_prop_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_prop_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_prop_cap",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_prop_lev",didx, UINT16_MAX, cinfo, state);
        break;
        //! Switch
    case DeviceType::SWCH:
        json_toggleentry("device_swch_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_swch_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_swch_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_swch_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_swch_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_swch_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_swch_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_swch_energy",didx, UINT16_MAX, cinfo, state);
        break;
        //! Rotor
    case DeviceType::ROT:
        json_toggleentry("device_rot_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rot_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rot_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_rot_temp",didx, UINT16_MAX, cinfo, state);
        break;
        //! Star Tracker
    case DeviceType::STT:
        json_toggleentry("device_stt_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_align",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_att",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_omega",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_retcode",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_stt_status",didx, UINT16_MAX, cinfo, state);
        break;
        //! Star Tracker
    case DeviceType::SUCHI:
        {
            json_toggleentry("device_suchi_name",didx, UINT16_MAX, cinfo, state);
            json_toggleentry("device_suchi_utc",didx, UINT16_MAX, cinfo, state);
            json_toggleentry("device_suchi_cidx",didx, UINT16_MAX, cinfo, state);
            json_toggleentry("device_suchi_temp",didx, UINT16_MAX, cinfo, state);
            json_toggleentry("device_suchi_align",didx, UINT16_MAX, cinfo, state);
            json_toggleentry("device_suchi_press",didx, UINT16_MAX, cinfo, state);
            for (uint16_t j=0; j<8; j++)
            {
                json_toggleentry("device_suchi_temps",didx,j, cinfo, state);
            }
            break;
        }
    case DeviceType::MCC:
        json_toggleentry("device_mcc_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mcc_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mcc_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mcc_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mcc_q",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mcc_o",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mcc_a",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_mcc_align",didx, UINT16_MAX, cinfo, state);
        break;
    case DeviceType::TCU:
        json_toggleentry("device_tcu_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcu_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcu_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcu_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tcu_mcnt",didx, UINT16_MAX, cinfo, state);
        for (uint16_t j=0; j<3; j++)
        {
            json_toggleentry("device_tcu_mcidx",didx,j, cinfo, state);
        }
        break;
    case DeviceType::BUS:
        json_toggleentry("device_bus_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_energy",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bus_wdt",didx, UINT16_MAX, cinfo, state);
        break;
    case DeviceType::TNC:
        json_toggleentry("device_tnc_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tnc_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tnc_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_tnc_temp",didx, UINT16_MAX, cinfo, state);
        break;
    case DeviceType::BCREG:
        json_toggleentry("device_bcreg_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_energy",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_power",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_mpptin_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_mpptin_amp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_mpptout_volt",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_bcreg_mpptout_amp",didx, UINT16_MAX, cinfo, state);
    case DeviceType::XYZSEN:
        json_toggleentry("device_xyzsen_name",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_xyzsen_utc",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_xyzsen_cidx",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_xyzsen_temp",didx, UINT16_MAX, cinfo, state);
        json_toggleentry("device_xyzsen_direction",didx, UINT16_MAX, cinfo, state);
        break;
    case DeviceType::COUNT:
    case DeviceType::NONE:
        break;
    }

    return 0;
}

//! Add port entry.
/*! Add entries specific to port number portidx to the JSON Namespace map.
 \param portidx Port number.

    \return The current number of entries, if successful, 0 if the entry could not be added.
 */
uint16_t json_mapportentry(uint16_t portidx, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    json_addentry("port_name", portidx, UINT16_MAX, (ptrdiff_t)offsetof(portstruc,name)+portidx*sizeof(portstruc), (uint16_t)JSON_TYPE_STRING, JSON_STRUCT_PORT, cinfo);
    iretn = json_addentry("port_type", portidx, UINT16_MAX, (ptrdiff_t)offsetof(portstruc,type)+portidx*sizeof(portstruc), (uint16_t)JSON_TYPE_UINT16, JSON_STRUCT_PORT, cinfo);

    if (iretn >= 0)
    {
        iretn = cinfo->jmapped;
    }
    return iretn;
}

//! Toggle port entry.
/*! Toggle enable state of entries specific to port number i in the JSON Namespace map.
 \param portidx Port number.

    \param state Enable state.
    \return The current number of entries, if successful, 0 if the entry could not be added.
 */
int32_t json_toggleportentry(uint16_t portidx, cosmosstruc *cinfo, bool state)
{
    int32_t iretn = 0;

    json_toggleentry("port_name", portidx, UINT16_MAX, cinfo, state);
    iretn = json_toggleentry("port_type", portidx, UINT16_MAX, cinfo,state);

    return iretn;
}

//! Create JSON stream from wildcard
/*! Generate a JSON stream based on a character string representing
 * all the \ref jsonlib_namespace names you wish to match.
    \param jstring Pointer to a string large enough to hold the end result.
    \param wildcard Character string representing a regular expression to be matched to all names in the \ref jsonlib_namespace.

    \param cinfo Reference to ::cosmosstruc to use.
    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_wildcard(string &jstring, string wildcard, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    jstring.clear();
    iretn = json_out_wildcard(jstring, wildcard, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON stream from list
/*! Generate a JSON stream based on a comma separated list of \ref jsonlib_namespace names.
    \param jstring Pointer to a string large enough to hold the end result.
    \param list List to convert.

    \param cinfo Reference to ::cosmosstruc to use.
    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_list(string &jstring, string list, cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    jstring.clear();
    iretn = json_out_list(jstring, list, cinfo);
    if (iretn < 0 && iretn != JSON_ERROR_EOS)
        return nullptr;

    return jstring.data();
}

//! Create JSON stream from entries
/*! Generate a JSON stream based on a vector of entries of \ref jsonlib_namespace names.
    \param jstring Pointer to a string large enough to hold the end result.
    \param table Vector of pointers to entries from ::jsonmap.

    \param cinfo Reference to ::cosmosstruc to use.
    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_table(string &jstring, vector<jsonentry*> table, cosmosstruc *cinfo)
{
    jstring.clear();
    for (auto entry: table)
    {
        if (entry != nullptr)
        {
            json_out_entry(jstring, *entry, cinfo);
        }
    }

    return jstring.data();
}

//! Create JSON Track string
/*! Generate a JSON stream showing the variables stored in an ::nodestruc.
    \param jstring Pointer to a string large enough to hold the end result.

    \param cinfo Reference to ::cosmosstruc to use.
 \param num Target index.
    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_target(string &jstring, cosmosstruc *cinfo, uint16_t num)
{
    int32_t iretn = 0;

    jstring.clear();
    iretn = json_out_1d(jstring, "target_utc",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_name",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_type",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_azfrom",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_elfrom",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_azto",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_elto",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_min",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_range",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_close",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "target_loc_pos_geod",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON Node string
/*! Generate a JSON stream showing the variables stored in an ::nodestruc.
    \param jstring Pointer to a string large enough to hold the end result.

    \param cinfo Reference to ::cosmosstruc to use.
    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_node(string &jstring, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    jstring.clear();
    iretn = json_out(jstring, "node_deci", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_utcstart", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_utc", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_name", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_lastevent", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_lasteventutc", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_type", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_mass", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_moi", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_battcap", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_battlev", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_powchg", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_powmode", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_powgen", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_powuse", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_icrf", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_icrf", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON Agent string
/*! Generate a JSON stream showing the variables stored in an ::agentstruc.
    \param jstring Pointer to a string large enough to hold the end result.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_agent(string &jstring, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    jstring.clear();
    iretn = json_out(jstring, "agent_utc", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_node", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_proc", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_user", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_aprd", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_bprd", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_port", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_bsz", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_addr", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_pid", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_stateflag", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_cpu", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_memory", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_jitter", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "agent_dcycle", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON Time string
/*! Generate a JSON stream showing the various internal times stored
 * in the global COSMOS data structure.
    \param jstring Pointer to a string large enough to hold the end
    result.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_time(string &jstring, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    jstring.clear();
    iretn = json_out(jstring, "node_deci", cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring, "node_utcstart", cinfo);
    if (iretn < 0)
        return nullptr;
    iretn = json_out(jstring, "node_utc", cinfo);
    if (iretn < 0)
        return nullptr;
    iretn = json_out(jstring, "node_utcoffset", cinfo);
    if (iretn < 0)
        return nullptr;

    return jstring.data();
}

//! Create JSON Heart Beat string
/*! Create a complete JSON formatted Heartbeat string using the data currently in the global COSMOS
 * structure.
    \param jstring Reference to a string structure to be used to build out the JSON string.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_beat(string &jstring, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    jstring.clear();
    iretn = json_out(jstring, "beat", cinfo);
    if (iretn < 0)
        return nullptr;

    return jstring.data();
}

//! Create JSON Beacon string
/*! Create a complete JSON formatted Beacon string using the data currently in the global COSMOS structure.
    \param jstring Reference to a string structure to be used to build out the JSON string.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_beacon(string &jstring, cosmosstruc *cinfo)
{
    //    int32_t iretn = 0;
    //    iretn = json_out(jstring, "node_name", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_type", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_utcstart", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_utc", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_utcoffset", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_loc_pos_eci", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_loc_att_icrf", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_powgen", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_powuse", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_powchg", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "node_battlev", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }
    //    iretn = json_out(jstring, "device_cpu_cnt", cinfo);
    //    if (iretn < 0)
    //    {
    //        return nullptr;
    //    }



    //    return jstring.data();

    string result;
    char tempstring[200];
    result = "{\"node_utc\",\"node_name\",\"node_lastevent\",\"node_lasteventutc\",\"node_type\",\"node_state\",\"node_powgen\",\"node_powuse\",\"node_powmode\",\"node_powchg\",\"node_battlev\",\"node_loc_bearth\",\"node_loc_pos_eci\",\"node_loc_att_icrf\"";

    for (uint16_t i=0; i<cinfo->devspec.cpu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_cpu_utc_%03d\",\"device_cpu_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_cpu_gib_%03d\",\"device_cpu_load_%03d\",\"device_cpu_boot_count_%03d\"",i,i,i);
        result += tempstring;
    }
    result += "}";


    jstring.clear();
    return json_of_list(jstring, result, cinfo);
}

//! Create JSON IMU string
/*! Create a complete JSON formatted IMU string for the indicated IMU using the data currently in the global COSMOS structure.
    \param jstring Reference to a string structure to be used to build out the JSON string.
    \param num Number of the IMU.

    \param cinfo Reference to ::cosmosstruc to use.

    \return Pointer to the string if successful, otherwise nullptr.
*/
const char *json_of_imu(string &jstring, uint16_t num, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    jstring.clear();
    iretn = json_out_1d(jstring, "device_imu_att",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "device_imu_align",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "device_imu_cidx",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "device_imu_cnt",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "device_imu_mag",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;    }
    iretn = json_out_1d(jstring, "device_imu_bdot",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring, "device_imu_pos",num, cinfo);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

const char *json_of_ephemeris(string &jstring, cosmosstruc *cinfo)
{
    // Location
    jstring.clear();
    json_out(jstring, "node_deci", cinfo);
    json_out(jstring, "node_utcstart", cinfo);
    json_out(jstring, "node_utc", cinfo);
    json_out(jstring, "node_utcoffset", cinfo);
    json_out(jstring, "node_name", cinfo);
    json_out(jstring, "node_type", cinfo);
    json_out(jstring, "node_loc_pos_eci", cinfo);

    return jstring.data();
}

const char *json_of_utc(string &jstring, cosmosstruc *cinfo)
{
    // Time
    jstring.clear();
    json_out(jstring, "node_deci", cinfo);
    json_out(jstring, "node_utcstart", cinfo);
    json_out(jstring, "node_utc", cinfo);
    json_out(jstring, "node_utcoffset", cinfo);
    return jstring.data();
}

//! Get list of all Namespace names
/*! Go through the Namespace map, extracting each valid Namespace name. Return this
 * as a JSON like list.
*/
string json_list_of_all(cosmosstruc *cinfo)
{
    string result;

    result = "{";
    for (vector<jsonentry> entryrow : cinfo->jmap)
    {
        for (jsonentry entry : entryrow)
        {
            char tempstring[200];
            sprintf(tempstring, "\"%s\",", entry.name.c_str());
            result += tempstring;
        }
    }
    if (result[result.size()-1] == ',')
    {
        result[result.size()-1] = '}';
    }
    else
    {
        result += "}";
    }

    return result;
}


string json_list_of_soh(cosmosstruc *cinfo)
{
    string result;
    char tempstring[200];

    result = "{\"node_utc\",\"node_name\",\"node_lastevent\",\"node_lasteventutc\",\"node_type\",\"node_state\",\"node_powgen\",\"node_powuse\",\"node_powchg\",\"node_powmode\",\"node_battlev\",\"node_loc_bearth\",\"node_loc_pos_eci\",\"node_loc_att_icrf\"";

    for (uint16_t i=0; i<cinfo->devspec.telem_cnt; ++i)
    {
        result += ",\"device_telem_utc_" + to_unsigned(i, 3, true) + "\"";
        result += ",\"device_telem_type_" + to_unsigned(i, 3, true) + "\"";
        result += ",\"device_telem_name_" + to_unsigned(i, 3, true) + "\"";
        result += ",\"device_telem_" + TelemTypeName[cinfo->devspec.telem[i].type] + "_" + to_unsigned(i, 3, true) + "\"";
        //        switch (cinfo->devspec.telem[i]].type)
        //        {
        //        case 0:
        //            result += ",\"device_telem_vuint8_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 1:
        //            result += ",\"device_telem_vint8_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 2:
        //            result += ",\"device_telem_vuint16_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 3:
        //            result += ",\"device_telem_vint16_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 4:
        //            result += ",\"device_telem_vuint32_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 5:
        //            result += ",\"device_telem_vint32_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 6:
        //            result += ",\"device_telem_vfloat_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 7:
        //            result += ",\"device_telem_vdouble_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        case 8:
        //            result += ",\"device_telem_vstring_" + to_unsigned(i, 3, true) + "\"";
        //            break;
        //        }
    }

    for (uint16_t i=0; i<cinfo->devspec.pload_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_pload_utc_%03d\",\"device_pload_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.ssen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_ssen_utc_%03d\",\"device_ssen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ssen_azimuth_%03d\",\"device_ssen_elevation_%03d\",\"device_ssen_qva_%03d\",\"device_ssen_qvb_%03d\",\"device_ssen_qvc_%03d\",\"device_ssen_qvd_%03d\"",i,i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.gyro_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_gyro_utc_%03d\",\"device_gyro_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gyro_omega_%03d\",\"device_gyro_alpha_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.imu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_imu_utc_%03d\",\"device_imu_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_imu_accel_%03d\",\"device_imu_omega_%03d\",\"device_imu_alpha_%03d\",\"device_imu_mag_%03d\",\"device_imu_bdot_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.mag_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mag_utc_%03d\",\"device_mag_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mag_omega_%03d\",\"device_mag_alpha_%03d\",\"device_mag_mag_%03d\",\"device_mag_bdot_%03d\"",i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.rw_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rw_utc_%03d\",\"device_rw_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rw_omg_%03d\",\"device_rw_alp_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.mtr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mtr_utc_%03d\",\"device_mtr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mtr_mom_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.cpu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_cpu_utc_%03d\",\"device_cpu_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_cpu_gib_%03d\",\"device_cpu_load_%03d\",\"device_cpu_boot_count_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.gps_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_gps_utc_%03d\",\"device_gps_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gps_geocs_%03d\",\"device_gps_geocv_%03d\",\"device_gps_dgeocs_%03d\",\"device_gps_dgeocv_%03d\"",i,i,i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gps_geods_%03d\",\"device_gps_geodv_%03d\",\"device_gps_dgeods_%03d\",\"device_gps_dgeodv_%03d\"",i,i,i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gps_time_status_%03d\",\"device_gps_solution_status_%03d\",\"device_gps_position_type_%03d\",\"device_gps_sats_used_%03d\"",i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.ant_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_ant_utc_%03d\",\"device_ant_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ant_align_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ant_azim_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ant_elev_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.rxr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rxr_utc_%03d\",\"device_rxr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_flag_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_rssi_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_freq_%03d\",\"device_rxr_opmode_%03d\",\"device_rxr_modulation_%03d\"",i,i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_band_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_byte_rate_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_powerin_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.txr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_txr_utc_%03d\",\"device_txr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_flag_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_rssi_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_freq_%03d\",\"device_txr_opmode_%03d\",\"device_txr_modulation_%03d\"",i,i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_byte_rate_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_powerout_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.tcv_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tcv_utc_%03d\",\"device_tcv_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_flag_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_rssi_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_freq_%03d\",\"device_tcv_opmode_%03d\",\"device_tcv_modulation_%03d\"",i,i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_band_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_byte_rate_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_powerin_%03d\",\"device_tcv_powerout_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.pvstrg_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_pvstrg_utc_%03d\",\"device_pvstrg_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_pvstrg_power_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.batt_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_batt_utc_%03d\",\"device_batt_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_batt_amp_%03d\",\"device_batt_volt_%03d\", \"device_batt_power_%03d\"",i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_batt_charge_%03d\",\"device_batt_percentage_%03d\", \"device_batt_time_remaining_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.htr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_htr_utc_%03d\",\"device_htr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_htr_setvertex_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_htr_state_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.motr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_motr_utc_%03d\",\"device_motr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_motr_rat_%03d\",\"device_motr_spd_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.tsen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tsen_utc_%03d\",\"device_tsen_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.thst_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_thst_utc_%03d\",\"device_thst_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_thst_align_%03d\",\"device_thst_flw_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.prop_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_prop_utc_%03d\",\"device_prop_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_prop_lev_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.swch_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_swch_utc_%03d\",\"device_swch_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_swch_utc_%03d\",\"device_swch_energy_%03d\",\"device_swch_amp_%03d\",\"device_swch_volt_%03d\",\"device_swch_power_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.bcreg_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_bcreg_utc_%03d\",\"device_bcreg_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_bcreg_utc_%03d\",\"device_bcreg_energy_%03d\",\"device_bcreg_amp_%03d\",\"device_bcreg_volt_%03d\",\"device_bcreg_power_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.rot_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rot_utc_%03d\",\"device_rot_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rot_angle_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.stt_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_stt_utc_%03d\",\"device_stt_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_stt_att_%03d\",\"device_stt_omega_%03d\",\"device_stt_alpha_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.mcc_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mcc_utc_%03d\",\"device_mcc_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mcc_q_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mcc_o_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mcc_a_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.tcu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tcu_utc_%03d\",\"device_tcu_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.bus_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_bus_utc_%03d\",\"device_bus_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_bus_utc_%03d\",\"device_bus_energy_%03d\",\"device_bus_amp_%03d\",\"device_bus_volt_%03d\",\"device_bus_power_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.psen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_psen_utc_%03d\",\"device_psen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_psen_press_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.suchi_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_suchi_utc_%03d\",\"device_suchi_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_suchi_press_%03d\",\"device_suchi_temps_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.cam_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_cam_utc_%03d\",\"device_cam_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.disk_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_disk_utc_%03d\",\"device_disk_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_disk_gib_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_disk_path_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.xyzsen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_xyzsen_utc_%03d\",\"device_xyzsen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_xyzsen_direction_%03d\"",i);
        result += tempstring;
    }

    result += "}";

    return result;

}

string json_list_of_fullsoh(cosmosstruc *cinfo)
{
    string result;
    char tempstring[200];

    result = "{\"node_utc\",\"node_name\",\"node_lastevent\",\"node_lasteventutc\",\"node_type\",\"node_state\",\"node_powgen\",\"node_powuse\",\"node_powchg\",\"node_powmode\",\"node_battlev\",\"node_loc_bearth\",\"node_loc_pos_eci\",\"node_loc_att_icrf\"";

    for (uint16_t i=0; i<cinfo->devspec.pload_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_pload_utc_%03d\",\"device_pload_temp_%03d\",\"device_pload_power_%03d\"", i, i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.ssen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_ssen_utc_%03d\",\"device_ssen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ssen_azimuth_%03d\",\"device_ssen_elevation_%03d\",\"device_ssen_qva_%03d\",\"device_ssen_qvb_%03d\",\"device_ssen_qvc_%03d\",\"device_ssen_qvd_%03d\"",i,i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.gyro_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_gyro_utc_%03d\",\"device_gyro_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gyro_amp_%03d\",\"device_gyro_volt_%03d\",\"device_gyro_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gyro_omega_%03d\",\"device_gyro_alpha_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.mag_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mag_utc_%03d\",\"device_mag_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mag_amp_%03d\",\"device_mag_volt_%03d\",\"device_mag_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mag_omega_%03d\",\"device_mag_alpha_%03d\",\"device_mag_mag_%03d\",\"device_mag_bdot_%03d\"",i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.imu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_imu_utc_%03d\",\"device_imu_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_imu_amp_%03d\",\"device_imu_volt_%03d\",\"device_imu_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_imu_accel_%03d\",\"device_imu_omega_%03d\",\"device_imu_alpha_%03d\",\"device_imu_mag_%03d\",\"device_imu_bdot_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.rw_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rw_utc_%03d\",\"device_rw_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rw_amp_%03d\",\"device_rw_volt_%03d\",\"device_rw_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rw_omg_%03d\",\"device_rw_alp_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.mtr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mtr_utc_%03d\",\"device_mtr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mtr_amp_%03d\",\"device_mtr_volt_%03d\",\"device_mtr_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mtr_mom_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.cpu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_cpu_utc_%03d\",\"device_cpu_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_cpu_amp_%03d\",\"device_cpu_volt_%03d\",\"device_cpu_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_cpu_gib_%03d\",\"device_cpu_load_%03d\",\"device_cpu_boot_count_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.gps_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_gps_utc_%03d\",\"device_gps_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gps_amp_%03d\",\"device_gps_volt_%03d\",\"device_gps_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gps_geocs_%03d\",\"device_gps_geocv_%03d\",\"device_gps_dgeocs_%03d\",\"device_gps_dgeocv_%03d\"",i,i,i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gps_geods_%03d\",\"device_gps_geodv_%03d\",\"device_gps_dgeods_%03d\",\"device_gps_dgeodv_%03d\"",i,i,i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_gps_time_status_%03d\",\"device_gps_solution_status_%03d\",\"device_gps_position_type_%03d\",\"device_gps_sats_used_%03d\"",i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.ant_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_ant_utc_%03d\",\"device_ant_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ant_amp_%03d\",\"device_ant_volt_%03d\",\"device_ant_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ant_align_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ant_azim_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ant_elev_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.rxr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rxr_utc_%03d\",\"device_rxr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_rxr_%03d\",\"device_rxr_volt_%03d\",\"device_rxr_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_flag_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_rssi_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_freq_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_band_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_powerin_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.txr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_txr_utc_%03d\",\"device_txr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_amp_%03d\",\"device_txr_volt_%03d\",\"device_txr_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_flag_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_rssi_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_freq_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_powerout_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.tcv_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tcv_utc_%03d\",\"device_tcv_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_amp_%03d\",\"device_tcv_volt_%03d\",\"device_tcv_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_flag_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_rssi_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_freq_%03d\",\"device_tcv_opmode_%03d\"",i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_band_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_powerin_%03d\",\"device_tcv_powerout_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.pvstrg_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_pvstrg_utc_%03d\",\"device_pvstrg_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_pvstrg_power_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.batt_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_batt_utc_%03d\",\"device_batt_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_batt_amp_%03d\",\"device_batt_volt_%03d\", \"device_batt_power_%03d\"",i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_batt_charge_%03d\",\"device_batt_percentage_%03d\", \"device_batt_time_remaining_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.htr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_htr_utc_%03d\",\"device_htr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_htr_amp_%03d\",\"device_htr_volt_%03d\",\"device_htr_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_htr_setvertex_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_htr_state_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.motr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_motr_utc_%03d\",\"device_motr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_motr_amp_%03d\",\"device_motr_volt_%03d\",\"device_motr_power_%03d\"", i, i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_motr_rat_%03d\",\"device_motr_spd_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.tsen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tsen_utc_%03d\",\"device_tsen_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.thst_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_thst_utc_%03d\",\"device_thst_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_thst_align_%03d\",\"device_thst_flw_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.prop_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_prop_utc_%03d\",\"device_prop_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_prop_lev_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.swch_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_swch_utc_%03d\",\"device_swch_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_swch_utc_%03d\",\"device_swch_energy_%03d\",\"device_swch_amp_%03d\",\"device_swch_volt_%03d\",\"device_swch_power_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.bcreg_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_bcreg_utc_%03d\",\"device_bcreg_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_bcreg_utc_%03d\",\"device_bcreg_energy_%03d\",\"device_bcreg_amp_%03d\",\"device_bcreg_volt_%03d\",\"device_bcreg_power_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.rot_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rot_utc_%03d\",\"device_rot_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rot_angle_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.stt_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_stt_utc_%03d\",\"device_stt_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_stt_att_%03d\",\"device_stt_omega_%03d\",\"device_stt_alpha_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.mcc_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mcc_utc_%03d\",\"device_mcc_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mcc_q_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mcc_o_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mcc_a_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.tcu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tcu_utc_%03d\",\"device_tcu_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.bus_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_bus_utc_%03d\",\"device_bus_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_bus_utc_%03d\",\"device_bus_energy_%03d\",\"device_bus_amp_%03d\",\"device_bus_volt_%03d\",\"device_bus_power_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.psen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_psen_utc_%03d\",\"device_psen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_psen_press_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.suchi_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_suchi_utc_%03d\",\"device_suchi_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_suchi_press_%03d\",\"device_suchi_temps_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.cam_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_cam_utc_%03d\",\"device_cam_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.disk_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_disk_utc_%03d\",\"device_disk_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_disk_gib_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_disk_path_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cinfo->devspec.xyzsen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_xyzsen_utc_%03d\",\"device_xyzsen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_xyzsen_direction_%03d\"",i);
        result += tempstring;
    }

    result += "}";

    return result;

}

const char *json_of_soh(string &jstring, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    jstring.clear();
    // Time
    json_out(jstring, "node_utcoffset", cinfo);

    // Satellite Information
    string soh = json_list_of_soh(cinfo);
    iretn = json_out_list(jstring, soh.c_str(), cinfo);
    if (iretn < 0 && iretn != JSON_ERROR_EOS)
        return nullptr;

    return jstring.data();
}

//! Create JSON for an event
/*! Generate a JSON stream that represents the current state of the
 * ::eventstruc in ::cosmosstruc.
    \param jstring User provided string for creating the JSON stream

    \param cinfo Reference to ::cosmosstruc to use.

    \return Pointer to the string created.
*/
const char *json_of_event(string &jstring, cosmosstruc *cinfo)
{
    jstring.clear();
    json_out(jstring, "event_utc", cinfo);
    if (json_get_double("event_utcexec", cinfo) != 0.) json_out(jstring, "event_utcexec", cinfo);
    json_out(jstring, "event_node", cinfo);
    json_out(jstring, "event_name", cinfo);
    json_out(jstring, "event_user", cinfo);
    json_out(jstring, "event_type", cinfo);
    json_out(jstring, "event_flag", cinfo);
    json_out(jstring, "event_data", cinfo);
    json_out(jstring, "event_condition", cinfo);
    if (json_get_double("event_dtime", cinfo) != 0.) json_out(jstring,"event_dtime", cinfo);
    if (json_get_double("event_ctime", cinfo) != 0.) json_out(jstring,"event_ctime", cinfo);
    if (json_get_double("event_denergy", cinfo) != 0.) json_out(jstring,"event_denergy", cinfo);
    if (json_get_double("event_cenergy", cinfo) != 0.) json_out(jstring,"event_cenergy", cinfo);
    if (json_get_double("event_dmass", cinfo) != 0.) json_out(jstring,"event_dmass", cinfo);
    if (json_get_double("event_cmass", cinfo) != 0.) json_out(jstring,"event_cmass", cinfo);
    if (json_get_double("event_dbytes", cinfo) != 0.) json_out(jstring,"event_dbytes", cinfo);
    if (json_get_double("event_cbytes", cinfo) != 0.) json_out(jstring,"event_cbytes", cinfo);

    return jstring.data();
}

const char *json_of_groundcontact(string &jstring, cosmosstruc *cinfo)
{
    int16_t i;

    jstring.clear();
    json_out(jstring, "node_deci", cinfo);
    json_out(jstring, "node_utcstart", cinfo);
    json_out(jstring, "node_utc", cinfo);
    json_out(jstring, "node_utcoffset", cinfo);
    json_out(jstring, "gs_cnt", cinfo);
    for (i=0; i<*(int16_t *)json_ptrto((char *)"gs_cnt", cinfo); i++)
    {
        json_out_1d(jstring, "gs_az",i, cinfo);
        json_out_1d(jstring, "gs_el",i, cinfo);
    }

    return jstring.data();
}

const char *json_of_device(string &jstring,uint16_t index, cosmosstruc *cinfo, double utc)
{
    jstring.clear();
    if (index < cinfo->device.size() && cinfo->device[index]->utc > utc)
    {
        json_out_1d(jstring, "device_all_utc",index, cinfo);
        json_out_1d(jstring, "device_all_pidx",index, cinfo);
        json_out_1d(jstring, "device_all_cidx",index, cinfo);
        json_out_1d(jstring, "device_all_type",index, cinfo);
        json_out_1d(jstring, "device_all_didx",index, cinfo);
    }

    return jstring.data();
}

const char *json_of_mtr(string &jstring,uint16_t index, cosmosstruc *cinfo)
{
    jstring.clear();
    json_out(jstring, "node_utc", cinfo);
    json_out(jstring, "node_utcoffset", cinfo);

    json_out_1d(jstring, "device_mtr_cidx",index, cinfo);
    json_out_1d(jstring, "device_mtr_mom",index, cinfo);
    json_out_1d(jstring, "device_mtr_mom",index, cinfo);
    json_out_1d(jstring, "device_mtr_rmom",index, cinfo);

    return jstring.data();
}


const char *json_of_rw(string &jstring,uint16_t index, cosmosstruc *cinfo)
{
    jstring.clear();
    json_out(jstring, "node_utc", cinfo);
    json_out(jstring, "node_utcoffset", cinfo);

    json_out_1d(jstring, "device_rw_utc",index, cinfo);
    json_out_1d(jstring, "device_rw_cidx",index, cinfo);
    json_out_1d(jstring, "device_rw_mom_x",index, cinfo);
    json_out_1d(jstring, "device_rw_mom_y",index, cinfo);
    json_out_1d(jstring, "device_rw_mom_z",index, cinfo);
    json_out_1d(jstring, "device_rw_omg",index, cinfo);
    json_out_1d(jstring, "device_rw_alp",index, cinfo);
    json_out_1d(jstring, "device_rw_romg",index, cinfo);
    json_out_1d(jstring, "device_rw_ralp",index, cinfo);

    return jstring.data();
}

const char *json_of_state_eci(string &jstring, cosmosstruc *cinfo)
{
    jstring.clear();
    //	json_out(jstring, "node_utc", cinfo);
    //	json_out(jstring, "node_utcoffset", cinfo);
    json_out(jstring, "node_loc_pos_eci", cinfo);
    json_out(jstring, "node_loc_att_icrf", cinfo);
    return jstring.data();
}

// get state vector in ECEF/Geocentric
const char *json_of_state_geoc(string &jstring, cosmosstruc *cinfo)
{
    jstring.clear();
    //	json_out(jstring, "node_utc", cinfo);
    //	json_out(jstring, "node_utcoffset", cinfo);
    json_out(jstring, "node_loc_pos_geoc", cinfo);
    json_out(jstring, "node_loc_att_geoc", cinfo);
    return jstring.data();
}

//! Dump Node description
/*! Create a JSON stream for general Node variables. Does not include any
 * derivative data (eg. area).
 \param jstring Reference to a string to build the JSON stream in.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_node(string &jstring, cosmosstruc *cinfo)
{

    jstring.clear();
    json_out(jstring, "node_type", cinfo);
    json_out_character(jstring, '\n');
    json_out(jstring, "node_piece_cnt", cinfo);
    json_out_character(jstring, '\n');
    json_out(jstring, "node_vertex_cnt", cinfo);
    json_out_character(jstring, '\n');
    json_out(jstring, "node_face_cnt", cinfo);
    json_out_character(jstring, '\n');
    json_out(jstring, "node_device_cnt", cinfo);
    json_out_character(jstring, '\n');
    json_out(jstring, "node_port_cnt", cinfo);
    json_out_character(jstring, '\n');

    return jstring.data();
}

//! Dump Vertex description
/*! Create a JSON stream for variables specific to the Vertices of the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Reference to a string to build the JSON stream in.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_vertices(string &jstring, cosmosstruc *cinfo)
{
    jstring.clear();
    // Dump vertices
    uint16_t *vertex_cnt = (uint16_t *)json_ptrto((char *)"node_vertex_cnt", cinfo);
    if (vertex_cnt != nullptr)
    {
        for (uint16_t i=0; i<*vertex_cnt; i++)
        {
            json_out_1d(jstring, "vertex",i, cinfo);
            // // json_out_character(jstring, '\n');
        }
    }


    return jstring.data();
}

//! Dump Face description
/*! Create a JSON stream for variables specific to the cinfo->faces of the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Reference to a string to build the JSON stream in.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_faces(string &jstring, cosmosstruc *cinfo)
{
    jstring.clear();
    // Dump faces
    uint16_t *face_cnt = (uint16_t *)json_ptrto((char *)"node_face_cnt", cinfo);
    if (face_cnt != nullptr)
    {
        for (uint16_t i=0; i<*face_cnt; i++)
        {
            json_out_1d(jstring, "face_normal",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "face_com",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "face_area",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "face_vertex_cnt",i, cinfo);
            // // json_out_character(jstring, '\n');
            uint16_t cnt = (uint16_t)json_get_int((char *)"face_vertex_cnt",i, cinfo);
            for (uint16_t j=0; j<cnt; j++)
            {
                json_out_2d(jstring, "face_vertex_idx",i,j, cinfo);
                // // json_out_character(jstring, '\n');
            }
        }
    }


    return jstring.data();
}

//! Dump Piece description
/*! Create a JSON stream for variables specific to the Pieces of the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Reference to a string to build the JSON stream in.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_pieces(string &jstring, cosmosstruc *cinfo)
{
    jstring.clear();
    // Dump structures
    uint16_t *piece_cnt = (uint16_t *)json_ptrto((char *)"node_piece_cnt", cinfo);
    if (piece_cnt != nullptr)
    {
        for (uint16_t i=0; i<*piece_cnt; i++)
        {
            json_out_1d(jstring, "piece_name",i, cinfo);
            // // json_out_character(jstring, '\n');
            //            json_out_1d(jstring, "piece_type",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_cidx",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_mass",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_density",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_emi",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_abs",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_hcap",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_hcon",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_dim",i, cinfo);
            // // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_area",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_com",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "piece_face_cnt",i, cinfo);
            // json_out_character(jstring, '\n');
            uint16_t cnt = (uint16_t)json_get_int((char *)"piece_face_cnt",i, cinfo);
            for (uint16_t j=0; j<cnt; j++)
            {
                json_out_2d(jstring, "piece_face_idx",i,j, cinfo);
                // json_out_character(jstring, '\n');
            }
        }
    }


    return jstring.data();
}

//! Dump General Device description
/*! Create a JSON stream for variables common to all Devices in the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Reference to a string to build the JSON stream in.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_devices_general(string &jstring, cosmosstruc *cinfo)
{

    jstring.clear();
    // Dump components
    uint16_t *device_cnt = (uint16_t *)json_ptrto((char *)"node_device_cnt", cinfo);
    if (device_cnt != nullptr)
    {
        for (uint16_t i=0; i<*device_cnt; i++)
        {
            json_out_1d(jstring, "device_all_type",i, cinfo);
            json_out_1d(jstring, "device_all_name",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_model",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_didx",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_pidx",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_bidx",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_addr",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_portidx",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_nvolt",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_namp",i, cinfo);
            // json_out_character(jstring, '\n');
            json_out_1d(jstring, "device_all_flag",i, cinfo);
            // json_out_character(jstring, '\n');
        }
    }


    return jstring.data();
}

//! Dump Specific Device description
/*! Create a JSON stream for variables specific to particular Devices in the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Reference to a string to build the JSON stream in.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_devices_specific(string &jstring, cosmosstruc *cinfo)
{
    uint16_t *cnt;
    char tstring[COSMOS_MAX_NAME+1];

    jstring.clear();
    // Dump device specific info
    for (uint16_t i=0; i<DeviceType::COUNT; ++i)
    {
        // Create Namespace name for Device Specific count
        sprintf(tstring,"device_%s_cnt",device_type_string[i].c_str());
        if ((cnt=(uint16_t *)json_ptrto(tstring, cinfo)) != nullptr && *cnt != 0)
        {
            // Only dump information for Devices that have non zero count
            for (uint16_t j=0; j<*cnt; ++j)
            {
                // Dump ploads
                if (!strcmp(device_type_string[i].c_str(),"pload"))
                {
                    json_out_1d(jstring, "device_pload_drate",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_pload_key_cnt",j, cinfo);
                    // json_out_character(jstring, '\n');
                    for (uint16_t k=0; k<json_get_int((char *)"device_pload_key_cnt",j, cinfo); ++k)
                    {
                        json_out_2d(jstring, "device_pload_key_name",j,k, cinfo);
                        // json_out_character(jstring, '\n');
                    }
                    continue;
                }

                // Dump Sun sensors
                if (!strcmp(device_type_string[i].c_str(),"ssen"))
                {
                    json_out_1d(jstring, "device_ssen_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump IMU's
                if (!strcmp(device_type_string[i].c_str(),"imu"))
                {
                    json_out_1d(jstring, "device_imu_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Reaction Wheel
                if (!strcmp(device_type_string[i].c_str(),"rw"))
                {
                    json_out_1d(jstring, "device_rw_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_rw_mom",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_rw_mxalp",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_rw_mxomg",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_rw_tc",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Magtorque Rods
                if (!strcmp(device_type_string[i].c_str(),"mtr"))
                {
                    json_out_1d(jstring, "device_mtr_mxmom",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_mtr_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    for (uint16_t k=0; k<7; ++k)
                    {
                        json_out_2d(jstring, "device_mtr_npoly",j,k, cinfo);
                        // json_out_character(jstring, '\n');
                    }
                    for (uint16_t k=0; k<7; ++k)
                    {
                        json_out_2d(jstring, "device_mtr_ppoly",j,k, cinfo);
                        // json_out_character(jstring, '\n');
                    }
                    json_out_1d(jstring, "device_mtr_tc",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Computer systems
                if (!strcmp(device_type_string[i].c_str(),"cpu"))
                {
                    json_out_1d(jstring, "device_cpu_maxgib",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_cpu_maxload",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump GPS's
                if (!strcmp(device_type_string[i].c_str(),"gps"))
                {
                    continue;
                }

                // Dump Antennas
                if (!strcmp(device_type_string[i].c_str(),"ant"))
                {
                    json_out_1d(jstring, "device_ant_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_ant_minelev",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Receivers
                if (!strcmp(device_type_string[i].c_str(),"rxr"))
                {
                    json_out_1d(jstring, "device_rxr_opmode",j, cinfo);
                    json_out_1d(jstring, "device_rxr_modulation",j, cinfo);
                    json_out_1d(jstring, "device_rxr_rssi",j, cinfo);
                    json_out_1d(jstring, "device_rxr_pktsize",j, cinfo);
                    json_out_1d(jstring, "device_rxr_freq",j, cinfo);
                    json_out_1d(jstring, "device_rxr_maxfreq",j, cinfo);
                    json_out_1d(jstring, "device_rxr_minfreq",j, cinfo);
                    json_out_1d(jstring, "device_rxr_byte_rate",j, cinfo);
                    json_out_1d(jstring, "device_rxr_powerin",j, cinfo);
                    json_out_1d(jstring, "device_rxr_powerout",j, cinfo);
                    json_out_1d(jstring, "device_rxr_maxpower",j, cinfo);
                    json_out_1d(jstring, "device_rxr_band",j, cinfo);
                    json_out_1d(jstring, "device_rxr_goodratio",j, cinfo);
                    json_out_1d(jstring, "device_rxr_utcin",j, cinfo);
                    json_out_1d(jstring, "device_rxr_uptime",j, cinfo);
                    json_out_1d(jstring, "device_rxr_bytesin",j, cinfo);
                    continue;
                }

                // Dump Transmitters
                if (!strcmp(device_type_string[i].c_str(),"txr"))
                {
                    json_out_1d(jstring, "device_txr_opmode",j, cinfo);
                    json_out_1d(jstring, "device_txr_modulation",j, cinfo);
                    json_out_1d(jstring, "device_txr_rssi",j, cinfo);
                    json_out_1d(jstring, "device_txr_pktsize",j, cinfo);
                    json_out_1d(jstring, "device_txr_freq",j, cinfo);
                    json_out_1d(jstring, "device_txr_maxfreq",j, cinfo);
                    json_out_1d(jstring, "device_txr_minfreq",j, cinfo);
                    json_out_1d(jstring, "device_txr_byte_rate",j, cinfo);
                    json_out_1d(jstring, "device_txr_powerin",j, cinfo);
                    json_out_1d(jstring, "device_txr_powerout",j, cinfo);
                    json_out_1d(jstring, "device_txr_maxpower",j, cinfo);
                    json_out_1d(jstring, "device_txr_band",j, cinfo);
                    json_out_1d(jstring, "device_txr_goodratio",j, cinfo);
                    json_out_1d(jstring, "device_txr_badcnt",j, cinfo);
                    json_out_1d(jstring, "device_txr_uptime",j, cinfo);
                    json_out_1d(jstring, "device_txr_bytesout",j, cinfo);
                    continue;
                }

                // Dump Transceivers
                if (!strcmp(device_type_string[i].c_str(),"tcv"))
                {
                    json_out_1d(jstring, "device_tcv_opmode",j, cinfo);
                    json_out_1d(jstring, "device_tcv_modulation",j, cinfo);
                    json_out_1d(jstring, "device_tcv_rssi",j, cinfo);
                    json_out_1d(jstring, "device_tcv_pktsize",j, cinfo);
                    json_out_1d(jstring, "device_tcv_freq",j, cinfo);
                    json_out_1d(jstring, "device_tcv_maxfreq",j, cinfo);
                    json_out_1d(jstring, "device_tcv_minfreq",j, cinfo);
                    json_out_1d(jstring, "device_tcv_byte_rate",j, cinfo);
                    json_out_1d(jstring, "device_tcv_powerin",j, cinfo);
                    json_out_1d(jstring, "device_tcv_powerout",j, cinfo);
                    json_out_1d(jstring, "device_tcv_maxpower",j, cinfo);
                    json_out_1d(jstring, "device_tcv_band",j, cinfo);
                    json_out_1d(jstring, "device_tcv_goodratio",j, cinfo);
                    json_out_1d(jstring, "device_tcv_badcnt",j, cinfo);
                    json_out_1d(jstring, "device_tcv_uptime",j, cinfo);
                    json_out_1d(jstring, "device_tcv_bytesin",j, cinfo);
                    json_out_1d(jstring, "device_tcv_bytesout",j, cinfo);
                    continue;
                }

                // Dump solar strings
                if (!strcmp(device_type_string[i].c_str(),"strg"))
                {
                    json_out_1d(jstring, "device_pvstrg_efi",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_pvstrg_efs",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_pvstrg_max",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump batteries
                if (!strcmp(device_type_string[i].c_str(),"batt"))
                {
                    json_out_1d(jstring, "device_batt_cap",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_batt_eff",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Heaters
                if (!strcmp(device_type_string[i].c_str(),"htr"))
                {
                    continue;
                }

                // Dump motors
                if (!strcmp(device_type_string[i].c_str(),"motr"))
                {
                    json_out_1d(jstring, "device_motr_rat",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_motr_max",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump temperature sensor
                if (!strcmp(device_type_string[i].c_str(),"tsen"))
                {
                    continue;
                }

                // Dump thsters
                if (!strcmp(device_type_string[i].c_str(),"thst"))
                {
                    json_out_1d(jstring, "device_thst_isp",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_thst_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump propellant tanks
                if (!strcmp(device_type_string[i].c_str(),"prop"))
                {
                    json_out_1d(jstring, "device_prop_cap",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Switch info
                if (!strcmp(device_type_string[i].c_str(),"swch"))
                {
                    continue;
                }

                // Dump Rotor info
                if (!strcmp(device_type_string[i].c_str(),"rot"))
                {
                    continue;
                }

                // Dump STT's
                if (!strcmp(device_type_string[i].c_str(),"stt"))
                {
                    json_out_1d(jstring, "device_stt_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Motion Capture info
                if (!strcmp(device_type_string[i].c_str(),"mcc"))
                {
                    json_out_1d(jstring, "device_mcc_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Torque Rod Control Unit info
                if (!strcmp(device_type_string[i].c_str(),"tcu"))
                {
                    json_out_1d(jstring, "device_tcu_mcnt",j, cinfo);
                    // json_out_character(jstring, '\n');
                    for (uint16_t k=0; k<json_get_int((char *)"device_tcu_mcnt",j, cinfo); ++k)
                    {
                        json_out_2d(jstring, "device_tcu_mcidx",j,k, cinfo);
                        // json_out_character(jstring, '\n');
                    }
                    continue;
                }

                // Dump power bus
                if (!strcmp(device_type_string[i].c_str(),"bus"))
                {
                    continue;
                }

                // Dump pressure sensor
                if (!strcmp(device_type_string[i].c_str(),"psen"))
                {
                    continue;
                }

                // Dump SUCHI
                if (!strcmp(device_type_string[i].c_str(),"suchi"))
                {
                    json_out_1d(jstring, "device_suchi_align",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Cameras
                if (!strcmp(device_type_string[i].c_str(),"cam"))
                {
                    json_out_1d(jstring, "device_cam_pwidth",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_cam_pheight",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_cam_width",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_cam_height",j, cinfo);
                    // json_out_character(jstring, '\n');
                    json_out_1d(jstring, "device_cam_flength",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Telemetry
                if (!strcmp(device_type_string[i].c_str(),"telem"))
                {
                    json_out_1d(jstring, "device_telem_type",j, cinfo);
                    json_out_1d(jstring, "device_telem_name",j, cinfo);
                    //                    switch (json_get_int((char *)"device_telem_type",j, cinfo))
                    //                    {
                    //                    case TELEM_TYPE_UINT8:
                    //                        json_out_1d(jstring, "device_telem_vuint8",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_INT8:
                    //                        json_out_1d(jstring, "device_telem_vint8",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_UINT16:
                    //                        json_out_1d(jstring, "device_telem_vuint16",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_INT16:
                    //                        json_out_1d(jstring, "device_telem_vint16",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_UINT32:
                    //                        json_out_1d(jstring, "device_telem_vuint32",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_INT32:
                    //                        json_out_1d(jstring, "device_telem_vint32",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_FLOAT:
                    //                        json_out_1d(jstring, "device_telem_vfloat",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_DOUBLE:
                    //                        json_out_1d(jstring, "device_telem_vdouble",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    case TELEM_TYPE_STRING:
                    //                        json_out_1d(jstring, "device_telem_vstring",j, cinfo);
                    //                        // json_out_character(jstring, '\n');
                    //                        break;
                    //                    }
                    continue;
                }

                // Dump Disks
                if (!strcmp(device_type_string[i].c_str(),"disk"))
                {
                    json_out_1d(jstring, "device_disk_maxgib",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

                // Dump XYZ sensors
                if (!strcmp(device_type_string[i].c_str(),"xyzsen"))
                {
                    json_out_1d(jstring, "device_xyzsen_direction",j, cinfo);
                    // json_out_character(jstring, '\n');
                    continue;
                }

            }
        }
    }

    return jstring.data();
}

//! Dump Port description
/*! Create a JSON stream for the Port information of the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Reference to a string to build the JSON stream in.

    \param cinfo Reference to ::cosmosstruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_ports(string &jstring, cosmosstruc *cinfo)
{

    jstring.clear();
    // Dump Port table
    for (uint16_t i=0; i<*(int16_t *)json_ptrto((char *)"node_port_cnt", cinfo); i++)
    {
        json_out_1d(jstring, "port_name",i, cinfo);
        // json_out_character(jstring, '\n');
        json_out_1d(jstring, "port_type",i, cinfo);
        // json_out_character(jstring, '\n');
    }

    return jstring.data();
}

void json_test(cosmosstruc *cinfo)
{
    uint16_t i, j;
    long hash;

    int32_t hashcount[1480]= {0};

    for (i=0; i<cinfo->jmap.size(); ++i)
    {
        for (j=0; j<cinfo->jmap[i].size(); ++j)
        {
            hash = json_hash(cinfo->jmap[i][j].name);

            printf("%s %d %d %ld\n", cinfo->jmap[i][j].name.c_str(),i,j,hash);

            hashcount[hash]++;
        }

    }
}

//! Get hash and index in JSON Namespace map
/*! Using the provided name, find it's location in the provided Namespace map and set the
 * values for the hash and index.
    \param name Namespace name.

    \param handle Pointer to ::jsonhandle of name.
    \return Zero, or negative error number.
*/
int32_t json_name_map(string name, cosmosstruc *cinfo, jsonhandle &handle)
{

    if (!cinfo->jmapped)
        return (JSON_ERROR_NOJMAP);

    if (cinfo->jmap.size() == 0)
        return (JSON_ERROR_NOJMAP);

    handle.hash = json_hash(name);

    for (handle.index=0; handle.index<cinfo->jmap[handle.hash].size(); ++handle.index)
        if (name == cinfo->jmap[handle.hash][handle.index].name)
        {
            return 0;
        }

    return (JSON_ERROR_NOENTRY);
}

//! Get hash and index in JSON Equation map
/*! Using the provided text, find it's location in the provided Equation map and set the
 * values for the hash and index. If the equation is not already in the table, add it.
    \param equation Equation text.

    \param handle Pointer to ::jsonhandle of name.
    \return Zero, or negative error number.
*/
int32_t json_equation_map(string equation, cosmosstruc *cinfo, jsonhandle *handle)
{
    const char *pointer;
    jsonequation tequation;
    char ops[] = "+-*/%&|><=!~^@#";
    int32_t iretn = 0;
    //    size_t textlen;

    if (!cinfo->jmapped)
        return (JSON_ERROR_NOJMAP);

    if (cinfo->emap.size() == 0)
        return (JSON_ERROR_NOJMAP);

    // Equations must start and end with ')'
    if (equation[0] != '(' || equation[equation.length()-1] != ')')
    {
        return (JSON_ERROR_SCAN);
    }

    handle->hash = json_hash(equation);

    for (handle->index=0; handle->index<cinfo->emap[handle->hash].size(); ++handle->index)
    {
        //        if (!strcmp(equation.c_str(), cinfo->emap[handle->hash][handle->index].text))
        if (equation == cinfo->emap[handle->hash][handle->index].text)
        {
            return 0;
        }
    }

    // Not found. We will have to add it to the map.
    pointer = &equation[1];

    // Extract first operand
    if ((iretn=json_parse_operand(pointer, &tequation.operand[0], cinfo)) < 0)
    {
        return (JSON_ERROR_SCAN);
    }

    // Extract operation
    json_skip_white(pointer);
    for (tequation.operation=0; tequation.operation<(int)strlen(ops); tequation.operation++)
    {
        if ((pointer)[0] == ops[tequation.operation])
            break;
    }
    if (tequation.operation == (int)strlen(ops))
        return (JSON_ERROR_SCAN);
    (pointer)++;

    // Extract second argument
    if ((iretn=json_parse_operand(pointer, &tequation.operand[1], cinfo)) < 0)
    {
        return (JSON_ERROR_SCAN);
    }

    // Remove trailing )
    while ((pointer)[0] != 0 && (pointer)[0] != ')')
        (pointer)++;
    if ((pointer)[0] != 0)
        (pointer)++;

    //    textlen = equation.size()+1;
    //    if ((tequation.text = (char *)calloc(1,textlen)) == nullptr)
    //    {
    //        return (JSON_ERROR_SCAN);
    //    }

    // Populate the equation
    //    strcpy(tequation.text,equation.c_str());
    tequation.text = equation;

    handle->index = (uint16_t)cinfo->emap[handle->hash].size();
    cinfo->emap[handle->hash].push_back(tequation);
    if (cinfo->emap[handle->hash].size() != handle->index+1u)
    {
        //        free(tequation.text);
        return (JSON_ERROR_SCAN);
    }

    return 0;

}

//! Clone ::cosmosstruc data areas, direct
/*! Copy one ::cosmosstruc to another.
    \param cinfo1 Source ::cosmosstruc.
    \param cinfo2 Target ::cosmosstruc.
    \return Zero, or negative error.
*/
int32_t json_shrink(cosmosstruc *cinfo)
{
    for (size_t i=0; i<cinfo->jmap.size(); ++i)
    {
        for (size_t j=0; j<cinfo->jmap[i].size(); ++j)
        {
            cinfo->jmap[i][j].shrinkusage();
        }
        vector<jsonentry>(cinfo->jmap[i]).swap(cinfo->jmap[i]);
    }
    vector<vector<jsonentry>>(cinfo->jmap).swap(cinfo->jmap);

    for (size_t i=0; i<cinfo->emap.size(); ++i)
    {
        for (size_t j=0; j<cinfo->emap[i].size(); ++j)
        {
            cinfo->emap[i][j].shrinkusage();
        }
        vector<jsonequation>(cinfo->emap[i]).swap(cinfo->emap[i]);
    }
    vector<vector<jsonequation>>(cinfo->emap).swap(cinfo->emap);

    for (size_t i=0; i<cinfo->unit.size(); ++i)
    {
        for (size_t j=0; j<cinfo->unit[i].size(); ++j)
        {
            cinfo->unit[i][j].shrinkusage();
        }
        vector<unitstruc>(cinfo->unit[i]).swap(cinfo->unit[i]);
    }
    vector<vector<unitstruc>>(cinfo->unit).swap(cinfo->unit);

    for (size_t i=0; i<cinfo->equation.size(); ++i)
    {
        cinfo->equation[i].shrinkusage();
    }
    vector<equationstruc>(cinfo->equation).swap(cinfo->equation);

    for (size_t i=0; i<cinfo->alias.size(); ++i)
    {
        cinfo->alias[i].shrinkusage();
    }
    vector<aliasstruc>(cinfo->alias).swap(cinfo->alias);

    cinfo->node.shrinkusage();

    vector<vertexstruc>(cinfo->vertexs).swap(cinfo->vertexs);
    vector<vertexstruc>(cinfo->normals).swap(cinfo->normals);

    for (size_t i=0; i<cinfo->faces.size(); ++i)
    {
        cinfo->faces[i].shrinkusage();
    }
    vector<facestruc>(cinfo->faces).swap(cinfo->faces);

    for (size_t i=0; i<cinfo->pieces.size(); ++i)
    {
        cinfo->pieces[i].shrinkusage();
    }
    vector<piecestruc>(cinfo->pieces).swap(cinfo->pieces);

    vector<devicestruc*>(cinfo->device).swap(cinfo->device);

    cinfo->devspec.shrinkusage();

    for (size_t i=0; i<cinfo->port.size(); ++i)
    {
        cinfo->port[i].shrinkusage();
    }
    vector<portstruc>(cinfo->port).swap(cinfo->port);

    for (size_t i=0; i<cinfo->agent.size(); ++i)
    {
        cinfo->agent[i].shrinkusage();
    }
    vector<agentstruc>(cinfo->agent).swap(cinfo->agent);

    for (size_t i=0; i<cinfo->event.size(); ++i)
    {
        cinfo->event[i].shrinkusage();
    }
    vector<eventstruc>(cinfo->event).swap(cinfo->event);

    for (size_t i=0; i<cinfo->target.size(); ++i)
    {
        cinfo->target[i].shrinkusage();
    }
    vector<targetstruc>(cinfo->target).swap(cinfo->target);

    for (size_t i=0; i<cinfo->user.size(); ++i)
    {
        cinfo->user[i].shrinkusage();
    }
    vector<userstruc>(cinfo->user).swap(cinfo->user);

    return 0;
}

uint32_t json_get_name_list_count(cosmosstruc *cinfo)
{
    if (cinfo->jmapped == false) return 0;

    uint32_t count = 0;
    for (uint32_t i = 0; i < cinfo->jmap.size(); i++)
    {
        for (uint32_t j = 0; j < cinfo->jmap[i].size(); j++)
        {
            ++count;
        }
    }
    return count;
}

//! Initialize Node configuration
/*! Load initial Node configuration file. Then calculate all derivative values (eg. COM)
    \param node Node to be initialized using node.ini. Node must be a directory in
    ::nodedir. If NULL, node.ini must be in current directory.
    \param cinfo Reference to ::cosmosstruc to use.
    \return 0, or negative error.
*/
int32_t node_init(string node, cosmosstruc *cinfo)
{
    int32_t iretn = 0;

    if (cinfo == nullptr)
        return (JSON_ERROR_NOJMAP);

    iretn = json_setup_node(node, cinfo);
    if (iretn < 0)
    {
        return iretn;
    }

    node_calc(cinfo);

    //! Load targeting information
    cinfo->node.target_cnt = (uint16_t)load_target(cinfo);

    return 0;
}

//! Calculate Satellite configuration values.
/*! Using the provided satellite structure, populate the derivative static quantities and initialize any
 * reasonable dynamic quantities.
    \param cinfo Reference to ::cosmosstruc to use.
    \return 0
*/
int32_t node_calc(cosmosstruc *cinfo)
{
    //    uint16_t n, i, j, k;
    //    double dm, ta, tb, tc;
    //    rvector tv0, tv1, tv2, tv3, dv, sv;

    cinfo->node.phys.hcap = cinfo->node.phys.heat = 0.;
    cinfo->node.phys.mass = 0.;
    cinfo->node.phys.moi = rv_zero();
    cinfo->node.phys.com = rv_zero();

    json_recenter_node(cinfo);

    for (size_t n=0; n<cinfo->pieces.size(); n++)
    {
        cinfo->pieces[n].mass = cinfo->pieces[n].volume * cinfo->pieces[n].density;
        if (cinfo->pieces[n].mass == 0.)
            cinfo->pieces[n].mass = .001f;
        //        cinfo->pieces[n].temp = 300.;
        cinfo->pieces[n].heat = cinfo->pieces[n].temp * cinfo->pieces[n].hcap;
        cinfo->node.phys.heat += cinfo->pieces[n].heat;
        cinfo->node.phys.mass += cinfo->pieces[n].mass;
        cinfo->node.phys.hcap += cinfo->pieces[n].hcap * cinfo->pieces[n].mass;

    }

    if (cinfo->node.phys.mass == 0.)
    {
        cinfo->node.phys.mass = 1.;
    }

    // Turn on power buses
    for (size_t n=0; n<cinfo->devspec.bus_cnt; n++)
    {
        cinfo->devspec.bus[n].flag |= DEVICE_FLAG_ON;
    }


    for (size_t n=0; n<cinfo->node.device_cnt; n++)
    {
        /*
    if (cinfo->device[n].pidx >= 0)
        {
        cinfo->node.com.col[0] += cinfo->pieces[cinfo->device[n].pidx].centroid.col[0] * cinfo->device[n]->mass;
        cinfo->node.com.col[1] += cinfo->pieces[cinfo->device[n].pidx].centroid.col[1] * cinfo->device[n]->mass;
        cinfo->node.com.col[2] += cinfo->pieces[cinfo->device[n].pidx].centroid.col[2] * cinfo->device[n]->mass;
        }
    if (cinfo->device[n].pidx >= 0)
        {
        cinfo->pieces[cinfo->device[n].pidx].heat += 300. * cinfo->pieces[cinfo->device[n].pidx].hcap * cinfo->device[n]->mass;
        cinfo->node.heat += 300. * cinfo->pieces[cinfo->device[n].pidx].hcap * cinfo->device[n]->mass;
        }
    cinfo->node.mass += cinfo->device[n]->mass;
    */
        //        cinfo->device[n]->temp = 300.;
        //        cinfo->device[n]->flag |= DEVICE_FLAG_ON;
        if (cinfo->device[n]->flag & DEVICE_FLAG_ON)
        {
            cinfo->device[n]->amp = cinfo->device[n]->namp;
            cinfo->device[n]->volt = cinfo->device[n]->nvolt;
            cinfo->device[n]->power = cinfo->device[n]->amp * cinfo->device[n]->volt;
        }
        if (cinfo->device[n]->bidx < cinfo->devspec.bus_cnt && cinfo->devspec.bus[cinfo->device[n]->bidx].volt < cinfo->device[n]->volt)
        {
            cinfo->devspec.bus[cinfo->device[n]->bidx].volt = cinfo->device[n]->volt;
        }
    }

    //    cinfo->node.phys.com = rv_smult(1./cinfo->node.phys.mass,cinfo->node.phys.com);
    cinfo->node.phys.hcap /= cinfo->node.phys.mass;

    for (size_t n=0; n<cinfo->pieces.size(); n++)
    {
        piecestruc *tpiece = &cinfo->pieces[n];
        tpiece->shove = Vector();
        tpiece->twist = Vector();
        switch (cinfo->pieces[n].face_cnt)
        {
        default:
            {
                double ta = tpiece->com.flattenx().norm();
                cinfo->node.phys.moi.x += tpiece->mass * ta * ta;
                ta = tpiece->com.flatteny().norm();
                cinfo->node.phys.moi.y += tpiece->mass * ta * ta;
                ta = tpiece->com.flattenz().norm();
                cinfo->node.phys.moi.z += tpiece->mass * ta * ta;
            }
            break;
        case 1:
            {
                tpiece->shove = -tpiece->area * (cinfo->faces[tpiece->face_idx[0]].normal.dot(tpiece->com)) * tpiece->com / (tpiece->com.norm() * tpiece->com.norm());
                tpiece->twist = -tpiece->area * tpiece->com.norm() * cinfo->faces[tpiece->face_idx[0]].normal - tpiece->com.norm() * tpiece->shove;
            }
            break;
        case 0:
            break;
        }
    }

    if (cinfo->node.phys.moi.norm() == 0.)
    {
        cinfo->node.phys.moi.clear(1.,1.,1.);
    }

    // Turn all CPU's on
    for (size_t n=0; n<cinfo->devspec.cpu_cnt; n++)
    {
        cinfo->devspec.cpu[n].flag |= DEVICE_FLAG_ON;
    }

    // Turn on all IMU's
    for (size_t n=0; n<cinfo->devspec.imu_cnt; n++)
    {
        cinfo->devspec.imu[n].flag |= DEVICE_FLAG_ON;
    }

    // Turn on all MGR's
    for (size_t n=0; n<cinfo->devspec.mag_cnt; n++)
    {
        cinfo->devspec.mag[n].flag |= DEVICE_FLAG_ON;
    }

    // Turn on all GYRO's
    for (size_t n=0; n<cinfo->devspec.gyro_cnt; n++)
    {
        cinfo->devspec.gyro[n].flag |= DEVICE_FLAG_ON;
    }

    // Turn on all GPS's
    for (size_t n=0; n<cinfo->devspec.gps_cnt; n++)
    {
        cinfo->devspec.gps[n].flag |= DEVICE_FLAG_ON;
    }

    cinfo->node.phys.battcap = 0.;
    for (size_t n=0; n<cinfo->devspec.batt_cnt; n++)
    {
        cinfo->node.phys.battcap += cinfo->devspec.batt[n].capacity;
        cinfo->devspec.batt[n].charge = cinfo->devspec.batt[n].capacity;
    }
    cinfo->node.phys.battlev = cinfo->node.phys.battcap;

    // Turn off reaction wheels
    for (size_t i=0; i<cinfo->devspec.rw_cnt; i++)
    {
        cinfo->devspec.rw[i].alp = cinfo->devspec.rw[i].omg = 0.;
    }

    // Set fictional torque to zero
    cinfo->node.phys.ftorque = rv_zero();

    return 0;
}

//! Dump tab delimited database files
/*! Create files that can be read in to a relational database representing the various elements of
 * the satellite. Tables are created for Parts, Components, Devices, Temperature Sensors and
 * Power Buses.
*/
void create_databases(cosmosstruc *cinfo)
{
    FILE *op;
    uint32_t i, j;
    piecestruc s;
    //    allstruc cs;
    devicestruc* cs;
    imustruc ims;
    sttstruc sts;
    int32_t iretn = 0;

    /*
 *	op = fopen("target.txt","w");
    fprintf(op,"gs_idx	gs_name	gs_pos_lat	gs_pos_lon	gs_pos_alt	gs_min gs_az	gs_el\n");
    for (i=0; i<cinfo->node.target_cnt; i++)
    {
        fprintf(op,"%d\t%s\t%u\n",i,cinfo->target[i].name,cinfo->target[i].type);
    }
    fclose(op);
*/

    //    op = fopen("piece.txt","w");
    //    fprintf(op,"PartIndex\tName\tType\tTemperatureIndex\tComponentIndex\tMass\tEmissivity\tAbsorptivity\tDimension\tHeatCapacity\tHeatConductivity\tArea\tTemp\tHeat\tPointCount\tPoint1X\tPoint1Y\tPoint1Z\tPoint2X\tPoint2Y\tPoint2Z\tPoint3X\tPoint3Y\tPoint3Z\tPoint4X\tPoint4Y\tPoint4Z\tPoint5X\tPoint5Y\tPoint5Z\tPoint6X\tPoint6Y\tPoint6Z\tPoint7X\tPoint7Y\tPoint7Z\tPoint8X\tPoint8Y\tPoint8Z\n");
    //    for (i=0; i<cinfo->pieces.size(); i++)
    //    {
    //        s = cinfo->pieces[i];
    //        fprintf(op,"%d\t%s\t%d\t%d\t%.4f\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.6f\t%u",i,s.name,s.type,s.cidx,s.mass,s.emi,s.abs,s.dim,s.hcap,s.hcon,s.area,s.pnt_cnt);
    //        for (j=0; j<s.face_cnt; j++)
    //        {
    //            fprintf(op,"\t%.6f\t%.6f\t%.6f",s.vertex_idx[j].col[0],s.vertex_idx[j].col[1],s.vertex_idx[j].col[2]);
    //        }
    //        fprintf(op,"\n");
    //    }
    //    fclose(op);

    op = fopen("comp.txt","w");
    fprintf(op,"device_all_idx\tdevice_all_type\tdevice_all_didx\tdevice_all_pidx\tdevice_all_bidx\tdevice_all_namp\tdevice_all_nvolt\tdevice_all_amp\tdevice_all_volt\tdevice_all_temp\tdevice_all_on\n");
    for (i=0; i<cinfo->node.device_cnt; i++)
    {
        cs = cinfo->device[i];
        fprintf(op,"%d\t%d\t%d\t%d\t%d\t%.15g\t%.15g\n",i,cs->type,cs->didx,cs->pidx,cs->bidx,cs->amp,cs->volt);
    }
    fclose(op);

    op = fopen("rw.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tMomentX\tMomentY\tMomentZ\tMaxAngularSpeed\tAngularSpeed\tAngularAcceleration\n");
    for (i=0; i<cinfo->devspec.rw_cnt; i++)
    {
        rwstruc rws = cinfo->devspec.rw[i];
        //        devicestruc d = cinfo->devspec.rw[i]];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,rws.cidx,rws.align.d.x,rws.align.d.y,rws.align.d.z,rws.align.w,rws.mom.col[0],rws.mom.col[1],rws.mom.col[2],rws.mxomg,rws.mxalp);
    }
    fclose(op);

    op = fopen("tsen.txt","w");
    fprintf(op,"TemperatureIndex\tCompIndex\tTemperature\n");
    for (i=0; i<cinfo->devspec.tsen_cnt; i++)
    {
        tsenstruc tsens = cinfo->devspec.tsen[i];
        fprintf(op,"%d\t%d\t%.15g\n",i,tsens.cidx,tsens.temp);
    }
    fclose(op);

    op = fopen("strg.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tEfficiencyB\tEfficiencyM\tMaxPower\tPower\n");
    for (i=0; i<cinfo->devspec.pvstrg_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cinfo->devspec.pvstrg[i].cidx,cinfo->devspec.pvstrg[i].effbase,cinfo->devspec.pvstrg[i].effslope,cinfo->devspec.pvstrg[i].maxpower,cinfo->devspec.pvstrg[i].power);
    }
    fclose(op);

    op = fopen("batt.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tCapacity\tEfficiency\tCharge\n");
    for (i=0; i<cinfo->devspec.batt_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\n",i,cinfo->devspec.batt[i].cidx,cinfo->devspec.batt[i].capacity,cinfo->devspec.batt[i].efficiency,cinfo->devspec.batt[i].charge);
    }
    fclose(op);

    op = fopen("ssen.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tQuadrantVoltageA\tQuadrantVoltageB\tQuadrantVoltageC\tQuadrantVoltageD\tAzimuth\tElevation\n");
    for (i=0; i<cinfo->devspec.ssen_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cinfo->devspec.ssen[i].cidx,cinfo->devspec.ssen[i].align.d.x,cinfo->devspec.ssen[i].align.d.y,cinfo->devspec.ssen[i].align.d.z,cinfo->devspec.ssen[i].align.w,cinfo->devspec.ssen[i].qva,cinfo->devspec.ssen[i].qvb,cinfo->devspec.ssen[i].qvc,cinfo->devspec.ssen[i].qvd,cinfo->devspec.ssen[i].azimuth,cinfo->devspec.ssen[i].elevation);
    }
    fclose(op);

    op = fopen("imu.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tPositionX\tPositionY\tPositionZ\tVelocityX\tVelocityY\tVelocityZ\tAccelerationX\tAccelerationY\tAccelerationZ\tAttitudeQx\tAttitudeQy\tAttitudeQz\tAttitudeQw\tAngularVelocityX\tAngularVelocityY\tAngularVelocityZ\tAngularAccelerationX\tAngularAccelerationY\tAngularAccelerationZ\tMagneticFieldX\tMagneticFieldY\tMagneticFieldZ\tCalibrationQx\tCalibrationQy\tCalibrationQz\tCalibrationQw\n");
    for (i=0; i<cinfo->devspec.imu_cnt; i++)
    {
        ims = cinfo->devspec.imu[i];
        devicestruc d = cinfo->devspec.imu[i];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,d.cidx,ims.align.d.x,ims.align.d.y,ims.align.d.z,ims.align.w);
    }
    fclose(op);

    op = fopen("gyro.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tOmegaX\tOmegaY\tOmegaZ\tAlphaX\tAlphaY\tAlphaZ\n");
    for (i=0; i<cinfo->devspec.gyro_cnt; i++)
    {
        gyrostruc ims = cinfo->devspec.gyro[i];
        devicestruc d = cinfo->devspec.gyro[i];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,d.cidx,ims.align.d.x,ims.align.d.y,ims.align.d.z,ims.align.w);
    }
    fclose(op);

    op = fopen("mag.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tPositionX\tPositionY\tPositionZ\tVelocityX\tVelocityY\tVelocityZ\tAccelerationX\tAccelerationY\tAccelerationZ\tAttitudeQx\tAttitudeQy\tAttitudeQz\tAttitudeQw\tAngularVelocityX\tAngularVelocityY\tAngularVelocityZ\tAngularAccelerationX\tAngularAccelerationY\tAngularAccelerationZ\tMagneticFieldX\tMagneticFieldY\tMagneticFieldZ\tCalibrationQx\tCalibrationQy\tCalibrationQz\tCalibrationQw\n");
    for (i=0; i<cinfo->devspec.mag_cnt; i++)
    {
        magstruc ims = cinfo->devspec.mag[i];
        devicestruc d = cinfo->devspec.mag[i];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,d.cidx,ims.align.d.x,ims.align.d.y,ims.align.d.z,ims.align.w);
    }
    fclose(op);

    op = fopen("stt.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tAttitudeQx\tAttitudeQy\tAttitudeQz\tAttitudeQw\tAngularVelocityX\tAngularVelocityY\tAngularVelocityZ\tAngularAccelerationX\tAngularAccelerationY\tAngularAccelerationZ\tCalibrationQx\tCalibrationQy\tCalibrationQz\tCalibrationQw\n");
    for (i=0; i<cinfo->devspec.stt_cnt; i++)
    {
        sts = cinfo->devspec.stt[i];
        devicestruc d = cinfo->devspec.stt[i];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,d.cidx,sts.align.d.x,sts.align.d.y,sts.align.d.z,sts.align.w);
    }
    fclose(op);

    op = fopen("mtr.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tMagneticMomentX\tMagneticMomentY\tMagneticMomentZ\tMagneticField\n");
    for (i=0; i<cinfo->devspec.mtr_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cinfo->devspec.mtr[i].cidx,cinfo->devspec.mtr[i].align.d.x,cinfo->devspec.mtr[i].align.d.y,cinfo->devspec.mtr[i].align.d.z,cinfo->devspec.mtr[i].align.w,cinfo->devspec.mtr[i].npoly[0],cinfo->devspec.mtr[i].npoly[1],cinfo->devspec.mtr[i].npoly[2],cinfo->devspec.mtr[i].mom);
    }
    fclose(op);

    op = fopen("gps.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tLatitude\tLongitude\tAltitude\tVelocityX\tVelocityY\tVelocityZ\n");
    for (i=0; i<cinfo->devspec.gps_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cinfo->devspec.gps[i].cidx,cinfo->devspec.gps[i].geocs.col[0],cinfo->devspec.gps[i].geocs.col[1],cinfo->devspec.gps[i].geocs.col[2],cinfo->devspec.gps[i].geocv.col[0],cinfo->devspec.gps[i].geocv.col[1],cinfo->devspec.gps[i].geocv.col[2]);
    }
    fclose(op);

    op = fopen("cpu.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tLoad\tMemoryUse\tMemoryFree\tDiskUse\n");
    for (i=0; i<cinfo->devspec.cpu_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.8g\t%.8g\t%.8g\n",i,cinfo->devspec.cpu[i].cidx,cinfo->devspec.cpu[i].maxgib,cinfo->devspec.cpu[i].load,cinfo->devspec.cpu[i].gib);
    }
    fclose(op);

    op = fopen("pload.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tKeyCount");
    for (i=0; i<MAXPLOADKEYCNT; i++)
    {
        fprintf(op,"\tKey%d",i);
    }
    for (i=0; i<MAXPLOADKEYCNT; i++)
    {
        fprintf(op,"\tValue%d",i);
    }
    fprintf(op,"\n");
    for (i=0; i<cinfo->devspec.pload_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%d",i,cinfo->devspec.pload[i].cidx,cinfo->devspec.pload[i].key_cnt);
        for (j=0; j<MAXPLOADKEYCNT; j++)
        {
            fprintf(op,"\t%d",cinfo->devspec.pload[i].keyidx[j]);
        }
        for (j=0; j<MAXPLOADKEYCNT; j++)
        {
            fprintf(op,"\t%.15g",cinfo->devspec.pload[i].keyval[j]);
        }
        fprintf(op,"\n");
    }
    fclose(op);

    op = fopen("motr.txt","w");
    fprintf(op,"motr_idx\tmotr_cidx\tmotr_spd\tmotr_rat\tmotr_rat\n");
    for (i=0; i<cinfo->devspec.motr_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\n",i,cinfo->devspec.motr[i].cidx,cinfo->devspec.motr[i].max,cinfo->devspec.motr[i].rat,cinfo->devspec.motr[i].spd);
    }
    fclose(op);

    op = fopen("swch.txt","w");
    fprintf(op,"swch_idx\tswch_cidx\n");
    for (i=0; i<cinfo->devspec.swch_cnt; i++)
    {
        iretn = fscanf(op,"%*d\t%hu\n",&cinfo->devspec.swch[i].cidx);
        if (iretn < 1)
        {
            break;
        }
    }
    fclose(op);

    op = fopen("thst.txt","w");
    fprintf(op,"thst_idx\tthst_cidx\tthst_idx\tthst_isp\tthst_flw\n");
    for (i=0; i<cinfo->devspec.thst_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\n",i,cinfo->devspec.thst[i].cidx,cinfo->devspec.thst[i].isp,cinfo->devspec.thst[i].flw);
    }
    fclose(op);

    op = fopen("prop.txt","w");
    fprintf(op,"prop_idx\tprop_cidx\tprop_cap\tprop_lev\n");
    for (i=0; i<cinfo->devspec.prop_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\n",i,cinfo->devspec.prop[i].cidx,cinfo->devspec.prop[i].cap,cinfo->devspec.prop[i].lev);
    }
    fclose(op);

    op = fopen("xyzsen.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tDirectionx\tDirectiony\tDirectionz\n");
    for (i=0; i<cinfo->devspec.xyzsen_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\n",i,cinfo->devspec.xyzsen[i].cidx,cinfo->devspec.xyzsen[i].direction.col[0],cinfo->devspec.xyzsen[i].direction.col[1],cinfo->devspec.xyzsen[i].direction.col[2]);
    }
    fclose(op);
}


//! Load Track list
/*! Load the file target.ini into an array of ::targetstruc. Space for the array is automatically allocated
 * and the number of items returned.

    \param cinfo Reference to ::cosmosstruc to use.
 *	\return Number of items loaded.
*/

int32_t load_target(cosmosstruc *cinfo)
{
    FILE *op;
    string fname;
    char inb[JSON_MAX_DATA];
    uint16_t count;

    fname = get_nodedir(cinfo->node.name) + "/target.ini";
    count = 0;
    if ((op=fopen(fname.c_str(),"r")) != nullptr)
    {
        //JIMNOTE:  take away this resize
        //EJPNOTE: keep the resize, it is cleaned up later
        cinfo->target.resize(MAX_NUMBER_OF_TARGETS);
        while (count < cinfo->target.size() && fgets(inb,JSON_MAX_DATA,op) != nullptr)
        {
            json_addentry("target_range",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,range)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_close",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,close)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_utc",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,utc)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_name",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,name)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_STRING, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_type",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,type)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_UINT16, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_azfrom",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,azfrom)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_azto",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,azto)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_elfrom",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,elfrom)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_elto",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,elto)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_min",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,min)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_size",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,size)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_LOCSTRUC, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_loc",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_LOCSTRUC, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_loc_pos_geod",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc.pos.geod)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_POS_GEOD, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            json_addentry("target_loc_pos_eci",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc.pos.eci)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_POS_ECI, (uint16_t)JSON_STRUCT_TARGET, cinfo);
            if (json_parse(inb, cinfo) >= 0)
            {
                if (cinfo->target[count].loc.utc == 0.)
                {
                    cinfo->target[count].loc.utc = currentmjd(cinfo->node.utcoffset);
                }
                // This may cause problems, but location information won't be complete without it
                loc_update(&cinfo->target[count].loc);
                ++count;
            }
        }
        fclose(op);
        //JIMNOTE:  take away this resize
        cinfo->target.resize(count);
        return (count);
    }
    else
        return 0;
}

//! Update Track list
/*! For each entry in the Track list, calculate the azimuth, elevation and range to and
 *from the current base location.
    \param cinfo Reference to ::cosmosstruc to use.
 *	\return 0, otherwise negative error.
 */
int32_t update_target(cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    //    rvector topo, dv, ds;
    for (uint32_t i=0; i<cinfo->target.size(); ++i)
    {
        iretn = update_target(cinfo->target[i].loc, cinfo->target[i]);
        //        loc_update(&cinfo->target[i].loc);
        //        Convert::geoc2topo(cinfo->target[i].loc.pos.geod.s,cinfo->node.loc.pos.geoc.s,topo);
        //        Convert::topo2azel(topo, cinfo->target[i].azto, cinfo->target[i].elto);
        //        Convert::geoc2topo(cinfo->node.loc.pos.geod.s,cinfo->target[i].loc.pos.geoc.s,topo);
        //        Convert::topo2azel(topo, cinfo->target[i].azfrom, cinfo->target[i].elfrom);
        //        ds = rv_sub(cinfo->target[i].loc.pos.geoc.s,cinfo->node.loc.pos.geoc.s);
        //        cinfo->target[i].range = length_rv(ds);
        //        dv = rv_sub(cinfo->target[i].loc.pos.geoc.v,cinfo->node.loc.pos.geoc.v);
        //        cinfo->target[i].close = length_rv(rv_sub(ds,dv)) - length_rv(ds);
    }
    return iretn;
}

int32_t update_target(Convert::locstruc source, targetstruc &target)
{
    rvector topo, dv, ds;

    Convert::loc_update(&target.loc);
    Convert::geoc2topo(target.loc.pos.geod.s, source.pos.geoc.s,topo);
    Convert::topo2azel(topo, target.azto, target.elto);
    Convert::geoc2topo(source.pos.geod.s, target.loc.pos.geoc.s, topo);
    Convert::topo2azel(topo, target.azfrom, target.elfrom);
    // Calculate direct vector from source to target
    ds = rv_sub(target.loc.pos.geoc.s, source.pos.geoc.s);
    target.range = length_rv(ds);
    // Calculate velocity of target WRT source
    dv = rv_sub(target.loc.pos.geoc.v, source.pos.geoc.v);
    // Closing speed is length of ds in 1 second minus length of ds now.
    target.close = length_rv(rv_sub(ds,dv)) - length_rv(ds);
    target.utc = target.loc.utc;
    return 0;
}

//! Load Event Dictionary
/*! Read a specific event dictionary for a specific node. The
* dictionary is stored as multiple condition based JSON event strings
* in a file of the specified name, in the cinfo directory of the specified node.
* The result is a vector of event structures.
 *	\param dict Reference to vector of ::eventstruc.

    \param cinfo Reference to ::cosmosstruc to use.
 *	\param file Name of dictionary file.
 *	\return Number of items loaded.
*/
size_t load_dictionary(vector<eventstruc> &dict, cosmosstruc *cinfo, const char *file)
{
    FILE *op;
    char inb[JSON_MAX_DATA];
    eventstruc event;
    jsonhandle handle;
    int32_t iretn = 0;

    string fname = (get_nodedir(cinfo->node.name) + "/") + file;
    if ((op=fopen(fname.c_str(),"r")) != nullptr)
    {
        while (fgets(inb,JSON_MAX_DATA,op) != nullptr)
        {
            json_clear_cosmosstruc(JSON_STRUCT_EVENT, cinfo);
            if (json_parse(inb, cinfo) >= 0)
            {
                if ((iretn=json_equation_map(cinfo->event[0].condition, cinfo, &handle)) < 0)
                    continue;
                event = cinfo->event[0];
                event.utcexec = 0.;
                event.handle = handle;
                dict.push_back(event);
            }
        }
        fclose(op);
        dict.shrink_to_fit();
        return (dict.size());
    }
    else
        return 0;
}

//! Calculate current Events.
/*! Using the provided Event Dictionary, calculate all the Events for
* current state of the provided Node. The Event Dictionary will be
* updated with any new values so that repeating Events can be properly
* assessed.
*	\param dictionary Reference to vector of ::eventstruc representing event dictionary.
*	\param cmeta Reference to ::cosmosstruc to use.
*	\param cinfo Reference to ::cosmosstruc to use.
*	\param events Reference to vector of ::eventstruc representing events.
*	\return Number of events created.
*/
size_t calc_events(vector<eventstruc> &dictionary, cosmosstruc *cinfo, vector<eventstruc> &events)
{
    double value;
//    char *sptr;
//    char *eptr;
    size_t sptr;
    size_t eptr;

    events.resize(0);
    for (uint32_t k=0; k<dictionary.size(); ++k)
    {
        if (!std::isnan(value=json_equation(&dictionary[k].handle, cinfo)) && value != 0. && dictionary[k].value == 0.)
        {
            dictionary[k].utc = cinfo->node.loc.utc;
            events.push_back(dictionary[k]);
            string tstring = json_get_string(dictionary[k].data, cinfo);
            //            strcpy(events[events.size()-1].data, tstring.c_str());
            events[events.size()-1].data = tstring;
            events[events.size()-1].node = cinfo->node.name;
            if ((sptr=events[events.size()-1].name.find("${")) != string::npos && (eptr=events[events.size()-1].name.find("}", sptr)) != string::npos)
            {
                tstring = json_get_string(events[events.size()-1].name.substr(sptr+2, (eptr-sptr)-2), cinfo);
                events[events.size()-1].name.replace(sptr, string::npos, tstring);
            }
//            if ((sptr=strstr(events[events.size()-1].name.c_str(),"${")) != nullptr && (eptr=strstr(sptr,"}")) != nullptr)
//            {
//                *eptr = 0;
//                tstring = json_get_string(sptr+2, cinfo);
//                strcpy(sptr, tstring.c_str());
//            }
        }
        dictionary[k].value = value;
    }

    events.shrink_to_fit();
    return (events.size());
}

uint16_t device_type_index(string name)
{
    for (size_t i=0; i<device_type_string.size(); ++i)
    {
        if (device_type_string[i] == name)
        {
            return i;
        }
    }

    return DeviceType::NONE;
}

string device_type_name(uint32_t type)
{
    string result;
    if (device_type_string.size() < DeviceType::COUNT)
    {
        device_type_string.resize(DeviceType::COUNT);
        for (size_t i=0; i<DeviceType::COUNT; ++i)
        {
            if (device_type_string[i] == "")
            {
                device_type_string[i] = "unknown";
            }
        }
    }

    if (type < DeviceType::COUNT)
    {
        result =  device_type_string[type];
    }

    return result;
}

::std::ostream& operator<<(::std::ostream& out, const beatstruc& b)	{
    return out<<std::fixed<<std::setprecision(9)
             <<"\tutc\t\t"<<b.utc<<endl
            <<"\tnode name \t<"<<string(b.node)<<">"<<endl
           <<"\tagent name \t<"<<string(b.proc)<<">"<<endl
          <<"\tntype\t\t"<<(int)b.ntype<<endl
         <<"\taddress \t<"<<string(b.addr)<<">"<<endl
        <<"\tport\t\t"<<b.port<<endl
       <<"\tbuffer\t\t"<<b.bsz<<endl
      <<"\tperiod\t\t"<<b.bprd<<endl
     <<"\tuser \t\t<"<<string(b.user)<<">"<<endl
    <<"\tcpu %\t\t"<<b.cpu<<endl
    <<"\tmem %\t\t"<<b.memory<<endl
    <<"\tjitter\t\t"<<b.jitter<<endl
    <<"\tdcycle\t\t"<<b.dcycle<<endl
    <<"\texits\t\t"<<b.exists<<endl;
}

//! @}


int32_t device_index(cosmosstruc *cinfo, string name)
{
    int32_t pidx = json_findpiece(cinfo, name);
    if(pidx < 0 ) {
        return pidx;
    }
    int32_t cindex = cinfo->pieces[pidx].cidx;
    int32_t dindex = cinfo->device[cindex]->didx;
    return dindex;
}

int32_t json_set_number(double val, jsonentry *entry, cosmosstruc *cinfo)
{
    uint8_t *data;
    int32_t iretn = 0;

    data = json_ptr_of_entry(*entry, cinfo);

    switch (entry->type)
    {
    case JSON_TYPE_UINT8:
        *(uint8_t *)data = (uint8_t)val;
        break;
    case JSON_TYPE_INT8:
        *(int8_t *)data = (int8_t)val;
        break;
    case JSON_TYPE_UINT16:
        *(uint16_t *)data = (uint16_t)val;
        break;
    case JSON_TYPE_UINT32:
        *(uint32_t *)data = (uint32_t)val;
        break;
    case JSON_TYPE_INT16:
        *(int16_t *)data = (int16_t)val;
        break;
    case JSON_TYPE_INT32:
        *(int32_t *)data = (int32_t)val;
        break;
    case JSON_TYPE_FLOAT:
        *(float *)data = (float)val;
        break;
    case JSON_TYPE_TIMESTAMP:
    case JSON_TYPE_DOUBLE:
        *(double *)data = (double)val;
        break;
    }
    return iretn;
}

bool device_has_property(uint16_t deviceType, string prop)
{
    devicestruc e;
    json11::Json json = e.to_json();
    if(!json[prop].is_null()) return true;
    switch(deviceType){
    case DeviceType::PLOAD:
        {
            ploadstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::SSEN:
        {
            ssenstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::IMU:
        {
            imustruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::RW:
        {
            rwstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::MTR:
        {
            mtrstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::CPU:
        {
            cpustruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::GPS:
        {
            gpsstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::ANT:
        {
            antstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::RXR:
        {
            rxrstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::TXR:
        {
            txrstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::TCV:
        {
            tcvstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::PVSTRG:
        {
            pvstrgstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::BATT:
        {
            battstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::HTR:
        {
            htrstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::MOTR:
        {
            motrstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::TSEN:
        {
            tsenstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::THST:
        {
            thststruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::PROP:
        {
            propstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::SWCH:
        {
            swchstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::ROT:
        {
            rotstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::STT:
        {
            sttstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::MCC:
        {
            mccstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::TCU:
        {
            tcustruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::BUS:
        {
            busstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::PSEN:
        {
            psenstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::SUCHI:
        {
            suchistruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::CAM:
        {
            camstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::TELEM:
        {
            telemstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::DISK:
        {
            diskstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::TNC:
        {
            tncstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::BCREG:
        {
            bcregstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    case DeviceType::XYZSEN:
        {
            xyzsenstruc e;
            json = e.to_json();
            if(!json[prop].is_null()) return true;
            break;
        }
    default:
        return false;
        break;

    }
    return false;
}

string json_memory_usage()
{
    string output;
    struct mallinfo m = mallinfo();
    output += to_label("arena", m.arena) + "\n";
    output += to_label("ordblks", m.ordblks) + "\n";
    output += to_label("smblks", m.smblks) + "\n";
    output += to_label("hblks" ,m.hblks) + "\n";
    output += to_label("hblkhd", m.hblkhd) + "\n";
    output += to_label("usmblks", m.usmblks) + "\n";
    output += to_label("fsmblks", m.fsmblks) + "\n";
    output += to_label("uordblks", m.uordblks) + "\n";
    output += to_label("fordblks", m.fordblks);
    return output;
}

//! Get vector of Node structures.
/*! Scan the COSMOS root directory and return a ::cosmosstruc for each
 * Node that is found.
 * \param node Vector of ::cosmosstruc for each Node.
 * \return Zero or negative error.
 */
int32_t json_get_nodes(vector<cosmosstruc> &node)
{
    DIR *jdp;
    string dtemp;
    string rootd;
    struct dirent *td;
    cosmosstruc *tnode;

    int32_t iretn = get_cosmosnodes(rootd);
    if (iretn < 0)
    {
        return iretn;
    }

    if ((tnode=json_init()) == nullptr)
    {
        return (NODE_ERROR_NODE);
    }

    dtemp = rootd;
    if ((jdp=opendir(dtemp.c_str())) != nullptr)
    {
        while ((td=readdir(jdp)) != nullptr)
        {
            if (td->d_name[0] != '.')
            {
                string nodepath = td->d_name;
                if (!json_setup_node(nodepath, tnode))
                {
                    node.push_back(*tnode);
                }
            }
        }
        closedir(jdp);
    }
    return 0;
}

//! Add to KML path
/*! Write a KML file to keep track of the path the node is following. Create the file if it doesn't alreay exist.
             * Append to it if it already exists.
             \param cinfo Pointer to ::cosmosstruc to use.
             \return 0, otherwise negative error.
            */
int32_t kml_write(cosmosstruc *cinfo)
{
    char buf[500];
    FILE *fin, *fout;
    double utc;

    utc = floor(cinfo->node.loc.utc);

    string path = data_type_path(cinfo->node.name, "outgoing", "google", utc, "points");
    fin = data_open(path, (char *)"a+");
    fprintf(fin,"%.5f,%.5f,%.5f\n",DEGOF(cinfo->node.loc.pos.geod.s.lon),DEGOF(cinfo->node.loc.pos.geod.s.lat),cinfo->node.loc.pos.geod.s.h);

    path = data_type_path(cinfo->node.name, "outgoing", "google",  utc, "kml");
    fout = data_open(path, (char *)"w");
    fprintf(fout,"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
    fprintf(fout,"<Document>\n");
    fprintf(fout,"<name>%s JD%5.0f</name>\n",cinfo->node.name.c_str(),utc);
    fprintf(fout,"<description>Track of node.</description>\n");
    fprintf(fout,"<Style id=\"yellowLineGreenPoly\">\n<LineStyle>\n<color>7f00ffff</color>\n<width>4</width>\n</LineStyle>\n");
    fprintf(fout,"<PolyStyle>\n<color>7f00ff00</color>\n</PolyStyle>\n</Style>\n");
    fprintf(fout,"<Placemark>\n<name>Node Path</name>\n<description>%s JD%5.0f</description>\n",cinfo->node.name.c_str(),utc);
    fprintf(fout,"<styleUrl>#yellowLineGreenPoly</styleUrl>\n<LineString>\n<extrude>1</extrude>\n<tessellate>1</tessellate>\n<altitudeMode>absolute</altitudeMode>\n");
    fprintf(fout,"<coordinates>\n");

    rewind (fin);
    while (fgets(buf, 500, fin) != nullptr)
    {
        fputs(buf, fout);
    }
    fclose(fin);

    fprintf(fout,"</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n");
    fclose(fout);

    return 0;
}
