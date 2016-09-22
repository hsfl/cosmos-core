/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
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

#include "configCosmos.h"
#include "jsonlib.h"
#include "convertlib.h"
#include "timelib.h"
#include "ephemlib.h"

#include <sys/stat.h>
#include <iostream>
#include <limits>
#include <fstream>

std::vector <std::string> device_type_string
{
    "pload",
    "ssen",
    "imu",
    "rw",
    "mtr",
    "cpu",
    "gps",
    "ant",
    "rxr",
    "txr",
    "tcv",
    "strg",
    "batt",
    "htr",
    "motr",
    "tsen",
    "thst",
    "prop",
    "swch",
    "rot",
    "stt",
    "mcc",
    "tcu",
    "bus",
    "psen",
    "suchi",
    "cam",
    "telem",
    "disk"
};

std::vector <std::string> port_type_string
{
    "rs232",
    "rs422",
    "ethernet",
    "usb",
    "path",
    "loopback",
    "propagator"
};

/*! \ingroup jsonlib
* \defgroup jsonlib_namespace JSON Name Space
* @{
* A hierarchical set of variable names meant to describe everything in COSMOS. Each name
* maps to a variable of a type specified in \ref jsonlib_type. Names are restricted to
* alphabetic characters, and can be no longer than ::COSMOS_MAX_NAME. Once mapped through use
* of ::json_setup, these names will be tied to elements of the ::cosmosstruc.
*/

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

//! Initialize JSON pointer map
/*! Create a ::cosmosstruc and use it to assign storage for each of the groups and entries
 * for each of the non Node based elements to the JSON Name Map.
    \return Pointer to new ::cosmosstruc or nullptr.
*/
cosmosstruc *json_create()
{
    cosmosstruc *cinfo = nullptr;
    unitstruc tunit;

    if ((cinfo = new cosmosstruc) == nullptr)
    {
        return nullptr;
    }

    // Make sure it's clear
    //    memset(cinfo, 0, sizeof(cosmosstruc));


    cinfo->meta.jmapped = 0;
    cinfo->meta.unit.resize(JSON_UNIT_COUNT);
    //    cinfo->pdata.target.resize(100);
    cinfo->meta.jmap.resize(JSON_MAX_HASH);
    cinfo->meta.emap.resize(JSON_MAX_HASH);

    // Make sure we aren't running out of memory
    if (cinfo->meta.unit.size() != JSON_UNIT_COUNT ||
        cinfo->meta.jmap.size() != JSON_MAX_HASH ||
        cinfo->meta.emap.size() != JSON_MAX_HASH)
    {
        delete [] cinfo;
        return nullptr;
    }

    cinfo->pdata.glossary.resize(1);
    cinfo->pdata.agent.resize(1);
    cinfo->pdata.event.resize(1);
    cinfo->pdata.user.resize(1);
    memset(&cinfo->pdata.node, 0, sizeof(nodestruc));
    memset(&cinfo->pdata.physics, 0, sizeof(physicsstruc));
    memset(&cinfo->pdata.devspec, 0, sizeof(devspecstruc));

    // Make sure we aren't running out of memory
    if (cinfo->pdata.glossary.size() != 1 ||
        cinfo->pdata.agent.size() != 1 ||
        cinfo->pdata.event.size() != 1 ||
        cinfo->pdata.user.size() != 1)
    {
        delete [] cinfo;
        return nullptr;
    }

    // Copy primary to secondary
    json_clone(cinfo->pdata, cinfo->sdata);

    // Create JSON Map unit table
    for (uint16_t i=0; i<cinfo->meta.unit.size(); ++i)
    {
        // SI Units
        tunit.type = JSON_UNIT_TYPE_IDENTITY;
        tunit.p0 = tunit.p1 = tunit.p2 = 0.;
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
        case JSON_UNIT_AREA:
            tunit.name = "m2";
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
        case JSON_UNIT_DENSITY:
            tunit.name = "kg/m3";
            break;
        case JSON_UNIT_ENERGY:
            tunit.name = "j";
            break;
        case JSON_UNIT_FORCE:
            tunit.name = "N";
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
        case JSON_UNIT_MAGDENSITY:
            tunit.name = "T";
            break;
        case JSON_UNIT_MAGFIELD:
            tunit.name = "A/m";
            break;
        case JSON_UNIT_MAGFLUX:
            tunit.name = "Wb";
            break;
        case JSON_UNIT_MASS:
            tunit.name = "kg";
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
        case JSON_UNIT_VOLTAGE:
            tunit.name = "V";
            break;
        case JSON_UNIT_VOLUME:
            tunit.name = "m3";
            break;
        }
        cinfo->meta.unit[i].push_back(tunit);

        // Alternate Units
        tunit.type = JSON_UNIT_TYPE_IDENTITY;
        tunit.p0 = tunit.p1 = tunit.p2 = 0.;
        switch (i)
        {
        case JSON_UNIT_NONE:
            break;
        case JSON_UNIT_ACCELERATION:
            tunit.name = "g";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/9.80665f;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_ANGLE:
            tunit.name = "deg";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = (float)RTOD;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_AREA:
            tunit.name = "ft2";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 10.76391f;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_CAPACITANCE:
            break;
        case JSON_UNIT_CHARGE:
            break;
        case JSON_UNIT_CURRENT:
            break;
        case JSON_UNIT_DENSITY:
            break;
        case JSON_UNIT_ENERGY:
            tunit.name = "erg";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e7;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_FORCE:
            tunit.name = "dyn";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e5;
            cinfo->meta.unit[i].push_back(tunit);
            tunit.name = "lb";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = .22481f;
            cinfo->meta.unit[i].push_back(tunit);
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
            cinfo->meta.unit[i].push_back(tunit);
            tunit.name = "cm";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e2;
            cinfo->meta.unit[i].push_back(tunit);
            tunit.name = "ft";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 3.280833f;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_LUMINANCE:
            break;
        case JSON_UNIT_MAGDENSITY:
            tunit.name = "nT";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e9;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_MAGFIELD:
            break;
        case JSON_UNIT_MAGFLUX:
            break;
        case JSON_UNIT_MASS:
            tunit.name = "g";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e3;
            cinfo->meta.unit[i].push_back(tunit);
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
            cinfo->meta.unit[i].push_back(tunit);
            tunit.name = "cm/s";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1e2;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_TEMPERATURE:
            tunit.name = "\260C";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p0 = -273.15f;
            tunit.p1 = 1.;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_TIME:
            tunit.name = "min";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/60.f;
            cinfo->meta.unit[i].push_back(tunit);
            tunit.name = "hr";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/3600.f;
            cinfo->meta.unit[i].push_back(tunit);
            tunit.name = "day";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = 1.f/86400.f;
            cinfo->meta.unit[i].push_back(tunit);
            break;
        case JSON_UNIT_VOLTAGE:
            break;
        case JSON_UNIT_VOLUME:
            break;
        }
    }

    // Here is where we add entries for all the single element names.
    json_addbaseentry(cinfo->meta);

    return (cinfo);
}

//! Remove JSON pointer map
/*! Frees up all space assigned to JSON pointer map. Includes any space allocated
 * through ::json_addentry.
*/
void json_destroy(cosmosstruc *cinfo)
{
    if (cinfo == nullptr)
    {
        return;
    }
    //    for (uint16_t i=0; i<2; ++i)
    //    {
    //        cdata.devspec.ant.resize(0);
    //        cdata.devspec.batt.resize(0);
    //        cdata.devspec.bus.resize(0);
    //        cdata.devspec.cam.resize(0);
    //        cdata.devspec.cpu.resize(0);
    //        cdata.devspec.gps.resize(0);
    //        cdata.devspec.htr.resize(0);
    //        cdata.devspec.imu.resize(0);
    //        cdata.devspec.mcc.resize(0);
    //        cdata.devspec.motr.resize(0);
    //        cdata.devspec.mtr.resize(0);
    //        cdata.devspec.tcu.resize(0);
    //        cdata.devspec.pload.resize(0);
    //        cdata.devspec.prop.resize(0);
    //        cdata.devspec.psen.resize(0);
    //        cdata.devspec.rot.resize(0);
    //        cdata.devspec.rw.resize(0);
    //        cdata.devspec.ssen.resize(0);
    //        cdata.devspec.strg.resize(0);
    //        cdata.devspec.stt.resize(0);
    //        cdata.devspec.suchi.resize(0);
    //        cdata.devspec.swch.resize(0);
    //        cdata.devspec.tcv.resize(0);
    //        cdata.devspec.txr.resize(0);
    //        cdata.devspec.rxr.resize(0);
    //        cdata.devspec.telem.resize(0);
    //        cdata.devspec.thst.resize(0);
    //        cdata.devspec.tsen.resize(0);
    //        cdata.device.resize(0);
    //    }

    delete cinfo;
    cinfo = nullptr;
}

//! Calculate JSON HASH
/*! Simple hash function (TCPL Section 6.6 Table Lookup)
    \param hstring String to calculate the hash for.
    \return The hash, as an unsigned 16 bit number.
*/

uint16_t json_hash(std::string hstring)
{
    uint16_t hashval;

    hashval = 0;
    for (uint8_t val : hstring)
    {
        hashval *= 31;
        hashval += val;
    }
    //	for (hashval = 0; *hstring != '\0'; hstring++)
    //		hashval = *hstring + 31 * hashval;
    return (hashval % JSON_MAX_HASH);
}

