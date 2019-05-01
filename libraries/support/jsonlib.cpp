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

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "support/convertlib.h"
#include "support/timelib.h"
#include "support/ephemlib.h"

#include <sys/stat.h>
#include <iostream>
#include <limits>
#include <fstream>

vector <string> device_type_string;

vector <string> port_type_string
{
    "rs232",
    "rs422",
    "ethernet",
    "usb",
    "path",
    "loopback",
    "propagator"
};

//! \ingroup jsonlib
//! \defgroup jsonlib_namespace JSON Name Space
//! @{
//! A hierarchical set of variable names meant to describe everything in COSMOS. Each name
//! maps to a variable of a type specified in \ref jsonlib_type. Names are restricted to
//! alphabetic characters, and can be no longer than ::COSMOS_MAX_NAME. Once mapped through use
//! of ::json_setup_node, these names will be tied to elements of the ::CosmosClass.

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

const char * cosmoshandler::json_beat_out(string &jstring)
{
	jstring.clear();
	
	if ((iretn=json_out_character(jstring,'{')) != 0)
        return (iretn);
	
	for (auto entry: candle->beats)		//for every entry
	{
		if (entry != NULL)
		{//cyt: add in fail case?
			json_out_entry(jstring, entry); 	//write name,value pairs to the string
		}
	}
	
	if ((iretn=json_out_character(jstring,'}')) != 0)
        return (iretn);
	
	return jstring.data();
}

int32_t json_out_entry(string &jstring, handlerstruc entry)
{
	int32_t iretn;
	
	if ((iretn=json_out_character(jstring,'{')) != 0)
        return (iretn);
	
	if ((iretn=json_out_name(jstring, entry.name)) != 0)
        return (iretn);
	
    switch(entry.type){
		case 1:
			json_out_int8(jstring, entry.value);
			break;
		case 2:
			json_out_int16(jstring, entry.value);
			break;
		case 3:
			json_out_int32(jstring, entry.value);
			break;
		case 4:
			json_out_uint8(jstring, entry.value);
			break;
		case 5:
			json_out_uint16(jstring, entry.value);
			break;
		case 6:
			json_out_uint32(jstring, entry.value);
			break;
		case 7: 
			json_out_float(jstring, entry.value);
			break;
		case 8:
			json_out_double(jstring, entry.value);
			break;
		// case 9: 
			// json_out_string(jstring, entry.value);
			// break;
		case 10:
			json_out_rvector(jstring, entry.value);
			break;
	}

	if ((iretn=json_out_character(jstring,'}')) != 0)
        return (iretn);
	
    return (iretn);
}

//! Signed 8 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 8 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the signed 8 bit integer
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int8(string &jstring, void* value)
{
	int32_t iretn;
	char tstring[15];
	int8_t * target;
	
	target=static_cast<int8_t*>(value);
	
	sprintf(tstring,"%hd",*target);
	
    iretn = json_append(jstring,tstring);
	return (iretn);	
}

//! Signed 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the signed 16 bit integer
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int16(string &jstring, void* value)
{
	int32_t iretn;
	char tstring[15];
	int16_t * target;
	
	target=static_cast<int16_t*>(value);
	
	sprintf(tstring,"%d",*target);
	
    iretn = json_append(jstring,tstring);
	return (iretn);	
}

//! Signed 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit signed integer.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the signed 32 bit integer
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_int32(string &jstring, void* value)
{
	int32_t iretn;
	char tstring[15];
	int32_t * target;
	
	target=static_cast<int32_t*>(value);
	
	sprintf(tstring,"%d",*target);
	
    iretn = json_append(jstring,tstring);
	return (iretn);	
}

//! Unsigned 8 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 8 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the unsigned 8 bit integer
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint8(string &jstring, void* value)

{
	int32_t iretn;
	char tstring[15];
	uint8_t * target;
	
	target=static_cast<uint8_t*>(value);
	
	sprintf(tstring,"%hu",*target);
	
    iretn = json_append(jstring,tstring);
	return (iretn);	
}

