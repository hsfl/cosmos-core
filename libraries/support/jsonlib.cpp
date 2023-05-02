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

    cinfo->emap.resize(JSON_MAX_HASH);

    //    cinfo->node.name.reserve(COSMOS_MAX_NAME+1);
    //    cinfo->node.agent.reserve(COSMOS_MAX_NAME+1);
    //    cinfo->node.lastevent.reserve(COSMOS_MAX_NAME+1);

    // TODO: enforce this maximum for all vert/tri.push_back calls
    cinfo->node.phys.vertices.clear();
    //    cinfo->node.phys.vertices.reserve(MAX_NUMBER_OF_VERTICES);
    cinfo->node.phys.triangles.clear();

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
        iretn = json_adddevice(cinfo, static_cast <uint16_t>(cinfo->pieces.size()), ctype);
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

//! Add new device
//! Take an empty ::devicestruc and fill it with the provided information.
//! Because the devspec push_back may (and does) cause devspec
//! to be reallocated, the previous entries in device for that particular DeviceType
//! no longer point to valid addresses and need to be re-pointed correctly.
//! However, this function is called in json_addpiece(), which is called by
//! json_createpiece(), which calls json_mapdeviceentry(), which uses the addresses
//! that may/will become invalidated to create jmap entries.
//! For running json_dump_node(), this isn't an issue because that function
//! works off the jmap entries but doesn't use the stored pointers.
//! Please call json_updatecosmosstruc() to fix this problem.
//!  \param type JSON_DEVICE_TYPE
//!  \return Zero, or negative error
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
    // TODO: fix
    // int32_t iretn = 0;
    // jsonhandle handle;
    // //    uint16_t count = 0;
    // // Add this alias only if it is not already in the map
    // if ((iretn = json_name_map(alias, cinfo, handle)))
    // {
    //     jsonentry tentry;
    //     tentry.name = alias;
    //     aliasstruc talias;
    //     talias.name = alias;
    //     string val_added = value;

    //     switch (value[0])
    //     {
    //     case '0':
    //     case '1':
    //     case '2':
    //     case '3':
    //     case '4':
    //     case '5':
    //     case '6':
    //     case '7':
    //     case '8':
    //     case '9':
    //     case '+':
    //     case '-':
    //     case '.':
    //     case '%':
    //         // Add a constant as an equation multiplied by 1 (kind of hack-ish).
    //         val_added = "(" + value + "*1.)";

    //         // If it begins with ( then it is an equation, otherwise treat as name
    //     case '(':
    //         // Add new equation
    //         iretn = json_equation_map(val_added, cinfo, &handle);
    //         if (iretn < 0)
    //         {
    //             return iretn;
    //         }
    //         talias.handle = handle;
    //         talias.type = JSON_TYPE_EQUATION;
    //         break;

    //         //        case '"': /// TODO: Aliases for just namespace members are not working.
    //         //            // Strip the quotes away.
    //         //            val_added.clear();
    //         //            for (size_t i = 0; i < value.size(); ++i) {
    //         //                if (value[i] != '"') { val_added += value[i]; }
    //         //            }
    //         //        default:
    //         //            // It is a Namespace name which should only be added if it is in the map
    //         //            if ((iretn = json_name_map(val_added, cinfo, handle)))
    //         //            {
    //         //                return iretn;
    //         //            }
    //         //            // Add new alias
    //         //            talias.handle = handle;
    //         //            talias.type = cinfo->jmap[handle.hash][handle.index].type;
    //         //            break;
    //     default:
    //         return GENERAL_ERROR_UNIMPLEMENTED;
    //     }
    //     // Place it in the Alias vector and point to it in the map
    //     cinfo->alias.push_back(talias);
    //     tentry.type = JSON_TYPE_ALIAS;
    //     tentry.group = JSON_STRUCT_ALIAS;
    //     //tentry.group = JSON_STRUCT_PTR; // Unsure why this change was made...
    //     tentry.offset = cinfo->alias.size() - 1;
    //     tentry.ptr = (uint8_t *)&cinfo->alias[tentry.offset];
    //     iretn = json_addentry(tentry, cinfo);
    //     if (iretn < 0)
    //     {
    //         return iretn;
    //     }
    // }
    // return cinfo->jmapped;
    return 0;
}