//! Enter an alias into the JSON Namespace.
/*! See if the provided name is in the Namespace. If so, add an entry
 * for the provided alias that points to the same location.
 * \param alias Name to add as an alias.
 * \param value Either the contents of an equation, or a Namespace name that
 * should already exist in the Namespace
 * \param cmeta Reference to ::cosmosmetastruc to use.
 * \return The current number of entries, if successful, otherwise negative error.
*/
int32_t json_addentry(std::string alias, std::string value, cosmosmetastruc &cmeta)
{
    int32_t iretn;
    jsonhandle handle;
    //    uint16_t count = 0;
    // Add this alias only if it is not already in the map
    if ((iretn = json_name_map(alias, cmeta, handle)))
    {
        jsonentry tentry;
        tentry.name = alias;
        aliasstruc talias;
        talias.name = alias;
        // If it begins with ( then it is an equation, otherwise treat as name
        switch (value[0])
        {
        case '(':
            // Add new equation
            iretn = json_equation_map(value, cmeta, &handle);
            if (iretn < 0)
            {
                return iretn;
            }
            talias.handle = handle;
            talias.type = JSON_TYPE_EQUATION;
            break;
        default:
            // It is a Namespace name which should only be added if it is in the map
            if ((iretn = json_name_map(value, cmeta, handle)))
            {
                return iretn;
            }
            // Add new alias
            talias.handle = handle;
            talias.type = cmeta.jmap[handle.hash][handle.index].type;
            break;
        }
        // Place it in the Alias vector and point to it in the map
        cmeta.alias.push_back(talias);
        tentry.type = JSON_TYPE_ALIAS;
        tentry.group = JSON_STRUCT_ALIAS;
        tentry.offset = cmeta.alias.size() - 1;
        tentry.size = COSMOS_SIZEOF(aliasstruc);
        iretn = json_addentry(tentry, cmeta);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    return cmeta.jmapped;
}

//! Enter an entry into the JSON Namespace.
/*! Enters a ::jsonentry in the JSON Data Name Space.
    \param entry The entry to be entered.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \return The current number of entries, if successful, negative error if the entry could not be
    added.
*/
int32_t json_addentry(jsonentry entry, cosmosmetastruc &cmeta)
{
    uint16_t hash = json_hash(entry.name);
    size_t csize = cmeta.jmap[hash].size();
    cmeta.jmap[hash].push_back(entry);
    if (cmeta.jmap[hash].size() != csize+1)
    {
        return JSON_ERROR_NOENTRY;
    }

    ++cmeta.jmapped;

    return (cmeta.jmapped);
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
    \param size Number of bytes ::jsonetnry take up.
    \param type COSMOS JSON Data Type.
    \param group COSMOS JSON Data Group.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param unit Index into JMAP unit table.
    \return The current number of entries, if successful, 0 if the entry could not be
    added, or if enough memory could not be allocated to hold the JSON stream.
*/
int32_t json_addentry(std::string name, uint16_t d1, uint16_t d2, ptrdiff_t offset, size_t size, uint16_t type, uint16_t group, cosmosmetastruc &cmeta, uint16_t unit)
{
    jsonentry tentry;
    char ename[COSMOS_MAX_NAME+1];

    // Determine extended name
    strcpy(ename,name.c_str());
    if (d1 < 65535)
        sprintf(&ename[strlen(ename)],"_%03u",d1);
    if (d2 < 65535)
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
    tentry.size = size;

    return json_addentry(tentry, cmeta);
}

//! Add an entry to the JSON Namespace map without units.
/*! Allocates the space for a new ::jsonentry and then enters the information
 * associating a pointer with an entry in the name table. The name will be of form "name" if it is a
 * scalar, "name_iii" if it is a first level array, "name_iii_iii" if it is second
 * level, where "iii" is the zero filled index for the appropriate level.
    \param name variable name from the JSON Data Name Space
    \param d1 array index for first level, otherwise -1
    \param d2 array index for second level, otherwise -1
    \param offset Offset to the data from the beginning of its group.
    \param size Number of bytes ::jsonetnry take up.
    \param type COSMOS JSON Data Type.
    \param group COSMOS JSON Data Group.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \return The current number of entries, if successful, 0 if the entry could not be
    added, or if enough memory could not be allocated to hold the JSON stream.
*/
int32_t json_addentry(std::string name, uint16_t d1, uint16_t d2, ptrdiff_t offset, size_t size, uint16_t type, uint16_t group, cosmosmetastruc &cmeta)
{
    return (json_addentry(name, d1, d2, offset, size, type, group, cmeta, 0));
}

//! Number of items in current JSON map
/*! Returns the number of JSON items currently mapped.
    \param hash JSON HASH value.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \return Number of JSON items mapped, otherwise zero.
*/
size_t json_count_hash(uint16_t hash, cosmosmetastruc &cmeta)
{
    return (cmeta.jmap[hash].size());
}

//! Number of items in the current JSON map
/*! Returns the number of JSON items currently mapped.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \return total number of JSON items mapped, otherwise zero.
*/

size_t json_count_total(cosmosmetastruc &cmeta)
{
    size_t i = 0;

    for(uint32_t j = 0; j < cmeta.jmap.size(); ++j)
        i += json_count_hash((uint16_t)j, cmeta);

    return i;
}

//! Perform JSON output for a JSON item by handle
/*! Appends the indicated JSON object to the supplied JSON Stream.
    \param jstring Reference to JSON stream.
    \param handle The ::jsonhandle of the desired object.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_handle(std::string &jstring, jsonhandle handle, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    if (!cmeta.jmapped)
        return (JSON_ERROR_NOJMAP);

    iretn = json_out_entry(jstring, &cmeta.jmap[handle.hash][handle.index], cmeta, cdata);

    return (iretn);
}

//! Perform JSON output for a single JSON entry
/*! Populates the jstring for the indicated ::jsonmap by ::jsonentry.
    \param jstring Reference to JSON stream.
    \param entry The ::jsonentry of the desired variable.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_entry(std::string &jstring, jsonentry* entry, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;
    uint8_t *data;

    //    if (!cmeta.jmapped)
    //        return (JSON_ERROR_NOJMAP);

    if ((iretn=json_out_character(jstring,'{')) != 0)
        return (iretn);

    data = json_ptr_of_offset(entry->offset, entry->group, cmeta, cdata);
    if ((iretn=json_out_value(jstring, entry->name, data, entry->type, cmeta, cdata)) != 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) != 0)
        return (iretn);

    return (iretn);
}

//! Output JSON Pair
/*! Append a name:value pair to the provided JSON String for a variable in
 * the JSON Namespace, based on the provided data and type.
 * \param jstring Reference to JSON String to append to.
 * \param name Namespace name of variable.
 * \param data Pointer to location in ::cosmosstruc of variable.
 * \param type Type of variable.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 * \return Zero or negative error.
*/
int32_t json_out_value(std::string &jstring, std::string name, uint8_t *data, uint16_t type, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    if (!cmeta.jmapped)
    {
        return JSON_ERROR_NOJMAP;
    }

    if ((iretn=json_out_name(jstring ,name)) != 0)
    {
        return iretn;
    }

    if ((iretn=json_out_type(jstring, data, type, cmeta, cdata)) != 0)
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 * \return Zero or negative error.
*/
int32_t json_out_type(std::string &jstring, uint8_t *data, uint16_t type, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn = 0;

    switch (type)
    {
    case JSON_TYPE_UINT8:
        if ((iretn=json_out_uint8(jstring,*(uint8_t *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_UINT16:
        if ((iretn=json_out_uint16(jstring,*(uint16_t *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_UINT32:
        if ((iretn=json_out_uint32(jstring,*(uint32_t *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_INT8:
        if ((iretn=json_out_int8(jstring,*(int8_t *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_INT16:
        if ((iretn=json_out_int16(jstring,*(int16_t *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_INT32:
        if ((iretn=json_out_int32(jstring,*(int32_t *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_FLOAT:
        if ((iretn=json_out_float(jstring,*(float *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_DOUBLE:
    case JSON_TYPE_TIMESTAMP:
        if ((iretn=json_out_double(jstring,*(double *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_STRING:
        if ((iretn=json_out_string(jstring,(char *)data,COSMOS_MAX_DATA)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_NAME:
        if ((iretn=json_out_string(jstring,(char *)data,COSMOS_MAX_NAME)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_GVECTOR:
        if ((iretn=json_out_gvector(jstring,*(gvector *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_CVECTOR:
        if ((iretn=json_out_cvector(jstring,*(cvector *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_QUATERNION:
        if ((iretn=json_out_quaternion(jstring,*(quaternion *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_RMATRIX:
        if ((iretn=json_out_rmatrix(jstring,*(rmatrix *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_DCM:
        if ((iretn=json_out_dcm(jstring,*(rmatrix *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_RVECTOR:
    case JSON_TYPE_TVECTOR:
        if ((iretn=json_out_rvector(jstring,*(rvector *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_POS_SELG:
    case JSON_TYPE_POS_GEOD:
        if ((iretn=json_out_geoidpos(jstring,*(geoidpos *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_POS_GEOS:
        if ((iretn=json_out_spherpos(jstring,*(spherpos *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_CARTPOS:
    case JSON_TYPE_POS_GEOC:
    case JSON_TYPE_POS_SELC:
    case JSON_TYPE_POS_ECI:
    case JSON_TYPE_POS_SCI:
    case JSON_TYPE_POS_BARYC:
        if ((iretn=json_out_cartpos(jstring,*(cartpos *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_DCMATT:
        if ((iretn=json_out_dcmatt(jstring,*(dcmatt *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_QATT:
    case JSON_TYPE_QATT_TOPO:
    case JSON_TYPE_QATT_GEOC:
    case JSON_TYPE_QATT_LVLH:
    case JSON_TYPE_QATT_ICRF:
    case JSON_TYPE_QATT_SELC:
        if ((iretn=json_out_qatt(jstring,*(qatt *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_HBEAT:
        if ((iretn=json_out_beatstruc(jstring,*(beatstruc *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_LOC_POS:
        if ((iretn=json_out_posstruc(jstring,*(posstruc *)data)) != 0)
            return (iretn);
        break;
    case JSON_TYPE_LOC_ATT:
        {
            if ((iretn=json_out_attstruc(jstring,*(attstruc *)data)) != 0)
                return (iretn);
            break;
        }
    case JSON_TYPE_LOC:
        {
            if ((iretn=json_out_locstruc(jstring,*(locstruc *)data)) != 0)
                return (iretn);
            break;
        }
    case JSON_TYPE_ALIAS:
        {
            aliasstruc *aptr = (aliasstruc *)data;
            switch (aptr->type)
            {
            case JSON_TYPE_EQUATION:
                {
                    jsonequation *eptr = &cmeta.emap[aptr->handle.hash][aptr->handle.index];
                    if ((iretn=json_out_double(jstring, json_equation(eptr, cmeta, cdata))) != 0)
                    {
                        return iretn;
                    }
                }
                break;
            default:
                {
                    jsonentry *eptr = &cmeta.jmap[aptr->handle.hash][aptr->handle.index];
                    if ((iretn=json_out_type(jstring, eptr->data.data(), eptr->type, cmeta, cdata)) != 0)
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
            if ((iretn=json_out_double(jstring, json_equation(ptr, cmeta, cdata))) != 0)
            {
                return iretn;
            }
            break;
        }
    }

    return (iretn);
}

//! Extend JSON stream
/*! Append the indicated string to the current JSON stream, extending it if necessary.
 * \param jstring JSON stting to be appended to.
    \param tstring String to be appended.
    \return 0 if successful, negative error number otherwise.
*/
int32_t json_append(std::string &jstring, const char *tstring)
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
int32_t json_out_character(std::string &jstring,char character)
{
    char tstring[2] = {0,0};
    int32_t iretn;

    tstring[0] = character;
    if ((iretn=json_append(jstring,tstring)) < 0)
        return (iretn);

    return 0;
}

//! Object name to JSON
/*! Appends an entry for the name piece of a JSON object (including ":") to the current JSON
 * string.
    \param jstring Reference to JSON stream.
    \param name The Object Name
    \return  0 if successful, otherwise negative error.
*/
int32_t json_out_name(std::string &jstring, std::string name)
{
    int32_t iretn;

    if ((iretn=json_out_string(jstring, name, COSMOS_MAX_NAME)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,':')) < 0)
        return (iretn);
    return 0;
}

//! Signed 8 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 8 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int8(std::string &jstring,int8_t value)
{
    int32_t iretn;
    char tstring[15];

    sprintf(tstring,"%hd",value);

    iretn = json_append(jstring,tstring);
    return (iretn);
}

//! Signed 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int16(std::string &jstring,int16_t value)
{
    int32_t iretn;
    char tstring[15];

    sprintf(tstring,"%d",value);

    iretn = json_append(jstring,tstring);
    return (iretn);
}

//! Signed 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int32(std::string &jstring,int32_t value)
{
    int32_t iretn;
    char tstring[15];

    sprintf(tstring,"%d",value);

    iretn = json_append(jstring,tstring);
    return (iretn);
}

//! Unsigned 8 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 8 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint8(std::string &jstring, uint8_t value)
{
    int32_t iretn;
    char tstring[15];

    sprintf(tstring,"%hu",value);

    iretn = json_append(jstring,tstring);
    return (iretn);
}

//! Unsigned 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint16(std::string &jstring,uint16_t value)
{
    int32_t iretn;
    char tstring[15];

    sprintf(tstring,"%u",value);

    iretn = json_append(jstring,tstring);
    return (iretn);
}

//! Unsigned 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint32(std::string &jstring,uint32_t value)
{
    int32_t iretn;
    char tstring[15];

    sprintf(tstring,"%u",value);

    iretn = json_append(jstring,tstring);
    return (iretn);
}

//! Single precision floating point32_t to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated float.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_float(std::string &jstring,float value)
{
    int32_t iretn = 0;
    char tstring[15];

    if (std::isfinite(value))
    {
        sprintf(tstring,"%.8g",value);
        iretn = json_append(jstring,tstring);
    }

    return (iretn);
}

//! Perform JSON output for a single nonindexed double
/*! Appends a JSON entry to the current JSON stream for the indicated double.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_double(std::string &jstring,double value)
{
    int32_t iretn = 0;
    char tstring[30];

    if (std::isfinite(value))
    {
        sprintf(tstring,"%.17g",value);
        iretn = json_append(jstring,tstring);
    }

    return (iretn);
}

//! String to JSON
/*! Appends a JSON entry to the current JSON stream for the string variable.
    \param jstring Reference to JSON stream.
    \param ostring String to append.
    \param len Maximum allowed size.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_string(std::string &jstring, std::string ostring, uint16_t len)
{
    int32_t iretn;
    uint16_t i;
    char tstring[5];

    if (len > JSON_MAX_DATA)
        len = JSON_MAX_DATA;

    if ((iretn=json_out_character(jstring,'"')) < 0)
        return (iretn);

    for (i=0; i<ostring.size(); i++)
    {
        if (i > len)
        {
            break;
        }

        if (ostring[i] == '"' || ostring[i] == '\\' || ostring[i] == '/')
        {
            if ((iretn=json_out_character(jstring,'\\')) < 0)
                return (iretn);
            if ((iretn=json_out_character(jstring, ostring[i])) < 0)
                return (iretn);
        }
        else if (ostring[i] < 32)
        {
            if ((iretn=json_out_character(jstring,'\\')) < 0)
                return (iretn);
            switch (ostring[i])
            {
            case '\b':
                if ((iretn=json_out_character(jstring,'b')) < 0)
                    return (iretn);
                break;
            case '\f':
                if ((iretn=json_out_character(jstring,'f')) < 0)
                    return (iretn);
                break;
            case '\n':
                if ((iretn=json_out_character(jstring,'n')) < 0)
                    return (iretn);
                break;
            case '\r':
                if ((iretn=json_out_character(jstring,'r')) < 0)
                    return (iretn);
                break;
            case '\t':
                if ((iretn=json_out_character(jstring,'t')) < 0)
                    return (iretn);
                break;
            default:
                if ((iretn=json_out_character(jstring,'u')) < 0)
                    return (iretn);
                sprintf(tstring,"%04x", ostring[i]);
                if ((iretn=json_out_string(jstring,tstring,4)) < 0)
                    return (iretn);
            }
        }
        else if (ostring[i] < 127)
        {
            if ((iretn=json_out_character(jstring, ostring[i])) < 0)
                return (iretn);
        }
        else
        {
            if ((iretn=json_out_character(jstring,'\\')) < 0)
                return (iretn);
            if ((iretn=json_out_character(jstring,'u')) < 0)
                return (iretn);
            sprintf(tstring,"%04x", ostring[i]);
            if ((iretn=json_out_string(jstring,tstring,4)) < 0)
                return (iretn);
        }
    }
    if ((iretn=json_out_character(jstring,'"')) < 0)
        return (iretn);

    return (iretn);
}

//! ::gvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::gvector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_gvector(std::string &jstring,gvector value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Latitude
    if ((iretn=json_out_name(jstring,(char *)"lat")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.lat)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Longitude
    if ((iretn=json_out_name(jstring,(char *)"lon")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.lon)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Height
    if ((iretn=json_out_name(jstring,(char *)"h")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.h)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::svector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::svector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_svector(std::string &jstring,svector value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Latitude
    if ((iretn=json_out_name(jstring,(char *)"phi")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.phi)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Longitude
    if ((iretn=json_out_name(jstring,(char *)"lambda")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.lambda)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Height
    if ((iretn=json_out_name(jstring,(char *)"r")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.r)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::rvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::rvector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_rvector(std::string &jstring,rvector value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return (iretn);

    // Output Col[0]
    if ((iretn=json_out_double(jstring,value.col[0])) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Col[1]
    if ((iretn=json_out_double(jstring,value.col[1])) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Col[2]
    if ((iretn=json_out_double(jstring,value.col[2])) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,']')) < 0)
        return (iretn);

    return 0;
}

//! ::quaternion to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::quaternion.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_quaternion(std::string &jstring,quaternion value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Orientation
    if ((iretn=json_out_name(jstring,(char *)"d")) < 0)
        return (iretn);
    if ((iretn=json_out_cvector(jstring,value.d)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Rotation
    if ((iretn=json_out_name(jstring,(char *)"w")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.w)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::cvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::cvector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_cvector(std::string &jstring,cvector value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output X
    if ((iretn=json_out_name(jstring,(char *)"x")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.x)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Y
    if ((iretn=json_out_name(jstring,(char *)"y")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.y)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Z
    if ((iretn=json_out_name(jstring,(char *)"z")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.z)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::cartpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::cartpos.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_cartpos(std::string &jstring,cartpos value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Time
    if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Position
    if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.s)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Velocity
    if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.v)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Acceleration
    if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.a)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::geoidpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::geoidpos.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_geoidpos(std::string &jstring,geoidpos value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Time
    if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Position
    if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
        return (iretn);
    if ((iretn=json_out_gvector(jstring,value.s)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Velocity
    if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
        return (iretn);
    if ((iretn=json_out_gvector(jstring,value.v)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Acceleration
    if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
        return (iretn);
    if ((iretn=json_out_gvector(jstring,value.a)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::spherpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::spherpos.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_spherpos(std::string &jstring,spherpos value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Time
    if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Position
    if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
        return (iretn);
    if ((iretn=json_out_svector(jstring,value.s)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Velocity
    if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
        return (iretn);
    if ((iretn=json_out_svector(jstring,value.v)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Acceleration
    if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
        return (iretn);
    if ((iretn=json_out_svector(jstring,value.a)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! Node name to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * Node name.
 * \param jstring JSON stream to append to
 * \param value The JSON data of the desired variable
 * \return  0 if successful, negative error otherwise
*/

int32_t json_out_node(std::string &jstring, std::string value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return (iretn);

    if ((iretn=json_out_name(jstring, "node_name")) < 0)
        return (iretn);
    if ((iretn=json_out_string(jstring, value, COSMOS_MAX_NAME)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! UTC Start to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * UTC start.
 * \param jstring JSON stream to append to
 * \param value The JSON data of the desired variable
 * \return  0 if successful, negative error otherwise
*/

int32_t json_out_utcstart(std::string &jstring, double value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    if ((iretn=json_out_name(jstring,(char *)"node_utcstart")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ECI position to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ECI based position.
 * \param jstring JSON stream to append to
 * \param value The JSON data of the desired variable
 * \return  0 if successful, negative error otherwise
*/

int32_t json_out_ecipos(std::string &jstring, cartpos value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Earth Centered Inertial
    if ((iretn=json_out_name(jstring,(char *)"node_loc_pos_eci")) < 0)
        return (iretn);
    if ((iretn=json_out_cartpos(jstring,value)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::posstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::posstruc.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_posstruc(std::string &jstring,posstruc value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Time
    if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Barycentric
    if ((iretn=json_out_name(jstring,(char *)"icrf")) < 0)
        return (iretn);
    if ((iretn=json_out_cartpos(jstring,value.icrf)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Earth Centered Inertial
    if ((iretn=json_out_name(jstring,(char *)"eci")) < 0)
        return (iretn);
    if ((iretn=json_out_cartpos(jstring,value.eci)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Selene Centered Inertial
    if ((iretn=json_out_name(jstring,(char *)"sci")) < 0)
        return (iretn);
    if ((iretn=json_out_cartpos(jstring,value.sci)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Geocentric
    if ((iretn=json_out_name(jstring,(char *)"geoc")) < 0)
        return (iretn);
    if ((iretn=json_out_cartpos(jstring,value.geoc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Selenocentric
    if ((iretn=json_out_name(jstring,(char *)"selc")) < 0)
        return (iretn);
    if ((iretn=json_out_cartpos(jstring,value.selc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Geodetic
    if ((iretn=json_out_name(jstring,(char *)"geod")) < 0)
        return (iretn);
    if ((iretn=json_out_geoidpos(jstring,value.geod)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Selenographic
    if ((iretn=json_out_name(jstring,(char *)"selg")) < 0)
        return (iretn);
    if ((iretn=json_out_geoidpos(jstring,value.selg)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Geocentric Spherical
    if ((iretn=json_out_name(jstring,(char *)"geos")) < 0)
        return (iretn);
    if ((iretn=json_out_spherpos(jstring,value.geos)) < 0)
        return (iretn);
    //	if ((iretn=json_out_character(jstring,',')) < 0)
    //		return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::attstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::attstruc.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_attstruc(std::string &jstring,attstruc value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Time
    if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Topocentric
    if ((iretn=json_out_name(jstring,(char *)"topo")) < 0)
        return (iretn);
    if ((iretn=json_out_qatt(jstring,value.topo)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output LVLH
    if ((iretn=json_out_name(jstring,(char *)"lvlh")) < 0)
        return (iretn);
    if ((iretn=json_out_qatt(jstring,value.lvlh)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Geocentric
    if ((iretn=json_out_name(jstring,(char *)"geoc")) < 0)
        return (iretn);
    if ((iretn=json_out_qatt(jstring,value.geoc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Selenocentric
    if ((iretn=json_out_name(jstring,(char *)"selc")) < 0)
        return (iretn);
    if ((iretn=json_out_qatt(jstring,value.selc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output ICRF
    if ((iretn=json_out_name(jstring,(char *)"icrf")) < 0)
        return (iretn);
    if ((iretn=json_out_qatt(jstring,value.icrf)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::locstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::locstruc.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_locstruc(std::string &jstring,locstruc value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Time
    if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Position
    if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
        return (iretn);
    if ((iretn=json_out_posstruc(jstring,value.pos)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Attitude
    if ((iretn=json_out_name(jstring,(char *)"att")) < 0)
        return (iretn);
    if ((iretn=json_out_attstruc(jstring,value.att)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Earth Magnetic Field
    if ((iretn=json_out_name(jstring,(char *)"bearth")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.bearth)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! Command event to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::longeventstruc specific ti a command event.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_commandevent(std::string &jstring,longeventstruc value)
{
    int32_t iretn;


    if ((iretn=json_out_character(jstring, '{')) < 0)
        return (iretn);
    if ((iretn=json_out_name(jstring, (char *)"event_utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring, value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return (iretn);

    if (value.utcexec != 0.)
    {
        if ((iretn=json_out_character(jstring, '{')) < 0)
            return (iretn);
        if ((iretn=json_out_name(jstring, (char *)"event_utcexec")) < 0)
            return (iretn);
        if ((iretn=json_out_double(jstring, value.utcexec)) < 0)
            return (iretn);
        if ((iretn=json_out_character(jstring, '}')) < 0)
            return (iretn);
    }

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return (iretn);
    if ((iretn=json_out_name(jstring, (char *)"event_name")) < 0)
        return (iretn);
    if ((iretn=json_out_string(jstring, value.name, COSMOS_MAX_NAME)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return (iretn);
    if ((iretn=json_out_name(jstring, (char *)"event_type")) < 0)
        return (iretn);
    if ((iretn=json_out_uint32(jstring, value.type)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return (iretn);
    if ((iretn=json_out_name(jstring, (char *)"event_flag")) < 0)
        return (iretn);
    if ((iretn=json_out_uint32(jstring, value.flag)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring, '{')) < 0)
        return (iretn);
    if ((iretn=json_out_name(jstring, (char *)"event_data")) < 0)
        return (iretn);
    if ((iretn=json_out_string(jstring, value.data, COSMOS_MAX_DATA)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring, '}')) < 0)
        return (iretn);

    if (value.flag & EVENT_FLAG_CONDITIONAL)
    {
        if ((iretn=json_out_character(jstring, '{')) < 0)
            return (iretn);
        if ((iretn=json_out_name(jstring, (char *)"event_condition")) < 0)
            return (iretn);
        if ((iretn=json_out_string(jstring, value.condition, COSMOS_MAX_DATA)) < 0)
            return (iretn);
        if ((iretn=json_out_character(jstring, '}')) < 0)
            return (iretn);
    }

    return 0;
}

/*! Appends a JSON entry to the current JSON stream for the indicated ::dcmatt.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_dcmatt(std::string &jstring,dcmatt value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Position
    if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
        return (iretn);
    if ((iretn=json_out_dcm(jstring,value.s)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Velocity
    if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.v)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Acceleration
    if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.a)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::qatt to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::qatt.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_qatt(std::string &jstring,qatt value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Time
    if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.utc)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Position
    if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
        return (iretn);
    if ((iretn=json_out_quaternion(jstring,value.s)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Velocity
    if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.v)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Acceleration
    if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
        return (iretn);
    if ((iretn=json_out_rvector(jstring,value.a)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! ::dcm to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::dcm.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_dcm(std::string &jstring,rmatrix value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return (iretn);

    // Output Row[0]
    if ((iretn=json_out_rvector(jstring,value.row[0])) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Row[1]
    if ((iretn=json_out_rvector(jstring,value.row[1])) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Row[2]
    if ((iretn=json_out_rvector(jstring,value.row[2])) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,']')) < 0)
        return (iretn);
    return 0;
}

//! ::rmatrix to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::rmatrix.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_rmatrix(std::string &jstring,rmatrix value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return (iretn);

    // Output Row[0]
    if ((iretn=json_out_rvector(jstring,value.row[0])) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Row[1]
    if ((iretn=json_out_rvector(jstring,value.row[1])) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Row[2]
    if ((iretn=json_out_rvector(jstring,value.row[2])) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Row[3]
    if ((iretn=json_out_rvector(jstring,value.row[3])) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,']')) < 0)
        return (iretn);
    return 0;
}

//! ::beatstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::beatstruc.
    \param jstring Reference to JSON stream.
   \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_beatstruc(std::string &jstring,beatstruc value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Ntype
    if ((iretn=json_out_name(jstring,(char *)"ntype")) < 0)
        return (iretn);
    if ((iretn=json_out_uint16(jstring,(uint16_t)value.ntype)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output IP Address
    if ((iretn=json_out_name(jstring,(char *)"addr")) < 0)
        return (iretn);
    if ((iretn=json_out_string(jstring,value.addr,16)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output IP Port
    if ((iretn=json_out_name(jstring,(char *)"port")) < 0)
        return (iretn);
    if ((iretn=json_out_int32(jstring,value.port)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Buffer Size
    if ((iretn=json_out_name(jstring,(char *)"bsz")) < 0)
        return (iretn);
    if ((iretn=json_out_int32(jstring,value.bsz)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Beat Period
    if ((iretn=json_out_name(jstring,(char *)"bprd")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.bprd)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
    return 0;
}

//! Perform JSON output for a single element of a 1D named JSON item.
/*! Calls ::json_out for the indexed element of the named JSON vector.
    \param jstring Reference to JSON stream.
    \param token The JSON name for the desired variable
    \param index The desired element number
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_1d(std::string &jstring, const char *token, uint16_t index, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    char name[COSMOS_MAX_NAME+1];
    int32_t iretn;

    if (strlen(token) > COSMOS_MAX_NAME+4)
        return (JSON_ERROR_NAME_LENGTH);

    if (index > 999)
        return (JSON_ERROR_INDEX_SIZE);

    sprintf(name,"%s_%03u",token,index);
    iretn = json_out(jstring, name, cmeta, cdata);
    return (iretn);
}

//! Perform JSON output for a single element of a 2D named JSON item.
/*! Calls ::json_out for the indexed element of the named JSON matrix.
    \param jstring Reference to JSON stream.
    \param token The JSON name for the desired variable
    \param row The desired row number
    \param col The desired column number
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_2d(std::string &jstring, const char *token, uint16_t row, uint16_t col, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    char name[COSMOS_MAX_NAME+1];
    int32_t iretn;

    if (strlen(token) > COSMOS_MAX_NAME+8)
        return (JSON_ERROR_NAME_LENGTH);

    if (row > 999 || col > 999)
        return (JSON_ERROR_INDEX_SIZE);

    sprintf(name,"%s_%03u_%03u",token,row,col);
    iretn = json_out(jstring, name, cmeta, cdata);
    return (iretn);
}

//! Perform JSON output for a single named JSON item
/*! Populates the jstring for the indicated ::jsonmap through reference to JSON name.
    \param jstring The jstring into which to store the result.
    \param token The JSON name for the desired variable.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out(std::string &jstring, std::string token, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jsonhandle h;

    if (!cmeta.jmapped)
        return (JSON_ERROR_NOJMAP);

    h.hash = json_hash(token);

    if (cmeta.jmap.size() == 0)
        return (JSON_ERROR_NOJMAP);

    for (h.index=0; h.index<cmeta.jmap[h.hash].size(); ++h.index)
        //		if (!strcmp(token.c_str(), cmeta.jmap[h.hash][h.index].name))
        if (token == cmeta.jmap[h.hash][h.index].name)
        {
            return (json_out_handle(jstring, h, cmeta, cdata));
        }

    return (JSON_ERROR_NOENTRY);
}

//! Output a list of named JSON items.
/*! Populates the jstring for the indicated ::jsonmap through reference to a list of JSON
 * names.
    \param jstring The jstring into which to store the result.
    \param tokens The comma separated list of JSON names for the desired variables.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_list(std::string &jstring,std::string tokens, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    std::string tstring;
    const char* ptr;
    int32_t iretn;

    ptr = &tokens[0];
    while (ptr[0] != 0 && ptr[0] != '{')
        ptr++;
    if ((iretn=json_skip_character(ptr,'{')) != 0)
        return (iretn);
    do
    {
        if ((iretn=json_extract_string(ptr, tstring)) != 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        json_out(jstring, tstring.c_str(), cmeta, cdata);
    } while (!json_skip_character(ptr,','));
    if ((iretn = json_skip_white(ptr)) < 0)
        return (iretn);
    if ((iretn=json_skip_character(ptr,'}')) != 0 && iretn!=JSON_ERROR_EOS)
        return (iretn);
    if ((iretn = json_skip_white(ptr)) < 0 && iretn!=JSON_ERROR_EOS)
        return (iretn);
    return 0;
}


//! Output JSON items from wildcard.
/*! Populates the jstring for the indicated ::jsonmap through reference to a regular expression matching of JSON
 * names.
    \param jstring The jstring into which to store the result.
    \param wildcard The regular expression to match against.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return  0 if successful, negative error otherwise
*/

int32_t json_out_wildcard(std::string &jstring, std::string wildcard, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn=0;
    jsonhandle h;

    for (h.hash=0; h.hash<cmeta.jmap.size(); ++h.hash)
    {
        for (h.index=0; h.index<cmeta.jmap[h.hash].size(); ++h.index)
        {
            if (string_cmp(wildcard.c_str(), cmeta.jmap[h.hash][h.index].name.c_str()))
            {
                iretn = json_out_handle(jstring, h, cmeta, cdata);
            }
        }
    }
    return (iretn);
}

//! Address from offset
/*! Calculate the actual address of an offset into either static or
 * dynamic space, using the provide cdata static and dynamic addresses.
 \param offset An offset taken from a ::jsonentry
 \param group The structure group from which the offset is measured.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return A pointer, castable into any desired type.
*/
uint8_t *json_ptr_of_offset(ptrdiff_t offset, uint16_t group, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *data=nullptr;

    switch (group)
    {
    case JSON_STRUCT_ABSOLUTE:
        data = offset + (uint8_t *)&cdata;
        break;
    case JSON_STRUCT_AGENT:
        data = offset+(uint8_t *)cdata.agent.data();
        break;
    case JSON_STRUCT_PHYSICS:
        data = offset+(uint8_t *)&(cdata.physics);
        break;
    case JSON_STRUCT_EVENT:
        data = offset+(uint8_t *)cdata.event.data();
        break;
    case JSON_STRUCT_NODE:
        data = offset+(uint8_t *)&(cdata.node);
        break;
    case JSON_STRUCT_DEVICE:
        data = offset+(uint8_t *)cdata.device.data();
        break;
    case JSON_STRUCT_DEVSPEC:
        data = offset+(uint8_t *)&(cdata.devspec);
        break;
    case JSON_STRUCT_PIECE:
        data = offset + (uint8_t *)cdata.piece.data();
        break;
    case JSON_STRUCT_TARGET:
        data = offset + (uint8_t *)cdata.target.data();
        break;
    case JSON_STRUCT_USER:
        data = offset + (uint8_t *)cdata.user.data();
        break;
    case JSON_STRUCT_PORT:
        data = offset+(uint8_t *)cdata.port.data();
        break;
    case JSON_STRUCT_GLOSSARY:
        data = offset+(uint8_t *)cdata.glossary.data();
        break;
    case JSON_STRUCT_TLE:
        data = offset+(uint8_t *)cdata.tle.data();
        break;
    case JSON_STRUCT_ALIAS:
        data = (uint8_t *)&cmeta.alias[(size_t)offset];
        break;
    case JSON_STRUCT_EQUATION:
        data = (uint8_t *)&cmeta.equation[offset];
        break;
    }
    return (data);
}

//! Output a vector of JSON entries.
/*! Populates the provided vector for the indicated ::jsonmap through reference to a list of JSON
 * names.
    \param table The vector of ::jsonentry into which to store the result.
    \param tokens The comma separated list of JSON names for the desired variables.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \return  0 if successful, negative error otherwise
*/
int32_t json_table_of_list(std::vector<jsonentry*> &table, std::string tokens, cosmosmetastruc &cmeta)
{
    std::string tstring;
    const char *ptr;
    int32_t iretn;
    jsonentry* tentry;

    ptr = &tokens[0];
    while (ptr[0] != 0 && ptr[0] != '{')
        ptr++;
    if ((iretn=json_skip_character(ptr,'{')) != 0)
        return (iretn);
    do
    {
        if ((iretn=json_extract_string(ptr, tstring)) != 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        tentry = json_entry_of(tstring, cmeta);
        table.push_back(tentry);
    } while (!json_skip_character(ptr,','));
    if ((iretn = json_skip_white(ptr)) < 0)
        return (iretn);
    if ((iretn=json_skip_character(ptr,'}')) != 0 && iretn!=JSON_ERROR_EOS)
        return (iretn);
    if ((iretn = json_skip_white(ptr)) < 0 && iretn!=JSON_ERROR_EOS)
        return (iretn);
    return 0;
}

//! Info on Namespace address
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a the namespace value that matches a given memory address.
 \param ptr Address of a variable that may match a namespace name.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return Pointer to the ::jsonentry for the token, or NULL.
*/
jsonentry *json_entry_of(uint8_t *ptr, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint16_t n, m;
    uint16_t group = UINT16_MAX;
    ptrdiff_t offset;

    if (!cmeta.jmapped)
        return nullptr;

    offset = -1;

    if ((offset = (uint8_t *)ptr - (uint8_t *)&(cdata.node)) > 0 && offset < (ptrdiff_t)sizeof(nodestruc))
    {
        group = JSON_STRUCT_NODE;
    }
    else
    {
        offset = -1;
        //#undef max;
        offset = (std::numeric_limits<std::ptrdiff_t>::max)();

    }
    if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cdata.node)) > 0 && offset < (ptrdiff_t)sizeof(agentstruc))
    {
        group = JSON_STRUCT_AGENT;
    }
    else
    {
        offset = -1;
    }
    if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cdata.node)) > 0 && offset < (ptrdiff_t)sizeof(physicsstruc))
    {
        group = JSON_STRUCT_PHYSICS;
    }
    else
    {
        offset = -1;
    }
    if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cdata.node)) > 0 && offset < (ptrdiff_t)sizeof(eventstruc))
    {
        group = JSON_STRUCT_EVENT;
    }
    else
    {
        offset = -1;
    }

    if (offset == -1)
        return ((jsonentry *)NULL);

    for (m=0; m<cmeta.jmap.size(); m++)
    {
        for (n=0; n<cmeta.jmap[m].size(); n++)
        {
            if (cmeta.jmap[m][n].group == group && cmeta.jmap[m][n].offset == offset)
            {
                return ((jsonentry *)&cmeta.jmap[m][n]);
            }
        }
    }
    return ((jsonentry *)NULL);
}

//! Info on Namespace name
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a given name.
 \param token Namespace name to look up
    \param cmeta Reference to ::cosmosmetastruc to use.
 \return Pointer to the ::jsonentry for the token, or NULL.
*/
jsonentry *json_entry_of(std::string token, cosmosmetastruc &cmeta)
{
    int16_t hash;
    uint16_t n;

    if (!cmeta.jmapped)
        return nullptr;

    hash = json_hash(token);

    if (cmeta.jmap[hash].size() == 0)
        return ((jsonentry *)NULL);

    for (n=0; n<cmeta.jmap[hash].size(); n++)
    {
        if (token == cmeta.jmap[hash][n].name)
        {
            return ((jsonentry *)&cmeta.jmap[hash][n]);
        }
    }
    return ((jsonentry *)NULL);
}

//! Info on Namespace equation
/*! Return a pointer to the Namespace Equation structure containing the
 * information for a given equation.
 \param handle ::jsonhandle for the entry in the global emap
    \param cmeta Reference to ::cosmosmetastruc to use.
 \return Pointer to the ::jsonequation for the token, or NULL.
*/
jsonequation *json_equation_of(jsonhandle handle, cosmosmetastruc &cmeta)
{
    if (!cmeta.jmapped || handle.hash >= cmeta.emap.size())
        return nullptr;

    if (cmeta.emap[handle.hash].size() > handle.index)
    {
        return ((jsonequation *)&cmeta.emap[handle.hash][handle.index]);
    }
    return ((jsonequation *)nullptr);
}

//! Info on Namespace name
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a given name.
 \param handle ::jsonhandle for the entry in the global jmap
    \param cmeta Reference to ::cosmosmetastruc to use.
 \return Pointer to the ::jsonentry for the token, or NULL.
*/
jsonentry *json_entry_of(jsonhandle handle, cosmosmetastruc &cmeta)
{
    if (!cmeta.jmapped || handle.hash >= cmeta.jmap.size())
        return nullptr;

    if (cmeta.jmap[handle.hash].size() > handle.index)
    {
        return ((jsonentry *)&cmeta.jmap[handle.hash][handle.index]);
    }
    return ((jsonentry *)nullptr);
}

//! Type of namespace name.
/*! Return the ::jsonlib_type of the token in the ::jsonmap.
 \param token the JSON name for the desired variable
    \param cmeta Reference to ::cosmosmetastruc to use.
 \return The ::jsonlib_type, otherwise 0.
*/
uint16_t json_type_of_name(std::string token, cosmosmetastruc &cmeta)
{
    jsonentry *entry;

    if (token[0] == '(')
        return (JSON_TYPE_EQUATION);

    if ((entry=json_entry_of(token, cmeta)) != NULL)
    {
        return (entry->type);
    }
    return 0;
}

//! Return the data pointer that matches a JSON name
/*! Look up the provided JSON data name in the indicated ::jsonmap and return the
 * associated data pointer.
 \param token the JSON name for the desired variable
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return The associated data pointer, if succesful, otherwise NULL
*/
uint8_t *json_ptrto(std::string token, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cmeta)) == NULL)
        return ((uint8_t *)NULL);
    else
        return (json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata));
}

//! Return the data pointer that matches a singly indexed JSON name
/*! Look up the provided JSON data name with the indicated index in the current ::jsonmap
 * and return the associated data pointer.
    \param token the JSON name for the desired variable
    \param index1 Primary index.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return The associated data pointer, if succesful, otherwise NULL
*/
uint8_t *json_ptrto_1d(std::string token, uint16_t index1, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return ((uint8_t *)NULL);

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(),index1);

    return (json_ptrto(token, cmeta, cdata));

}

//! Return the data pointer that matches a doubly indexed JSON name
/*! Look up the provided JSON data name with the indicated indices in the current ::jsonmap
 * and return the associated data pointer.
    \param token the JSON name for the desired variable
    \param index1 Primary index.
    \param index2 Secondary index.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return The associated data pointer, if succesful, otherwise NULL
*/
uint8_t *json_ptrto_2d(std::string token, uint16_t index1, uint16_t index2, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    char tstring[9+COSMOS_MAX_NAME];

    // Return error if index too large
    if (index1 > 999 || index2 > 999)
        return ((uint8_t *)NULL);

    // Create extended name, shortening if necessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);

    return (json_ptrto(token, cmeta, cdata));

}

//! Return integer from handle.
/*! If the value at this ::jsonhandle can in any way be interepreted as a number,
 * return it as an int32_t.
 \param handle ::jsonhandle for a valid COSMOS Namespace entry.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return Value cast as an int32_t, or 0.
*/
int32_t json_get_int(jsonhandle &handle, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t value=0;

    if (cmeta.jmap[handle.hash].size() <= handle.index)
    {
        return 0;
    }

    value = json_get_int(&cmeta.jmap[handle.hash][handle.index], cmeta, cdata);
    return value;
}

//! Return integer from entry.
/*! If the value stored in this ::jsonentry can in any way be interepreted as a number,
 * return it as an int32_t.
 \param entry A valid COSMOS Namespace entry.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return Value cast as an int32_t, or 0.
*/
int32_t json_get_int(jsonentry *entry, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *dptr=nullptr;
    int32_t value=0;

    dptr = json_ptr_of_offset(entry->offset,entry->group, cmeta, cdata);
    if (dptr == nullptr)
    {
        return 0;
    }
    else
    {
        switch (entry->type)
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
            value = atol((char *)(dptr));
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value = (int32_t)json_equation(tpointer, cmeta, cdata);
            }
            break;
        case JSON_TYPE_ALIAS:
            {
                jsonentry *eptr;
                if ((eptr=json_entry_of((*(jsonhandle *)(dptr)), cmeta)) == nullptr)
                {
                    value =  0;
                }
                else
                {
                    value = json_get_int(eptr, cmeta, cdata);
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int(std::string token, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t value=0;
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cmeta)) == nullptr)
    {
        return 0;
    }

    value = json_get_int(ptr, cmeta, cdata);
    return (value);

}

//! Return integer from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int(std::string token, uint16_t index1, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t value;
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(), index1);

    value = json_get_int(tstring, cmeta, cdata);

    return (value);
}

//! Return integer from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int(std::string token, uint16_t index1, uint16_t index2, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t value;
    char tstring[9+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);

    value = json_get_int(tstring, cmeta, cdata);

    return (value);
}

//! Return unsigned integer from handle.
/*! If the value at this ::jsonhandle can in any way be interepreted as a number,
 * return it as an uint32_t.
 \param handle ::jsonhandle for a valid COSMOS Namespace entry.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as an uint32_t, or 0.
*/
uint32_t json_get_uint(jsonhandle &handle, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint32_t value=0;

    if (cmeta.jmap[handle.hash].size() <= handle.index)
    {
        return 0;
    }

    value = json_get_uint(&cmeta.jmap[handle.hash][handle.index], cmeta, cdata);
    return value;
}

//! Return unsigned integer from entry.
/*! If the value stored in this ::jsonentry can in any way be interepreted as a number,
 * return it as an uint32_t.
 \param entry A valid COSMOS Namespace entry.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as an uint32_t, or 0.
*/
uint32_t json_get_uint(jsonentry *entry, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *dptr=nullptr;
    uint32_t value=0;

    dptr = json_ptr_of_offset(entry->offset,entry->group, cmeta, cdata);
    if (dptr == nullptr)
    {
        return 0;
    }
    else
    {
        switch (entry->type)
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
            value = atol((char *)(dptr));
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value = (uint32_t)json_equation(tpointer, cmeta, cdata);
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
                        if ((eptr=json_equation_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_equation(eptr, cmeta, cdata);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_get_uint(eptr, cmeta, cdata);
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as a signed 32 bit unsigned integer, or 0.
*/
uint32_t json_get_uint(std::string token, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint32_t value=0;
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cmeta)) == nullptr)
    {
        return 0;
    }

    value = json_get_uint(ptr, cmeta, cdata);
    return (value);

}

//! Return unsigned integer from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit unsigned integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as a signed 32 bit unsigned integer, or 0.
*/
uint32_t json_get_uint(std::string token, uint16_t index1, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint32_t value;
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(), index1);

    value = json_get_uint(tstring, cmeta, cdata);

    return (value);
}

//! Return unsigned integer from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit unsigned integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as a signed 32 bit unsigned integer, or 0.
*/
uint32_t json_get_uint(std::string token, uint16_t index1, uint16_t index2, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint32_t value;
    char tstring[9+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);

    value = json_get_uint(tstring, cmeta, cdata);

    return (value);
}

//! Return double from handle.
/*! If the value at this ::jsonhandle can in any way be interepreted as a number,
 * return it as a double.
 \param handle ::jsonhandle for a valid COSMOS Namespace entry.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as an double, or 0.
*/
double json_get_double(jsonhandle &handle, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    double value=0.;

    if (cmeta.jmap[handle.hash].size() <= handle.index)
    {
        return 0.;
    }

    value = json_get_double(&cmeta.jmap[handle.hash][handle.index], cmeta, cdata);
    return value;
}

//! Return double from name.
/*! If the named value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(std::string token, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    double value=0.;
    jsonentry *entry;
    const char* tokenp = &token[0];

    if (!std::isnan(value=json_equation(tokenp, cmeta, cdata)))
        return (value);

    if ((entry=json_entry_of(token, cmeta)) == nullptr)
    {
        return (NAN);
    }

    if (!std::isnan(value=json_get_double(entry, cmeta, cdata)))
        return (value);

    return (NAN);
}

//! Return double from entry.
/*! If the named value can in any way be interepreted as a number,
 * return it as a double.
 \param entry Pointer to a valid ::jsonentry.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(jsonentry *entry, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *dptr=nullptr;
    double value=0.;

    dptr = json_ptr_of_offset(entry->offset,entry->group, cmeta, cdata);
    if (dptr == nullptr)
    {
        return 0.;
    }
    else
    {
        switch (entry->type)
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
            value = atof((char *)dptr);
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value = (double)json_equation(tpointer, cmeta, cdata);
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
                        if ((eptr=json_equation_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_equation(eptr, cmeta, cdata);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value =  0;
                        }
                        else
                        {
                            value = json_get_double(eptr, cmeta, cdata);
                        }
                    }
                    break;
                }
                //                jsonentry *eptr;
                //                if ((eptr=json_entry_of((*(jsonhandle *)(dptr)), cmeta, cdata)) == nullptr)
                //                {
                //                    value =  0;
                //                }
                //                else
                //                {
                //                    value = json_get_double(eptr, cmeta, cdata);
                //                }
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as an rvector, or 0.
*/
rvector json_get_rvector(jsonentry *entry, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *dptr=nullptr;
    rvector value={{0., 0., 0.}};

    dptr = json_ptr_of_offset(entry->offset,entry->group, cmeta, cdata);
    if (dptr == nullptr)
    {
        return value;
    }
    else
    {
        switch (entry->type)
        {
        case JSON_TYPE_CARTPOS:
        case JSON_TYPE_POS_GEOC:
        case JSON_TYPE_POS_SELC:
        case JSON_TYPE_POS_ECI:
        case JSON_TYPE_POS_SCI:
        case JSON_TYPE_POS_BARYC:
            {
                cartpos tpos = (cartpos)(*(cartpos *)(dptr));
                value = tpos.s;
            }
            break;
        case JSON_TYPE_RVECTOR:
        case JSON_TYPE_TVECTOR:
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
            value.col[0] = atof((char *)dptr);
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value.col[0] = (double)json_equation(tpointer, cmeta, cdata);
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
                        if ((eptr=json_equation_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value.col[0] =  0;
                        }
                        else
                        {
                            value.col[0] = json_equation(eptr, cmeta, cdata);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value.col[0] =  0;
                        }
                        else
                        {
                            value.col[0] = json_get_double(eptr, cmeta, cdata);
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as an quaternion, or 0.
*/
quaternion json_get_quaternion(jsonentry *entry, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *dptr=nullptr;
    quaternion value={{0., 0., 0.}, 0.};

    dptr = json_ptr_of_offset(entry->offset,entry->group, cmeta, cdata);
    if (dptr == nullptr)
    {
        return value;
    }
    else
    {
        switch (entry->type)
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
        case JSON_TYPE_TVECTOR:
            {
                rvector tvalue = (rvector)(*(rvector *)(dptr));
                value.d.x = tvalue.col[0];
                value.d.y = tvalue.col[1];
                value.d.z = tvalue.col[2];
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
            value.d.x = atof((char *)dptr);
            break;
        case JSON_TYPE_EQUATION:
            {
                const char *tpointer = (char *)dptr;
                value.d.x = (double)json_equation(tpointer, cmeta, cdata);
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
                        if ((eptr=json_equation_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value.d.x =  0;
                        }
                        else
                        {
                            value.d.x = json_equation(eptr, cmeta, cdata);
                        }
                    }
                    break;
                default:
                    {
                        jsonentry *eptr;
                        if ((eptr=json_entry_of(aptr->handle, cmeta)) == nullptr)
                        {
                            value.d.x =  0;
                        }
                        else
                        {
                            value.d.x = json_get_double(eptr, cmeta, cdata);
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(std::string token, uint16_t index1, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    double value;
    char tstring[5+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token.c_str(), index1);


    value = json_get_double(tstring, cmeta, cdata);

    return (value);
}

//! Return double from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Value cast as a double, or 0.
*/
double json_get_double(std::string token, uint16_t index1, uint16_t index2, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    double value;
    char tstring[9+COSMOS_MAX_NAME];

    if (index1 > 999)
        return 0;

    // Create extended name, shortening if neccessary
    sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token.c_str(), index1,index2);


    value = json_get_double(tstring, cmeta, cdata);

    return (value);
}

//! Return string from name.
/*! If the named value is a string, just copy it. Otherwise, print
whatever numerical value as a string. Return a pointer to an internal
storage buffer for the string. Note: this value is changed each time
you call this function.
    \param token Valid COSMOS Namespace name.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Pointer to a char buffer containing the string.
*/
std::string json_get_string(std::string token, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jsonentry *ptr;
    std::string tstring;

    if ((ptr=json_entry_of(token, cmeta)) != NULL)
    {
        tstring = json_get_string(ptr, cmeta, cdata);
    }

    return tstring;
}

//! Return string from entry.
/*! If the named value is a string, just copy it. Otherwise, print
whatever numerical value as a string. Return a pointer to an internal
storage buffer for the string. Note: this value is changed each time
you call this function.
    \param ptr Pointer to a valid ::jsonentry..
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return Pointer to a char buffer containing the string.
*/
std::string json_get_string(jsonentry *ptr, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    std::string tstring;
    char tbuf[200];

    if (ptr == NULL)
    {
        return (tstring);
    }

    switch (ptr->type)
    {
    case JSON_TYPE_UINT16:
        sprintf(tbuf,"%u",(*(uint16_t *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata))));
        tstring = tbuf;
        break;
    case JSON_TYPE_UINT32:
        sprintf(tbuf,"%u",(*(uint32_t *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata))));
        tstring = tbuf;
        break;
    case JSON_TYPE_INT16:
        sprintf(tbuf,"%d",(*(int16_t *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata))));
        tstring = tbuf;
        break;
    case JSON_TYPE_INT32:
        sprintf(tbuf,"%d",(*(int32_t *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata))));
        tstring = tbuf;
        break;
    case JSON_TYPE_FLOAT:
        sprintf(tbuf,"%.8g",(*(float *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata))));
        tstring = tbuf;
        break;
    case JSON_TYPE_DOUBLE:
    case JSON_TYPE_TIMESTAMP:
        sprintf(tbuf,"%.17g",(*(double *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata)) + .5));
        tstring = tbuf;
        break;
    case JSON_TYPE_STRING:
    case JSON_TYPE_NAME:
        tstring = (char *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata));
        //		strcpy(tbuf,(char *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata)));
        break;
    case JSON_TYPE_POS_ECI:
        cartpos tval = (*(cartpos *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata)));
        sprintf(tbuf, "[%.17g %.17g %.17g] [%.17g %.17g %.17g] [%.17g %.17g %.17g]",
                tval.s.col[0], tval.s.col[1], tval.s.col[2],
                tval.v.col[0], tval.v.col[1], tval.v.col[2],
                tval.a.col[0], tval.a.col[1], tval.a.col[2]);
        tstring = tbuf;
        break;
    }

    return (tstring);
}

//! Set name from double.
/*! If the provided double can in any way be placed in the name it
 * will.
 \param value Double precision value to be stored in the name space.
 \param token Valid COSMOS Namespace name.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Negative error or zero.
*/
int32_t json_set_double_name(double value, char *token, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    utype *nval;
    jsonentry *ptr;

    if ((ptr=json_entry_of(token, cmeta)) == NULL)
        return 0;

    nval = (utype *)(json_ptr_of_offset(ptr->offset,ptr->group, cmeta, cdata));
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

  \return Result of the equation, or NAN.
*/
double json_equation(const char* &ptr, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    std::string equation;
    int32_t iretn;
    jsonhandle handle;

    if ((iretn=json_parse_equation(ptr, equation)) < 0)
        return (NAN);

    if (cmeta.emap.size() == 0)
        return (NAN);

    if ((iretn=json_equation_map(equation, cmeta, &handle)) < 0)
    {
        return (NAN);
    }

    return(json_equation(&cmeta.emap[handle.hash][handle.index], cmeta, cdata));
}

//! Return the results of a known JSON equation handle
/*! Calculate a ::json_equation using already looked up values for the hash and index.
    \param handle Values for hash and index that point to an entry in the map.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Result of the equation, or NAN.
*/
double json_equation(jsonhandle *handle, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    return(json_equation(&cmeta.emap[handle->hash][handle->index], cmeta, cdata));
}

//! Return the results of a known JSON equation entry
/*! Calculate a ::json_equation using already looked up entry from the map.
    \param ptr Pointer to a ::jsonequation from the map.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Result of the equation, or NAN.
*/
double json_equation(jsonequation *ptr, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
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
            a[i] = json_equation(&cmeta.emap[ptr->operand[i].data.hash][ptr->operand[i].data.index], cmeta, cdata);
            break;
        case JSON_OPERAND_NAME:
            a[i] = json_get_double(&cmeta.jmap[ptr->operand[i].data.hash][ptr->operand[i].data.index], cmeta, cdata);
            break;
        }
    }

    switch(ptr->operation)
    {
    case JSON_OPERATION_NOT:
        c = !a[0];
        break;
    case JSON_OPERATION_COMPLEMENT:
        c = ~(uint32_t)(a[0]);
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
        c = a[0] && a[1];
        break;
    case JSON_OPERATION_OR:
        c = a[0] || a[1];
        break;
    case JSON_OPERATION_GT:
        c = a[0] > a[1];
        break;
    case JSON_OPERATION_LT:
        c = a[0] < a[1];
        break;
    case JSON_OPERATION_EQ:
        c = a[0] == a[1];
        break;
    case JSON_OPERATION_POWER:
        c = pow(a[0], a[1]);
    }
    return (c);
}

//! Extract JSON value matching name.
/*! Scan through the provided JSON stream looking for the supplied
    Namespace name. If it is found, return its value as a character
    string.
    \param json A string of JSON data
    \param token The Namespace name to be extracted.
    \return A character string representing the extracted value, otherwise NULL.
*/
std::string json_extract_namedobject(std::string json, std::string token)
{
    std::string tstring;
    const char *ptr;
    int32_t iretn;

    // Look for namespace name in string
    if ((ptr=(char*)strstr(json.c_str(), token.c_str())) == NULL)
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
int32_t json_extract_value(const char *&ptr, std::string &value)
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
std::string json_convert_string(std::string object)
{
    std::string result;
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
double json_convert_double(std::string object)
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
/*! Scan through the provided JSON stream, matching names to the ::json_name_list.
 * for each match that is found, create a ::jsontoken entry and add it to a vector
 * of tokens.
 * \param jstring string containing JSON stream.
 * \param tokens vector of ::jsontoken.
    \param cmeta Reference to ::cosmosmetastruc to use.
 * \return Zero or negative error.
 */
int32_t json_tokenize(std::string jstring, cosmosmetastruc &cmeta, std::vector<jsontoken> &tokens)
{
    const char *cpoint;
    size_t length;
    int32_t iretn;
    jsontoken ttoken;

    std::string val = json_extract_namedobject(jstring, "node_utc");
    if (val.length()!=0) ttoken.utc = json_convert_double(val);
    else
    {
        //Some older sets of telemetry do not include "node_utc" so the utc must be found elsewhere:
        if ((val = json_extract_namedobject(jstring, "node_loc_pos_eci")).length()!=0)
        {
            if ((val=json_extract_namedobject(val, "utc")).length()!=0) ttoken.utc = json_convert_double(val);
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
            if ((iretn = json_tokenize_namedobject(cpoint, cmeta, ttoken)) < 0)
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
    return (iretn);
}

//! Tokenize next JSON Named Pair
/*! Extract the next Named Pair from the provided JSON stream and place it in a ::jsontoken.
 * Leave pointer at the next Object in the string.
 * \param ptr Pointer to a pointer to a JSON stream.
 *    \param cmeta Reference to ::cosmosmetastruc to use.
 * \param token ::jsontoken to return.
 * \return Zero, or a negative error.
*/
int32_t json_tokenize_namedobject(const char* &ptr, cosmosmetastruc &cmeta, jsontoken &token)
{
    int32_t iretn=0;
    std::string ostring;
    uint16_t hash, index;

    if (!(cmeta.jmapped))
    {
        return (JSON_ERROR_NOJMAP);
    }

    // Skip over opening brace
    if (ptr[0] != '{')
    {
        if ((iretn = json_skip_value(ptr)) < 0)
            return (iretn);
        else
            return (JSON_ERROR_SCAN);
    }

    ptr++;

    // Extract string that should hold name of this object.
    if ((iretn = json_extract_string(ptr, ostring)) < 0)
    {
        if (iretn != JSON_ERROR_EOS)
        {
            if ((iretn = json_skip_value(ptr)) < 0)
                return (iretn);
            else
                return (JSON_ERROR_SCAN);
        }
        else
            return (iretn);
    }
    // Calculate hash
    hash = json_hash(ostring);

    // See if there is a match in the ::jsonmap.
    for (index=0; index<cmeta.jmap[hash].size(); ++index)	{
        if (ostring == cmeta.jmap[hash][index].name)
        {
            break;
        }
    }

    if (index == cmeta.jmap[hash].size())
    {
        if ((iretn = json_skip_value(ptr)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return (iretn);
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
                if ((iretn = json_skip_value(ptr)) < 0)
                    return (iretn);
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return (iretn);
        }
        // Skip separator
        if ((iretn = json_skip_character(ptr,':')) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_value(ptr)) < 0)
                    return (iretn);
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return (iretn);
        }
        // Skip white space before value
        if ((iretn = json_skip_white(ptr)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_value(ptr)) < 0)
                    return (iretn);
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return (iretn);
        }
        // Read value
        std::string input;
        if ((iretn = json_extract_value(ptr, input)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_value(ptr)) < 0)
                    return (iretn);
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return (iretn);
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
            return (iretn);
        }
        // Skip over closing brace
        if ((iretn = json_skip_character(ptr,'}')) < 0)
        {
            if ((iretn = json_skip_value(ptr)) < 0)
                return (iretn);
            else
                return (JSON_ERROR_SCAN);
        }
    }

    //	ptr++;
    json_skip_white(ptr);
    return (iretn);
}

//! Parse JSON using Name Space.
/*! Scan through the provided JSON stream, matching names to the ::json_name_list. For
 * each match that is found, load the associated data item with the accompanying data.
 * This function supports complex data types.
    \param jstring A string of JSON data
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Zero or negative error.
*/
int32_t json_parse(std::string jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    const char *cpoint;
    size_t length;
    int32_t iretn;
    uint32_t count = 0;

    length = jstring.size();
    cpoint = &jstring[0];
    while (*cpoint != 0 && *cpoint != '{')
        cpoint++;
    do
    {
        // is this the only reference to endlines?
        if (*cpoint != 0)// && *cpoint != '\r' && *cpoint != '\n')
            //if (*cpoint != 0 && *cpoint != '\r' && *cpoint != '\n')
        {
            if ((iretn = json_parse_namedobject(cpoint, cmeta, cdata)) < 0)
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

    if (!iretn) iretn = count;
    return (iretn);
}

//! Parse next JSON Named Pair
/*! Extract the next Named Pair from the provided JSON stream. Return a pointer to the next
 * Object in the string, and an error flag.
    \param ptr Pointer to a pointer to a JSON stream.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return Zero, or a negative error.
*/
int32_t json_parse_namedobject(const char* &ptr, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint32_t hash, n;
    int32_t iretn=0;
    std::string ostring;

    if (!(cmeta.jmapped))
    {
        return (JSON_ERROR_NOJMAP);
    }

    if (ptr[0] != '{')
    {
        if ((iretn = json_skip_value(ptr)) < 0)
            return (iretn);
        else
            return (JSON_ERROR_SCAN);
    }

    ptr++;

    // Extract string that should hold name of this object.
    if ((iretn = json_extract_string(ptr, ostring)) < 0)
    {
        if (iretn != JSON_ERROR_EOS)
        {
            if ((iretn = json_skip_value(ptr)) < 0)
                return (iretn);
            else
                return (JSON_ERROR_SCAN);
        }
        else
            return (iretn);
    }

    // Calculate hash
    hash = json_hash(ostring);

    // See if there is a match in the ::jsonmap.
    for (n=0; n<cmeta.jmap[hash].size(); ++n)	{
        if (ostring == cmeta.jmap[hash][n].name)
        {
            break;
        }
    }

    if (n == cmeta.jmap[hash].size())	{
        if ((iretn = json_skip_value(ptr)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return (iretn);
        }
        else
            return (JSON_ERROR_NOENTRY);
    }
    else
    {
        if ((iretn = json_skip_white(ptr)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_value(ptr)) < 0)
                    return (iretn);
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return (iretn);
        }
        if ((iretn = json_skip_character(ptr,':')) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_value(ptr)) < 0)
                    return (iretn);
                else
                    return (JSON_ERROR_SCAN);
            }
            else
                return (iretn);
        }
        if ((iretn = json_parse_value(ptr,cmeta.jmap[hash][n].type,cmeta.jmap[hash][n].offset,cmeta.jmap[hash][n].group, cmeta, cdata)) < 0)
        {
            if (iretn != JSON_ERROR_EOS)
            {
                if ((iretn = json_skip_value(ptr)) < 0)
                    return (iretn);
                else
                    return (JSON_ERROR_SCAN);
            }
            else
            {
                return (iretn);
            }
        }
    }

    ptr++;
    json_skip_white(ptr);
    return (iretn);
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

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    if (ptr[0] != character)
        return (JSON_ERROR_SCAN);
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
int32_t json_parse_name(const char* &ptr, std::string& ostring)
{
    int32_t iretn = 0;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    //Skip whitespace before name
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return (iretn);
    }

    //Parse name
    if ((iretn = json_extract_string(ptr, ostring)) < 0)
        return (iretn);

    //Skip whitespace after name
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return (iretn);
    }

    if ((iretn = json_skip_character(ptr, ':')) < 0)
        return (iretn);

    //Skip whitespace after seperator
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return (iretn);
    }

    return (iretn);
}

//! Parse the next JSON equation out of a JSON stream.
/*! Look for a valid JSON equation in the provided JSON stream and copy
 * it to the provided buffer. Otherwise flag an error.
 \param ptr Pointer to a pointer to a JSON stream.
 \param equation Reference to a location to copy the equation.
 \return Zero, otherwise negative error.
*/
int32_t json_parse_equation(const char* &ptr, std::string& equation)
{
    int32_t iretn = 0;
    register uint16_t i2;
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

    return (iretn);
}

//! Parse the next JSON equation operand out of a JSON stream.
/*! Look for a valid JSON equation operand in the provided JSON stream and
 * copy it to a ::jsonoperand.
 \param ptr Pointer to a pointer to a JSON stream.
 \param operand Pointer to a ::jsonoperand.
    \param cmeta Reference to ::cosmosmetastruc to use.
 \return Zero, otherwise negative error.
*/
int32_t json_parse_operand(const char* &ptr, jsonoperand *operand, cosmosmetastruc &cmeta)
{
    std::string tstring;
    int32_t iretn;

    json_skip_white(ptr);
    switch(ptr[0])
    {
    case '(':
        // It's an equation
        if ((iretn=json_parse_equation(ptr, tstring)) < 0)
        {
            return (JSON_ERROR_SCAN);
        }
        if ((iretn=json_equation_map(tstring, cmeta, &operand->data)) < 0)
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
        if ((iretn=json_name_map(tstring, cmeta, operand->data)) < 0)
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
int32_t json_extract_string(const char* &ptr, std::string &ostring)
{
    int32_t iretn = 0;
    register uint32_t i2;
    size_t ilen;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    //Skip whitespace before string
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return (iretn);
    }

    ilen = strlen(ptr);

    // Start of object, get string
    ostring.clear();
    for (i2=1; i2<ilen; i2++)
    {
        if (ptr[i2] == '"')
            break;
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

    return (iretn);
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
        return (iretn);
    }
    ilen = strlen(ptr);

    // First, check for integer: series of digits
    i1 = 0;
    if (ptr[i1] == '-')
        ++i1;
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

    // Third, check for exponent: e or E followed by optional - and series of digits
    if (ptr[i1] == 'e' || ptr[i1] == 'E')
    {
        ++i1;
        if (ptr[i1] == '-')
            ++i1;
        while (i1 < ilen && ptr[i1] >= '0' && ptr[i1] <= '9')
        {
            ++i1;
        }
    }

    // Finally, scan resulting string and move pointer to new location: i1 equals first position after number
    sscanf(ptr,"%lf",number);
    ptr = &ptr[i1];
    return (iretn);
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

//! Skip next value in JSON string
/*! Skip over characters until you reach the next value in a JSON string.
    \param ptr Double pointer to the JSON string
    \return Zero, or negative error.
*/
int32_t json_skip_value(const char* &ptr)
{

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    while (ptr[0] != 0 && ptr[0] != '{')
        ptr++;

    if (ptr[0] == 0 || ptr[1] == 0)
        return (JSON_ERROR_EOS);
    else
        return 0;
}

int32_t json_set_string(std::string val, uint16_t type, ptrdiff_t offset, uint16_t group, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *data;

    data = json_ptr_of_offset(offset,group, cmeta, cdata);
    if (data == nullptr)
    {
        return JSON_ERROR_NAN;
    }

    switch (type)
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

int32_t json_set_number(double val, uint16_t type, ptrdiff_t offset, uint16_t group, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint8_t *data;
    int32_t iretn = 0;

    data = json_ptr_of_offset(offset,group, cmeta, cdata);

    switch (type)
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
    return (iretn);
}

int32_t json_parse_value(const char* &ptr, uint16_t type, ptrdiff_t offset, uint16_t group, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    std::string input;
    int32_t iretn = 0;
    double val;
    cartpos *rp;
    geoidpos *gp;
    spherpos *sp;
    dcmatt *ra;
    qatt *qa;
    uint8_t *data;
    std::string empty;

    if (ptr[0] == 0)
        return (JSON_ERROR_EOS);

    data = json_ptr_of_offset(offset, group, cmeta, cdata);

    //Skip whitespace before value
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return (iretn);
    }

    switch (type)
    {
    case JSON_TYPE_UINT8:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(uint8_t *)data = (uint8_t)val;
        break;

    case JSON_TYPE_INT8:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(int8_t *)data = (int8_t)val;
        break;

    case JSON_TYPE_UINT16:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(uint16_t *)data = (uint16_t)val;
        break;
    case JSON_TYPE_UINT32:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(uint32_t *)data = (uint32_t)val;
        break;
    case JSON_TYPE_INT16:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(int16_t *)data = (int16_t)val;
        break;
    case JSON_TYPE_INT32:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(int32_t *)data = (int32_t)val;
        break;
    case JSON_TYPE_FLOAT:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(float *)data = (float)val;
        break;
    case JSON_TYPE_TIMESTAMP:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(double *)data = (double)val;
        break;
    case JSON_TYPE_DOUBLE:
        if (std::isnan(val=json_equation(ptr, cmeta, cdata)))
        {
            if ((iretn = json_parse_number(ptr,&val)) < 0)
                return (iretn);
        }
        *(double *)data = (double)val;
        break;
    case JSON_TYPE_STRING:
    case JSON_TYPE_NAME:
        if ((iretn = json_extract_string(ptr, input)) < 0)
            return (iretn);
        if (input.size())
        {
            if (type == JSON_TYPE_NAME)
            {
                if (input.size() > COSMOS_MAX_NAME)
                {
                    input.resize(COSMOS_MAX_NAME-1);
                }
            }
            else
            {
                if (input.size() > COSMOS_MAX_DATA)
                {
                    input.resize(COSMOS_MAX_DATA-1);
                }
            }
            strcpy((char *)data, (char*)&input[0]);
        }
        break;
    case JSON_TYPE_GVECTOR:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(gvector,lat),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(gvector,lon),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(gvector,h),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_CVECTOR:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cvector,x),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cvector,y),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cvector,z),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_RVECTOR:
    case JSON_TYPE_TVECTOR:
        if ((iretn = json_skip_character(ptr,'[')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(rvector,col[0]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(rvector,col[1]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(rvector,col[2]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,']')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_QUATERNION:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CVECTOR,offset+(ptrdiff_t)offsetof(quaternion,d),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(quaternion,w),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_RMATRIX:
        if ((iretn = json_skip_character(ptr,'[')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[0]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[1]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[2]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[3]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,']')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_DCM:
        if ((iretn = json_skip_character(ptr,'[')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[0]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[1]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[2]),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,']')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_POS_SELG:
    case JSON_TYPE_POS_GEOD:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr,input)) < 0)
            return (iretn);

        gp = (geoidpos *)(data+(ptrdiff_t)offsetof(geoidpos,utc));
        gp->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cartpos,utc),group, cmeta, cdata)) < 0)
                return (iretn);
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return (iretn);
            if ((iretn = json_parse_name(ptr, empty)) < 0)
                return (iretn);
        }
        if (gp->utc == 0.)
        {
            gp->utc = currentmjd(cdata.node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_GVECTOR,offset+(ptrdiff_t)offsetof(geoidpos,s),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_GVECTOR,offset+(ptrdiff_t)offsetof(geoidpos,v),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_GVECTOR,offset+(ptrdiff_t)offsetof(geoidpos,a),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        gp->pass++;
        break;
    case JSON_TYPE_POS_GEOS:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr,input)) < 0)
            return (iretn);

        sp = (spherpos *)(data+(ptrdiff_t)offsetof(spherpos,utc));
        sp->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cartpos,utc),group, cmeta, cdata)) < 0)
                return (iretn);
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return (iretn);
            if ((iretn = json_parse_name(ptr, empty)) < 0)
                return (iretn);
        }
        if (sp->utc == 0.)
        {
            sp->utc = currentmjd(cdata.node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_SVECTOR,offset+(ptrdiff_t)offsetof(spherpos,s),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_SVECTOR,offset+(ptrdiff_t)offsetof(spherpos,v),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_SVECTOR,offset+(ptrdiff_t)offsetof(spherpos,a),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        sp->pass++;
        break;
    case JSON_TYPE_CARTPOS:
    case JSON_TYPE_POS_GEOC:
    case JSON_TYPE_POS_SELC:
    case JSON_TYPE_POS_ECI:
    case JSON_TYPE_POS_SCI:
    case JSON_TYPE_POS_BARYC:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr,input)) < 0)
            return (iretn);

        rp = (cartpos *)(data+(ptrdiff_t)offsetof(cartpos,utc));
        rp->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cartpos,utc),group, cmeta, cdata)) < 0)
                return (iretn);
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return (iretn);
            if ((iretn = json_parse_name(ptr, empty)) < 0)
                return (iretn);
        }
        if (rp->utc == 0.)
        {
            rp->utc = currentmjd(cdata.node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(cartpos,s),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(cartpos,v),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(cartpos,a),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        rp->pass++;
        break;
    case JSON_TYPE_DCMATT:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DCM,offset+(ptrdiff_t)offsetof(dcmatt,s),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(dcmatt,v),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(dcmatt,a),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        ra = (dcmatt *)(data+(ptrdiff_t)offsetof(dcmatt,utc));
        ra->utc = *(double *)json_ptrto((char *)"node_utc", cmeta, cdata);
        break;
    case JSON_TYPE_QATT:
    case JSON_TYPE_QATT_TOPO:
    case JSON_TYPE_QATT_GEOC:
    case JSON_TYPE_QATT_ICRF:
    case JSON_TYPE_QATT_SELC:
    case JSON_TYPE_QATT_LVLH:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr,input)) < 0)
            return (iretn);

        qa = (qatt *)(data+(ptrdiff_t)offsetof(qatt,utc));
        qa->utc = 0.;
        if (input == "utc")
        {
            if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(qatt,utc),group, cmeta, cdata)) < 0)
                return (iretn);
            if ((iretn = json_skip_character(ptr,',')) < 0)
                return (iretn);
            if ((iretn = json_parse_name(ptr, empty)) < 0)
                return (iretn);
        }
        if (qa->utc == 0.)
        {
            qa->utc = currentmjd(cdata.node.utcoffset);
        }

        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_QUATERNION,offset+(ptrdiff_t)offsetof(qatt,s),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(qatt,v),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_parse_name(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(qatt,a),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        qa->pass++;
        break;
    case JSON_TYPE_HBEAT:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(beatstruc,utc),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_STRING,offset+(ptrdiff_t)offsetof(beatstruc,node),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_STRING,offset+(ptrdiff_t)offsetof(beatstruc,proc),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_INT32,offset+(ptrdiff_t)offsetof(beatstruc,ntype),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_STRING,offset+(ptrdiff_t)offsetof(beatstruc,addr),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_INT32,offset+(ptrdiff_t)offsetof(beatstruc,port),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_INT32,offset+(ptrdiff_t)offsetof(beatstruc,bsz),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(beatstruc,bprd),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_LOC_POS:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(posstruc,utc),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,icrf),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,eci),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,sci),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,geoc),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,selc),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POS_GEOD,offset+(ptrdiff_t)offsetof(posstruc,geod),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POS_GEOD,offset+(ptrdiff_t)offsetof(posstruc,selg),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POS_GEOS,offset+(ptrdiff_t)offsetof(posstruc,geos),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        break;
    case JSON_TYPE_LOC:
        if ((iretn = json_skip_character(ptr,'{')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(locstruc,utc),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_POSSTRUC,offset+(ptrdiff_t)offsetof(locstruc,pos),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_ATTSTRUC,offset+(ptrdiff_t)offsetof(locstruc,att),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,',')) < 0)
            return (iretn);
        if ((iretn = json_extract_string(ptr, empty)) < 0)
            return (iretn);
        if ((iretn = json_skip_white(ptr)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,':')) < 0)
            return (iretn);
        if ((iretn = json_parse_value(ptr, (uint16_t)JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(locstruc,bearth),group, cmeta, cdata)) < 0)
            return (iretn);
        if ((iretn = json_skip_character(ptr,'}')) < 0)
            return (iretn);
        break;
    }

    //Skip whitespace after value
    if ((iretn = json_skip_white(ptr)) < 0)
    {
        return (iretn);
    }

    return (iretn);
}

//! Clear global data structure
/*! Zero out elements of the ::cosmosstruc. The entire structure can be cleared, or the
 * clearing can be confined to either the Dynamic or Static piece. This allows you to remove
 * the effects of previous calls to ::json_parse.
    \param type One of JSON_MAP_ALL, JSON_MAP_AGENT, JSON_MAP_BEACON, JSON_MAP_NODESTATIC, or
    JSON_MAP_NODEDYNAMIC
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return 0, or a negative \ref error.
*/
int32_t json_clear_cosmosstruc(int32_t type, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    switch (type)
    {
    case JSON_STRUCT_NODE:
        memset(&(cdata.node),0,sizeof(nodestruc));
        break;
    case JSON_STRUCT_EVENT:
        memset(cdata.event.data(),0,cdata.event.size()*sizeof(eventstruc));
        break;
    case JSON_STRUCT_PIECE:
        memset(cdata.piece.data(),0,cdata.piece.size()*sizeof(piecestruc));
        break;
    case JSON_STRUCT_DEVICE:
        cdata.device.clear();
        break;
    case JSON_STRUCT_DEVSPEC:
        memset(&(cdata.devspec),0,sizeof(devspecstruc));
        break;
    case JSON_STRUCT_PHYSICS:
        memset(&(cdata.physics),0,sizeof(physicsstruc));
        break;
    case JSON_STRUCT_AGENT:
        memset(cdata.agent.data(),0,cdata.agent.size()*sizeof(agentstruc));
        break;
    case JSON_STRUCT_USER:
        memset(cdata.user.data(),0,cdata.user.size()*sizeof(userstruc));
        break;
    case JSON_STRUCT_PORT:
        memset(cdata.port.data(),0,cdata.port.size()*sizeof(portstruc));
        break;
    case JSON_STRUCT_TARGET:
        memset(cdata.target.data(),0,cdata.target.size()*sizeof(targetstruc));
        break;
    case JSON_STRUCT_GLOSSARY:
        memset(cdata.glossary.data(),0,cdata.glossary.size()*sizeof(glossarystruc));
        break;
    case JSON_STRUCT_TLE:
        memset(cdata.tle.data(),0,cdata.tle.size()*sizeof(tlestruc));
        break;
    case JSON_STRUCT_ALIAS:
        memset(cmeta.alias.data(),0,cmeta.alias.size()*sizeof(aliasstruc));
        break;
    case JSON_STRUCT_EQUATION:
        memset(cmeta.equation.data(),0,cmeta.equation.size()*sizeof(equationstruc));
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
 * \param create_flag Whether or not to create node directory if it doesn't already exist.
    \return 0, or a negative ::error
*/
int32_t json_load_node(std::string node, jsonnode &json, bool create_flag)
{
    int32_t iretn;
    struct stat fstat;
    std::ifstream ifs;
    char *ibuf;
    std::string fname;
    std::string nodepath;

    size_t nodestart;
    if ((nodestart = node.rfind('/')) == std::string::npos)
    {
        nodepath = get_nodedir(node, create_flag);
    }
    else
    {
        nodepath = node;
        node = node.substr(nodestart+1, std::string::npos);
    }

    // First: parse data for summary information - includes piece_cnt, device_cnt and port_cnt
    fname = nodepath + "/node.ini";

    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        std::cerr << "error " << DATA_ERROR_NODES_FOLDER << ": could not find cosmos/nodes folder" << std::endl;
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
    else
    {
        fname = nodepath + "/tle.ini";

        if (!stat(fname.c_str(),&fstat) && fstat.st_size)
        {
            int32_t iretn;
            cartpos eci;
            std::vector <tlestruc> tles;
            iretn = load_lines(fname, tles);
            if (iretn > 0)
            {
                if ((iretn=lines2eci(currentmjd()-10./86400., tles, eci)) == 0)
                {
                    json_out_ecipos(json.state, eci);
                }
            }
        }
    }

    // Set node_utcstart
    fname = nodepath + "/node_utcstart.ini";
    double utcstart;
    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        // First time, so write it
        utcstart = currentmjd();
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
        }
        else
        {
            iretn = fscanf(ifp, "%lg", &utcstart);
            if (iretn != 1)
            {
                utcstart = currentmjd();
            }
            fclose(ifp);
        }
    }
    json_out_utcstart(json.utcstart, utcstart);

    // Second: enter information for pieces
    fname = nodepath + "/pieces.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        return (NODE_ERROR_NODE);
    }

    ifs.open(fname);
    if (!ifs.is_open())
    {
        return (NODE_ERROR_NODE);
    }

    ibuf = (char *)calloc(1,fstat.st_size+1);
    ifs.read(ibuf, fstat.st_size);
    ifs.close();
    ibuf[fstat.st_size] = 0;
    json.pieces = ibuf;
    free(ibuf);

    // Third: enter information for all devices
    fname = nodepath + "/devices_general.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        return (NODE_ERROR_NODE);
    }

    ifs.open(fname);
    if (!ifs.is_open())
    {
        return (NODE_ERROR_NODE);
    }

    ibuf = (char *)calloc(1,fstat.st_size+1);
    ifs.read(ibuf, fstat.st_size);
    ifs.close();
    ibuf[fstat.st_size] = 0;
    json.devgen = ibuf;
    free(ibuf);

    // Fourth: enter information for specific devices
    fname = nodepath + "/devices_specific.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        return (NODE_ERROR_NODE);
    }

    ifs.open(fname);
    if (!ifs.is_open())
    {
        return (NODE_ERROR_NODE);
    }

    ibuf = (char *)calloc(1,fstat.st_size+1);
    ifs.read(ibuf, fstat.st_size);
    ifs.close();
    ibuf[fstat.st_size] = 0;
    json.devspec = ibuf;
    free(ibuf);

    // Fifth: enter information for ports
    // Resize, then add names for ports
    fname = nodepath + "/ports.ini";
    if ((iretn=stat(fname.c_str(),&fstat)) == -1)
    {
        return (NODE_ERROR_NODE);
    }

    ifs.open(fname);
    if (!ifs.is_open())
    {
        return (NODE_ERROR_NODE);
    }

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
            json.aliases = ibuf;
            free(ibuf);
        }
    }
    return 0;
}

//! Setup JSON Namespace using Node description JSON
/*! Create an entry in the JSON mapping tables between each name in the Name Space and the
 * \ref cosmosstruc. Load descriptive information from a structure of JSON descriptions.
 * \param json Structure containing JSON descriptions.
 * \param cinfo Pointer to cinfo ::cosmosstruc.
 * \param create_flag Whether or not to create node directory if it doesn't already exist.
   \return 0, or a negative ::error
*/
int32_t json_setup_node(jsonnode json, cosmosstruc *cinfo, bool create_flag)
{
    int32_t iretn;
    struct stat fstat;
    std::ifstream ifs;
    std::string fname;

    if (!cinfo->meta.jmapped)
        return (JSON_ERROR_NOJMAP);

    cinfo->pdata.node.utcoffset = 0.;
    // First: parse data for summary information - includes piece_cnt, device_cnt and port_cnt
    if (!json.node.empty())
    {
        if ((iretn = json_parse(json.node, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return (iretn);
        }
    }
    else
    {
        return NODE_ERROR_NODE;
    }

    std::string nodepath;
    std::string node = cinfo->pdata.node.name;
    nodepath = get_nodedir(node, create_flag);

    // 1A: load state vector, if it is present
    if (!json.state.empty())
    {
        if ((iretn = json_parse(json.state, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return (iretn);
        }
        loc_update(&cinfo->pdata.node.loc);
    }

    // Set node_utcstart
    if (!json.utcstart.empty())
    {
        if ((iretn = json_parse(json.utcstart, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
        {
            return (iretn);
        }
    }

    // Second: enter information for pieces
    // Resize, then add entries to map for pieces
    if (cinfo->pdata.node.piece_cnt)
    {
        cinfo->pdata.piece.resize(cinfo->pdata.node.piece_cnt);
        if (cinfo->pdata.piece.size() != cinfo->pdata.node.piece_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }
        for (uint16_t i=0; i<cinfo->pdata.node.piece_cnt; i++)
        {
            //Add relevant names to namespace
            json_addpieceentry(i, cinfo->meta);
            // Initialize to no component
            cinfo->pdata.piece[i].cidx = DEVICE_TYPE_NONE;
        }

        // Parse data for piece information
        if (!json.pieces.empty())
        {
            if ((iretn = json_parse(json.pieces, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return (iretn);
            }
        }

        // Third: enter information for all devices
        // Resize, then add entries to map for devices
        cinfo->pdata.device.resize(cinfo->pdata.node.device_cnt);
        if (cinfo->pdata.device.size() != cinfo->pdata.node.device_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }

        // Add entries to map for Devices and set pointers in nodestruc for comp
        for (uint16_t i=0; i< cinfo->pdata.node.device_cnt; i++)
        {
            // Add relevant names for generic device to namespace
            json_addcompentry(i, cinfo->meta, cinfo->pdata);
            // Initialize to no port
            cinfo->pdata.device[i].all.gen.portidx = PORT_TYPE_NONE;
        }

        // Parse data for general device information
        if (!json.devgen.empty())
        {
            if ((iretn = json_parse(json.devgen, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return (iretn);
            }
        }

        // Fourth: enter information for specific devices
        // Add entries to map for Devices specific information
        for (uint16_t i=0; i< cinfo->pdata.node.device_cnt; i++)
        {
            json_adddeviceentry(i, cinfo->meta, cinfo->pdata);
        }

        // Parse data for specific device information
        if (!json.devspec.empty())
        {
            if ((iretn = json_parse(json.devspec, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return (iretn);
            }
        }

        // Clean up any errors
        for (uint16_t i=0; i< cinfo->pdata.node.device_cnt; i++)
        {
            cinfo->pdata.device[i].all.gen.cidx = i;
        }

        // Fifth: enter information for ports
        // Resize, then add names for ports
        cinfo->pdata.port.resize(cinfo->pdata.node.port_cnt);
        if (cinfo->pdata.port.size() != cinfo->pdata.node.port_cnt)
        {
            return (AGENT_ERROR_MEMORY);
        }

        for (uint16_t i=0; i<cinfo->pdata.node.port_cnt; i++)
        {
            json_addentry("port_name",i,65535u,(ptrdiff_t)offsetof(portstruc,name)+i*sizeof(portstruc),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_PORT, cinfo->meta);
            json_addentry("port_type",i,65535u,(ptrdiff_t)offsetof(portstruc,type)+i*sizeof(portstruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_PORT, cinfo->meta);
        }

        // Parse data for port information
        if (!json.ports.empty())
        {
            if ((iretn = json_parse(json.ports, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return (iretn);
            }
        }

        node_calc(cinfo->pdata);

        //! Load targeting information
        if (!json.targets.empty())
        {
            if ((iretn = json_parse(json.targets, cinfo->meta, cinfo->pdata)) < 0 && iretn != JSON_ERROR_EOS)
            {
                return (iretn);
            }
        }

        //! Load alias map
        fname = nodepath + "/aliases.ini";
        if ((iretn=stat(fname.c_str(),&fstat)) == 0)
        {
            ifs.open(fname);
            if (ifs.is_open())
            {
                std::string alias;
                while (std::getline(ifs, alias, ' '))
                {
                    std::string cname;
                    std::getline(ifs, cname);
                    json_addentry(alias, cname, cinfo->meta);
                } ;
            }
        }

    }

    cinfo->json = json;

    return 0;
}

//! Setup JSON Namespace using file.
/*! Create an entry in the JSON mapping tables between each name in the Name Space and the
 * \ref cosmosstruc. Load descriptive information from files in a Node directory of the goven name.
 *	\param node Name and/or path of node directory. If name, then a path will be created
 * based on nodedir setting. If path, then name will be extracted from the end.
 *	\param cinfo Pointer to cinfo ::cosmosstruc.
 * \param create_flag Whether or not to create node directory if it doesn't already exist.
    \return 0, or a negative ::error
*/
int32_t json_setup_node(std::string node, cosmosstruc *cinfo, bool create_flag)
{
    int32_t iretn;

    if (!cinfo->meta.jmapped)
        return (JSON_ERROR_NOJMAP);

    jsonnode json;
    iretn = json_load_node(node, json, create_flag);
    if (iretn < 0)
    {
        return iretn;
    }

    iretn = json_setup_node(json, cinfo, create_flag);
    if (iretn < 0)
    {
        return iretn;
    }

    if (cinfo->pdata.node.type == NODE_TYPE_SUN)
    {
        jplpos(JPL_EARTH, JPL_SUN, currentmjd(cinfo->pdata.node.utcoffset), &cinfo->pdata.node.loc.pos.eci);
        cinfo->pdata.node.loc.pos.eci.pass++;
        pos_eci(&cinfo->pdata.node.loc);
    }

    if (cinfo->pdata.node.type == NODE_TYPE_MOON)
    {
        jplpos(JPL_EARTH, JPL_MOON, currentmjd(cinfo->pdata.node.utcoffset), &cinfo->pdata.node.loc.pos.eci);
        cinfo->pdata.node.loc.pos.eci.pass++;
        pos_eci(&cinfo->pdata.node.loc);
    }

    if (cinfo->pdata.node.type == NODE_TYPE_MARS)
    {
        jplpos(JPL_EARTH, JPL_MARS, currentmjd(cinfo->pdata.node.utcoffset), &cinfo->pdata.node.loc.pos.eci);
        cinfo->pdata.node.loc.pos.eci.pass++;
        pos_eci(&cinfo->pdata.node.loc);
    }

    return 0;
}

//! Save Node entries to disk
/*! Create all of the initialization files that represent the Node in the provided
 * ::cosmosstruc.
 * \param cmeta Reference to ::cosmosmetastruc to use.
 * \param cdata Reference to ::cosmosdatastruc to use.
 * \return Zero if successful, otherwise negative error.
 */
int32_t json_dump_node(cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    std::string jst;

    // Node
    std::string output = json_node(jst, cmeta, cdata);
    std::string fileloc = get_nodedir(cdata.node.name);
    if (fileloc.empty())
    {
        return DATA_ERROR_NODES_FOLDER;
    }
    std::string filename = fileloc + "/node.ini";
    FILE *file = fopen(filename.c_str(), "w");
    if (file == NULL)
    {
        return -errno;
    }
    fputs(output.c_str(), file);
    fclose(file);

    // Pieces
    output = json_pieces(jst, cmeta, cdata);
    filename = fileloc + "/pieces.ini";
    file = fopen(filename.c_str(), "w");
    if (file == NULL)
    {
        return -errno;
    }
    fputs(output.c_str(), file);
    fclose(file);

    // General Devices
    output = json_devices_general(jst, cmeta, cdata);
    filename = fileloc + "/devices_general.ini";
    file = fopen(filename.c_str(), "w");
    if (file == NULL)
    {
        return -errno;
    }
    fputs(output.c_str(), file);
    fclose(file);

    // Specific Devices
    output = json_devices_specific(jst, cmeta, cdata);
    filename = fileloc + "/devices_specific.ini";
    file = fopen(filename.c_str(), "w");
    if (file == NULL)
    {
        return -errno;
    }
    fputs(output.c_str(), file);
    fclose(file);

    // Ports
    output = json_ports(jst, cmeta, cdata);
    filename = fileloc + "/ports.ini";
    file = fopen(filename.c_str(), "w");
    if (file == NULL)
    {
        return -errno;
    }
    fputs(output.c_str(), file);
    fclose(file);

    // Aliases
    if (cmeta.alias.size() || cmeta.equation.size())
    {
        filename = fileloc + "/aliases.ini";
        file = fopen(filename.c_str(), "w");
        if (file == NULL)
        {
            return -errno;
        }
        for (aliasstruc &alias : cmeta.alias)
        {
            fprintf(file, "%s %s\n", alias.name.c_str(), cmeta.jmap[alias.handle.hash][alias.handle.index].name.c_str());
        }
        for (equationstruc &equation : cmeta.equation)
        {
            fprintf(file, "%s %s\n", equation.name.c_str(), equation.value.c_str());
        }
        fclose(file);
    }

    return 0;
}

//! Add base entries to JMAP
/*! Add all of the base entries to the Namespace map.
*	\param cmeta Reference to ::cosmosmetastruc to use.
    \return The current number of entries, if successful, 0 if the entry could not be
 */
uint16_t json_addbaseentry(cosmosmetastruc &cmeta)
{
    // User structure
    json_addentry("user_node", UINT16_MAX, UINT16_MAX,offsetof(userstruc,node),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_USER, cmeta);
    json_addentry("user_name", UINT16_MAX, UINT16_MAX,offsetof(userstruc,name),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_USER, cmeta);
    json_addentry("user_tool", UINT16_MAX, UINT16_MAX,offsetof(userstruc,tool),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_USER, cmeta);
    json_addentry("user_cpu", UINT16_MAX, UINT16_MAX,offsetof(userstruc,cpu),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_USER, cmeta);


    // Agent structure
    json_addentry("agent_addr", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,addr),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_aprd", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,aprd),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_beat", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat),COSMOS_SIZEOF(beatstruc), (uint16_t)JSON_TYPE_HBEAT,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_bprd", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,bprd),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_bsz", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,bsz), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_node", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,node),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_ntype", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,ntype), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_pid", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,pid),4, (uint16_t)JSON_TYPE_INT32,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_port", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,port), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_proc", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,proc),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_stateflag", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,stateflag), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_user", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,user),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_utc", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,utc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_cpu", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,cpu),COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_memory", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,memory),COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_AGENT, cmeta);
    json_addentry("agent_jitter", UINT16_MAX, UINT16_MAX,offsetof(agentstruc,beat)+offsetof(beatstruc,jitter),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_AGENT, cmeta);

    // Event structure
    json_addentry("event_cbytes", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,cbytes), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_cenergy", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,cenergy), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_cmass", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,cmass), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_condition", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,condition),COSMOS_MAX_DATA, (uint16_t)JSON_TYPE_STRING,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_ctime", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,ctime),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_data", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,data),COSMOS_MAX_DATA, (uint16_t)JSON_TYPE_STRING,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_dbytes", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,dbytes), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_denergy", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,denergy), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_dmass", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,dmass), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_dtime", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,dtime),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_flag", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,flag), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_name", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,name),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_node", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,node),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_type", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,type), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_user", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,user),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_utc", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,utc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_utcexec", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,utcexec),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_EVENT, cmeta);
    json_addentry("event_value", UINT16_MAX, UINT16_MAX,offsetof(longeventstruc,value),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_EVENT, cmeta);

    // Physics structure
    json_addentry("physics_dt", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,dt),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_dtj", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,dtj),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_mjdbase", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mjdbase),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_mjdaccel", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mjdaccel),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_mjddiff", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mjddiff),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_mode", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mode),4, (uint16_t)JSON_TYPE_INT32,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_thrust", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,thrust),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_adrag", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,adrag),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_rdrag", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,rdrag),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_atorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,atorque),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_rtorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,rtorque),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_gtorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,gtorque),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_htorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,htorque),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_ctorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ctorque),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_ftorque", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_ftorque_x", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque.col[0]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_ftorque_y", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque.col[1]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_ftorque_z", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,ftorque.col[2]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_hcap", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,hcap), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_area", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,area), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_moi", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,moi),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_com", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,com),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_mass", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,mass), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PHYSICS, cmeta);
    json_addentry("physics_heat", UINT16_MAX, UINT16_MAX,offsetof(physicsstruc,heat), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PHYSICS, cmeta);

    // Node Structure
    json_addentry("node_utcoffset", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,utcoffset),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_name", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,name),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_type", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,type), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_state", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,state), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_hcap", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,hcap), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_mass", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,mass), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_area", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,area), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_moi", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,moi),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_battcap", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,battcap), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_charging", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,charging), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_powgen", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,powgen), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_powuse", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,powuse), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_battlev", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,battlev), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_utc", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,utc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_utcstart", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,utcstart),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc),COSMOS_SIZEOF(locstruc), (uint16_t)JSON_TYPE_LOC,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_utc", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.utc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos),COSMOS_SIZEOF(posstruc), (uint16_t)JSON_TYPE_POSSTRUC,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geod", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geod),COSMOS_SIZEOF(geoidpos), (uint16_t)JSON_TYPE_POS_GEOD,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geod_v_lat", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geod.s.lat),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geod_v_lon", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geod.s.lon),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geod_v_h", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geod.s.h),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geod_s_lat", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geod.s.lat),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geod_s_lon", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geod.s.lon),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geod_s_h", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geod.s.h),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geoc", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geoc),COSMOS_SIZEOF(cartpos), (uint16_t)JSON_TYPE_POS_GEOC,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geoc_v_x", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geoc.v.col[0]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geoc_v_y", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geoc.v.col[1]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geoc_v_z", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geoc.v.col[2]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geoc_s_x", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geoc.s.col[0]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geoc_s_y", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geoc.s.col[1]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geoc_s_z", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geoc.s.col[2]),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_geos", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.geos),COSMOS_SIZEOF(spherpos), (uint16_t)JSON_TYPE_POS_GEOS,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_eci", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.eci),COSMOS_SIZEOF(cartpos), (uint16_t)JSON_TYPE_POS_ECI,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_eci_s", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.eci.s),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_eci_v", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.eci.v),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_eci_a", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.eci.a),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_sci", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.sci),COSMOS_SIZEOF(cartpos), (uint16_t)JSON_TYPE_POS_SCI,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_selc", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.selc),COSMOS_SIZEOF(cartpos), (uint16_t)JSON_TYPE_POS_SELC,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_selg", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.selg),COSMOS_SIZEOF(geoidpos), (uint16_t)JSON_TYPE_POS_SELG,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_icrf", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.icrf),COSMOS_SIZEOF(cartpos), (uint16_t)JSON_TYPE_POS_BARYC,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_sunsize", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.sunsize), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_sunradiance", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.sunradiance), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_earthsep", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.earthsep), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_pos_moonsep", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.pos.moonsep), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att),COSMOS_SIZEOF(attstruc), (uint16_t)JSON_TYPE_ATTSTRUC,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_icrf", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.icrf),COSMOS_SIZEOF(qatt), (uint16_t)JSON_TYPE_QATT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_icrf_s", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.icrf.s),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_icrf_v", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.icrf.v),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_icrf_a", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.icrf.a),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_topo", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.topo),COSMOS_SIZEOF(qatt), (uint16_t)JSON_TYPE_QATT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_topo_s", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.topo.s),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_topo_v", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.topo.v),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_topo_a", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.topo.a),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_geoc", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.geoc),COSMOS_SIZEOF(qatt), (uint16_t)JSON_TYPE_QATT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_geoc_s", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.geoc.s),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_geoc_v", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.geoc.v),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_geoc_a", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.geoc.a),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_lvlh", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.lvlh),COSMOS_SIZEOF(qatt), (uint16_t)JSON_TYPE_QATT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_lvlh_s", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.lvlh.s),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_lvlh_v", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.lvlh.v),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_lvlh_a", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.lvlh.a),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_selc", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.selc),COSMOS_SIZEOF(qatt), (uint16_t)JSON_TYPE_QATT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_selc_s", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.selc.s),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_selc_v", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.selc.v),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_att_selc_a", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.att.selc.a),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_loc_bearth", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,loc.bearth),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_azfrom", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,azfrom), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_azto", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,azto), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_elfrom", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,elfrom), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_elto", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,elto), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_range", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,range), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_NODE, cmeta);
    json_addentry("comp_cnt", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,device_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("piece_cnt", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,piece_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("port_cnt", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,port_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("target_cnt", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,target_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("node_glossary_cnt", UINT16_MAX, UINT16_MAX,offsetof(nodestruc,glossary_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_NODE, cmeta);
    json_addentry("device_ant_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,ant_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_batt_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,batt_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_bus_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,bus_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_cam_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,cam_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_cpu_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,cpu_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_disk_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,disk_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_gps_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,gps_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_htr_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,htr_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_imu_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,imu_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_mcc_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,mcc_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_motr_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,motr_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_mtr_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,mtr_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_pload_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,pload_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_prop_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,prop_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_psen_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,psen_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_rot_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,rot_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_rw_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,rw_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_rxr_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,rxr_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_ssen_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,ssen_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_strg_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,strg_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_stt_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,stt_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_suchi_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,suchi_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_swch_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,swch_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_tcu_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,tcu_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_tcv_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,tcv_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_telem_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,telem_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_thst_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,thst_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_tsen_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,tsen_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);
    json_addentry("device_txr_cnt", UINT16_MAX, UINT16_MAX,offsetof(devspecstruc,txr_cnt), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVSPEC, cmeta);

    return (cmeta.jmapped);
}

//! Add piece entry.
/*! Add an entry for piece number i to the JSON Namespace map.
 \param i Piece number.
*	\param cmeta Reference to ::cosmosmetastruc to use.
    \return The current number of entries, if successful, 0 if the entry could not be
 */
uint16_t json_addpieceentry(uint16_t i, cosmosmetastruc &cmeta)
{
    json_addentry("piece_name",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,name)+i*sizeof(piecestruc),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_type",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,type)+i*sizeof(piecestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_cidx",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,cidx)+i*sizeof(piecestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_mass",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,mass)+i*sizeof(piecestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_emi",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,emi)+i*sizeof(piecestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_abs",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,abs)+i*sizeof(piecestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_dim",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,dim)+i*sizeof(piecestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_hcap",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,hcap)+i*sizeof(piecestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_hcon",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,hcon)+i*sizeof(piecestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_area",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,area)+i*sizeof(piecestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_com",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,centroid)+i*sizeof(piecestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PIECE, cmeta);
    json_addentry("piece_pnt_cnt",i, UINT16_MAX, (ptrdiff_t)offsetof(piecestruc,pnt_cnt)+i*sizeof(piecestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_PIECE, cmeta);
    for (uint16_t j=0; j<MAXPNT; ++j)
    {
        json_addentry("piece_pnt",i,j,(ptrdiff_t)offsetof(piecestruc,points)+i*sizeof(piecestruc)+j*sizeof(rvector),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_PIECE, cmeta);
    }

    return (cmeta.jmapped);
}

//! Add component entry.
/*! Add an entry for component number i to the JSON Namespace map.
 \param i Component number.
*	\param cmeta Reference to ::cosmosmetastruc to use.
*	\param cdata Reference to ::cosmosdatastruc to use.
    \return The current number of entries, if successful, 0 if the entry could not be
 */
uint16_t json_addcompentry(uint16_t i, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    cdata.device[i].all.gen.cidx = i;

    json_addentry("comp_type",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,type)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_model",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,model)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_flag",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,flag)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_addr",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,addr)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_cidx",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_didx",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,didx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_pidx",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,pidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_bidx",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,bidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_portidx",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,portidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_namp",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,namp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_nvolt",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,volt)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_amp",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,amp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_volt",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,volt)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_temp",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
    json_addentry("comp_utc",i, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
    cdata.devspec.all.push_back((allstruc *)&cdata.device[i].all);

    return (cmeta.jmapped);
}

//! Add device entry.
/*! Add entries specific to device number i to the JSON Namespace map.
 \param i Device number.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return The current number of entries, if successful, 0 if the entry could not be added.
 */
uint16_t json_adddeviceentry(uint16_t i, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    if (i >= cdata.device.size())
    {
        return 0;
    }

    uint16_t didx = cdata.device[i].all.gen.didx;
    switch (cdata.device[i].all.gen.type)
    {
    case DEVICE_TYPE_TELEM:
        json_addentry("device_telem_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_type",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,type)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vuint8",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vuint8)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint8_t), (uint16_t)JSON_TYPE_UINT8,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vint8",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vint8)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint8_t), (uint16_t)JSON_TYPE_INT8,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vuint16",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vuint16)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vint16",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vint16)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_INT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vuint32",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vuint32)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vint32",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vint32)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_INT32,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vfloat",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vfloat)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vdouble",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vdouble)+i*sizeof(devicestruc), COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_telem_vstring",didx, UINT16_MAX, (ptrdiff_t)offsetof(telemstruc,vstring)+i*sizeof(devicestruc), COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_STRING,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.telem.push_back((telemstruc *)&cdata.device[i].telem);
        cdata.devspec.telem_cnt = (uint16_t)cdata.devspec.telem.size();
        break;
    case DEVICE_TYPE_PLOAD:
        json_addentry("device_pload_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_pload_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_pload_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(ploadstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_pload_bps",didx, UINT16_MAX, (ptrdiff_t)offsetof(ploadstruc,bps)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_pload_key_cnt",didx, UINT16_MAX, (ptrdiff_t)offsetof(ploadstruc,key_cnt)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        for (uint16_t j=0; j<MAXPLOADKEYCNT; j++)
        {
            json_addentry("device_pload_keyidx",didx,j,(ptrdiff_t)offsetof(ploadstruc,keyidx)+i*sizeof(devicestruc)+j*sizeof(uint16_t), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
            json_addentry("device_pload_keyval",didx,j,(ptrdiff_t)offsetof(ploadstruc,keyval)+i*sizeof(devicestruc)+j*sizeof(float), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        }
        cdata.devspec.pload.push_back((ploadstruc *)&cdata.device[i].pload);
        cdata.devspec.pload_cnt = (uint16_t)cdata.devspec.pload.size();
        break;
        //! Elevation and Azimuth Sun Sensor
    case DEVICE_TYPE_SSEN:
        json_addentry("device_ssen_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(genstruc,cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_qva",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,qva)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_qvb",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,qvb)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_qvc",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,qvc)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_qvd",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,qvd)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_azimuth",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,azimuth)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ssen_elevation",didx, UINT16_MAX, (ptrdiff_t)offsetof(ssenstruc,elevation)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.ssen.push_back((ssenstruc *)&cdata.device[i].ssen);
        cdata.devspec.ssen_cnt = (uint16_t)cdata.devspec.ssen.size();
        break;
        //! Inertial Measurement Unit
    case DEVICE_TYPE_IMU:
        json_addentry("device_imu_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_accel",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,accel)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_accel_x",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,accel)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_accel_y",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,accel)+sizeof(double)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_accel_z",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,accel)+2*sizeof(double)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_omega",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,omega)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_mag",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,mag)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_mag_x",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,mag)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_mag_y",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,mag)+sizeof(double)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_mag_z",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,mag)+2*sizeof(double)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_imu_bdot",didx, UINT16_MAX, (ptrdiff_t)offsetof(imustruc,bdot)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.imu.push_back((imustruc *)&cdata.device[i].imu);
        cdata.devspec.imu_cnt = (uint16_t)cdata.devspec.imu.size();
        break;
        //! Reaction Wheel
    case DEVICE_TYPE_RW:
        json_addentry("device_rw_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_mom",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,mom)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_mxomg",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,mxomg)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_mxalp",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,mxalp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_tc",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,tc)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_omg",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,omg)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_alp",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,alp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_romg",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,romg)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rw_ralp",didx, UINT16_MAX, (ptrdiff_t)offsetof(rwstruc,ralp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.rw.push_back((rwstruc *)&cdata.device[i].rw);
        cdata.devspec.rw_cnt = (uint16_t)cdata.devspec.rw.size();
        break;
        //! Magnetic Torque Rod
    case DEVICE_TYPE_MTR:
        json_addentry("device_mtr_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mtr_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mtr_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mtr_mxmom",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,mxmom)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mtr_tc",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,tc)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mtr_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        for (uint16_t j=0; j<7; j++)
        {
            json_addentry("device_mtr_npoly",didx,j,(ptrdiff_t)offsetof(mtrstruc,npoly)+i*sizeof(devicestruc)+j*sizeof(float), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
            json_addentry("device_mtr_ppoly",didx,j,(ptrdiff_t)offsetof(mtrstruc,ppoly)+i*sizeof(devicestruc)+j*sizeof(float), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        }
        json_addentry("device_mtr_mom",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,mom)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mtr_rmom",didx, UINT16_MAX, (ptrdiff_t)offsetof(mtrstruc,rmom)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.mtr.push_back((mtrstruc *)&cdata.device[i].mtr);
        cdata.devspec.mtr_cnt = (uint16_t)cdata.devspec.mtr.size();
        break;
        //! Camera
    case DEVICE_TYPE_CAM:
        json_addentry("device_cam_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cam_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cam_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cam_pwidth",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,pwidth)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cam_pheight",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,pheight)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cam_width",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,width)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cam_height",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,height)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cam_flength",didx, UINT16_MAX, (ptrdiff_t)offsetof(camstruc,flength)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.cam.push_back((camstruc *)&cdata.device[i].cam);
        cdata.devspec.cam_cnt = (uint16_t)cdata.devspec.cam.size();
        break;
        //! Processing Unit
    case DEVICE_TYPE_CPU:
        json_addentry("device_cpu_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_uptime",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,uptime)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_maxgib",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,maxgib)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_maxload",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,maxload)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_load",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,load)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_gib",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,gib)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_cpu_boot_count",didx, UINT16_MAX, (ptrdiff_t)offsetof(cpustruc,boot_count)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.cpu.push_back((cpustruc *)&cdata.device[i].cpu);
        cdata.devspec.cpu_cnt = (uint16_t)cdata.devspec.cpu.size();
        break;
    case DEVICE_TYPE_DISK:
        json_addentry("device_disk_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(diskstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_disk_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(diskstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_disk_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(diskstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_disk_maxgib",didx, UINT16_MAX, (ptrdiff_t)offsetof(diskstruc,maxgib)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_disk_gib",didx, UINT16_MAX, (ptrdiff_t)offsetof(diskstruc,gib)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.disk.push_back((diskstruc *)&cdata.device[i].disk);
        cdata.devspec.disk_cnt = (uint16_t)cdata.devspec.disk.size();
        break;
        //! GPS Unit
    case DEVICE_TYPE_GPS:
        json_addentry("device_gps_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dutc",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dutc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocs",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocs)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocs_x",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocs.col[0])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocs_y",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocs.col[1])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocs_z",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocs.col[2])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocv",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocv)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocv_x",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocv.col[0])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocv_y",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocv.col[1])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geocv_z",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geocv.col[2])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocs",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocs)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocs_x",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocs.col[0])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocs_y",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocs.col[1])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocs_z",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocs.col[2])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocv",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocv)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocv_x",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocv.col[0])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocv_y",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocv.col[1])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeocv_z",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeocv.col[2])+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geods",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geods)+i*sizeof(devicestruc),COSMOS_SIZEOF(gvector), (uint16_t)JSON_TYPE_GVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geods_lat",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geods.lat)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geods_lon",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geods.lon)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geods_h",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geods.h)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geodv",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geodv)+i*sizeof(devicestruc),COSMOS_SIZEOF(gvector), (uint16_t)JSON_TYPE_GVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geodv_lat",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geodv.lat)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geodv_lon",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geodv.lon)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_geodv_h",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,geodv.h)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeods",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeods)+i*sizeof(devicestruc),COSMOS_SIZEOF(gvector), (uint16_t)JSON_TYPE_GVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeods_lat",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeods.lat)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeods_lon",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeods.lon)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeods_h",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeods.h)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeodv",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeodv)+i*sizeof(devicestruc),COSMOS_SIZEOF(gvector), (uint16_t)JSON_TYPE_GVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeodv_lat",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeodv.lat)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeodv_lon",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeodv.lon)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_dgeodv_h",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,dgeodv.h)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_heading",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,heading)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_sats_used",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,sats_used)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_sats_visible",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,sats_visible)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_time_status",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,time_status)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_position_type",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,position_type)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_gps_solution_status",didx, UINT16_MAX, (ptrdiff_t)offsetof(gpsstruc,solution_status)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);

        cdata.devspec.gps.push_back((gpsstruc *)&cdata.device[i].gps);
        cdata.devspec.gps_cnt = (uint16_t)cdata.devspec.gps.size();
        break;
        //! Antenna
    case DEVICE_TYPE_ANT:
        json_addentry("device_ant_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(antstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ant_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(antstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ant_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(antstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ant_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(antstruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ant_azim",didx, UINT16_MAX, (ptrdiff_t)offsetof(antstruc,azim)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ant_elev",didx, UINT16_MAX, (ptrdiff_t)offsetof(antstruc,elev)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_ant_minelev",didx, UINT16_MAX, (ptrdiff_t)offsetof(antstruc,minelev)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.ant.push_back((antstruc *)&cdata.device[i].ant);
        cdata.devspec.ant_cnt = (uint16_t)cdata.devspec.ant.size();
        break;
        //! Radio Receiver
    case DEVICE_TYPE_RXR:
        json_addentry("device_rxr_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_opmode",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,opmode)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_freq",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,freq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_freq",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,freq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_maxfreq",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,maxfreq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_minfreq",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,minfreq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_power",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,power)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rxr_band",didx, UINT16_MAX, (ptrdiff_t)offsetof(rxrstruc,band)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.rxr.push_back((rxrstruc *)&cdata.device[i].rxr);
        cdata.devspec.rxr_cnt = (uint16_t)cdata.devspec.rxr.size();
        break;
        //! Radio Transmitter
    case DEVICE_TYPE_TXR:
        json_addentry("device_txr_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_opmode",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,opmode)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_freq",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,freq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_freq",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,freq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_maxfreq",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,maxfreq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_minfreq",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,minfreq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_power",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,power)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_txr_maxpower",didx, UINT16_MAX, (ptrdiff_t)offsetof(txrstruc,maxpower)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.txr.push_back((txrstruc *)&cdata.device[i].txr);
        cdata.devspec.txr_cnt = (uint16_t)cdata.devspec.txr.size();
        break;
        //! Radio Transceiver
    case DEVICE_TYPE_TCV:
        json_addentry("device_tcv_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_opmode",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,opmode)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_freq",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,freq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_maxfreq",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,maxfreq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_minfreq",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,minfreq)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_powerin",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,powerin)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_powerout",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,powerout)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_maxpower",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,maxpower)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcv_band",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcvstruc,band)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.tcv.push_back((tcvstruc *)&cdata.device[i].tcv);
        cdata.devspec.tcv_cnt = (uint16_t)cdata.devspec.tcv.size();
        break;
        //! Photo Voltaic String
    case DEVICE_TYPE_STRG:
        json_addentry("device_strg_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(strgstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_strg_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(strgstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_strg_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(strgstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_strg_efi",didx, UINT16_MAX, (ptrdiff_t)offsetof(strgstruc,effbase)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_strg_efs",didx, UINT16_MAX, (ptrdiff_t)offsetof(strgstruc,effslope)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_strg_max",didx, UINT16_MAX, (ptrdiff_t)offsetof(strgstruc,maxpower)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_strg_power",didx, UINT16_MAX, (ptrdiff_t)offsetof(strgstruc,power)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.strg.push_back((strgstruc *)&cdata.device[i].strg);
        cdata.devspec.strg_cnt = (uint16_t)cdata.devspec.strg.size();
        break;
        //! Battery
    case DEVICE_TYPE_BATT:
        json_addentry("device_batt_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(battstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_batt_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(battstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_batt_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(battstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_batt_cap",didx, UINT16_MAX, (ptrdiff_t)offsetof(battstruc,capacity)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_batt_eff",didx, UINT16_MAX, (ptrdiff_t)offsetof(battstruc,efficiency)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_batt_lev",didx, UINT16_MAX, (ptrdiff_t)offsetof(battstruc,charge)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.batt.push_back((battstruc *)&cdata.device[i].batt);
        cdata.devspec.batt_cnt = (uint16_t)cdata.devspec.batt.size();
        break;
        //! Heater
    case DEVICE_TYPE_HTR:
        json_addentry("device_htr_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(htrstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_htr_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(htrstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_htr_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(htrstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.htr.push_back((htrstruc *)&cdata.device[i].htr);
        cdata.devspec.htr_cnt = (uint16_t)cdata.devspec.htr.size();
        break;
        //! Motor
    case DEVICE_TYPE_MOTR:
        json_addentry("device_motr_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(motrstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_motr_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(motrstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_motr_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(motrstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_motr_rat",didx, UINT16_MAX, (ptrdiff_t)offsetof(motrstruc,rat)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_motr_max",didx, UINT16_MAX, (ptrdiff_t)offsetof(motrstruc,max)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_motr_spd",didx, UINT16_MAX, (ptrdiff_t)offsetof(motrstruc,spd)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.motr.push_back((motrstruc *)&cdata.device[i].motr);
        cdata.devspec.motr_cnt = (uint16_t)cdata.devspec.motr.size();
        break;
        //! Pressure Sensor
    case DEVICE_TYPE_PSEN:
        json_addentry("device_psen_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(psenstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_psen_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(psenstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_psen_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(psenstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_psen_press",didx, UINT16_MAX, (ptrdiff_t)offsetof(psenstruc,press)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.psen.push_back((psenstruc *)&cdata.device[i].psen);
        cdata.devspec.psen_cnt = (uint16_t)cdata.devspec.psen.size();
        break;
        //! Temperature Sensor
    case DEVICE_TYPE_TSEN:
        json_addentry("device_tsen_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(tsenstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tsen_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(tsenstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tsen_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(tsenstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.tsen.push_back((tsenstruc *)&cdata.device[i].tsen);
        cdata.devspec.tsen_cnt = (uint16_t)cdata.devspec.tsen.size();
        break;
        //! Thruster
    case DEVICE_TYPE_THST:
        json_addentry("device_thst_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(thststruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_thst_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(thststruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_thst_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(thststruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_thst_isp",didx, UINT16_MAX, (ptrdiff_t)offsetof(thststruc,isp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_thst_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(thststruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_thst_flw",didx, UINT16_MAX, (ptrdiff_t)offsetof(thststruc,flw)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.thst.push_back((thststruc *)&cdata.device[i].thst);
        cdata.devspec.thst_cnt = (uint16_t)cdata.devspec.thst.size();
        break;
        //! Propellant Tank
    case DEVICE_TYPE_PROP:
        json_addentry("device_prop_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(propstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_prop_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(propstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_prop_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(propstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_prop_cap",didx, UINT16_MAX, (ptrdiff_t)offsetof(propstruc,cap)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_prop_lev",didx, UINT16_MAX, (ptrdiff_t)offsetof(propstruc,lev)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.prop.push_back((propstruc *)&cdata.device[i].prop);
        cdata.devspec.prop_cnt = (uint16_t)cdata.devspec.prop.size();
        break;
        //! Switch
    case DEVICE_TYPE_SWCH:
        json_addentry("device_swch_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(swchstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_swch_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(swchstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_swch_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(swchstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.swch.push_back((swchstruc *)&cdata.device[i].swch);
        cdata.devspec.swch_cnt = (uint16_t)cdata.devspec.swch.size();
        break;
        //! Rotor
    case DEVICE_TYPE_ROT:
        json_addentry("device_rot_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(rotstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rot_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(rotstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_rot_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(rotstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.rot.push_back((rotstruc *)&cdata.device[i].rot);
        cdata.devspec.rot_cnt = (uint16_t)cdata.devspec.rot.size();
        break;
        //! Star Tracker
    case DEVICE_TYPE_STT:
        json_addentry("device_stt_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_stt_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_stt_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_stt_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_stt_att",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,att)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_stt_omega",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,omega)+i*sizeof(devicestruc),COSMOS_SIZEOF(rvector), (uint16_t)JSON_TYPE_RVECTOR,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_stt_retcode",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,retcode)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_stt_status",didx, UINT16_MAX, (ptrdiff_t)offsetof(sttstruc,status)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint32_t), (uint16_t)JSON_TYPE_UINT32,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.stt.push_back((sttstruc *)&cdata.device[i].stt);
        cdata.devspec.stt_cnt = (uint16_t)cdata.devspec.stt.size();
        break;
        //! Star Tracker
    case DEVICE_TYPE_SUCHI:
        {
            json_addentry("device_suchi_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(suchistruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
            json_addentry("device_suchi_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(suchistruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
            json_addentry("device_suchi_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(suchistruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
            json_addentry("device_suchi_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(suchistruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
            json_addentry("device_suchi_press",didx, UINT16_MAX, (ptrdiff_t)offsetof(suchistruc,press)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
            for (uint16_t j=0; j<8; j++)
            {
                json_addentry("device_suchi_temps",didx,j,(ptrdiff_t)offsetof(suchistruc,temps)+i*sizeof(devicestruc)+j*sizeof(float), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
            }
            suchistruc* ptr = &cdata.device[i].suchi;
            cdata.devspec.suchi.push_back(ptr);
            cdata.devspec.suchi_cnt = (uint16_t)cdata.devspec.suchi.size();
            break;
        }
    case DEVICE_TYPE_MCC:
        json_addentry("device_mcc_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(mccstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mcc_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(mccstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mcc_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(mccstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mcc_q",didx, UINT16_MAX, (ptrdiff_t)offsetof(mccstruc,q)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_mcc_align",didx, UINT16_MAX, (ptrdiff_t)offsetof(mccstruc,align)+i*sizeof(devicestruc),COSMOS_SIZEOF(quaternion), (uint16_t)JSON_TYPE_QUATERNION,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.mcc.push_back((mccstruc *)&cdata.device[i].mcc);
        cdata.devspec.mcc_cnt = (uint16_t)cdata.devspec.mcc.size();
        break;
    case DEVICE_TYPE_TCU:
        json_addentry("device_tcu_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcustruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcu_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcustruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcu_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcustruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_tcu_mcnt",didx, UINT16_MAX, (ptrdiff_t)offsetof(tcustruc,mcnt)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        for (uint16_t j=0; j<3; j++)
        {
            json_addentry("device_tcu_mcidx",didx,j,(ptrdiff_t)offsetof(tcustruc,mcidx)+i*sizeof(devicestruc)+j*sizeof(int16_t), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        }
        cdata.devspec.tcu.push_back((tcustruc *)&cdata.device[i].tcu);
        cdata.devspec.tcu_cnt = (uint16_t)cdata.devspec.tcu.size();
        break;
    case DEVICE_TYPE_BUS:
        json_addentry("device_bus_utc",didx, UINT16_MAX, (ptrdiff_t)offsetof(busstruc,gen.utc)+i*sizeof(devicestruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_bus_cidx",didx, UINT16_MAX, (ptrdiff_t)offsetof(busstruc,gen.cidx)+i*sizeof(devicestruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_bus_temp",didx, UINT16_MAX, (ptrdiff_t)offsetof(busstruc,gen.temp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_bus_amp",didx, UINT16_MAX, (ptrdiff_t)offsetof(busstruc,gen.amp)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_bus_volt",didx, UINT16_MAX, (ptrdiff_t)offsetof(busstruc,gen.volt)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_bus_energy",didx, UINT16_MAX, (ptrdiff_t)offsetof(busstruc,energy)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        json_addentry("device_bus_wdt",didx, UINT16_MAX, (ptrdiff_t)offsetof(busstruc,wdt)+i*sizeof(devicestruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT,JSON_STRUCT_DEVICE, cmeta);
        cdata.devspec.bus.push_back((busstruc *)&cdata.device[i].bus);
        cdata.devspec.bus_cnt = (uint16_t)cdata.devspec.bus.size();
        break;
    }

    return (cmeta.jmapped);
}

//! Create JSON stream from wildcard
/*! Generate a JSON stream based on a character string representing
 * all the ::jsonlib_namespace names you wish to match.
    \param jstring Pointer to a string large enough to hold the end result.
    \param wildcard Character string representing a regular expression to be matched to all names in the ::jsonlib_namespace.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_wildcard(std::string &jstring, std::string wildcard, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out_wildcard(jstring, wildcard, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON stream from list
/*! Generate a JSON stream based on a comma separated list of ::jsonlib_namespace names.
    \param jstring Pointer to a string large enough to hold the end result.
    \param list List to convert.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_list(std::string &jstring, std::string list, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out_list(jstring, list, cmeta, cdata);
    if (iretn < 0 && iretn != JSON_ERROR_EOS)
        return nullptr;

    return jstring.data();
}

//! Create JSON stream from entries
/*! Generate a JSON stream based on a vector of entries of ::jsonlib_namespace names.
    \param jstring Pointer to a string large enough to hold the end result.
    \param table Vector of pointers to entries from ::jsonmap.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_table(std::string &jstring, std::vector<jsonentry*> table, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jstring.clear();
    for (auto entry: table)
    {
        if (entry != NULL)
        {
            json_out_entry(jstring, entry, cmeta, cdata);
        }
    }

    return jstring.data();
}

//! Create JSON Track string
/*! Generate a JSON stream showing the variables stored in an ::nodestruc.
    \param jstring Pointer to a string large enough to hold the end result.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 \param num Target index.
    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_target(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata, uint16_t num)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out_1d(jstring,(char *)"target_utc",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_name",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_type",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_azfrom",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_elfrom",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_azto",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_elto",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_min",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_range",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_close",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"target_loc_pos_geod",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON Node string
/*! Generate a JSON stream showing the variables stored in an ::nodestruc.
    \param jstring Pointer to a string large enough to hold the end result.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_node(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out(jstring,(char *)"node_utcstart", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_utc", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_name", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_type", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_mass", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_moi", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_battcap", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_battlev", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_powchg", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_powgen", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_powuse", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_icrf", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_icrf", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON Agent string
/*! Generate a JSON stream showing the variables stored in an ::agentstruc.
    \param jstring Pointer to a string large enough to hold the end result.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_agent(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out(jstring,(char *)"agent_utc", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_node", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_proc", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_user", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_aprd", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_bprd", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_port", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_bsz", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_addr", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_pid", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_stateflag", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_cpu", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_memory", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"agent_jitter", cmeta, cdata);
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
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_time(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out(jstring,(char *)"node_utcstart", cmeta, cdata);
    if (iretn < 0)
        return nullptr;
    iretn = json_out(jstring,(char *)"node_utc", cmeta, cdata);
    if (iretn < 0)
        return nullptr;
    iretn = json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);
    if (iretn < 0)
        return nullptr;

    return jstring.data();
}

//! Create JSON Heart Beat string
/*! Create a complete JSON formatted Heartbeat string using the data currently in the global COSMOS
 * structure.
    \param jstring Pointer to a ::jstring structure to be used to build out the JSON string.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_beat(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out(jstring,(char *)"beat", cmeta, cdata);
    if (iretn < 0)
        return nullptr;

    return jstring.data();
}

//! Create JSON Beacon string
/*! Create a complete JSON formatted Beacon string using the data currently in the global COSMOS structure.
    \param jstring Pointer to a ::jstring structure to be used to build out the JSON string.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_beacon(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out(jstring,(char *)"node_name", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_type", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_utcstart", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_utc", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_loc_pos_eci", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_loc_att_icrf", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_powgen", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_powuse", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_powchg", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out(jstring,(char *)"node_battlev", cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

//! Create JSON IMU string
/*! Create a complete JSON formatted IMU string for the indicated IMU using the data currently in the global COSMOS structure.
    \param jstring Pointer to a ::jstring structure to be used to build out the JSON string.
    \param num Number of the IMU.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Pointer to the string if successful, otherwise NULL.
*/
const char *json_of_imu(std::string &jstring, uint16_t num, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    iretn = json_out_1d(jstring,(char *)"device_imu_att",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"device_imu_align",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"device_imu_cidx",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"device_imu_cnt",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"device_imu_mag",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"device_imu_bdot",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }
    iretn = json_out_1d(jstring,(char *)"device_imu_pos",num, cmeta, cdata);
    if (iretn < 0)
    {
        return nullptr;
    }

    return jstring.data();
}

const char *json_of_ephemeris(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    // Location
    jstring.clear();
    json_out(jstring,(char *)"node_utcstart", cmeta, cdata);
    json_out(jstring,(char *)"node_utc", cmeta, cdata);
    json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);
    json_out(jstring,(char *)"node_name", cmeta, cdata);
    json_out(jstring,(char *)"node_type", cmeta, cdata);
    json_out(jstring,(char *)"node_loc_pos_eci", cmeta, cdata);

    return jstring.data();
}

const char *json_of_utc(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    // Time
    jstring.clear();
    json_out(jstring,(char *)"node_utcstart", cmeta, cdata);
    json_out(jstring,(char *)"node_utc", cmeta, cdata);
    json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);
    return jstring.data();
}

//! Get list of all Namespace names
/*! Go through the Namespace map, extracting each valid Namespace name. Return this
 * as a JSON like list.
*/
std::string json_list_of_all(cosmosmetastruc &cmeta)
{
    std::string result;

    result = "{";
    for (std::vector<jsonentry> entryrow : cmeta.jmap)
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


std::string json_list_of_soh(cosmosdatastruc &cdata)
{
    std::string result;
    char tempstring[200];

    result = "{\"node_name\",\"node_type\",\"node_state\",\"node_powgen\",\"node_powuse\",\"node_charging\",\"node_battlev\",\"node_loc_bearth\",\"node_loc_pos_eci\",\"node_loc_att_icrf\"";

    for (uint16_t i=0; i<cdata.devspec.pload_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_pload_utc_%03d\",\"device_pload_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.ssen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_ssen_utc_%03d\",\"device_ssen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_ssen_azimuth_%03d\",\"device_ssen_elevation_%03d\",\"device_ssen_qva_%03d\",\"device_ssen_qvb_%03d\",\"device_ssen_qvc_%03d\",\"device_ssen_qvd_%03d\"",i,i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.imu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_imu_utc_%03d\",\"device_imu_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_imu_accel_%03d\",\"device_imu_omega_%03d\",\"device_imu_alpha_%03d\",\"device_imu_mag_%03d\",\"device_imu_bdot_%03d\"",i,i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.rw_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rw_utc_%03d\",\"device_rw_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rw_omg_%03d\",\"device_rw_alp_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.mtr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mtr_utc_%03d\",\"device_mtr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mtr_mom_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.cpu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_cpu_utc_%03d\",\"device_cpu_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_cpu_gib_%03d\",\"device_cpu_load_%03d\",\"device_cpu_boot_count_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.gps_cnt; ++i)
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

    for (uint16_t i=0; i<cdata.devspec.ant_cnt; ++i)
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

    for (uint16_t i=0; i<cdata.devspec.rxr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rxr_utc_%03d\",\"device_rxr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_freq_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_band_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rxr_power_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.txr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_txr_utc_%03d\",\"device_txr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_freq_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_txr_power_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.tcv_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tcv_utc_%03d\",\"device_tcv_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_freq_%03d\",\"device_tcv_opmode_%03d\"",i,i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_band_%03d\"",i);
        result += tempstring;
        sprintf(tempstring, ",\"device_tcv_powerin_%03d\",\"device_tcv_powerout_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.strg_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_strg_utc_%03d\",\"device_strg_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_strg_power_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.batt_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_batt_utc_%03d\",\"device_batt_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_batt_charge_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.htr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_htr_utc_%03d\",\"device_htr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_htr_setpoint_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.motr_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_motr_utc_%03d\",\"device_motr_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_motr_rat_%03d\",\"device_motr_spd_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.tsen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tsen_utc_%03d\",\"device_tsen_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.thst_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_thst_utc_%03d\",\"device_thst_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_thst_align_%03d\",\"device_thst_flw_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.prop_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_prop_utc_%03d\",\"device_prop_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_prop_lev_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.swch_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_swch_utc_%03d\",\"device_swch_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.rot_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_rot_utc_%03d\",\"device_rot_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_rot_angle_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.stt_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_stt_utc_%03d\",\"device_stt_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_stt_att_%03d\",\"device_stt_omega_%03d\",\"device_stt_alpha_%03d\"",i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.mcc_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_mcc_utc_%03d\",\"device_mcc_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_mcc_q_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.tcu_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_tcu_utc_%03d\",\"device_tcu_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.bus_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_bus_utc_%03d\",\"device_bus_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_bus_utc_%03d\",\"device_bus_energy_%03d\",\"device_bus_amp_%03d\",\"device_bus_volt_%03d\"",i,i,i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.psen_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_psen_utc_%03d\",\"device_psen_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_psen_press_%03d\"",i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.suchi_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_suchi_utc_%03d\",\"device_suchi_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_suchi_press_%03d\",\"device_suchi_temps_%03d\"",i,i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.cam_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_cam_utc_%03d\",\"device_cam_temp_%03d\"", i, i);
        result += tempstring;
    }

    for (uint16_t i=0; i<cdata.devspec.disk_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_disk_utc_%03d\",\"device_disk_temp_%03d\"", i, i);
        result += tempstring;
        sprintf(tempstring, ",\"device_disk_gib_%03d\"",i);
        result += tempstring;
    }

    result += "}";

    return result;

}

const char *json_of_soh(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int32_t iretn;

    jstring.clear();
    // Time
    json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);

    // Satellite Information
    std::string soh = json_list_of_soh(cdata);
    iretn = json_out_list(jstring, soh.c_str(), cmeta, cdata);
    if (iretn < 0 && iretn != JSON_ERROR_EOS)
        return nullptr;

    return jstring.data();
}

//! Create JSON for an event
/*! Generate a JSON stream that represents the current state of the
 * ::eventstruc in ::cosmosstruc.
    \param jstring User provided ::jstring for creating the JSON stream
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

    \return Pointer to the string created.
*/
const char *json_of_event(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jstring.clear();
    json_out(jstring,(char *)"event_utc", cmeta, cdata);
    if (json_get_double("event_utcexec", cmeta, cdata) != 0.) json_out(jstring,(char *)"event_utcexec", cmeta, cdata);
    json_out(jstring,(char *)"event_node", cmeta, cdata);
    json_out(jstring,(char *)"event_name", cmeta, cdata);
    json_out(jstring,(char *)"event_user", cmeta, cdata);
    json_out(jstring,(char *)"event_type", cmeta, cdata);
    json_out(jstring,(char *)"event_flag", cmeta, cdata);
    json_out(jstring,(char *)"event_data", cmeta, cdata);
    json_out(jstring,(char *)"event_condition", cmeta, cdata);
    if (json_get_double("event_dtime", cmeta, cdata) != 0.) json_out(jstring,"event_dtime", cmeta, cdata);
    if (json_get_double("event_ctime", cmeta, cdata) != 0.) json_out(jstring,"event_ctime", cmeta, cdata);
    if (json_get_double("event_denergy", cmeta, cdata) != 0.) json_out(jstring,"event_denergy", cmeta, cdata);
    if (json_get_double("event_cenergy", cmeta, cdata) != 0.) json_out(jstring,"event_cenergy", cmeta, cdata);
    if (json_get_double("event_dmass", cmeta, cdata) != 0.) json_out(jstring,"event_dmass", cmeta, cdata);
    if (json_get_double("event_cmass", cmeta, cdata) != 0.) json_out(jstring,"event_cmass", cmeta, cdata);
    if (json_get_double("event_dbytes", cmeta, cdata) != 0.) json_out(jstring,"event_dbytes", cmeta, cdata);
    if (json_get_double("event_cbytes", cmeta, cdata) != 0.) json_out(jstring,"event_cbytes", cmeta, cdata);

    return jstring.data();
}

const char *json_of_groundcontact(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    int16_t i;

    jstring.clear();
    json_out(jstring,(char *)"node_utcstart", cmeta, cdata);
    json_out(jstring,(char *)"node_utc", cmeta, cdata);
    json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);
    json_out(jstring,(char *)"gs_cnt", cmeta, cdata);
    for (i=0; i<*(int16_t *)json_ptrto((char *)"gs_cnt", cmeta, cdata); i++)
    {
        json_out_1d(jstring,(char *)"gs_az",i, cmeta, cdata);
        json_out_1d(jstring,(char *)"gs_el",i, cmeta, cdata);
    }

    return jstring.data();
}

const char *json_of_mtr(std::string &jstring,uint16_t index, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jstring.clear();
    json_out(jstring,(char *)"node_utc", cmeta, cdata);
    json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);

    json_out_1d(jstring,(char *)"device_mtr_cidx",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_mtr_mom",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_mtr_mom",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_mtr_rmom",index, cmeta, cdata);

    return jstring.data();
}


const char *json_of_rw(std::string &jstring,uint16_t index, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jstring.clear();
    json_out(jstring,(char *)"node_utc", cmeta, cdata);
    json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);

    json_out_1d(jstring,(char *)"device_rw_utc",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_cidx",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_mom_x",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_mom_y",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_mom_z",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_omg",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_alp",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_romg",index, cmeta, cdata);
    json_out_1d(jstring,(char *)"device_rw_ralp",index, cmeta, cdata);

    return jstring.data();
}

const char *json_of_state_eci(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jstring.clear();
    //	json_out(jstring,(char *)"node_utc", cmeta, cdata);
    //	json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);
    json_out(jstring,(char *)"node_loc_pos_eci", cmeta, cdata);
    json_out(jstring,(char *)"node_loc_att_icrf", cmeta, cdata);
    return jstring.data();
}

// get state vector in ECEF/Geocentric
const char *json_of_state_geoc(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    jstring.clear();
    //	json_out(jstring,(char *)"node_utc", cmeta, cdata);
    //	json_out(jstring,(char *)"node_utcoffset", cmeta, cdata);
    json_out(jstring,(char *)"node_loc_pos_geoc", cmeta, cdata);
    json_out(jstring,(char *)"node_loc_att_geoc", cmeta, cdata);
    return jstring.data();
}

//! Dump Node description
/*! Create a JSON stream for general Node variables. Does not include any
 * derivative data (eg. area).
 \param jstring Pointer to a ::jstring to build the JSON stream in.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_node(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{

    jstring.clear();
    json_out(jstring,(char *)"node_type", cmeta, cdata);
    json_out_character(jstring, '\n');
    json_out(jstring,(char *)"piece_cnt", cmeta, cdata);
    json_out_character(jstring, '\n');
    json_out(jstring,(char *)"comp_cnt", cmeta, cdata);
    json_out_character(jstring, '\n');
    json_out(jstring,(char *)"port_cnt", cmeta, cdata);
    json_out_character(jstring, '\n');

    return jstring.data();
}

//! Dump Piece description
/*! Create a JSON stream for variables specific to the Pieces of the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Pointer to a ::jstring to build the JSON stream in.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_pieces(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint16_t cnt;

    jstring.clear();
    // Dump structures
    uint16_t *piece_cnt = (uint16_t *)json_ptrto((char *)"piece_cnt", cmeta, cdata);
    if (piece_cnt != nullptr)
    {
        for (uint16_t i=0; i<*piece_cnt; i++)
        {
            json_out_1d(jstring,(char *)"piece_name",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_type",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_cidx",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_mass",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_emi",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_abs",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_hcap",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_hcon",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_dim",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"piece_pnt_cnt",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            cnt = (uint16_t)json_get_int((char *)"piece_pnt_cnt",i, cmeta, cdata);
            for (uint16_t j=0; j<cnt; j++)
            {
                json_out_2d(jstring,(char *)"piece_pnt",i,j, cmeta, cdata);
                json_out_character(jstring, '\n');
            }
        }
    }


    return jstring.data();
}

//! Dump General Device description
/*! Create a JSON stream for variables common to all Devices in the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Pointer to a ::jstring to build the JSON stream in.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_devices_general(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{

    jstring.clear();
    // Dump components
    uint16_t *comp_cnt = (uint16_t *)json_ptrto((char *)"comp_cnt", cmeta, cdata);
    if (comp_cnt != nullptr)
    {
        for (uint16_t i=0; i<*comp_cnt; i++)
        {
            json_out_1d(jstring,(char *)"comp_type",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_model",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_didx",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_pidx",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_bidx",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_addr",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_portidx",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_nvolt",i, cmeta, cdata);
            json_out_character(jstring, '\n');
            json_out_1d(jstring,(char *)"comp_namp",i, cmeta, cdata);
            json_out_character(jstring, '\n');
        }
    }


    return jstring.data();
}

//! Dump Specific Device description
/*! Create a JSON stream for variables specific to particular Devices in the Node. Does not include any
 * derivative data (eg. area).
 \param jstring Pointer to a ::jstring to build the JSON stream in.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_devices_specific(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    uint16_t *cnt;
    char tstring[COSMOS_MAX_NAME+1];

    jstring.clear();
    // Dump device specific info
    for (uint16_t i=0; i<DEVICE_TYPE_COUNT; ++i)
    {
        // Create Namespace name for Device Specific count
        sprintf(tstring,"device_%s_cnt",device_type_string[i].c_str());
        if ((cnt=(uint16_t *)json_ptrto(tstring, cmeta, cdata)) != nullptr && *cnt != 0)
        {
            // Only dump information for Devices that have non zero count
            for (uint16_t j=0; j<*cnt; ++j)
            {
                // Dump ploads
                if (!strcmp(device_type_string[i].c_str(),"pload"))
                {
                    json_out_1d(jstring,(char *)"device_pload_bps",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_pload_key_cnt",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    for (uint16_t k=0; k<json_get_int((char *)"device_pload_key_cnt",j, cmeta, cdata); ++k)
                    {
                        json_out_2d(jstring,(char *)"device_pload_key_name",j,k, cmeta, cdata);
                        json_out_character(jstring, '\n');
                    }
                    continue;
                }

                // Dump Sun sensors
                if (!strcmp(device_type_string[i].c_str(),"ssen"))
                {
                    json_out_1d(jstring,(char *)"device_ssen_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump IMU's
                if (!strcmp(device_type_string[i].c_str(),"imu"))
                {
                    json_out_1d(jstring,(char *)"device_imu_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Reaction Wheel
                if (!strcmp(device_type_string[i].c_str(),"rw"))
                {
                    json_out_1d(jstring,(char *)"device_rw_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rw_mom",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rw_mxalp",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rw_mxomg",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rw_tc",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Magtorque Rods
                if (!strcmp(device_type_string[i].c_str(),"mtr"))
                {
                    json_out_1d(jstring,(char *)"device_mtr_mxmom",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_mtr_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    for (uint16_t k=0; k<7; ++k)
                    {
                        json_out_2d(jstring,(char *)"device_mtr_npoly",j,k, cmeta, cdata);
                        json_out_character(jstring, '\n');
                    }
                    for (uint16_t k=0; k<7; ++k)
                    {
                        json_out_2d(jstring,(char *)"device_mtr_ppoly",j,k, cmeta, cdata);
                        json_out_character(jstring, '\n');
                    }
                    json_out_1d(jstring,(char *)"device_mtr_tc",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Computer systems
                if (!strcmp(device_type_string[i].c_str(),"cpu"))
                {
                    json_out_1d(jstring,(char *)"device_cpu_maxgib",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_cpu_maxload",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
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
                    json_out_1d(jstring,(char *)"device_ant_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_ant_minelev",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Receivers
                if (!strcmp(device_type_string[i].c_str(),"rxr"))
                {
                    json_out_1d(jstring,(char *)"device_rxr_freq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rxr_maxfreq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rxr_minfreq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rxr_band",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_rxr_opmode",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Transmitters
                if (!strcmp(device_type_string[i].c_str(),"txr"))
                {
                    json_out_1d(jstring,(char *)"device_txr_freq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_txr_maxfreq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_txr_minfreq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_txr_opmode",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_txr_maxpower",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Transceivers
                if (!strcmp(device_type_string[i].c_str(),"tcv"))
                {
                    json_out_1d(jstring,(char *)"device_tcv_freq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_tcv_maxfreq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_tcv_minfreq",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_tcv_band",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_tcv_opmode",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_tcv_maxpower",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump solar strings
                if (!strcmp(device_type_string[i].c_str(),"strg"))
                {
                    json_out_1d(jstring,(char *)"device_strg_efi",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_strg_efs",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_strg_max",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump batteries
                if (!strcmp(device_type_string[i].c_str(),"batt"))
                {
                    json_out_1d(jstring,(char *)"device_batt_cap",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_batt_eff",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
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
                    json_out_1d(jstring,(char *)"device_motr_rat",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_motr_max",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
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
                    json_out_1d(jstring,(char *)"device_thst_isp",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_thst_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump propellant tanks
                if (!strcmp(device_type_string[i].c_str(),"prop"))
                {
                    json_out_1d(jstring,(char *)"device_prop_cap",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
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
                    json_out_1d(jstring,(char *)"device_stt_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Motion Capture info
                if (!strcmp(device_type_string[i].c_str(),"mcc"))
                {
                    json_out_1d(jstring,(char *)"device_mcc_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Torque Rod Control Unit info
                if (!strcmp(device_type_string[i].c_str(),"tcu"))
                {
                    json_out_1d(jstring,(char *)"device_tcu_mcnt",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    for (uint16_t k=0; k<json_get_int((char *)"device_tcu_mcnt",j, cmeta, cdata); ++k)
                    {
                        json_out_2d(jstring,(char *)"device_tcu_mcidx",j,k, cmeta, cdata);
                        json_out_character(jstring, '\n');
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
                    json_out_1d(jstring,(char *)"device_suchi_align",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Cameras
                if (!strcmp(device_type_string[i].c_str(),"cam"))
                {
                    json_out_1d(jstring,(char *)"device_cam_pwidth",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_cam_pheight",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_cam_width",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_cam_height",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    json_out_1d(jstring,(char *)"device_cam_flength",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    continue;
                }

                // Dump Telemetry
                if (!strcmp(device_type_string[i].c_str(),"telem"))
                {
                    json_out_1d(jstring,(char *)"device_telem_type",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
                    switch (json_get_int((char *)"device_telem_type",j, cmeta, cdata))
                    {
                    case TELEM_TYPE_UINT8:
                        json_out_1d(jstring,(char *)"device_telem_vuint8",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_INT8:
                        json_out_1d(jstring,(char *)"device_telem_vint8",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_UINT16:
                        json_out_1d(jstring,(char *)"device_telem_vuint16",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_INT16:
                        json_out_1d(jstring,(char *)"device_telem_vint16",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_UINT32:
                        json_out_1d(jstring,(char *)"device_telem_vuint32",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_INT32:
                        json_out_1d(jstring,(char *)"device_telem_vint32",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_FLOAT:
                        json_out_1d(jstring,(char *)"device_telem_vfloat",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_DOUBLE:
                        json_out_1d(jstring,(char *)"device_telem_vdouble",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    case TELEM_TYPE_STRING:
                        json_out_1d(jstring,(char *)"device_telem_vstring",j, cmeta, cdata);
                        json_out_character(jstring, '\n');
                        break;
                    }
                    continue;
                }

                // Dump Disks
                if (!strcmp(device_type_string[i].c_str(),"disk"))
                {
                    json_out_1d(jstring,(char *)"device_disk_maxgib",j, cmeta, cdata);
                    json_out_character(jstring, '\n');
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
 \param jstring Pointer to a ::jstring to build the JSON stream in.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.

 \return Pointer to the created JSON stream.
*/
const char *json_ports(std::string &jstring, cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{

    jstring.clear();
    // Dump Port table
    for (uint16_t i=0; i<*(int16_t *)json_ptrto((char *)"port_cnt", cmeta, cdata); i++)
    {
        json_out_1d(jstring,(char *)"port_name",i, cmeta, cdata);
        json_out_character(jstring, '\n');
        json_out_1d(jstring,(char *)"port_type",i, cmeta, cdata);
        json_out_character(jstring, '\n');
    }

    return jstring.data();
}

void json_test(cosmosmetastruc &cmeta)
{
    uint16_t i, j;
    long hash;

    int32_t hashcount[1480]= {0};

    for (i=0; i<cmeta.jmap.size(); ++i)
    {
        for (j=0; j<cmeta.jmap[i].size(); ++j)
        {
            hash = json_hash(cmeta.jmap[i][j].name);

            printf("%s %d %d %ld\n", cmeta.jmap[i][j].name.c_str(),i,j,hash);

            hashcount[hash]++;
        }

    }
}

//! Get hash and index in JSON Namespace map
/*! Using the provided name, find it's location in the provided Namespace map and set the
 * values for the hash and index.
    \param name Namespace name.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param handle Pointer to ::jsonhandle of name.
    \return Zero, or negative error number.
*/
int32_t json_name_map(std::string name, cosmosmetastruc &cmeta, jsonhandle &handle)
{

    if (!cmeta.jmapped)
        return (JSON_ERROR_NOJMAP);

    if (cmeta.jmap.size() == 0)
        return (JSON_ERROR_NOJMAP);

    handle.hash = json_hash(name);

    for (handle.index=0; handle.index<cmeta.jmap[handle.hash].size(); ++handle.index)
        if (name == cmeta.jmap[handle.hash][handle.index].name)
        {
            return 0;
        }

    return (JSON_ERROR_NOENTRY);
}

//! Get hash and index in JSON Equation map
/*! Using the provided text, find it's location in the provided Equation map and set the
 * values for the hash and index. If the equation is not already in the table, add it.
    \param equation Equation text.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param handle Pointer to ::jsonhandle of name.
    \return Zero, or negative error number.
*/
int32_t json_equation_map(std::string equation, cosmosmetastruc &cmeta, jsonhandle *handle)
{
    const char *pointer;
    jsonequation tequation;
    char ops[] = "+-*/%&|><=!~";
    int32_t iretn;
    size_t textlen;

    if (!cmeta.jmapped)
        return (JSON_ERROR_NOJMAP);

    if (cmeta.emap.size() == 0)
        return (JSON_ERROR_NOJMAP);

    // Equations must start and end with ')'
    if (equation[0] != '(' || equation[equation.length()-1] != ')')
    {
        return (JSON_ERROR_SCAN);
    }

    handle->hash = json_hash(equation);

    for (handle->index=0; handle->index<cmeta.emap[handle->hash].size(); ++handle->index)
    {
        if (!strcmp(equation.c_str(), cmeta.emap[handle->hash][handle->index].text))
        {
            return 0;
        }
    }

    // Not found. We will have to add it to the map.
    pointer = &equation[1];

    // Extract first operand
    if ((iretn=json_parse_operand(pointer, &tequation.operand[0], cmeta)) < 0)
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
    if ((iretn=json_parse_operand(pointer, &tequation.operand[1], cmeta)) < 0)
    {
        return (JSON_ERROR_SCAN);
    }

    // Remove trailing )
    while ((pointer)[0] != 0 && (pointer)[0] != ')')
        (pointer)++;
    if ((pointer)[0] != 0)
        (pointer)++;

    textlen = equation.size()+1;
    if ((tequation.text = (char *)calloc(1,textlen)) == NULL)
    {
        return (JSON_ERROR_SCAN);
    }

    // Populate the equation
    strcpy(tequation.text,equation.c_str());

    handle->index = (uint16_t)cmeta.emap[handle->hash].size();
    cmeta.emap[handle->hash].push_back(tequation);
    if (cmeta.emap[handle->hash].size() != handle->index+1u)
    {
        free(tequation.text);
        return (JSON_ERROR_SCAN);
    }

    return 0;

}

//! Clone ::cosmosstruc data areas, indirect
/*! Copy the secondary ::cosmosdatastruc to the primary ::cosmosdatasruc.
    \param cinfo Pointer to clone ::cosmosstruc.
    \return Zero, or negative error.
*/
int32_t json_clone(cosmosstruc *cinfo)
{
    int32_t iretn;
    if (cinfo == nullptr)
    {
        return (JSON_ERROR_NOJMAP);
    }
    iretn = json_clone(cinfo->pdata, cinfo->sdata);
    return iretn;
}

//! Clone ::cosmosstruc data areas, direct
/*! Copy one ::cosmosdatastruc to another.
    \param cdata1 Source ::cosmosdatastruc.
    \param cdata2 Target ::cosmosdatastruc.
    \return Zero, or negative error.
*/
int32_t json_clone(cosmosdatastruc &cdata1, cosmosdatastruc &cdata2)
{

    cdata2 = cdata1;
    for (uint16_t i=0; i<cdata2.node.device_cnt; ++i)
    {
        switch(cdata2.device[i].all.gen.type)
        {
        case DEVICE_TYPE_TELEM:
            cdata2.devspec.telem[cdata2.device[i].all.gen.didx] = &cdata2.device[i].telem;
            break;
        case DEVICE_TYPE_PLOAD:
            cdata2.devspec.pload[cdata2.device[i].all.gen.didx] = &cdata2.device[i].pload;
            break;
        case DEVICE_TYPE_SSEN:
            cdata2.devspec.ssen[cdata2.device[i].all.gen.didx] = &cdata2.device[i].ssen;
            break;
        case DEVICE_TYPE_IMU:
            cdata2.devspec.imu[cdata2.device[i].all.gen.didx] = &cdata2.device[i].imu;
            break;
        case DEVICE_TYPE_RW:
            cdata2.devspec.rw[cdata2.device[i].all.gen.didx] = &cdata2.device[i].rw;
            break;
        case DEVICE_TYPE_MTR:
            cdata2.devspec.mtr[cdata2.device[i].all.gen.didx] = &cdata2.device[i].mtr;
            break;
        case DEVICE_TYPE_CAM:
            cdata2.devspec.cam[cdata2.device[i].all.gen.didx] = &cdata2.device[i].cam;
            break;
        case DEVICE_TYPE_CPU:
            cdata2.devspec.cpu[cdata2.device[i].all.gen.didx] = &cdata2.device[i].cpu;
            break;
        case DEVICE_TYPE_GPS:
            cdata2.devspec.gps[cdata2.device[i].all.gen.didx] = &cdata2.device[i].gps;
            break;
        case DEVICE_TYPE_ANT:
            cdata2.devspec.ant[cdata2.device[i].all.gen.didx] = &cdata2.device[i].ant;
            break;
        case DEVICE_TYPE_RXR:
            cdata2.devspec.rxr[cdata2.device[i].all.gen.didx] = &cdata2.device[i].rxr;
            break;
        case DEVICE_TYPE_TXR:
            cdata2.devspec.txr[cdata2.device[i].all.gen.didx] = &cdata2.device[i].txr;
            break;
        case DEVICE_TYPE_TCV:
            cdata2.devspec.tcv[cdata2.device[i].all.gen.didx] = &cdata2.device[i].tcv;
            break;
        case DEVICE_TYPE_STRG:
            cdata2.devspec.strg[cdata2.device[i].all.gen.didx] = &cdata2.device[i].strg;
            break;
        case DEVICE_TYPE_BATT:
            cdata2.devspec.batt[cdata2.device[i].all.gen.didx] = &cdata2.device[i].batt;
            break;
        case DEVICE_TYPE_HTR:
            cdata2.devspec.htr[cdata2.device[i].all.gen.didx] = &cdata2.device[i].htr;
            break;
        case DEVICE_TYPE_MOTR:
            cdata2.devspec.motr[cdata2.device[i].all.gen.didx] = &cdata2.device[i].motr;
            break;
        case DEVICE_TYPE_PSEN:
            cdata2.devspec.psen[cdata2.device[i].all.gen.didx] = &cdata2.device[i].psen;
            break;
        case DEVICE_TYPE_TSEN:
            cdata2.devspec.tsen[cdata2.device[i].all.gen.didx] = &cdata2.device[i].tsen;
            break;
        case DEVICE_TYPE_THST:
            cdata2.devspec.thst[cdata2.device[i].all.gen.didx] = &cdata2.device[i].thst;
            break;
        case DEVICE_TYPE_PROP:
            cdata2.devspec.prop[cdata2.device[i].all.gen.didx] = &cdata2.device[i].prop;
            break;
        case DEVICE_TYPE_SWCH:
            cdata2.devspec.swch[cdata2.device[i].all.gen.didx] = &cdata2.device[i].swch;
            break;
        case DEVICE_TYPE_ROT:
            cdata2.devspec.rot[cdata2.device[i].all.gen.didx] = &cdata2.device[i].rot;
            break;
        case DEVICE_TYPE_STT:
            cdata2.devspec.stt[cdata2.device[i].all.gen.didx] = &cdata2.device[i].stt;
            break;
        case DEVICE_TYPE_MCC:
            cdata2.devspec.mcc[cdata2.device[i].all.gen.didx] = &cdata2.device[i].mcc;
            break;
        case DEVICE_TYPE_TCU:
            cdata2.devspec.tcu[cdata2.device[i].all.gen.didx] = &cdata2.device[i].tcu;
            break;
        case DEVICE_TYPE_BUS:
            cdata2.devspec.bus[cdata2.device[i].all.gen.didx] = &cdata2.device[i].bus;
            break;
        case DEVICE_TYPE_SUCHI:
            cdata2.devspec.suchi[cdata2.device[i].all.gen.didx] = &cdata2.device[i].suchi;
            break;
        }
    }


    return 0;
}

uint32_t json_get_name_list_count(cosmosmetastruc &cmeta)
{
    if (cmeta.jmapped == false) return 0;

    uint32_t count = 0;
    for (uint32_t i = 0; i < cmeta.jmap.size(); i++)
    {
        for (uint32_t j = 0; j < cmeta.jmap[i].size(); j++)
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
int32_t node_init(std::string node, cosmosstruc *cinfo)
{
    int32_t iretn;

    if (cinfo == nullptr || !cinfo->meta.jmapped)
        return (JSON_ERROR_NOJMAP);

    iretn = json_setup_node(node, cinfo);
    if (iretn < 0)
    {
        return iretn;
    }

    node_calc(cinfo->pdata);

    //! Load targeting information
    cinfo->pdata.node.target_cnt = (uint16_t)load_target(cinfo->meta, cinfo->pdata);

    return 0;
}

//! Calculate Satellite configuration values.
/*! Using the provided satellite structure, populate the derivative static quantities and initialize any
 * reasonable dynamic quantities.
    \param cdata Reference to ::cosmosdatastruc to use.
    \return 0
*/
int32_t node_calc(cosmosdatastruc &cdata)
{
    uint16_t n, i, j, k;
    double dm, ta, tb, tc;
    rvector tv0, tv1, tv2, tv3, dv, sv;

    cdata.physics.hcap = cdata.physics.heat = 0.;
    cdata.physics.mass = 0.;
    cdata.physics.moi = rv_zero();
    cdata.physics.com = rv_zero();

    for (n=0; n<cdata.piece.size(); n++)
    {
        if (cdata.piece[n].mass == 0.)
            cdata.piece[n].mass = .001f;
        cdata.piece[n].temp = 300.;
        cdata.piece[n].heat = 300.f * cdata.piece[n].hcap;
        cdata.physics.heat += cdata.piece[n].heat;
        cdata.physics.mass += cdata.piece[n].mass;
        cdata.physics.hcap += cdata.piece[n].hcap * cdata.piece[n].mass;
        cdata.piece[n].area = 0.;
        if (cdata.piece[n].pnt_cnt)
        {
            dm = cdata.piece[n].mass / cdata.piece[n].pnt_cnt;
            cdata.piece[n].centroid = rv_zero();
            for (i=0; i<cdata.piece[n].pnt_cnt; i++)
            {
                cdata.piece[n].centroid.col[0] += cdata.piece[n].points[i].col[0] * dm;
                cdata.piece[n].centroid.col[1] += cdata.piece[n].points[i].col[1] * dm;
                cdata.piece[n].centroid.col[2] += cdata.piece[n].points[i].col[2] * dm;
            }
            cdata.physics.com.col[0] += cdata.piece[n].centroid.col[0];
            cdata.physics.com.col[1] += cdata.piece[n].centroid.col[1];
            cdata.physics.com.col[2] += cdata.piece[n].centroid.col[2];
            cdata.piece[n].centroid.col[0] /= cdata.piece[n].mass;
            cdata.piece[n].centroid.col[1] /= cdata.piece[n].mass;
            cdata.piece[n].centroid.col[2] /= cdata.piece[n].mass;
            switch (cdata.piece[n].type)
            {
            case PIECE_TYPE_EXTERNAL_PANEL:
                for (i=0; i<cdata.piece[n].pnt_cnt-1; i++)
                {
                    tv0 = rv_sub(cdata.piece[n].points[i],cdata.piece[n].centroid);
                    tv1 = rv_sub(cdata.piece[n].points[i+1],cdata.piece[n].centroid);
                    dv = rv_sub(tv1,tv0);
                    sv = rv_add(tv1,tv0);
                    cdata.piece[n].area += (float)sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]))/2.f;
                }
                tv0 = rv_sub(cdata.piece[n].points[cdata.piece[n].pnt_cnt-1],cdata.piece[n].centroid);
                tv1 = rv_sub(cdata.piece[n].points[0],cdata.piece[n].centroid);
                dv = rv_sub(tv1,tv0);
                sv = rv_add(tv1,tv0);
                cdata.piece[n].area += (float)sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]))/2.f;
                tv0 = cdata.piece[n].points[0];
                tv1 = cdata.piece[n].points[1];
                tv2 = cdata.piece[n].points[2];
                ta = tv0.col[1]*(tv1.col[2]-tv2.col[2])+tv1.col[1]*(tv2.col[2]-tv0.col[2])+tv2.col[1]*(tv0.col[2]-tv1.col[2]);
                tb = tv0.col[2]*(tv1.col[0]-tv2.col[0])+tv1.col[2]*(tv2.col[0]-tv0.col[0])+tv2.col[2]*(tv0.col[0]-tv1.col[0]);
                tc = tv0.col[0]*(tv1.col[1]-tv2.col[1])+tv1.col[0]*(tv2.col[1]-tv0.col[1])+tv2.col[0]*(tv0.col[1]-tv1.col[1]);
                //				td = -(tv0.col[0]*(tv1.col[1]*tv2.col[2]-tv2.col[1]*tv1.col[2])+tv1.col[0]*(tv2.col[1]*tv0.col[2]-tv0.col[1]*tv2.col[2])+tv2.col[0]*(tv0.col[1]*tv1.col[2]-tv1.col[1]*tv0.col[2]));
                cdata.piece[n].normal.col[0] = ta;
                cdata.piece[n].normal.col[1] = tb;
                cdata.piece[n].normal.col[2] = tc;
                break;
            case PIECE_TYPE_INTERNAL_PANEL:
                for (i=0; i<cdata.piece[n].pnt_cnt-1; i++)
                {
                    tv0 = rv_sub(cdata.piece[n].points[i],cdata.piece[n].centroid);
                    tv1 = rv_sub(cdata.piece[n].points[i+1],cdata.piece[n].centroid);
                    dv = rv_sub(tv1,tv0);
                    sv = rv_add(tv1,tv0);
                    cdata.piece[n].area += (float)sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]));
                }
                tv0 = rv_sub(cdata.piece[n].points[cdata.piece[n].pnt_cnt-1],cdata.piece[n].centroid);
                tv1 = rv_sub(cdata.piece[n].points[0],cdata.piece[n].centroid);
                dv = rv_sub(tv1,tv0);
                sv = rv_add(tv1,tv0);
                cdata.piece[n].area += (float)sqrt((dv.col[0]*dv.col[0]+dv.col[1]*dv.col[1]+dv.col[2]*dv.col[2])*(sv.col[0]*sv.col[0]+sv.col[1]*sv.col[1]+sv.col[2]*sv.col[2]))/2.f;
                break;
            case PIECE_TYPE_BOX:
                tv0 = cdata.piece[n].points[0];
                tv1 = cdata.piece[n].points[1];
                tv2 = cdata.piece[n].points[2];
                tv3 = cdata.piece[n].points[6];
                ta = length_rv(rv_sub(tv1,tv0));
                tb = length_rv(rv_sub(tv2,tv1));
                tc = length_rv(rv_sub(tv3,tv0));
                cdata.piece[n].area = (float)(2. * ta * tb + 4. * ta * tc);
                break;
            case PIECE_TYPE_CYLINDER:
                tv0 = cdata.piece[n].points[0];
                tv1 = cdata.piece[n].points[1];
                ta = length_rv(rv_sub(tv1,tv0));
                cdata.piece[n].area = (float)(DPI * cdata.piece[n].dim * cdata.piece[n].dim * ta);
                break;
            case PIECE_TYPE_SPHERE:
                cdata.piece[n].area = (float)((4./3.) *DPI * pow(cdata.piece[n].dim,3.));
                break;
            }
        }
    }
    if (cdata.node.mass == 0.)
    {
        cdata.node.mass = cdata.physics.mass;
    }
    else
    {
        cdata.physics.mass = cdata.node.mass;
    }

    // Turn on power buses
    for (n=0; n<cdata.devspec.bus_cnt; n++)
    {
        cdata.devspec.bus[n]->gen.flag |= DEVICE_FLAG_ON;
    }


    for (n=0; n<cdata.node.device_cnt; n++)
    {
        /*
    if (cdata.device[n].all.gen.pidx >= 0)
        {
        cdata.node.com.col[0] += cdata.piece[cdata.device[n].all.gen.pidx].centroid.col[0] * cdata.device[n].all.gen.mass;
        cdata.node.com.col[1] += cdata.piece[cdata.device[n].all.gen.pidx].centroid.col[1] * cdata.device[n].all.gen.mass;
        cdata.node.com.col[2] += cdata.piece[cdata.device[n].all.gen.pidx].centroid.col[2] * cdata.device[n].all.gen.mass;
        }
    if (cdata.device[n].all.gen.pidx >= 0)
        {
        cdata.piece[cdata.device[n].all.gen.pidx].heat += 300. * cdata.piece[cdata.device[n].all.gen.pidx].hcap * cdata.device[n].all.gen.mass;
        cdata.node.heat += 300. * cdata.piece[cdata.device[n].all.gen.pidx].hcap * cdata.device[n].all.gen.mass;
        }
    cdata.node.mass += cdata.device[n].all.gen.mass;
    */
        cdata.device[n].all.gen.temp = 300.;
        cdata.device[n].all.gen.flag |= DEVICE_FLAG_ON;
        cdata.device[n].all.gen.amp = cdata.device[n].all.gen.amp;
        cdata.device[n].all.gen.volt = cdata.device[n].all.gen.volt;
        if (cdata.device[n].all.gen.bidx < cdata.devspec.bus_cnt && cdata.devspec.bus[cdata.device[n].all.gen.bidx]->gen.volt < cdata.device[n].all.gen.volt)
            cdata.devspec.bus[cdata.device[n].all.gen.bidx]->gen.volt = cdata.device[n].all.gen.volt;
    }

    cdata.physics.com = rv_smult(1./cdata.physics.mass,cdata.physics.com);
    cdata.physics.hcap /= cdata.physics.mass;

    for (n=0; n<cdata.piece.size(); n++)
    {
        if (cdata.piece[n].pnt_cnt)
        {
            cdata.piece[n].centroid = rv_sub(cdata.piece[n].centroid,cdata.physics.com);
            cdata.piece[n].normal = rv_sub(cdata.piece[n].normal,cdata.physics.com);
            tv0 = cdata.piece[n].centroid;
            tv0.col[0] = 0.;
            ta = length_rv(tv0);
            cdata.physics.moi.col[0] += cdata.piece[n].mass * ta * ta;
            tv0 = cdata.piece[n].centroid;
            tv0.col[1] = 0.;
            ta = length_rv(tv0);
            cdata.physics.moi.col[1] += cdata.piece[n].mass * ta * ta;
            tv0 = cdata.piece[n].centroid;
            tv0.col[2] = 0.;
            ta = length_rv(tv0);
            cdata.physics.moi.col[2] += cdata.piece[n].mass * ta * ta;
            for (i=0; i<cdata.piece[n].pnt_cnt; i++)
            {
                cdata.piece[n].points[i] = rv_sub(cdata.piece[n].points[i],cdata.physics.com);
            }
            if (cdata.piece[n].type == PIECE_TYPE_EXTERNAL_PANEL)
            {
                cdata.piece[n].twist = rv_cross(rv_sub(cdata.piece[n].centroid,cdata.piece[n].normal),cdata.piece[n].normal);
                cdata.piece[n].twist = rv_smult(-cdata.piece[n].area/(length_rv(cdata.piece[n].normal)*length_rv(cdata.piece[n].normal)),cdata.piece[n].twist);
                cdata.piece[n].shove = rv_zero();
                for (i=0; i<cdata.piece[n].pnt_cnt-1; i++)
                {
                    tv0 = rv_sub(cdata.piece[n].points[i],cdata.piece[n].centroid);
                    tv1 = rv_sub(cdata.piece[n].points[i+1],cdata.piece[n].centroid);
                    ta = length_rv(tv0);
                    for (j=0; j<=ta*100; j++)
                    {
                        tv2 = rv_smult(.01*j/ta,tv0);
                        tv3 = rv_smult(.01*j/ta,tv1);
                        dv = rv_sub(tv3,tv2);
                        tb = length_rv(dv);
                        for (k=0; k<tb*100; k++)
                        {
                            sv = rv_add(cdata.piece[n].centroid,rv_add(tv2,rv_smult(.01*k/tb,dv)));
                            tc = 1./(length_rv(sv)*length_rv(sv));
                            cdata.piece[n].shove = rv_add(cdata.piece[n].shove,rv_smult(tc,sv));
                        }
                    }
                }
                cdata.piece[n].shove = rv_smult(-1./10000.,cdata.piece[n].shove);
            }
        }
    }

    if (length_rv(cdata.node.moi) == 0.)
    {
        cdata.node.moi = cdata.physics.moi;
    }
    else
    {
        cdata.physics.moi = cdata.node.moi;
    }

    // Turn all CPU's on
    for (n=0; n<cdata.devspec.cpu_cnt; n++)
    {
        cdata.device[cdata.devspec.cpu[n]->gen.cidx].all.gen.flag |= DEVICE_FLAG_ON;
    }

    // Turn on all IMU's
    for (n=0; n<cdata.devspec.imu_cnt; n++)
    {
        cdata.device[cdata.devspec.imu[n]->gen.cidx].all.gen.flag |= DEVICE_FLAG_ON;
    }

    // Turn on all GPS's
    for (n=0; n<cdata.devspec.gps_cnt; n++)
    {
        cdata.device[cdata.devspec.gps[n]->gen.cidx].all.gen.flag |= DEVICE_FLAG_ON;
    }

    cdata.node.battcap = 0.;
    for (n=0; n<cdata.devspec.batt_cnt; n++)
    {
        cdata.node.battcap += cdata.devspec.batt[n]->capacity;
        cdata.devspec.batt[n]->charge = cdata.devspec.batt[n]->capacity;
    }
    cdata.node.battlev = cdata.node.battcap;

    // Turn off reaction wheels
    for (i=0; i<cdata.devspec.rw_cnt; i++)
    {
        cdata.devspec.rw[i]->alp = cdata.devspec.rw[i]->omg = 0.;
    }

    // Set fictional torque to zero
    cdata.physics.ftorque = rv_zero();

    return 0;
}

//! Dump tab delimited database files
/*! Create files that can be read in to a relational database representing the various elements of
 * the satellite. Tables are created for Parts, Components, Devices, Temperature Sensors and
 * Power Buses.
*/
void create_databases(cosmosdatastruc &cdata)
{
    FILE *op;
    uint32_t i, j;
    piecestruc s;
    genstruc cs;
    rwstruc rws;
    imustruc ims;
    sttstruc sts;
    int32_t iretn;

    /*
 *	op = fopen("target.txt","w");
    fprintf(op,"gs_idx	gs_name	gs_pos_lat	gs_pos_lon	gs_pos_alt	gs_min gs_az	gs_el\n");
    for (i=0; i<cdata.node.target_cnt; i++)
    {
        fprintf(op,"%d\t%s\t%u\n",i,cdata.target[i].name,cdata.target[i].type);
    }
    fclose(op);
*/

    op = fopen("piece.txt","w");
    fprintf(op,"PartIndex\tName\tType\tTemperatureIndex\tComponentIndex\tMass\tEmissivity\tAbsorptivity\tDimension\tHeatCapacity\tHeatConductivity\tArea\tTemp\tHeat\tPointCount\tPoint1X\tPoint1Y\tPoint1Z\tPoint2X\tPoint2Y\tPoint2Z\tPoint3X\tPoint3Y\tPoint3Z\tPoint4X\tPoint4Y\tPoint4Z\tPoint5X\tPoint5Y\tPoint5Z\tPoint6X\tPoint6Y\tPoint6Z\tPoint7X\tPoint7Y\tPoint7Z\tPoint8X\tPoint8Y\tPoint8Z\n");
    for (i=0; i<cdata.piece.size(); i++)
    {
        s = cdata.piece[i];
        fprintf(op,"%d\t%s\t%d\t%d\t%.4f\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.6f\t%u",i,s.name,s.type,s.cidx,s.mass,s.emi,s.abs,s.dim,s.hcap,s.hcon,s.area,s.pnt_cnt);
        for (j=0; j<s.pnt_cnt; j++)
        {
            fprintf(op,"\t%.6f\t%.6f\t%.6f",s.points[j].col[0],s.points[j].col[1],s.points[j].col[2]);
        }
        fprintf(op,"\n");
    }
    fclose(op);

    op = fopen("comp.txt","w");
    fprintf(op,"comp_idx\tcomp_type\tcomp_didx\tcomp_pidx\tcomp_bidx\tcomp_namp\tcomp_nvolt\tcomp_amp\tcomp_volt\tcomp_temp\tcomp_on\n");
    for (i=0; i<cdata.node.device_cnt; i++)
    {
        cs = cdata.device[i].all.gen;
        fprintf(op,"%d\t%d\t%d\t%d\t%d\t%.15g\t%.15g\n",i,cs.type,cs.didx,cs.pidx,cs.bidx,cs.amp,cs.volt);
    }
    fclose(op);

    op = fopen("rw.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tMomentX\tMomentY\tMomentZ\tMaxAngularSpeed\tAngularSpeed\tAngularAcceleration\n");
    for (i=0; i<cdata.devspec.rw_cnt; i++)
    {
        rws = *cdata.devspec.rw[i];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,rws.gen.cidx,rws.align.d.x,rws.align.d.y,rws.align.d.z,rws.align.w,rws.mom.col[0],rws.mom.col[1],rws.mom.col[2],rws.mxomg,rws.mxalp);
    }
    fclose(op);

    op = fopen("tsen.txt","w");
    fprintf(op,"TemperatureIndex\tCompIndex\tTemperature\n");
    for (i=0; i<cdata.devspec.tsen_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\n",i,cdata.devspec.tsen[i]->gen.cidx,cdata.devspec.tsen[i]->gen.temp);
    }
    fclose(op);

    op = fopen("strg.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tEfficiencyB\tEfficiencyM\tMaxPower\tPower\n");
    for (i=0; i<cdata.devspec.strg_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata.devspec.strg[i]->gen.cidx,cdata.devspec.strg[i]->effbase,cdata.devspec.strg[i]->effslope,cdata.devspec.strg[i]->maxpower,cdata.devspec.strg[i]->power);
    }
    fclose(op);

    op = fopen("batt.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tCapacity\tEfficiency\tCharge\n");
    for (i=0; i<cdata.devspec.batt_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\n",i,cdata.devspec.batt[i]->gen.cidx,cdata.devspec.batt[i]->capacity,cdata.devspec.batt[i]->efficiency,cdata.devspec.batt[i]->charge);
    }
    fclose(op);

    op = fopen("ssen.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tQuadrantVoltageA\tQuadrantVoltageB\tQuadrantVoltageC\tQuadrantVoltageD\tAzimuth\tElevation\n");
    for (i=0; i<cdata.devspec.ssen_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata.devspec.ssen[i]->gen.cidx,cdata.devspec.ssen[i]->align.d.x,cdata.devspec.ssen[i]->align.d.y,cdata.devspec.ssen[i]->align.d.z,cdata.devspec.ssen[i]->align.w,cdata.devspec.ssen[i]->qva,cdata.devspec.ssen[i]->qvb,cdata.devspec.ssen[i]->qvc,cdata.devspec.ssen[i]->qvd,cdata.devspec.ssen[i]->azimuth,cdata.devspec.ssen[i]->elevation);
    }
    fclose(op);

    op = fopen("imu.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tPositionX\tPositionY\tPositionZ\tVelocityX\tVelocityY\tVelocityZ\tAccelerationX\tAccelerationY\tAccelerationZ\tAttitudeQx\tAttitudeQy\tAttitudeQz\tAttitudeQw\tAngularVelocityX\tAngularVelocityY\tAngularVelocityZ\tAngularAccelerationX\tAngularAccelerationY\tAngularAccelerationZ\tMagneticFieldX\tMagneticFieldY\tMagneticFieldZ\tCalibrationQx\tCalibrationQy\tCalibrationQz\tCalibrationQw\n");
    for (i=0; i<cdata.devspec.imu_cnt; i++)
    {
        ims = *cdata.devspec.imu[i];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,ims.gen.cidx,ims.align.d.x,ims.align.d.y,ims.align.d.z,ims.align.w);
    }
    fclose(op);

    op = fopen("stt.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tAttitudeQx\tAttitudeQy\tAttitudeQz\tAttitudeQw\tAngularVelocityX\tAngularVelocityY\tAngularVelocityZ\tAngularAccelerationX\tAngularAccelerationY\tAngularAccelerationZ\tCalibrationQx\tCalibrationQy\tCalibrationQz\tCalibrationQw\n");
    for (i=0; i<cdata.devspec.stt_cnt; i++)
    {
        sts = *cdata.devspec.stt[i];
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\n",i,sts.gen.cidx,sts.align.d.x,sts.align.d.y,sts.align.d.z,sts.align.w);
    }
    fclose(op);

    op = fopen("mtr.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tAlignmentQx\tAlignmentQy\tAlignmentQz\tAlignmentQw\tMagneticMomentX\tMagneticMomentY\tMagneticMomentZ\tMagneticField\n");
    for (i=0; i<cdata.devspec.mtr_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata.devspec.mtr[i]->gen.cidx,cdata.devspec.mtr[i]->align.d.x,cdata.devspec.mtr[i]->align.d.y,cdata.devspec.mtr[i]->align.d.z,cdata.devspec.mtr[i]->align.w,cdata.devspec.mtr[i]->npoly[0],cdata.devspec.mtr[i]->npoly[1],cdata.devspec.mtr[i]->npoly[2],cdata.devspec.mtr[i]->mom);
    }
    fclose(op);

    op = fopen("gps.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tLatitude\tLongitude\tAltitude\tVelocityX\tVelocityY\tVelocityZ\n");
    for (i=0; i<cdata.devspec.gps_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\n",i,cdata.devspec.gps[i]->gen.cidx,cdata.devspec.gps[i]->geocs.col[0],cdata.devspec.gps[i]->geocs.col[1],cdata.devspec.gps[i]->geocs.col[2],cdata.devspec.gps[i]->geocv.col[0],cdata.devspec.gps[i]->geocv.col[1],cdata.devspec.gps[i]->geocv.col[2]);
    }
    fclose(op);

    op = fopen("cpu.txt","w");
    fprintf(op,"DeviceIndex\tComponentIndex\tLoad\tMemoryUse\tMemoryFree\tDiskUse\n");
    for (i=0; i<cdata.devspec.cpu_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.8g\t%.8g\t%.8g\n",i,cdata.devspec.cpu[i]->gen.cidx,cdata.devspec.cpu[i]->maxgib,cdata.devspec.cpu[i]->load,cdata.devspec.cpu[i]->gib);
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
    for (i=0; i<cdata.devspec.pload_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%d",i,cdata.devspec.pload[i]->gen.cidx,cdata.devspec.pload[i]->key_cnt);
        for (j=0; j<MAXPLOADKEYCNT; j++)
        {
            fprintf(op,"\t%d",cdata.devspec.pload[i]->keyidx[j]);
        }
        for (j=0; j<MAXPLOADKEYCNT; j++)
        {
            fprintf(op,"\t%.15g",cdata.devspec.pload[i]->keyval[j]);
        }
        fprintf(op,"\n");
    }
    fclose(op);

    op = fopen("motr.txt","w");
    fprintf(op,"motr_idx\tmotr_cidx\tmotr_spd\tmotr_rat\tmotr_rat\n");
    for (i=0; i<cdata.devspec.motr_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\t%.15g\n",i,cdata.devspec.motr[i]->gen.cidx,cdata.devspec.motr[i]->max,cdata.devspec.motr[i]->rat,cdata.devspec.motr[i]->spd);
    }
    fclose(op);

    op = fopen("swch.txt","w");
    fprintf(op,"swch_idx\tswch_cidx\n");
    for (i=0; i<cdata.devspec.swch_cnt; i++)
    {
        iretn = fscanf(op,"%*d\t%hu\n",&cdata.devspec.swch[i]->gen.cidx);
        if (iretn < 1)
        {
            break;
        }
    }
    fclose(op);

    op = fopen("thst.txt","w");
    fprintf(op,"thst_idx\tthst_cidx\tthst_idx\tthst_isp\tthst_flw\n");
    for (i=0; i<cdata.devspec.thst_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\n",i,cdata.devspec.thst[i]->gen.cidx,cdata.devspec.thst[i]->isp,cdata.devspec.thst[i]->flw);
    }
    fclose(op);

    op = fopen("prop.txt","w");
    fprintf(op,"prop_idx\tprop_cidx\tprop_cap\tprop_lev\n");
    for (i=0; i<cdata.devspec.prop_cnt; i++)
    {
        fprintf(op,"%d\t%d\t%.15g\t%.15g\n",i,cdata.devspec.prop[i]->gen.cidx,cdata.devspec.prop[i]->cap,cdata.devspec.prop[i]->lev);
    }
    fclose(op);
}


//! Load Track list
/*! Load the file target.ini into an array of ::targetstruc. Space for the array is automatically allocated
 * and the number of items returned.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 *	\return Number of items loaded.
*/

int32_t load_target(cosmosmetastruc &cmeta, cosmosdatastruc &cdata)
{
    FILE *op;
    std::string fname;
    char inb[JSON_MAX_DATA];
    uint16_t count;

    fname = get_nodedir(cdata.node.name) + "/target.ini";
    count = 0;
    if ((op=fopen(fname.c_str(),"r")) != NULL)
    {
        cdata.target.resize(100);
        while (count < cdata.target.size() && fgets(inb,JSON_MAX_DATA,op) != NULL)
        {
            json_addentry("target_range",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,range)+count*sizeof(targetstruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_close",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,close)+count*sizeof(targetstruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_utc",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,utc)+count*sizeof(targetstruc),COSMOS_SIZEOF(double), (uint16_t)JSON_TYPE_DOUBLE, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_name",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,name)+count*sizeof(targetstruc),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_type",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,type)+count*sizeof(targetstruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_azfrom",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,azfrom)+count*sizeof(targetstruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_azto",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,azto)+count*sizeof(targetstruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_elfrom",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,elfrom)+count*sizeof(targetstruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_elto",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,elto)+count*sizeof(targetstruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_min",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,min)+count*sizeof(targetstruc), COSMOS_SIZEOF(float), (uint16_t)JSON_TYPE_FLOAT, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_loc",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc)+count*sizeof(targetstruc),COSMOS_SIZEOF(locstruc), (uint16_t)JSON_TYPE_LOC, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_loc_pos_geod",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc.pos.geod)+count*sizeof(targetstruc),COSMOS_SIZEOF(geoidpos), (uint16_t)JSON_TYPE_POS_GEOD, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            json_addentry("target_loc_pos_eci",count, UINT16_MAX, (ptrdiff_t)offsetof(targetstruc,loc.pos.eci)+count*sizeof(targetstruc),COSMOS_SIZEOF(cartpos), (uint16_t)JSON_TYPE_POS_ECI, (uint16_t)JSON_STRUCT_TARGET, cmeta);
            if (json_parse(inb, cmeta, cdata) >= 0)
            {
                if (cdata.target[count].loc.utc == 0.)
                {
                    cdata.target[count].loc.utc = currentmjd(cdata.node.utcoffset);
                }
                // This may cause problems, but location information won't be complete without it
                loc_update(&cdata.target[count].loc);
                ++count;
            }
        }
        fclose(op);
        cdata.target.resize(count);
        return (count);
    }
    else
        return 0;
}

//! Update Track list
/*! For each entry in the Track list, calculate the azimuth, elevation and range to and
 *from the current base location.
    \param cdata Reference to ::cosmosdatastruc to use.
 *	\return 0, otherwise negative error.
 */
int32_t update_target(cosmosdatastruc &cdata)
{
    rvector topo, dv, ds;
    for (uint32_t i=0; i<cdata.target.size(); ++i)
    {
        loc_update(&cdata.target[i].loc);
        geoc2topo(cdata.target[i].loc.pos.geod.s,cdata.node.loc.pos.geoc.s,topo);
        topo2azel(topo,&cdata.target[i].azto,&cdata.target[i].elto);
        geoc2topo(cdata.node.loc.pos.geod.s,cdata.target[i].loc.pos.geoc.s,topo);
        topo2azel(topo,&cdata.target[i].azfrom,&cdata.target[i].elfrom);
        ds = rv_sub(cdata.target[i].loc.pos.geoc.s,cdata.node.loc.pos.geoc.s);
        cdata.target[i].range = length_rv(ds);
        dv = rv_sub(cdata.target[i].loc.pos.geoc.v,cdata.node.loc.pos.geoc.v);
        cdata.target[i].close = length_rv(rv_sub(ds,dv)) - length_rv(ds);
    }
    //	for (uint32_t i=0; i<cdata.target.size(); ++i)
    //	{
    //		cdata.target[i].azto = NAN;
    //		cdata.target[i].elto = NAN;
    //		cdata.target[i].azfrom = NAN;
    //		cdata.target[i].elfrom = NAN;
    //		cdata.target[i].range = NAN;
    //		cdata.target[i].close = NAN;
    //	}
    return 0;
}

int32_t update_target(locstruc source, targetstruc &target)
{
    rvector topo, dv, ds;

    loc_update(&target.loc);
    geoc2topo(target.loc.pos.geod.s, source.pos.geoc.s,topo);
    topo2azel(topo, &target.azto, &target.elto);
    geoc2topo(source.pos.geod.s, target.loc.pos.geoc.s, topo);
    topo2azel(topo, &target.azfrom, &target.elfrom);
    ds = rv_sub(target.loc.pos.geoc.s, source.pos.geoc.s);
    target.range = length_rv(ds);
    dv = rv_sub(target.loc.pos.geoc.v, source.pos.geoc.v);
    target.close = length_rv(rv_sub(ds,dv)) - length_rv(ds);
    return 0;
}

//! Load Event Dictionary
/*! Read a specific  event dictionary for a specific node. The
* dictionary is stored as multiple condition based JSON event strings
* in a file of the specified name, in the cinfo directory of the specified node.
* The result is a vector of event structures.
 *	\param dict Reference to vector of ::shorteventstruc.
    \param cmeta Reference to ::cosmosmetastruc to use.
    \param cdata Reference to ::cosmosdatastruc to use.
 *	\param file Name of dictionary file.
 *	\return Number of items loaded.
*/
size_t load_dictionary(std::vector<shorteventstruc> &dict, cosmosmetastruc &cmeta, cosmosdatastruc &cdata, const char *file)
{
    FILE *op;
    char inb[JSON_MAX_DATA];
    shorteventstruc sevent;
    jsonhandle handle;
    int32_t iretn;

    std::string fname = (get_nodedir(cdata.node.name) + "/") + file;
    if ((op=fopen(fname.c_str(),"r")) != NULL)
    {
        while (fgets(inb,JSON_MAX_DATA,op) != NULL)
        {
            json_clear_cosmosstruc(JSON_STRUCT_EVENT, cmeta, cdata);
            if (json_parse(inb, cmeta, cdata) > 0)
            {
                if ((iretn=json_equation_map(cdata.event[0].l.condition, cmeta, &handle)) < 0)
                    continue;
                sevent = cdata.event[0].s;
                sevent.utcexec = 0.;
                sevent.handle = handle;
                dict.push_back(sevent);
            }
        }
        fclose(op);
        dict.shrink_to_fit();
        return (dict.size());
    }
    else
        return 0;
}

//! Load tab delimited database files
/*! Read tab delimited files that make up a relational database representing
 * the various elements of the satellite. Tables represent Parts, Components,
 * Devices, Temperature Sensors and Power Buses.
    \param name Node name
    \param type Type of database.
    \param cdata Reference to ::cosmosdatastruc to use.
*/
void load_databases(char *name, uint16_t type, cosmosdatastruc &cdata)
{
    FILE *op;
    uint16_t i, j;
    int32_t iretn;
    piecestruc *s;
    genstruc *cs;
    rwstruc *rws;
    imustruc *ims;
    sttstruc *sts;

    strcpy(cdata.node.name, name);
    cdata.node.type = type;

    /*
 *	op = fopen("target.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%s\t%hu\n",&cdata.target[i].name[0],&cdata.target[i].type);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.node.target_cnt = (uint16_t)i;
    }
*/

    op = fopen("piece.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            s = &cdata.piece[i];
            iretn = fscanf(op,"%*d\t%s\t%hu\t%hu\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%hu",&s->name[0],&s->type,&s->cidx,&s->mass,&s->emi,&s->abs,&s->dim,&s->hcap,&s->hcon,&s->area,&s->pnt_cnt);
            if (iretn > 0)
            {
                for (j=0; j<s->pnt_cnt; j++)
                {
                    iretn = fscanf(op,"\t%lg\t%lg\t%lg",&s->points[j].col[0],&s->points[j].col[1],&s->points[j].col[2]);
                }
                iretn = fscanf(op,"\n");
                ++i;
            }
        } while (iretn > 0);
        fclose(op);
        cdata.node.piece_cnt = (uint16_t)i;
    }

    op = fopen("comp.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            cs = &cdata.device[i].all.gen;
            iretn = fscanf(op,"%*d\t%hu\t%hu\t%hu\t%hu\t%g\t%g\n",&cs->type,&cs->didx,&cs->pidx,&cs->bidx,&cs->namp,&cs->volt);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.node.device_cnt = (uint16_t)i;
    }

    op = fopen("ant.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&cdata.devspec.ant[i]->gen.cidx,&cdata.devspec.ant[i]->align.d.x,&cdata.devspec.ant[i]->align.d.y,&cdata.devspec.ant[i]->align.d.z,&cdata.devspec.ant[i]->align.w);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.ant_cnt = (uint16_t)i;
    }

    op = fopen("batt.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%g\t%g\n",&cdata.devspec.batt[i]->gen.cidx,&cdata.devspec.batt[i]->capacity,&cdata.devspec.batt[i]->efficiency);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.batt_cnt = (uint16_t)i;
    }

    op = fopen("cpu.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%f\t%f\n",&cdata.devspec.cpu[i]->gen.cidx,&cdata.devspec.cpu[i]->maxgib,&cdata.devspec.cpu[i]->maxload);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.cpu_cnt = (uint16_t)i;
    }

    op = fopen("gps.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\n",&cdata.devspec.gps[i]->gen.cidx);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.gps_cnt = (uint16_t)i;
    }

    op = fopen("imu.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            ims = cdata.devspec.imu[i];
            iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&ims->gen.cidx,&ims->align.d.x,&ims->align.d.y,&ims->align.d.z,&ims->align.w);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.imu_cnt = (uint16_t)i;
    }

    op = fopen("mtr.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\t%g\t%g\t%g\t%g\n",&cdata.devspec.mtr[i]->gen.cidx,&cdata.devspec.mtr[i]->align.d.x,&cdata.devspec.mtr[i]->align.d.y,&cdata.devspec.mtr[i]->align.d.z,&cdata.devspec.mtr[i]->align.w,&cdata.devspec.mtr[i]->npoly[0],&cdata.devspec.mtr[i]->npoly[1],&cdata.devspec.mtr[i]->npoly[2],&cdata.devspec.mtr[i]->mxmom);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.mtr_cnt = (uint16_t)i;
    }

    op = fopen("pload.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%u\t%hu",&cdata.devspec.pload[i]->gen.cidx,&cdata.devspec.pload[i]->bps,&cdata.devspec.pload[i]->key_cnt);
            for (j=0; j<cdata.devspec.pload[i]->key_cnt; j++)
            {
                //				iretn = fscanf(op,"\t%s",&cdata.devspec.pload[i]->key[j][0]);
            }
            iretn = fscanf(op,"\n");
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.pload_cnt = (uint16_t)i;
    }

    op = fopen("rw.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            rws = cdata.devspec.rw[i];
            iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\t%lg\t%lg\t%lg\t%g\t%g\n",&rws->gen.cidx,&rws->align.d.x,&rws->align.d.y,&rws->align.d.z,&rws->align.w,&rws->mom.col[0],&rws->mom.col[1],&rws->mom.col[2],&rws->mxomg,&rws->mxalp);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.rw_cnt = (uint16_t)i;
    }

    op = fopen("ssen.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&cdata.devspec.ssen[i]->gen.cidx,&cdata.devspec.ssen[i]->align.d.x,&cdata.devspec.ssen[i]->align.d.y,&cdata.devspec.ssen[i]->align.d.z,&cdata.devspec.ssen[i]->align.w);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.ssen_cnt = (uint16_t)i;
    }

    op = fopen("strg.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%g\t%g\t%g\n",&cdata.devspec.strg[i]->gen.cidx,&cdata.devspec.strg[i]->effbase,&cdata.devspec.strg[i]->effslope,&cdata.devspec.strg[i]->maxpower);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.strg_cnt = (uint16_t)i;
    }

    op = fopen("stt.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            sts = cdata.devspec.stt[i];
            iretn = fscanf(op,"%*d\t%hu\t%lg\t%lg\t%lg\t%lg\n",&sts->gen.cidx,&sts->align.d.x,&sts->align.d.y,&sts->align.d.z,&sts->align.w);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.stt_cnt = (uint16_t)i;
    }

    op = fopen("tsen.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\n",&cdata.devspec.tsen[i]->gen.cidx);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.tsen_cnt = (uint16_t)i;
    }

    op = fopen("motr.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%g\t%g\n",&cdata.devspec.motr[i]->gen.cidx,&cdata.devspec.motr[i]->max,&cdata.devspec.motr[i]->rat);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.motr_cnt = (uint16_t)i;
    }

    op = fopen("swch.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\n",&cdata.devspec.swch[i]->gen.cidx);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.swch_cnt = (uint16_t)i;
    }

    op = fopen("thst.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%g\t%lg\t%lg\t%lg\t%lg\n",&cdata.devspec.thst[i]->gen.cidx,&cdata.devspec.thst[i]->isp,&cdata.devspec.thst[i]->align.d.x,&cdata.devspec.thst[i]->align.d.y,&cdata.devspec.thst[i]->align.d.z,&cdata.devspec.thst[i]->align.w);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.thst_cnt = (uint16_t)i;
    }

    op = fopen("prop.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\t%g\n",&cdata.devspec.prop[i]->gen.cidx,&cdata.devspec.prop[i]->cap);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.prop_cnt = (uint16_t)i;
    }

    op = fopen("tcv.txt","r");
    if (op != NULL)
    {
        i = 0;
        do
        {
            iretn = fscanf(op,"%*d\t%hu\n",&cdata.devspec.tcv[i]->gen.cidx);
            if (iretn > 0)
                ++i;
        } while (iretn > 0);
        fclose(op);
        cdata.devspec.tcv_cnt = (uint16_t)i;
    }

}

//! Calculate current Events.
/*! Using the provided Event Dictionary, calculate all the Events for
* current state of the provided Node. The Event Dictionary will be
* updated with any new values so that repeating Events can be properly
* assessed.
*	\param dictionary Reference to vector of ::shortenventstruc representing event dictionary.
*	\param cmeta Reference to ::cosmosmetastruc to use.
*	\param cdata Reference to ::cosmosdatastruc to use.
*	\param events Reference to vector of ::shortenventstruc representing events.
*	\return Number of events created.
*/
size_t calc_events(std::vector<shorteventstruc> &dictionary, cosmosmetastruc &cmeta, cosmosdatastruc &cdata, std::vector<shorteventstruc> &events)
{
    double value;
    //	const char *cp;
    char *sptr, *eptr;

    events.resize(0);
    for (uint32_t k=0; k<dictionary.size(); ++k)
    {
        if (!std::isnan(value=json_equation(&dictionary[k].handle, cmeta, cdata)) && value != 0. && dictionary[k].value == 0.)
        {
            dictionary[k].utc = cdata.node.loc.utc;
            events.push_back(dictionary[k]);
            std::string tstring = json_get_string(dictionary[k].data, cmeta, cdata);
            strcpy(events[events.size()-1].data, tstring.c_str());
            strcpy(events[events.size()-1].node,cdata.node.name);
            if ((sptr=strstr(events[events.size()-1].name,"${")) != NULL && (eptr=strstr(sptr,"}")) != NULL)
            {
                *eptr = 0;
                tstring = json_get_string(sptr+2, cmeta, cdata);
                strcpy(sptr, tstring.c_str());
            }
        }
        dictionary[k].value = value;
    }

    events.shrink_to_fit();
    return (events.size());
}

std::string device_type_name(uint32_t type)
{
    std::string result;

    if (type < DEVICE_TYPE_COUNT)
    {
        result =  device_type_string[type];
    }

    return result;
}

std::string port_type_name(uint32_t type)
{
    std::string result;

    if (type < PORT_TYPE_COUNT)
    {
        result =  port_type_string[type];
    }

    return result;
}

//! @}