//! Unsigned 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the unsigned 16 bit integer
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint16(string &jstring, void* value)
{
	int32_t iretn;
	char tstring[15];
	uint16_t * target;
	
	target=static_cast<uint16_t*>(value);
	
	sprintf(tstring,"%u",*target);
	
    iretn = json_append(jstring,tstring);
	return (iretn);	
}

//! Unsigned 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit unsigned integer.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the unsigned 32 bit integer
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_uint32(string &jstring, void* value)
{
	int32_t iretn;
	char tstring[15];
	uint32_t * target;
	
	target=static_cast<uint32_t*>(value);
	
	sprintf(tstring,"%u",*target);
	
    iretn = json_append(jstring,tstring);
	return (iretn);	
}

//! Single precision floating vertex32_t to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated float.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the JSON data
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_float(string &jstring, void* value)
{
    int32_t iretn = 0;
    char tstring[15];
	float target;

	target = static_cast<float*>(value);
	
    if (!isfinite(*target)) 
    {
        *target = 0.0;
    }

    sprintf(tstring,"%.8g",*target);
    iretn = json_append(jstring,tstring);
    return (iretn);
}

//! Perform JSON output for a single nonindexed double
/*! Appends a JSON entry to the current JSON stream for the indicated double.
    \param jstring Reference to JSON stream.
    \param value The void cast pointer to the JSON data
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_double(string &jstring, void* value)
{
    int32_t iretn = 0;
    char tstring[30];
	double* target;

	target = static_cast<double*>(value);
	
    if (!isfinite(*target))  
    {
        *target=0.0;
    }

    sprintf(tstring,"%.17g",*target);
    iretn = json_append(jstring,tstring);

    return (iretn);
}

// int32_t json_out_string(string &jstring, void* value)
// {
	// int32_t iretn;
	// string * target;
	
	// iretn = json_out_string(jstring, *target, COSMOS_MAX_NAME);
// }
int32_t json_out_rvector(string &jstring, void* value)
{
	int32_t iretn;
	rvector* target;
	
	target = static_cast<rvector*>(value);
	
	iretn = json_out_rvector(jstring, *target);
	return (iretn);
	
}

//! Initialize JSON pointer map
/*! Create a ::CosmosClass and use it to assign storage for each of the groups and entries
 * for each of the non Node based elements to the JSON Name Map.
    \return Pointer to new ::CosmosClass or nullptr.
*/
int32_t json_create(CosmosClass* cinfo)
{       //cyt: currently commenting out jmap related sections
    //    extern char etext;
//    CosmosClass *cinfo;
    unitstruc tunit;

//    if ((cinfo = new CosmosClass) == nullptr)
//    {
//        return nullptr;
//    }

    // Make sure it's clear
    //    memset(cinfo, 0, sizeof(CosmosClass));

    //    cinfo->jmapbase = &etext;
//    cinfo->jmapped = 0;
    cinfo->unit.resize(JSON_UNIT_COUNT);
    //    cinfo->target.resize(100);
//    cinfo->jmap.resize(JSON_MAX_HASH);
//    cinfo->emap.resize(JSON_MAX_HASH);

    // Make sure we aren't running out of memory
    if (cinfo->unit.size() != JSON_UNIT_COUNT /*||
        cinfo->jmap.size() != JSON_MAX_HASH ||
        cinfo->emap.size() != JSON_MAX_HASH*/)
    {
        delete [] cinfo;
        return AGENT_ERROR_JSON_CREATE;
    }

    cinfo->glossary.resize(1);
    cinfo->agent.resize(1);
//    cinfo->event.resize(1);
    cinfo->user.resize(1);
//    memset(&cinfo->node, 0, sizeof(nodestruc));
//    memset(&cinfo->physics, 0, sizeof(physicsstruc));
//    memset(&cinfo->devspec, 0, sizeof(devspecstruc));

    // Make sure we aren't running out of memory
    if (cinfo->glossary.size() != 1 ||
        cinfo->agent.size() != 1 ||
//        cinfo->event.size() != 1 ||
        cinfo->user.size() != 1)
    {
//        delete [] cinfo;
        return AGENT_ERROR_JSON_CREATE;
    }

    // Copy primary to secondary
    //    json_clone(cinfo->sdata);

    // Create JSON Map unit table
    for (uint16_t i=0; i<cinfo->unit.size(); ++i)
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
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_ANGLE:
            tunit.name = "deg";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = (float)RTOD;
            cinfo->unit[i].push_back(tunit);
            break;
        case JSON_UNIT_ANGULAR_RATE:
            tunit.name = "deg/s";
            tunit.type = JSON_UNIT_TYPE_POLY;
            tunit.p1 = (float)RTOD;
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

    // Create component names
    device_type_string.clear();
    device_type_string.resize((uint16_t)DeviceType::COUNT);
    device_type_string[(uint16_t)DeviceType::ANT] = "ant";
    device_type_string[(uint16_t)DeviceType::BATT] = "batt";
    device_type_string[(uint16_t)DeviceType::BCREG] = "bcreg";
    device_type_string[(uint16_t)DeviceType::BUS] = "bus";
    device_type_string[(uint16_t)DeviceType::CAM] = "cam";
    device_type_string[(uint16_t)DeviceType::CPU] = "cpu";
    device_type_string[(uint16_t)DeviceType::DISK] = "disk";
    device_type_string[(uint16_t)DeviceType::GPS] = "gps";
    device_type_string[(uint16_t)DeviceType::HTR] = "htr";
    device_type_string[(uint16_t)DeviceType::IMU] = "imu";
    device_type_string[(uint16_t)DeviceType::MCC] = "mcc";
    device_type_string[(uint16_t)DeviceType::MOTR] = "motr";
    device_type_string[(uint16_t)DeviceType::MTR] = "mtr";
    device_type_string[(uint16_t)DeviceType::PLOAD] = "pload";
    device_type_string[(uint16_t)DeviceType::PROP] = "prop";
    device_type_string[(uint16_t)DeviceType::PSEN] = "psen";
    device_type_string[(uint16_t)DeviceType::PVSTRG] = "pvstrg";
    device_type_string[(uint16_t)DeviceType::ROT] = "rot";
    device_type_string[(uint16_t)DeviceType::RW] = "rw";
    device_type_string[(uint16_t)DeviceType::RXR] = "rxr";
    device_type_string[(uint16_t)DeviceType::SSEN] = "ssen";
    device_type_string[(uint16_t)DeviceType::STT] = "stt";
    device_type_string[(uint16_t)DeviceType::SUCHI] = "suchi";
    device_type_string[(uint16_t)DeviceType::SWCH] = "swch";
    device_type_string[(uint16_t)DeviceType::TCU] = "tcu";
    device_type_string[(uint16_t)DeviceType::TCV] = "tcv";
    device_type_string[(uint16_t)DeviceType::TELEM] = "telem";
    device_type_string[(uint16_t)DeviceType::THST] = "thst";
    device_type_string[(uint16_t)DeviceType::TNC] = "tnc";
    device_type_string[(uint16_t)DeviceType::TSEN] = "tsen";
    device_type_string[(uint16_t)DeviceType::TXR] = "txr";

    // Here is where we add entries for all the single element names.
//    json_mapbaseentries(cinfo);

    return 0;
}

