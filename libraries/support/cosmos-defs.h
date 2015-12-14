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

/*!	\file cosmos-defs.h
	\brief COSMOS definitions
	Definitions specific to the COSMOS environement
*/
//!	\defgroup  defs COSMOS Definitions
//! Constant definitions useful throughout COSMOS.

#ifndef _COSMOS_DEFS_H
#define _COSMOS_DEFS_H 1

#include <map>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

//using namespace std;

//! \ingroup defs
//! \defgroup defs_storage Constants defining limits on storage.
//! @{
//! These constants define the limits for various kinds of storage in COSMOS.
//! Largest JSON data
#define COSMOS_MAX_DATA 1024
//! Largest JSON name
#define COSMOS_MAX_NAME 40
//! @}

//! \ingroup defs
//! \defgroup defs_piece Constants defining Part types.
//! @{
//! These constants define the different Part types that a Node can be made of.

//! External Panel: n vertices and a thickness, subject to external forces
#define PIECE_TYPE_EXTERNAL_PANEL 0
//! Internal Panel: n vertices and a thickness
#define PIECE_TYPE_INTERNAL_PANEL 1
//! Box: 8 vertices defining 2 parallel sides and a wall thickness, first set curled pointing out, second set pointing in
#define PIECE_TYPE_BOX 2
//! Cylinder: 3 points and a wall thickness, first end, second end, point on radius of second end
#define PIECE_TYPE_CYLINDER 3
//! Sphere: 2 points and a wall thickness; center and point on surface.
#define PIECE_TYPE_SPHERE 4
//! Dimensionless:
#define PIECE_TYPE_DIMENSIONLESS 5
//! Cone: same as Cylinder except first end is a point.
#define PIECE_TYPE_CONE 6
//! @}


//! \ingroup defs
//! \defgroup defs_node_type Constants defining Node types.
//! @{
enum NODE_TYPE
{
NODE_TYPE_SATELLITE=0,
NODE_TYPE_GROUNDSTATION=1,
NODE_TYPE_MOC=2,
NODE_TYPE_VEHICLE=3,
NODE_TYPE_UAV=4,
NODE_TYPE_TARGET=5,
NODE_TYPE_BALLOON=6,
NODE_TYPE_SHIP=7,
NODE_TYPE_DATA=8,
NODE_TYPE_COMPUTER=9,
NODE_TYPE_SUN=10,
NODE_TYPE_MOON=11,
NODE_TYPE_MARS=12,
NODE_TYPE_LOCATION=13
    };

//! @}

//! \ingroup defs
//! \defgroup defs_event_flag Constants defining Event flags.
//! @{
/*! These constants are for defining Events. FLAGs define an aspect of the
 * Event. The defined value represents the bit position for that Flag. Where
 * the desired aspect has more than one level, FLAG only points to the first bit
 * position. In this case SCALE represents a multiplier to be used to place
 * the Flag value at the appropriate position. As an example, the exit event
 * of a paired event with an alarm level of 3 and a color of 7 would be
 * flagged as:
 * EVENT_FLAG_PAIR | EVENT_FLAG_EXIT | 3 * EVENT_SCALE_ALARM | 7 * EVENT_SCALE_COLOR
 */

// Bits 0-12

//! 8 bit flag, 0 - 7, see ::EVENT_SCALE_WARNING.
#define EVENT_FLAG_WARNING 0x0e00
//! Flash countdown timer (1 = 10sec, 2 = 20sec, 3 = 40sec, 4 = 80sec, 5 = 160sec, 6 = 320sec, 7 = 640sec).
#define EVENT_SCALE_WARNING 0x0200
//! 8 bit flag, 0 - 7, see ::EVENT_SCALE_ALARM.
#define EVENT_FLAG_ALARM 0x0180
//! Audible alarm
#define EVENT_SCALE_ALARM 0x0080
//! 8 bit flag, 0 - 7, see ::EVENT_SCALE_PRIORITY.
#define EVENT_FLAG_PRIORITY 0x0070
//! Event display priority (0 = never , 1 = always, 2 = orbit, 3 = 15min, 4 = 5min, 5 = max zoom)
#define EVENT_SCALE_PRIORITY 0x0010
//! 16 bit flag, see ::EVENT_SCALE_COLOR.
#define EVENT_FLAG_COLOR 0x000f
//! Display color (1=gray, 2= magenta, 3= cyan, 4= green, 5=orange, 6= yellow, 7=red, 8=brown, 9=white)
#define EVENT_SCALE_COLOR 0x0001