/*! Add an entry to the COSMOS Namespace map.
 * Enters the information associating a pointer with an entry in the name table.
 * The name will be of form "name" if it is a scalar, "name_iii" if it is a first level array,
 * "name_iii_iii" if it is second level, where "iii" is the zero filled index for the appropriate level.
    \param name Variable name from the JSON Data Name Space
    \param d1 Array index for first level, otherwise -1
    \param d2 Array index for second level, otherwise -1
    \param ptr Pointer to the data to map.
    \param type COSMOS JSON Data Type.
    \return int32_t 0
*/
int32_t json_addentry(string name, uint16_t d1, uint16_t d2, uint8_t* ptr, uint16_t type, cosmosstruc *cinfo)
{
    // TODO: fix
    // char ename[COSMOS_MAX_NAME+1];

    // // Determine extended name
    // strcpy(ename,name.c_str());
    // if (d1 < UINT16_MAX)
    //     sprintf(&ename[strlen(ename)],"_%03u",d1);
    // if (d2 < UINT16_MAX)
    //     sprintf(&ename[strlen(ename)],"_%03u",d2);

    // cinfo->add_name(ename, ptr, type);

    return 0;
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

    // TODO: fix
    // dptr = json_ptr_of_entry(entry, cinfo);
    // if (dptr == nullptr)
    // {
    //     return 0;
    // }
    // else
    // {
    //     switch (entry.type)
    //     {
    //     case JSON_TYPE_UINT16:
    //         value = (int32_t)(*(uint16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_UINT32:
    //         value = (int32_t)(*(uint32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT16:
    //         value = (int32_t)(*(int16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT32:
    //         value = (int32_t)(*(int32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_FLOAT:
    //         value = (int32_t)(*(float *)(dptr));
    //         break;
    //     case JSON_TYPE_DOUBLE:
    //     case JSON_TYPE_TIMESTAMP:
    //         value = (int32_t)(*(double *)(dptr) + .5);
    //         break;
    //     case JSON_TYPE_STRING:
    //         value = stoi(*(string *)(dptr));
    //         break;
    //     case JSON_TYPE_EQUATION:
    //         {
    //             const char *tpointer = (char *)dptr;
    //             value = (int32_t)json_equation(tpointer, cinfo);
    //         }
    //         break;
    //     case JSON_TYPE_ALIAS:
    //         {
    //             jsonentry *eptr;
    //             if ((eptr=json_entry_of((*(jsonhandle *)(dptr)), cinfo)) == nullptr)
    //             {
    //                 value =  0;
    //             }
    //             else
    //             {
    //                 value = json_get_int(*eptr, cinfo);
    //             }
    //         }
    //         break;
    //     }
    //     return value;
    // }
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

    // TODO: fix
    // if (!std::isnan(value=json_equation(tokenp, cinfo)))
    //     return (value);

    // if ((entry=json_entry_of(token, cinfo)) == nullptr)
    // {
    //     return (NAN);
    // }

    // if (!std::isnan(value=json_get_double(*entry, cinfo)))
    //     return (value);

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

    // TODO: fix
    // dptr = json_ptr_of_entry(entry, cinfo);
    // if (dptr == nullptr)
    // {
    //     return 0.;
    // }
    // else
    // {
    //     switch (entry.type)
    //     {
    //     case JSON_TYPE_UINT16:
    //         value = (double)(*(uint16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_UINT32:
    //         value = (double)(*(uint32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT16:
    //         value = (double)(*(int16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT32:
    //         value = (double)(*(int32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_FLOAT:
    //         value = (double)(*(float *)(dptr));
    //         break;
    //     case JSON_TYPE_DOUBLE:
    //     case JSON_TYPE_TIMESTAMP:
    //         value = (double)(*(double *)(dptr));
    //         break;
    //     case JSON_TYPE_STRING:
    //         value = stof(*(string *)dptr);
    //         break;
    //     case JSON_TYPE_EQUATION:
    //         {
    //             const char *tpointer = (char *)dptr;
    //             value = (double)json_equation(tpointer, cinfo);
    //         }
    //         break;
    //     case JSON_TYPE_ALIAS:
    //         {
    //             aliasstruc *aptr = (aliasstruc *)dptr;
    //             switch (aptr->type)
    //             {
    //             case JSON_TYPE_EQUATION:
    //                 {
    //                     jsonequation *eptr;
    //                     if ((eptr=json_equation_of(aptr->handle, cinfo)) == nullptr)
    //                     {
    //                         value =  0;
    //                     }
    //                     else
    //                     {
    //                         value = json_equation(eptr, cinfo);
    //                     }
    //                 }
    //                 break;
    //             default:
    //                 {
    //                     jsonentry *eptr;
    //                     if ((eptr=json_entry_of(aptr->handle, cinfo)) == nullptr)
    //                     {
    //                         value =  0;
    //                     }
    //                     else
    //                     {
    //                         value = json_get_double(*eptr, cinfo);
    //                     }
    //                 }
    //                 break;
    //             }
    //         }
    //         break;
    //     default:
    //         value = 0;
    //     }

    //     return value;
    // }
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

    // TODO: fix
    // dptr = json_ptr_of_entry(entry, cinfo);
    // if (dptr == nullptr)
    // {
    //     return value;
    // }
    // else
    // {
    //     switch (entry.type)
    //     {
    //     case JSON_TYPE_SPHERPOS:
    //     case JSON_TYPE_POS_GEOS:
    //         {
    //             Convert::spherpos tpos = (Convert::spherpos)(*(Convert::spherpos *)(dptr));
    //             value.col[0] = tpos.s.phi;
    //             value.col[1] = tpos.s.lambda;
    //             value.col[2] = tpos.s.r;
    //         }
    //         break;
    //     case JSON_TYPE_GEOIDPOS:
    //     case JSON_TYPE_POS_GEOD:
    //     case JSON_TYPE_POS_SELG:
    //         {
    //             Convert::geoidpos tpos = (Convert::geoidpos)(*(Convert::geoidpos *)(dptr));
    //             value.col[0] = tpos.s.lat;
    //             value.col[1] = tpos.s.lon;
    //             value.col[2] = tpos.s.h;
    //         }
    //         break;
    //     case JSON_TYPE_CARTPOS:
    //     case JSON_TYPE_POS_GEOC:
    //     case JSON_TYPE_POS_SELC:
    //     case JSON_TYPE_POS_ECI:
    //     case JSON_TYPE_POS_SCI:
    //     case JSON_TYPE_POS_ICRF:
    //         {
    //             Convert::cartpos tpos = (Convert::cartpos)(*(Convert::cartpos *)(dptr));
    //             value = tpos.s;
    //         }
    //         break;
    //     case JSON_TYPE_VECTOR:
    //     case JSON_TYPE_RVECTOR:
    //         //        case JSON_TYPE_TVECTOR:
    //     case JSON_TYPE_CVECTOR:
    //     case JSON_TYPE_SVECTOR:
    //     case JSON_TYPE_AVECTOR:
    //         value = (rvector)(*(rvector *)(dptr));
    //         break;
    //     case JSON_TYPE_UINT16:
    //         value.col[0] = (double)(*(uint16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_UINT32:
    //         value.col[0] = (double)(*(uint32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT16:
    //         value.col[0] = (double)(*(int16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT32:
    //         value.col[0] = (double)(*(int32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_FLOAT:
    //         value.col[0] = (double)(*(float *)(dptr));
    //         break;
    //     case JSON_TYPE_DOUBLE:
    //     case JSON_TYPE_TIMESTAMP:
    //         value.col[0] = (double)(*(double *)(dptr));
    //         break;
    //     case JSON_TYPE_STRING:
    //         value.col[0] = stof(*(string *)dptr);
    //         break;
    //     case JSON_TYPE_EQUATION:
    //         {
    //             const char *tpointer = (char *)dptr;
    //             value.col[0] = (double)json_equation(tpointer, cinfo);
    //         }
    //         break;
    //     case JSON_TYPE_ALIAS:
    //         {
    //             aliasstruc *aptr = (aliasstruc *)dptr;
    //             switch (aptr->type)
    //             {
    //             case JSON_TYPE_EQUATION:
    //                 {
    //                     jsonequation *eptr;
    //                     if ((eptr=json_equation_of(aptr->handle, cinfo)) == nullptr)
    //                     {
    //                         value.col[0] =  0;
    //                     }
    //                     else
    //                     {
    //                         value.col[0] = json_equation(eptr, cinfo);
    //                     }
    //                 }
    //                 break;
    //             default:
    //                 {
    //                     jsonentry *eptr;
    //                     if ((eptr=json_entry_of(aptr->handle, cinfo)) == nullptr)
    //                     {
    //                         value.col[0] =  0;
    //                     }
    //                     else
    //                     {
    //                         value.col[0] = json_get_double(*eptr, cinfo);
    //                     }
    //                 }
    //                 break;
    //             }
    //         }
    //         break;
    //     default:
    //         value.col[0] = 0.;
    //     }

    //     return value;
    // }
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

    // TODO: fix
    // dptr = json_ptr_of_entry(entry, cinfo);
    if (dptr == nullptr)
    {
    //     return value;
    // }
    // else
    // {
    //     switch (entry.type)
    //     {
    //     case JSON_TYPE_QATT:
    //     case JSON_TYPE_QATT_GEOC:
    //     case JSON_TYPE_QATT_SELC:
    //     case JSON_TYPE_QATT_ICRF:
    //     case JSON_TYPE_QATT_LVLH:
    //     case JSON_TYPE_QATT_TOPO:
    //         {
    //             value = (quaternion)(*(quaternion *)(dptr));
    //         }
    //         break;
    //     case JSON_TYPE_QUATERNION:
    //         value = (quaternion)(*(quaternion *)(dptr));
    //         break;
    //     case JSON_TYPE_RVECTOR:
    //         //        case JSON_TYPE_TVECTOR:
    //         {
    //             rvector tvalue = (rvector)(*(rvector *)(dptr));
    //             value.d.x = tvalue.col[0];
    //             value.d.y = tvalue.col[1];
    //             value.d.z = tvalue.col[2];
    //         }
    //         break;
    //     case JSON_TYPE_GVECTOR:
    //         {
    //             gvector tvalue = (gvector)(*(gvector *)(dptr));
    //             value.d.x = tvalue.lat;
    //             value.d.y = tvalue.lon;
    //             value.d.z = tvalue.h;
    //         }
    //         break;
    //     case JSON_TYPE_SVECTOR:
    //         {
    //             svector tvalue = (svector)(*(svector *)(dptr));
    //             value.d.x = tvalue.phi;
    //             value.d.y = tvalue.lambda;
    //             value.d.z = tvalue.r;
    //         }
    //         break;
    //     case JSON_TYPE_AVECTOR:
    //         {
    //             avector tvalue = (avector)(*(avector *)(dptr));
    //             value.d.x = tvalue.h;
    //             value.d.y = tvalue.e;
    //             value.d.z = tvalue.b;
    //         }
    //         break;
    //     case JSON_TYPE_UINT16:
    //         value.d.x = (double)(*(uint16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_UINT32:
    //         value.d.x = (double)(*(uint32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT16:
    //         value.d.x = (double)(*(int16_t *)(dptr));
    //         break;
    //     case JSON_TYPE_INT32:
    //         value.d.x = (double)(*(int32_t *)(dptr));
    //         break;
    //     case JSON_TYPE_FLOAT:
    //         value.d.x = (double)(*(float *)(dptr));
    //         break;
    //     case JSON_TYPE_DOUBLE:
    //     case JSON_TYPE_TIMESTAMP:
    //         value.d.x = (double)(*(double *)(dptr));
    //         break;
    //     case JSON_TYPE_STRING:
    //         value.d.x = stof(*(string *)dptr);
    //         break;
    //     case JSON_TYPE_EQUATION:
    //         {
    //             const char *tpointer = (char *)dptr;
    //             value.d.x = (double)json_equation(tpointer, cinfo);
    //         }
    //         break;
    //     case JSON_TYPE_ALIAS:
    //         {
    //             aliasstruc *aptr = (aliasstruc *)dptr;
    //             switch (aptr->type)
    //             {
    //             case JSON_TYPE_EQUATION:
    //                 {
    //                     jsonequation *eptr;
    //                     if ((eptr=json_equation_of(aptr->handle, cinfo)) == nullptr)
    //                     {
    //                         value.d.x =  0;
    //                     }
    //                     else
    //                     {
    //                         value.d.x = json_equation(eptr, cinfo);
    //                     }
    //                 }
    //                 break;
    //             default:
    //                 {
    //                     jsonentry *eptr;
    //                     if ((eptr=json_entry_of(aptr->handle, cinfo)) == nullptr)
    //                     {
    //                         value.d.x =  0;
    //                     }
    //                     else
    //                     {
    //                         value.d.x = json_get_double(*eptr, cinfo);
    //                     }
    //                 }
    //                 break;
    //             }
    //         }
    //         break;
    //     default:
    //         value.d.x = 0.;
    //     }

        return value;
    }
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
    // TODO: fix
    // if ((iretn=json_parse_equation(ptr, equation)) < 0) {
    //     return (NAN);
    // }

    // if (cinfo->emap.size() == 0) {
    //     return (NAN);
    // }

    // if ((iretn=json_equation_map(equation, cinfo, &handle)) < 0) {
    //     return (NAN);
    // }

    // return(json_equation(&cinfo->emap[handle.hash][handle.index], cinfo));
    return 0;
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
            // TODO: fix
            // a[i] = json_get_double(cinfo->jmap[ptr->operand[i].data.hash][ptr->operand[i].data.index], cinfo);
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
    // TODO: fix
    // if ((iretn=json_skip_white(ptr)) < 0)
    // {
    //     return(JSON_ERROR_SCAN);
    // }

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
    // TODO: fix
    // string tstring;
    // int32_t iretn = 0;

    // json_skip_white(ptr);
    // switch(ptr[0])
    // {
    // case '(':
    //     // It's an equation
    //     if ((iretn=json_parse_equation(ptr, tstring)) < 0)
    //     {
    //         return (JSON_ERROR_SCAN);
    //     }
    //     if ((iretn=json_equation_map(tstring, cinfo, &operand->data)) < 0)
    //     {
    //         return (JSON_ERROR_SCAN);
    //     }
    //     operand->type = JSON_OPERAND_EQUATION;
    //     break;
    // case '"':
    //     // It's a string
    //     if ((iretn=json_extract_string(ptr, tstring)) < 0)
    //     {
    //         return (JSON_ERROR_SCAN);
    //     }
    //     if ((iretn=json_name_map(tstring, cinfo, operand->data)) < 0)
    //     {
    //         return (JSON_ERROR_SCAN);
    //     }
    //     operand->type = JSON_OPERAND_NAME;
    //     break;
    // case '0':
    // case '1':
    // case '2':
    // case '3':
    // case '4':
    // case '5':
    // case '6':
    // case '7':
    // case '8':
    // case '9':
    // case '+':
    // case '-':
    // case '.':
    // case '%':
    //     // It's a constant
    //     operand->type = JSON_OPERAND_CONSTANT;
    //     if ((iretn=json_parse_number(ptr,&operand->value)) < 0)
    //         return (JSON_ERROR_SCAN);
    //     break;
    // default:
    //     operand->type = JSON_OPERAND_NULL;
    //     break;
    // }

    return 0;
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
        // TODO: fix, double-check, why is json_out_node necessary here?
        // json_out_node(json.node, node);
        json.node += ibuf;
        free(ibuf);
    }

    // 1A: load state vector TLE's, if present