////! Remove JSON pointer map
///*! Frees up all space assigned to JSON pointer map. Includes any space allocated
// * through ::json_addentry.
//*/
//void json_destroy(CosmosClass *cinfo)
//{
//    if (cinfo == nullptr)
//    {
//        return;
//    }
//    delete cinfo;       //cyt: check usage of class deconstructor
//    cinfo = nullptr;
//    return;
//}

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
int32_t json_out_name(string &jstring, string name)
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
int32_t json_out_int8(string &jstring,int8_t value)
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
int32_t json_out_int16(string &jstring,int16_t value)
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
int32_t json_out_int32(string &jstring,int32_t value)
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
int32_t json_out_uint8(string &jstring, uint8_t value)
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
int32_t json_out_uint16(string &jstring,uint16_t value)
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
int32_t json_out_uint32(string &jstring,uint32_t value)
{
    int32_t iretn;
    char tstring[15];

    sprintf(tstring,"%u",value);

    iretn = json_append(jstring,tstring);
    return (iretn);
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
    return (iretn);
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

    return (iretn);
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
int32_t json_out_gvector(string &jstring,gvector value)
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
int32_t json_out_svector(string &jstring,svector value)
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

//! ::avector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::avector.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_avector(string &jstring, avector value)
{
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'{')) < 0)
        return (iretn);

    // Output Bank
    if ((iretn=json_out_name(jstring,(char *)"b")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.b)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Elevation
    if ((iretn=json_out_name(jstring,(char *)"e")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.e)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output Heading
    if ((iretn=json_out_name(jstring,(char *)"h")) < 0)
        return (iretn);
    if ((iretn=json_out_double(jstring,value.h)) < 0)
        return (iretn);

    if ((iretn=json_out_character(jstring,'}')) < 0)
        return (iretn);
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
    int32_t iretn;

    if ((iretn=json_out_character(jstring,'[')) < 0)
        return (iretn);

    // Output x
    if ((iretn=json_out_double(jstring,value.x)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output y
    if ((iretn=json_out_double(jstring,value.y)) < 0)
        return (iretn);
    if ((iretn=json_out_character(jstring,',')) < 0)
        return (iretn);

    // Output z
    if ((iretn=json_out_double(jstring,value.z)) < 0)
        return (iretn);

    // Optionally output w
    if (value.w != 0.)
    {
        if ((iretn=json_out_character(jstring,',')) < 0)
            return (iretn);
        if ((iretn=json_out_double(jstring,value.w)) < 0)
            return (iretn);
    }

    if ((iretn=json_out_character(jstring,']')) < 0)
        return (iretn);

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
int32_t json_out_quaternion(string &jstring,quaternion value)
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
int32_t json_out_cvector(string &jstring,cvector value)
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
int32_t json_out_cartpos(string &jstring,cartpos value)
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
int32_t json_out_geoidpos(string &jstring,geoidpos value)
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
int32_t json_out_spherpos(string &jstring,spherpos value)
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

int32_t json_out_node(string &jstring, string value)
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

int32_t json_out_utcstart(string &jstring, double value)
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

//! ::locstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::locstruc.
    \param jstring Reference to JSON stream.
    \param value The JSON data of the desired variable
    \return  0 if successful, negative error otherwise
*/
int32_t json_out_locstruc(string &jstring,locstruc value)
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

////! Command event to JSON
///*! Appends a JSON entry to the current JSON stream for the indicated
// * ::longeventstruc specific to a command event.
//    \param jstring Reference to JSON stream.
//    \param value The JSON data of the desired variable
//    \return  0 if successful, negative error otherwise
//*/
//int32_t json_out_commandevent(string &jstring,longeventstruc value)
//{
//    int32_t iretn;


//    if ((iretn=json_out_character(jstring, '{')) < 0)
//        return (iretn);
//    if ((iretn=json_out_name(jstring, (char *)"event_utc")) < 0)
//        return (iretn);
//    if ((iretn=json_out_double(jstring, value.utc)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring, '}')) < 0)
//        return (iretn);

//    if (value.utcexec != 0.)
//    {
//        if ((iretn=json_out_character(jstring, '{')) < 0)
//            return (iretn);
//        if ((iretn=json_out_name(jstring, (char *)"event_utcexec")) < 0)
//            return (iretn);
//        if ((iretn=json_out_double(jstring, value.utcexec)) < 0)
//            return (iretn);
//        if ((iretn=json_out_character(jstring, '}')) < 0)
//            return (iretn);
//    }

//    if ((iretn=json_out_character(jstring, '{')) < 0)
//        return (iretn);
//    if ((iretn=json_out_name(jstring, (char *)"event_name")) < 0)
//        return (iretn);
//    if ((iretn=json_out_string(jstring, value.name, COSMOS_MAX_NAME)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring, '}')) < 0)
//        return (iretn);

//    if ((iretn=json_out_character(jstring, '{')) < 0)
//        return (iretn);
//    if ((iretn=json_out_name(jstring, (char *)"event_type")) < 0)
//        return (iretn);
//    if ((iretn=json_out_uint32(jstring, value.type)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring, '}')) < 0)
//        return (iretn);

//    if ((iretn=json_out_character(jstring, '{')) < 0)
//        return (iretn);
//    if ((iretn=json_out_name(jstring, (char *)"event_flag")) < 0)
//        return (iretn);
//    if ((iretn=json_out_uint32(jstring, value.flag)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring, '}')) < 0)
//        return (iretn);

//    if ((iretn=json_out_character(jstring, '{')) < 0)
//        return (iretn);
//    if ((iretn=json_out_name(jstring, (char *)"event_data")) < 0)
//        return (iretn);
//    if ((iretn=json_out_string(jstring, value.data, COSMOS_MAX_DATA)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring, '}')) < 0)
//        return (iretn);

//    if (value.flag & EVENT_FLAG_CONDITIONAL)
//    {
//        if ((iretn=json_out_character(jstring, '{')) < 0)
//            return (iretn);
//        if ((iretn=json_out_name(jstring, (char *)"event_condition")) < 0)
//            return (iretn);
//        if ((iretn=json_out_string(jstring, value.condition, COSMOS_MAX_DATA)) < 0)
//            return (iretn);
//        if ((iretn=json_out_character(jstring, '}')) < 0)
//            return (iretn);
//    }

//    return 0;
//}

////! ::beatstruc to JSON
///*! Appends a JSON entry to the current JSON stream for the indicated ::beatstruc.
//    \param jstring Reference to JSON stream.
//   \param value The JSON data of the desired variable
//    \return  0 if successful, negative error otherwise
//*/
//int32_t json_out_beatstruc(string &jstring,beatstruc value)
//{
//    int32_t iretn;

//    if ((iretn=json_out_character(jstring,'{')) < 0)
//        return (iretn);

//    // Output Ntype
//    if ((iretn=json_out_name(jstring,(char *)"ntype")) < 0)
//        return (iretn);
//    if ((iretn=json_out_uint16(jstring,(uint16_t)value.ntype)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring,',')) < 0)
//        return (iretn);

//    // Output IP Address
//    if ((iretn=json_out_name(jstring,(char *)"addr")) < 0)
//        return (iretn);
//    if ((iretn=json_out_string(jstring,value.addr,16)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring,',')) < 0)
//        return (iretn);

//    // Output IP Port
//    if ((iretn=json_out_name(jstring,(char *)"port")) < 0)
//        return (iretn);
//    if ((iretn=json_out_int32(jstring,value.port)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring,',')) < 0)
//        return (iretn);

//    // Output Buffer Size
//    if ((iretn=json_out_name(jstring,(char *)"bsz")) < 0)
//        return (iretn);
//    if ((iretn=json_out_int32(jstring,value.bsz)) < 0)
//        return (iretn);
//    if ((iretn=json_out_character(jstring,',')) < 0)
//        return (iretn);

//    // Output Beat Period
//    if ((iretn=json_out_name(jstring,(char *)"bprd")) < 0)
//        return (iretn);
//    if ((iretn=json_out_double(jstring,value.bprd)) < 0)
//        return (iretn);

//    if ((iretn=json_out_character(jstring,'}')) < 0)
//        return (iretn);
//    return 0;
//}


//! Extract JSON value matching name.
/*! Scan through the provided JSON stream looking for the supplied
    Namespace name. If it is found, return its value as a character
    string.
    \param json A string of JSON data
    \param token The Namespace name to be extracted.
    \return A character string representing the extracted value, otherwise NULL.
*/
string json_extract_namedobject(string json, string token)
{
    string tstring;
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

////! Tokenize using JSON Name Space.
///*! Scan through the provided JSON stream, matching names to the ::jsonmap.
// * for each match that is found, create a ::jsontoken entry and add it to a vector
// * of tokens.
// * \param jstring string containing JSON stream.
// * \param tokens vector of ::jsontoken.
//    \param cmeta Reference to ::cosmosmetastruc to use.
// * \return Zero or negative error.
// */
//int32_t json_tokenize(string jstring, CosmosClass *cinfo, vector<jsontoken> &tokens)
//{
//    const char *cvertex;
//    size_t length;
//    int32_t iretn;
//    jsontoken ttoken;

//    string val = json_extract_namedobject(jstring, "node_utc");
//    if (val.length()!=0) ttoken.utc = json_convert_double(val);
//    else
//    {
//        //Some older sets of telemetry do not include "node_utc" so the utc must be found elsewhere:
//        if ((val = json_extract_namedobject(jstring, "node_loc_pos_eci")).length()!=0)
//        {
//            if ((val=json_extract_namedobject(val, "utc")).length()!=0) ttoken.utc = json_convert_double(val);
//        }
//    }

//    length = jstring.size();
//    cvertex = &jstring[0];
//    while (*cvertex != 0 && *cvertex != '{')
//        cvertex++;
//    tokens.clear();
//    do
//    {
//        if (*cvertex != 0)
//        {
//            if ((iretn = json_tokenize_namedobject(cvertex, cinfo, ttoken)) < 0)
//            {
//                if (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP)
//                    iretn = 0;
//            }
//            else
//            {
//                tokens.push_back(ttoken);
//            }
//        }
//        else
//            iretn = JSON_ERROR_EOS;
//    } while (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP && *cvertex != 0 && (size_t)(cvertex-&jstring[0]) <= length);

//    if (!iretn) iretn = (int32_t)tokens.size();
//    return (iretn);
//}

////! Tokenize next JSON Named Pair
///*! Extract the next Named Pair from the provided JSON stream and place it in a ::jsontoken.
// * Leave pointer at the next Object in the string.
// * \param ptr Pointer to a pointer to a JSON stream.
// *    \param cmeta Reference to ::cosmosmetastruc to use.
// * \param token ::jsontoken to return.
// * \return Zero, or a negative error.
//*/
//int32_t json_tokenize_namedobject(const char* &ptr, CosmosClass *cinfo, jsontoken &token)
//{
//    int32_t iretn=0;
//    string ostring;
//    uint16_t hash, index;

//    if (!(cinfo->jmapped))
//    {
//        return (JSON_ERROR_NOJMAP);
//    }

//    // Skip over opening brace
//    if (ptr[0] != '{')
//    {
//        if ((iretn = json_skip_value(ptr)) < 0)
//            return (iretn);
//        else
//            return (JSON_ERROR_SCAN);
//    }

//    ptr++;

//    // Extract string that should hold name of this object.
//    if ((iretn = json_extract_string(ptr, ostring)) < 0)
//    {
//        if (iretn != JSON_ERROR_EOS)
//        {
//            if ((iretn = json_skip_value(ptr)) < 0)
//                return (iretn);
//            else
//                return (JSON_ERROR_SCAN);
//        }
//        else
//            return (iretn);
//    }
//    // Calculate hash
//    hash = json_hash(ostring);

//    // See if there is a match in the ::jsonmap.
//    for (index=0; index<cinfo->jmap[hash].size(); ++index)	{
//        if (ostring == cinfo->jmap[hash][index].name)
//        {
//            break;
//        }
//    }

//    if (index == cinfo->jmap[hash].size())
//    {
//        if ((iretn = json_skip_value(ptr)) < 0 && iretn != JSON_ERROR_EOS)
//        {
//            return (iretn);
//        }
//        else
//            return (JSON_ERROR_NOENTRY);
//    }
//    else
//    {
//        // Skip white space before separator
//        if ((iretn = json_skip_white(ptr)) < 0)
//        {
//            if (iretn != JSON_ERROR_EOS)
//            {
//                if ((iretn = json_skip_value(ptr)) < 0)
//                    return (iretn);
//                else
//                    return (JSON_ERROR_SCAN);
//            }
//            else
//                return (iretn);
//        }
//        // Skip separator
//        if ((iretn = json_skip_character(ptr,':')) < 0)
//        {
//            if (iretn != JSON_ERROR_EOS)
//            {
//                if ((iretn = json_skip_value(ptr)) < 0)
//                    return (iretn);
//                else
//                    return (JSON_ERROR_SCAN);
//            }
//            else
//                return (iretn);
//        }
//        // Skip white space before value
//        if ((iretn = json_skip_white(ptr)) < 0)
//        {
//            if (iretn != JSON_ERROR_EOS)
//            {
//                if ((iretn = json_skip_value(ptr)) < 0)
//                    return (iretn);
//                else
//                    return (JSON_ERROR_SCAN);
//            }
//            else
//                return (iretn);
//        }
//        // Read value
//        string input;
//        if ((iretn = json_extract_value(ptr, input)) < 0)
//        {
//            if (iretn != JSON_ERROR_EOS)
//            {
//                if ((iretn = json_skip_value(ptr)) < 0)
//                    return (iretn);
//                else
//                    return (JSON_ERROR_SCAN);
//            }
//            else
//                return (iretn);
//        }
//        if (input.size())
//        {
//            token.value = input;
//            token.handle.hash = hash;
//            token.handle.index = index;
//        }
//        //Skip whitespace after value
//        if ((iretn = json_skip_white(ptr)) < 0)
//        {
//            return (iretn);
//        }
//        // Skip over closing brace
//        if ((iretn = json_skip_character(ptr,'}')) < 0)
//        {
//            if ((iretn = json_skip_value(ptr)) < 0)
//                return (iretn);
//            else
//                return (JSON_ERROR_SCAN);
//        }
//    }

//    //	ptr++;
//    json_skip_white(ptr);
//    return (iretn);
//}

////! Parse JSON using Name Space.
///*! Scan through the provided JSON stream, matching names to the ::jsonmap. For
// * each match that is found, load the associated data item with the accompanying data.
// * This function supports complex data types.
//    \param jstring A string of JSON data
//    \param cmeta Reference to ::cosmosmetastruc to use.
//    \param cdata Reference to ::cosmosdatastruc to use.

//    \return Zero or negative error.
//*/
//int32_t json_parse(string jstring, CosmosClass *cinfo)
//{
//    const char *cvertex;
//    size_t length;
//    int32_t iretn;
//    uint32_t count = 0;

//    length = jstring.size();
//    cvertex = &jstring[0];
//    while (*cvertex != 0 && *cvertex != '{')
//        cvertex++;
//    do
//    {
//        // is this the only reference to endlines?
//        if (*cvertex != 0)// && *cvertex != '\r' && *cvertex != '\n')
//            //if (*cvertex != 0 && *cvertex != '\r' && *cvertex != '\n')
//        {
//            if ((iretn = json_parse_namedobject(cvertex, cinfo)) < 0)
//            {
//                if (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP)
//                    iretn = 0;
//            }
//            else
//            {
//                ++count;
//            }
//        }
//        else
//            iretn = JSON_ERROR_EOS;
//    } while (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP && *cvertex != 0 && (size_t)(cvertex-&jstring[0]) <= length);

//    if (iretn >= 0)
//    {

//        cinfo->timestamp = currentmjd();
//        iretn = count;
//    }
//    return (iretn);
//}

////! Parse next JSON Named Pair
///*! Extract the next Named Pair from the provided JSON stream. Return a pointer to the next
// * Object in the string, and an error flag.
//    \param ptr Pointer to a pointer to a JSON stream.
//    \param cmeta Reference to ::cosmosmetastruc to use.
//    \param cdata Reference to ::cosmosdatastruc to use.
//    \return Zero, or a negative error.
//*/
//int32_t json_parse_namedobject(const char* &ptr, CosmosClass *cinfo)
//{
//    uint32_t hash;
//    int32_t iretn=0;
//    string ostring;

//    if (!(cinfo->jmapped))
//    {
//        return (JSON_ERROR_NOJMAP);
//    }

//    if (ptr[0] != '{')
//    {
//        if ((iretn = json_skip_value(ptr)) < 0)
//            return (iretn);
//        else
//            return (JSON_ERROR_SCAN);
//    }

//    ptr++;

//    // Extract string that should hold name of this object.
//    if ((iretn = json_extract_string(ptr, ostring)) < 0)
//    {
//        if (iretn != JSON_ERROR_EOS)
//        {
//            if ((iretn = json_skip_value(ptr)) < 0)
//                return (iretn);
//            else
//                return (JSON_ERROR_SCAN);
//        }
//        else
//            return (iretn);
//    }

//    // Calculate hash
//    hash = json_hash(ostring);

//    // See if there is a match in the ::jsonmap.
//    size_t n;
//    for (n=0; n<cinfo->jmap[hash].size(); ++n)	{
//        if (ostring == cinfo->jmap[hash][n].name)
//        {
//            break;
//        }
//    }

//    if (n == cinfo->jmap[hash].size())
//    {
//        if ((iretn = json_skip_value(ptr)) < 0 && iretn != JSON_ERROR_EOS)
//        {
//            return (iretn);
//        }
//        else
//            return (JSON_ERROR_NOENTRY);
//    }
//    else
//    {
//        if ((iretn = json_skip_white(ptr)) < 0)
//        {
//            if (iretn != JSON_ERROR_EOS)
//            {
//                if ((iretn = json_skip_value(ptr)) < 0)
//                    return (iretn);
//                else
//                    return (JSON_ERROR_SCAN);
//            }
//            else
//                return (iretn);
//        }
//        if ((iretn = json_skip_character(ptr,':')) < 0)
//        {
//            if (iretn != JSON_ERROR_EOS)
//            {
//                if ((iretn = json_skip_value(ptr)) < 0)
//                    return (iretn);
//                else
//                    return (JSON_ERROR_SCAN);
//            }
//            else
//                return (iretn);
//        }
//        //        if ((iretn = json_parse_value(ptr,cinfo->jmap[hash][n].type,cinfo->jmap[hash][n].offset,cinfo->jmap[hash][n].group, cinfo)) < 0)
//        if ((iretn = json_parse_value(ptr, cinfo->jmap[hash][n], cinfo)) < 0)
//        {
//            if (iretn != JSON_ERROR_EOS)
//            {
//                if ((iretn = json_skip_value(ptr)) < 0)
//                    return (iretn);
//                else
//                    return (JSON_ERROR_SCAN);
//            }
//            else
//            {
//                return (iretn);
//            }
//        }
//    }

//    ptr++;
//    json_skip_white(ptr);
//    if (iretn >= 0)
//    {
//        cinfo->jmap[hash][n].enabled = true;
//    }
//    return (iretn);
//}

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
    int32_t iretn;

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
int32_t json_parse_name(const char* &ptr, string& ostring)
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