// bits 13-16

//! Show a countdown timer for this event.
#define EVENT_FLAG_COUNTDOWN	0x1000
//! Event is the exit member of a set of paired events.
#define EVENT_FLAG_EXIT			0x2000
//! Event is part of a pair (eg. enter and leave Umbra).
#define EVENT_FLAG_PAIR			0x4000
//! Event actualy happened (versus being predicted).
#define EVENT_FLAG_ACTUAL		0x8000

// bits 17-19

//! Conditional event
#define EVENT_FLAG_CONDITIONAL	0x10000
//! Repeating event
#define EVENT_FLAG_REPEAT		0x20000
//! Event true last time
#define EVENT_FLAG_TRUE			0x40000

//! @}

//! \ingroup defs
//! \defgroup defs_event_type Constants defining Event types.
//! @{
/*! These constants are for defining Events. TYPEs represent what
 * inititiated the event. If the type is ::EVENT_TYPE_CALCULATED, then it
 * was initiiated by some generic equation.
 */


#define EVENT_TYPE_REQUEST 0x1001

//! Physical Events


//Physical Events
#define EVENT_TYPE_PHYSICAL 0x1000
#define EVENT_TYPE_LATA 0x1101
#define EVENT_TYPE_LATD 0x1102
#define EVENT_TYPE_LATMAX 0x1110
#define EVENT_TYPE_LATMIN 0x1120
#define EVENT_TYPE_APOGEE 0x1201
#define EVENT_TYPE_PERIGEE 0x1202
#define EVENT_TYPE_UMBRA 0x1206
#define EVENT_TYPE_PENUMBRA 0x1208

//Ground Station Events
#define EVENT_TYPE_GS 0x1400
#define EVENT_TYPE_GS5 0x1401
#define EVENT_TYPE_GS10 0x1402
#define EVENT_TYPE_GSMAX 0x1404

//
#define EVENT_TYPE_TARG 0x1800
#define EVENT_TYPE_TARGMIN 0x1801

//Command Events
#define EVENT_TYPE_COMMAND 0x2000
#define EVENT_TYPE_BUS 0x2100
#define EVENT_TYPE_EPS 0x2110
#define EVENT_TYPE_ADCS 0x2120
#define EVENT_TYPE_PAYLOAD 0x2400
#define EVENT_TYPE_SYSTEM 0x2800

//Log Events
#define EVENT_TYPE_LOG 0x4000

//Message Events
#define EVENT_TYPE_MESSAGE 0x8000



//! @}

//! \ingroup defs
//! \defgroup defs_MOSTpanels Constants denoting the different types of generic information panels in MOST.
//! @{
#define PANEL_TYPE_EPS 1
#define PANEL_TYPE_ADCS 2
#define PANEL_TYPE_OBCS 3
#define PANEL_TYPE_COMM 4
#define PANEL_TYPE_PLOAD 5
#define PANEL_TYPE_TCS 6
#define PANEL_TYPE_PROP 7
//! @}

//! \ingroup defs
//! \defgroup defs_physics Constants defining simulator propagation modes.
//! @{
#define PHYSICS_MODE_ACTUAL 0
#define PHYSICS_MODE_LVLH 1
#define PHYSICS_MODE_SURFACE 2
//! @}

//! COSMOS heartbeat Multicast address
#define COSMOSMCAST "225.1.1.1"

#endif
