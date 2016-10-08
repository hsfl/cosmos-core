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

/*! \file demlib.h
*	\brief DEM Support header file
*/

//! \ingroup support
//! \defgroup demlib DEM Library
//! DEM (Digital Elevation Model).
//!
//! The COSMOS system integrates Digital Elevation Models (DEMs)for a variety of planetary surfaces.
//! Full globe support at 30 arc sec resolution is available for Earth, Mars, and Moon. Additional higher
//! resolution DEMs of select areas can be added. Additional placeholders are available for other major
//! planetary bodies, as well as targets of opportunity, such as asteroids can be used as the data
//! available.
//!
//! DEMs are stored as floating point numbers in groups of 4; representing altitude in meters, and a
//! Normal (NMAP) in x, y, and z. DEMs for each planetary body are stored at different resolutions, allowing
//! for a minimum of overhead. Only the DEMs required for the desired location and resolution are loaded.
//! Memory use is limited by recycling the oldest accessed DEMs and reclaiming their memory.
//!
//! General usage involves first first opening the DEM system for a given body through ::map_dem_open. This
//! This will return a handle that can be used for all subsequent DEM calls for that body. See \ref demlib_planets
//! for the constants for the various planets. Once a planetary body is open calls to ::map_dem_pixel will return
//! the DEM value for the requested Latitude and Longitude, at the requested resolution.

#ifndef MAP_DEM_H
#define MAP_DEM_H

#include "support/configCosmos.h"

//! \ingroup demlib
//! \defgroup demlib_constants DEM library constants
//! @{

#define MAP_DEM_ERROR_BODY -1
#define MAP_DEM_ERROR_SPECS -2

#define DEM_TYPE_SINGLE 1
#define DEM_TYPE_MULTI 2

#define DEM_CACHE_NUM 5
#define MAX_DEMINDEX 100
#define MAX_DEM_BODIES 20

//! @}

//! \ingroup demlib
//! \defgroup demlib_typedefs DEM library typedefs
//! @{

//! Location value
/*! Stores the altitude (in meters) and the topographic normal vector for a requested point.
*/
typedef struct
{
	float alt;
	float nmap[3];
} dem_pixel;

//! DEM support structure
/*! Internal structure for storing the various DEMs that are read in.
  */
typedef struct
{
	double utc;
	char name[50];
	double lonul;
	double latul;
	double lonlr;
	double latlr;
	double psize;
	double scale;
	uint32_t xcount;
	uint32_t ycount;
	double dlon;
	double dlat;
//	dem_pixel **pixel;
	std::vector< std::vector<dem_pixel> > pixel;
} map_dem_dem;

//! Planetary body support structure
/*! Internal structure for storing the information about a specific planetary body.
  */
typedef struct
{
	char name[50];
	double orbit;
	double radius;
	double highest;
	double vscale;
	double hscale;
	double htov;
	uint16_t demcount;
	map_dem_dem dems[30000];
	int16_t demindexi[200][400][MAX_DEMINDEX];
	uint16_t demindexc[200][400];
} map_dem_body;

//! @}

//! \ingroup demlib
//! \defgroup demlib_functions DEM library function calls
//! @{

map_dem_body *planet_dem(int body);
void map_dem_close(map_dem_body *body);
void map_dem_cache(int body,int num);
map_dem_body *map_dem_open(int body);
void map_dem_scale(map_dem_body *body, double vscale, double hscale);
int map_dem_tilt(int body, double lon, double lat, double scale, dem_pixel *pixel);
dem_pixel map_dem_pixel(int body, double lon, double lat, double res);
dem_pixel map_dem_pixel(int body,double lon, double lat, double res);
double map_dem_alt(int body, double lon, double lat, double res);
int map_dem_init();
//! @}

#endif