//    fname = nodepath + "/state.ini";

//    if (!stat(fname.c_str(),&fstat) && fstat.st_size)
//    {
//        ifs.open(fname);
//        if (ifs.is_open())
//        {
//            ibuf = (char *)calloc(1,fstat.st_size+1);
//            ifs.read(ibuf, fstat.st_size);
//            ifs.close();
//            ibuf[fstat.st_size] = 0;
//            json.state = ibuf;
//            free(ibuf);
//        }
//    }
    // If not, use TLE if it is present
//    else {
//        fname = nodepath + "/state.tle";

//        if (!stat(fname.c_str(),&fstat) && fstat.st_size)
//        {
//            iretn = 0;
//            Convert::cartpos eci;
//            vector <Convert::tlestruc> tles;
//            iretn = load_lines(fname, tles);
//            if (iretn > 0)
//            {
//                if ((iretn=lines2eci(currentmjd()-10./1440., tles, eci)) == 0)
//                {
//                    json_out_ecipos(json.state, eci);
//                }
//            }
//        }
//    }

    // Set node_utcstart
    fname = nodepath + "/node_utcstart.dat";
    double utcstart;
    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        // First time, so write it
        utcstart = currentmjd();
        // printf("jsonlib: create utcstart %f\n", utcstart);
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
            // printf("jsonlib: fix utcstart %f\n", utcstart);
        }
        else
        {
            iretn = fscanf(ifp, "%lg", &utcstart);
            if (iretn != 1)
            {
                utcstart = currentmjd();
                // printf("jsonlib: read utcstart %f\n", utcstart);
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

//! Recalculates centroid, normal, and area for each face, then recenters all vectors to the node's center of mass
//! \param cinfo Pointer to the node's cosmosstruc
void rebase_node(cosmosstruc *cinfo)
{
    update_faces(cinfo->node.phys.vertices, cinfo->node.phys.faces);

    Vector tcom = update_pieces(cinfo);

    // Calculate special norm for external panels?
    // TODO: Check if the code omitted here from json_recenter_node() but was not in objlib.cpp is needed

    // Recenter all vectors to total Center of Mass
    for (vertexstruc &vertex : cinfo->node.phys.vertices)
    {
        vertex -= tcom;
    }
    for (facestruc &face : cinfo->node.phys.faces)
    {
        face.com -= tcom;
        face.normal -= tcom;
    }
    for (piecestruc &piece : cinfo->pieces)
    {
        piece.com -= tcom;
    }
}

//! Calculates centroid, normal, and area for each face
//! Called by rebase_node()
//! \param vertices Vector of vertices that the face has indexes to for its vertices
//! \param faces Vector of faces to recalculate statics for
void update_faces(const vector<Vector>& vertices, vector<facestruc>& faces)
{
    for (facestruc &face : faces)
    {
        if (face.vertex_idx.size() < 2)
        {
            continue;
        }

        Vector fcentroid = vertices[face.vertex_idx[0]];
        fcentroid += vertices[face.vertex_idx[1]];
        Vector v1 = vertices[face.vertex_idx[0]] - vertices[face.vertex_idx[face.vertex_idx.size()-1]];
        Vector v2 = vertices[face.vertex_idx[1]] - vertices[face.vertex_idx[0]];
        Vector fnormal = v1.cross(v2);
        for (size_t j=2; j<face.vertex_idx.size(); ++j)
        {
            fcentroid += vertices[face.vertex_idx[j]];
            v1 = v2;
            v2 = vertices[face.vertex_idx[j]] - vertices[face.vertex_idx[j-1]];
            fnormal += v1.cross(v2);
        }
        fcentroid /= face.vertex_idx.size();
        v1 = v2;
        v2 = vertices[face.vertex_idx[0]] - vertices[face.vertex_idx[face.vertex_idx.size()-1]];
        fnormal += v1.cross(v2);
        fnormal.normalize();
        face.normal = fnormal;

        face.com = Vector ();
        face.area = 0.;
        v1 = vertices[face.vertex_idx[face.vertex_idx.size()-1]] - fcentroid;
        for (size_t j=0; j<face.vertex_idx.size(); ++j)
        {
            v2 = vertices[face.vertex_idx[j]] - fcentroid;
            // Area of triangle made by v1, v2 and Face centroid
            double tarea = v1.area(v2);
            // Sum
            face.area += tarea;
            // Centroid of triangle made by v1, v2 amd Face centroid
            Vector tcentroid = (v1 + v2) / 3. + fcentroid;
            // Weighted sum
            // Vector test = tarea * tcentroid;
            face.com += tcentroid * tarea;
            v1 = v2;
        }
        // Divide by summed weights
        if (face.area)
        {
            face.com /= face.area;
        }
    }
}

//! Recalculate volume and center of mass for each piece
//! \param cinfo Pointer to node's cosmosstruc
//! \return Total center of mass
Vector update_pieces(cosmosstruc* cinfo)
{
    Vector tcom = Vector();
    double tvolume = 0.;
    for (size_t i=0; i<cinfo->pieces.size(); ++i)
    {
        // Clean up any missing faces and calculate center of mass for each Piece using Faces
        cinfo->pieces[i].com = Vector ();
        for (size_t j=0; j<cinfo->pieces[i].face_cnt; ++j)
        {
            if (cinfo->node.phys.faces.size() <= cinfo->pieces[i].face_idx[j])
            {
                cinfo->node.phys.faces.resize(cinfo->pieces[i].face_idx[j]+1);
                cinfo->node.phys.faces[cinfo->pieces[i].face_idx[j]].com = Vector ();
                cinfo->node.phys.faces[cinfo->pieces[i].face_idx[j]].area = 0.;
                cinfo->node.phys.faces[cinfo->pieces[i].face_idx[j]].normal = Vector ();
            }
            cinfo->pieces[i].com += cinfo->node.phys.faces[cinfo->pieces[i].face_idx[j]].com;
        }
        if (cinfo->pieces[i].face_cnt)
        {
            cinfo->pieces[i].com /= cinfo->pieces[i].face_cnt;
        }

        // Calculate volume for each Piece using center of mass for each Face
        cinfo->pieces[i].volume = 0.;
        for (size_t j=0; j<cinfo->pieces[i].face_cnt; ++j)
        {
            Vector dv = cinfo->node.phys.faces[(cinfo->pieces[i].face_idx[j])].com - cinfo->pieces[i].com;
            if (dv.norm() != 0.)
            {
                cinfo->pieces[i].volume += cinfo->node.phys.faces[(cinfo->pieces[i].face_idx[j])].area * dv.norm() / 3.;
            }
        }
        cinfo->node.face_cnt = cinfo->node.phys.faces.size();
        tvolume += cinfo->pieces[i].volume;
        tcom +=  cinfo->pieces[i].com * cinfo->pieces[i].volume;
    }
    if (tvolume)
    {
        tcom /= tvolume;
    }

    return tcom;
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

    rebase_node(cinfo);

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
                tpiece->shove = -tpiece->area * (cinfo->node.phys.faces[tpiece->face_idx[0]].normal.dot(tpiece->com)) * tpiece->com / (tpiece->com.norm() * tpiece->com.norm());
                tpiece->twist = -tpiece->area * tpiece->com.norm() * cinfo->node.phys.faces[tpiece->face_idx[0]].normal - tpiece->com.norm() * tpiece->shove;
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

int32_t json_updatecosmosstruc(cosmosstruc *cinfo)
{
    for (auto &dev : cinfo->devspec.ant)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.batt)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.bcreg)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.bus)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.cam)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.cpu)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.disk)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.gps)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.gyro)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.htr)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.imu)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.mag)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.mcc)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.motr)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.mtr)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.pload)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.prop)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.psen)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.pvstrg)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.rot)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.rw)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.rxr)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.ssen)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.stt)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.suchi)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.swch)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.tcu)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.tcv)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.telem)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.thst)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.tnc)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.tsen)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.txr)
    {
        cinfo->device[dev.cidx] = &dev;
    }
    for (auto &dev : cinfo->devspec.xyzsen)
    {
        cinfo->device[dev.cidx] = &dev;
    }

    cinfo->node.vertex_cnt = cinfo->node.phys.vertices.size();
    cinfo->node.face_cnt = cinfo->node.phys.faces.size();
    cinfo->node.piece_cnt = cinfo->pieces.size();
    cinfo->node.device_cnt = cinfo->device.size();
    cinfo->node.port_cnt = cinfo->port.size();
    cinfo->node.agent_cnt = cinfo->agent.size();
    cinfo->node.event_cnt = cinfo->event.size();
    cinfo->node.target_cnt = cinfo->target.size();
    cinfo->node.user_cnt = cinfo->user.size();
    cinfo->node.tle_cnt = cinfo->tle.size();

    cinfo->add_default_names();

    return 0;
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

    cinfo->node.utcoffset = 0.;
    // First: parse data for summary information - includes piece_cnt, device_cnt and port_cnt
    if (!json.node.empty())
    {
        cinfo->from_json(json.node);
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
        cinfo->from_json(json.state);
    }

    // Second: enter information for pieces
    // Vertices
    cinfo->node.phys.vertices.clear();
    if (cinfo->node.vertex_cnt)
    {
        // be careful about resizing vertex past MAX_NUMBER_VERTEXS
        cinfo->node.phys.vertices.resize(cinfo->node.vertex_cnt);
        if (cinfo->node.phys.vertices.size() != cinfo->node.vertex_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }

        // Parse data for vertex information
        if (!json.vertexs.empty())
        {
            cinfo->from_json(json.vertexs);
        }
    }


    // Faces
    cinfo->node.phys.faces.clear();
    if (cinfo->node.face_cnt)
    {
        cinfo->node.phys.faces.resize(cinfo->node.face_cnt);
        if (cinfo->node.phys.faces.size() != cinfo->node.face_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }

        // Parse data for face information
        if (!json.faces.empty())
        {
            cinfo->from_json(json.faces);
        }

        // Do it a second time because now we know how many vertices in each face.
        for (uint16_t i=0; i<cinfo->node.face_cnt; i++)
        {
            //Add relevant names to namespace
            cinfo->node.phys.faces[i].vertex_idx.resize(cinfo->node.phys.faces[i].vertex_cnt);
        }

        // Parse data for face information
        if (!json.faces.empty())
        {
            cinfo->from_json(json.faces);
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
            // Initialize to no component
            cinfo->pieces[i].cidx = DeviceType::NONE;
        }

        // Parse data for piece information
        if (!json.pieces.empty())
        {
            cinfo->from_json(json.pieces);
        }

        // Do it a second time, now that we know how many faces in each piece
        for (uint16_t i=0; i<cinfo->node.piece_cnt; i++)
        {
            cinfo->pieces[i].face_idx.resize(cinfo->pieces[i].face_cnt);
        }

        // Parse data for piece information
        if (!json.pieces.empty())
        {
            cinfo->from_json(json.pieces);
        }

        // Third: enter information for all devices
        // Aadd entries to map for devices
        // Add entries to map for Devices and set pointers in nodestruc for comp and devspec
        // Parse data for general device information
        if (!json.devgen.empty())
        {
            string error;
            json11::Json p = json11::Json::parse(json.devgen,error);
            if(error.empty())
            {
                
                // TODO: fix
                // Json devgen;
                // devgen.extract_contents(json.devgen);
                // Json::Object jobj = devgen.ObjectContents;
                // vector<size_t> pidx;
                // for (auto &dev : jobj)
                // {
                //     size_t cidx = stol(dev.first.substr(dev.first.find_last_of('_')+1));
                //     if (dev.first.find("all_pidx") != string::npos)
                //     {
                //         pidx.push_back(dev.second.nvalue);
                //     }
                //     else if (dev.first.find("all_type") != string::npos)
                //     {
                //         iretn = json_adddevice(cinfo, pidx[cidx], (DeviceType)(dev.second.nvalue));
                //     }
                // }

                // json_updatecosmosstruc(cinfo);

                // for (uint16_t i=0; i< cinfo->node.device_cnt; i++)
                // {
                //     // Initialize to disabled
                //     cinfo->device[i]->state = 0;
                //     // Initialize to no port
                //     cinfo->device[i]->portidx = PORT_TYPE_NONE;
                // }

                // if ((iretn = json_parse(json.devgen, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
                // {
                //     return iretn;
                // }
            }
            
        }

        // Fix any mis registered pieces
//         for (size_t i=0; i < cinfo->node.piece_cnt; ++i)
//         {
//             if (cinfo->pieces[i].cidx != UINT16_MAX)
//             {
//                 if (cinfo->pieces[i].cidx >= cinfo->device.size())
//                 {
//                     cinfo->pieces[i].cidx = UINT16_MAX;
//                 } else {
//                     cinfo->device[cinfo->pieces[i].cidx]->pidx = i;
//                 }
//             }
//         }

//         // Parse data for specific device information
//         if (!json.devspec.empty())
//         {
//             if ((iretn = json_parse(json.devspec, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
//             {
//                 return iretn;
//             }
//         }

//         // Clean up any errors and perform some initializations
//         for (uint16_t i=0; i< cinfo->node.device_cnt; i++)
//         {
//             cinfo->device[i]->cidx = i;
//             cinfo->device[i]->amp = cinfo->device[i]->namp;
//             cinfo->device[i]->volt = cinfo->device[i]->nvolt;
//         }

//         // Fifth: recover any state vector information
//         iretn = load_tle(cinfo);
// //        if (iretn < 0)
// //        {
// //            return iretn;
// //        }

//         // Sixth: enter information for ports
//         // Resize, then add names for ports
//         cinfo->port.resize(cinfo->node.port_cnt);
//         if (cinfo->port.size() != cinfo->node.port_cnt)
//         {
//             return (AGENT_ERROR_MEMORY);
//         }

//         // Parse data for port information
//         if (!json.ports.empty())
//         {
//             if ((iretn = json_parse(json.ports, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
//             {
//                 return iretn;
//             }
//         }

//         node_calc(cinfo);

//         //! Load targeting information
//         if (!json.targets.empty())
//         {
//             if ((iretn = json_parse(json.targets, cinfo)) < 0 && iretn != JSON_ERROR_EOS)
//             {
//                 return iretn;
//             }
//         }

//         //! Load alias map
//         fname = nodepath + "/aliases.ini";
//         if ((iretn=stat(fname.c_str(),&fstat)) == 0)
//         {
//             ifs.open(fname);
//             if (ifs.is_open())
//             {
//                 string alias;
//                 while (getline(ifs, alias, ' '))
//                 {
//                     string cname;
//                     getline(ifs, cname);
//                     json_addentry(alias, cname, cinfo);
//                 } ;
//             }
//         }

        // Fix all pointers and name mappings one last time
        json_updatecosmosstruc(cinfo);

    }

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
    string filename;
    //    json_mapentries(cinfo);
    json_updatecosmosstruc(cinfo);

    // Node
    string output = cinfo->get_json("node");
    string fileloc = get_nodedir(cinfo->node.name, true);
    if (fileloc.empty()) { return DATA_ERROR_NODES_FOLDER; }
    rename((fileloc+"/node.ini").c_str(), (fileloc+"/node.ini.old").c_str());
    //    string filename = fileloc + "/node.ini";
    FILE *file = fopen((fileloc+"/node.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Vertices
    output = cinfo->get_json("node.phys.vertices");
    rename((fileloc+"/vertices.ini").c_str(), (fileloc+"/vertices.ini.old").c_str());
    //    filename = fileloc + "/vertices.ini";
    file = fopen((fileloc + "/vertices.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // cinfo->faces
    output = cinfo->get_json("node.phys.faces");
    rename((fileloc+"/faces.ini").c_str(), (fileloc+"/faces.ini.old").c_str());
    //    filename = fileloc + "/faces.ini";
    file = fopen((fileloc + "/faces.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Pieces
    output = cinfo->get_json("node.phys.pieces");
    rename((fileloc+"/pieces.ini").c_str(), (fileloc+"/pieces.ini.old").c_str());
    //    filename = fileloc + "/pieces.ini";
    file = fopen((fileloc + "/pieces.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // General Devices
    output = cinfo->get_json("device");
    rename((fileloc+"/devices_general.ini").c_str(), (fileloc+"/devices_general.ini.old").c_str());
    //    filename = fileloc + "/devices_general.ini";
    file = fopen((fileloc + "/devices_general.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Specific Devices
    output = cinfo->get_json("devspec");
    rename((fileloc+"/devices_specific.ini").c_str(), (fileloc+"/devices_specific.ini.old").c_str());
    //    filename = fileloc + "/devices_specific.ini";
    file = fopen((fileloc + "/devices_specific.ini").c_str(), "w");
    if (file == nullptr) { return -errno; }
    fputs(output.c_str(), file);
    fclose(file);

    // Ports
    output = cinfo->get_json("port");
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

//! Get hash and index in JSON Equation map
/*! Using the provided text, find it's location in the provided Equation map and set the
 * values for the hash and index. If the equation is not already in the table, add it.
    \param equation Equation text.

    \param handle Pointer to ::jsonhandle of name.
    \return Zero, or negative error number.
*/
int32_t json_equation_map(string equation, cosmosstruc *cinfo, jsonhandle *handle)
{
    // TODO: fix
    // const char *pointer;
    // jsonequation tequation;
    // char ops[] = "+-*/%&|><=!~^@#";
    // int32_t iretn = 0;
    // //    size_t textlen;

    // // if (!cinfo->jmapped)
    // //     return (JSON_ERROR_NOJMAP);

    // if (cinfo->emap.size() == 0)
    //     return (JSON_ERROR_NOJMAP);

    // // Equations must start and end with ')'
    // if (equation[0] != '(' || equation[equation.length()-1] != ')')
    // {
    //     return (JSON_ERROR_SCAN);
    // }

    // handle->hash = json_hash(equation);

    // for (handle->index=0; handle->index<cinfo->emap[handle->hash].size(); ++handle->index)
    // {
    //     //        if (!strcmp(equation.c_str(), cinfo->emap[handle->hash][handle->index].text))
    //     if (equation == cinfo->emap[handle->hash][handle->index].text)
    //     {
    //         return 0;
    //     }
    // }

    // // Not found. We will have to add it to the map.
    // pointer = &equation[1];

    // // Extract first operand
    // if ((iretn=json_parse_operand(pointer, &tequation.operand[0], cinfo)) < 0)
    // {
    //     return (JSON_ERROR_SCAN);
    // }

    // // Extract operation
    // json_skip_white(pointer);
    // for (tequation.operation=0; tequation.operation<(int)strlen(ops); tequation.operation++)
    // {
    //     if ((pointer)[0] == ops[tequation.operation])
    //         break;
    // }
    // if (tequation.operation == (int)strlen(ops))
    //     return (JSON_ERROR_SCAN);
    // (pointer)++;

    // // Extract second argument
    // if ((iretn=json_parse_operand(pointer, &tequation.operand[1], cinfo)) < 0)
    // {
    //     return (JSON_ERROR_SCAN);
    // }

    // // Remove trailing )
    // while ((pointer)[0] != 0 && (pointer)[0] != ')')
    //     (pointer)++;
    // if ((pointer)[0] != 0)
    //     (pointer)++;

    // //    textlen = equation.size()+1;
    // //    if ((tequation.text = (char *)calloc(1,textlen)) == nullptr)
    // //    {
    // //        return (JSON_ERROR_SCAN);
    // //    }

    // // Populate the equation
    // //    strcpy(tequation.text,equation.c_str());
    // tequation.text = equation;

    // handle->index = (uint16_t)cinfo->emap[handle->hash].size();
    // cinfo->emap[handle->hash].push_back(tequation);
    // if (cinfo->emap[handle->hash].size() != handle->index+1u)
    // {
    //     //        free(tequation.text);
    //     return (JSON_ERROR_SCAN);
    // }

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

    vector<vertexstruc>(cinfo->node.phys.vertices).swap(cinfo->node.phys.vertices);

    for (size_t i=0; i<cinfo->node.phys.faces.size(); ++i)
    {
        cinfo->node.phys.faces[i].shrinkusage();
    }
    vector<facestruc>(cinfo->node.phys.faces).swap(cinfo->node.phys.faces);

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

    // TODO: fix

    // fname = get_nodedir(cinfo->node.name) + "/target.ini";
    // count = 0;
    // if ((op=fopen(fname.c_str(),"r")) != nullptr)
    // {
    //     //JIMNOTE:  take away this resize
    //     //EJPNOTE: keep the resize, it is cleaned up later
    //     cinfo->target.resize(MAX_NUMBER_OF_TARGETS);
    //     while (count < cinfo->target.size() && fgets(inb,JSON_MAX_DATA,op) != nullptr)
    //     {
    //         json_addentry("target_range",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,range)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_close",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,close)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_utc",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,utc)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_name",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,name)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_STRING, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_type",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,type)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_UINT16, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_azfrom",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,azfrom)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_azto",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,azto)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_elfrom",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,elfrom)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_elto",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,elto)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_min",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,min)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_size",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,size)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_LOCSTRUC, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_loc",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_LOCSTRUC, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_loc_pos_geod",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc.pos.geod)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_POS_GEOD, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         json_addentry("target_loc_pos_eci",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc.pos.eci)+count*sizeof(targetstruc), (uint16_t)JSON_TYPE_POS_ECI, (uint16_t)JSON_STRUCT_TARGET, cinfo);
    //         if (json_parse(inb, cinfo) >= 0)
    //         {
    //             if (cinfo->target[count].loc.utc == 0.)
    //             {
    //                 cinfo->target[count].loc.utc = currentmjd(cinfo->node.utcoffset);
    //             }
    //             // This may cause problems, but location information won't be complete without it
    //             loc_update(&cinfo->target[count].loc);
    //             ++count;
    //         }
    //     }
    //     fclose(op);
    //     //JIMNOTE:  take away this resize
    //     //EJPNOTE: keep the resize, when combined with the original resize it keeps the pointers from being changed
    //     cinfo->target.resize(count);
    //     return (count);
    // }
    // else
        return 0;
}

//! Load TLE list
/*! Load the file state.tle into an array of ::tlestruc. Space for the array is automatically allocated
 * and the number of items returned.

    \param cinfo Reference to ::cosmosstruc to use.
 *	\return Number of items loaded.
*/

int32_t load_tle(cosmosstruc *cinfo)
{
    int32_t iretn = 0;
    string fname;
    struct stat fstat;

    // TODO: fix
    // fname = get_nodedir(cinfo->node.name) + "/state.tle";
    // if (!stat(fname.c_str(),&fstat) && fstat.st_size)
    // {
    //     iretn = load_lines(fname, cinfo->tle);
    //     if (iretn <= 0)
    //     {
    //         return iretn;
    //     }
    //     if (iretn > MAX_NUMBER_OF_TLES)
    //     {
    //         cinfo->node.tle_cnt = MAX_NUMBER_OF_TLES;
    //     }
    //     else
    //     {
    //         cinfo->node.tle_cnt = iretn;
    //     }
    //     cinfo->tle.resize(cinfo->node.tle_cnt);
    //     for (uint16_t i=0; i<cinfo->node.tle_cnt; ++i)
    //     {
    //         json_addentry("tle_utc",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,utc)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_name",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,name)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_STRING, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_snumber",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,snumber)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_UINT16, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_id",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,id)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_STRING, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_bstar",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,bstar)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_i",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,i)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_raan",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,raan)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_e",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,e)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_ap",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,ap)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_ma",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,ma)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_mm",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,mm)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //         json_addentry("tle_orbit",i, UINT16_MAX, (ptrdiff_t)offsetof(Convert::tlestruc,orbit)+i*sizeof(Convert::tlestruc), (uint16_t)JSON_TYPE_UINT32, (uint16_t)JSON_STRUCT_TLE, cinfo);
    //     }
    //     return (cinfo->node.tle_cnt);
    // }
    return GENERAL_ERROR_BAD_FD;
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
        // TODO: fix
        // while (fgets(inb,JSON_MAX_DATA,op) != nullptr)
        // {
        //     json_clear_cosmosstruc(JSON_STRUCT_EVENT, cinfo);
        //     if (json_parse(inb, cinfo) >= 0)
        //     {
        //         if ((iretn=json_equation_map(cinfo->event[0].condition, cinfo, &handle)) < 0)
        //             continue;
        //         event = cinfo->event[0];
        //         event.utcexec = 0.;
        //         event.handle = handle;
        //         dict.push_back(event);
        //     }
        // }
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
        // TODO: fix
        // if (!std::isnan(value=json_equation(&dictionary[k].handle, cinfo)) && value != 0. && dictionary[k].value == 0.)
        // {
        //     dictionary[k].utc = cinfo->node.loc.utc;
        //     events.push_back(dictionary[k]);
        //     string tstring = json_get_string(dictionary[k].data, cinfo);
        //     //            strcpy(events[events.size()-1].data, tstring.c_str());
        //     events[events.size()-1].data = tstring;
        //     events[events.size()-1].node = cinfo->node.name;
        //     if ((sptr=events[events.size()-1].name.find("${")) != string::npos && (eptr=events[events.size()-1].name.find("}", sptr)) != string::npos)
        //     {
        //         tstring = json_get_string(events[events.size()-1].name.substr(sptr+2, (eptr-sptr)-2), cinfo);
        //         events[events.size()-1].name.replace(sptr, string::npos, tstring);
        //     }
        //     //            if ((sptr=strstr(events[events.size()-1].name.c_str(),"${")) != nullptr && (eptr=strstr(sptr,"}")) != nullptr)
        //     //            {
        //     //                *eptr = 0;
        //     //                tstring = json_get_string(sptr+2, cinfo);
        //     //                strcpy(sptr, tstring.c_str());
        //     //            }
        // }
        // dictionary[k].value = value;
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
