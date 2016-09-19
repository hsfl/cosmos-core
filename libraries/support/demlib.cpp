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

#include "configCosmos.h"

#include "demlib.h"
#include "timelib.h"
#include "datalib.h"
//#ifdef _MSC_BUILD
//#include "dirent/dirent.h"
//#else
//#include <dirent.h>
//#endif
#include <sys/stat.h>

map_dem_body *bodies[20] = {NULL};
char bodynames[20][15] = {"mercury","venus","earth","mars","jupiter","saturn","uranus","neptune","pluto","moon","sun","near","","","","","","","",""};
static uint32_t maxalloc=300000000L, totalloc=0;
//static sem_t *bsem,tsem;
static std::mutex bsem;
// running: 0 = uninitialized, 1 = ready, 2 = insufficient memory
static int running = 0;


//! Body for DEM
/*! Provides the basic information used by the DEM engine for the requested planetary body.
	\param  body Integer value of planetary body. See \ref
    convertlib_constants for values.
	\return ::map_dem_body for the requested planetary body.
*/
map_dem_body *planet_dem(int body)
{
	if (bodies[body-1] == NULL)
	{
		if ((bodies[body-1] = map_dem_open(body)) == NULL)
		{
			return nullptr;
		}
	}
	return (bodies[body-1]);
}

int map_dem_init()
{
	void *tptr;

	if (!running)
	{
//#ifdef COSMOS_MAC_OS
//		if ((bsem=sem_open("demcache",O_CREAT,O_RDWR,1)) != SEM_FAILED)
//#else
//		bsem = &tsem;
//		if ((sem_init(bsem,0,1)) != 0)
//#endif
//		{
//			return (DEM_ERROR_SEMINIT);
//		}
	}
	// Determine reasonable maxalloc, and whether we should run at all
	for (maxalloc=1U<<31; maxalloc>1U<<27; maxalloc=maxalloc>>1)
	{
		if ((tptr=malloc(maxalloc)) != NULL)
		{
			maxalloc = maxalloc>>4;
			running = 1;
			free(tptr);
			return 0;
		}
	}
	maxalloc = 0;
	// running: 0 = uninitialized, 1 = ready, 2 = insufficient memory
	running = 2;
	return DEM_ERROR_INSUFFICIENT_MEMORY;
}

void map_dem_cache(int body, int num)
{
	int indexl, count;
	uint32_t i, dalloc;
	double utcl;

	if (num > 10)
		num = 10;
//	sem_wait(bsem);
	bsem.lock();
	if (bodies[body-1] == NULL)
	{
//		sem_post(bsem);
		bsem.unlock();
		return;
	}

	do
	{
		count = 0;
		utcl = currentmjd(0.);
		indexl = -1;
		for (i=0; i<bodies[body-1]->demcount; i++)
		{
			if (bodies[body-1]->dems[i].pixel.size())
			{
				count++;
				if (bodies[body-1]->dems[i].utc <= utcl)
				{
					indexl = i;
					utcl = bodies[body-1]->dems[i].utc;
				}
			}
		}
		if (count > num)
		{
			bodies[body-1]->dems[indexl].utc = 0.;
			bodies[body-1]->dems[indexl].pixel.clear();
			dalloc = bodies[body-1]->dems[indexl].ycount * (bodies[body-1]->dems[indexl].xcount*sizeof(dem_pixel) + sizeof(dem_pixel *));
			if (totalloc >= dalloc)
			{
				totalloc -= dalloc;
			}
			else
			{
				totalloc = 0;
			}
		}
	} while (count > num);
//	sem_post(bsem);
	bsem.unlock();
}

map_dem_body *map_dem_open(int bodynum)
{
	int maxcount=0;
	// int maxir, maxic;
	char ttname[200];
	std::string tname;
	FILE *fp, *fp1;
	map_dem_body *body;
	int iretn, iretn1, ir, ic, irmin, irmax, icmin, icmax;
	uint16_t demtype, dc;

	if (bodies[bodynum-1] != NULL)
		return (bodies[bodynum-1]);

	if (!running)
	{
		if ((iretn=map_dem_init()) < 0)
		{
			errno = -iretn;
			return nullptr;
		}
	}

	// running: 0 = uninitialized, 1 = ready, 2 = insufficient memory
	if (running == 2)
	{
		errno = -DEM_ERROR_INSUFFICIENT_MEMORY;
		return nullptr;
	}

	iretn = get_cosmosresources(tname);
	if (iretn < 0)
	{
		errno = -iretn;
		return nullptr;
	}
	tname += "/mapping/";
	tname += bodynames[bodynum-1];
	tname += "/body.dat";
	if ((fp=fopen(tname.c_str(),"r"))==NULL)
	{
		errno = -iretn;
		return nullptr;
	}
	body = (map_dem_body *)calloc(1,sizeof(map_dem_body));
	if (body == NULL)
	{
		errno = -DEM_ERROR_INSUFFICIENT_MEMORY;
		return nullptr;
	}


	iretn = fscanf(fp,"%lf %lf %lf",&body->orbit,&body->radius,&body->highest);
	body->highest = body->radius + body->highest/1000.;
	fclose(fp);
	strcpy(body->name,bodynames[bodynum-1]);
	body->vscale = body->hscale = body->htov = 1.;

	iretn = get_cosmosresources(tname);
	if (iretn < 0)
	{
		errno = -iretn;
		return nullptr;
	}
	tname += "/mapping/";
	tname += bodynames[bodynum-1];
	tname += "/dems5.dat";
	if ((fp=fopen(tname.c_str(),"r"))==NULL)
	{
		errno = -DEM_ERROR_NOTFOUND;
		return nullptr;
	}

	dc = 0;
	while ((iretn=fscanf(fp,"%s %lf %lf %u %u %lf %hu",body->dems[dc].name,&body->dems[dc].lonul,&body->dems[dc].latul,&body->dems[dc].xcount,&body->dems[dc].ycount,&body->dems[dc].psize,&demtype)) != EOF)
	{
		body->dems[dc].psize = RADOF(body->dems[dc].psize);
		body->dems[dc].pixel.clear();
		switch (demtype)
		{
		case DEM_TYPE_SINGLE:
			body->dems[dc].lonul = RADOF(body->dems[dc].lonul)-body->dems[dc].psize/2.;
			body->dems[dc].lonlr = body->dems[dc].lonul + (body->dems[dc].xcount) * body->dems[dc].psize;
			body->dems[dc].latul = RADOF(body->dems[dc].latul)+body->dems[dc].psize/2.;
			body->dems[dc].latlr = body->dems[dc].latul - (body->dems[dc].ycount) * body->dems[dc].psize;
			dc++;
			break;
		case DEM_TYPE_MULTI:
			std::string tname;
			iretn = get_cosmosresources(tname);
			if (iretn < 0)
			{
				errno = -iretn;
				return nullptr;
			}
			tname += "/mapping/";
			tname += bodynames[bodynum-1];
			tname += "/";
			tname += body->dems[dc].name;
			tname += "/dems5.dat";
			if ((fp1=fopen(tname.c_str(),"r"))==NULL)
			{
				errno = -DEM_ERROR_NOTFOUND;
				return nullptr;
			}

//			strcpy(tname,body->dems[dc].name);
			tname = body->dems[dc].name;

			while ((iretn1=fscanf(fp1,"%s %lf %lf %u %u %lf %hu",ttname,&body->dems[dc].lonul,&body->dems[dc].latul,&body->dems[dc].xcount,&body->dems[dc].ycount,&body->dems[dc].psize,&demtype)) != EOF)
			{
				sprintf(body->dems[dc].name,"%s/%s",tname.c_str(),ttname);
				body->dems[dc].psize = RADOF(body->dems[dc].psize);
//				body->dems[dc].pixel = NULL;
				body->dems[dc].pixel.clear();
				body->dems[dc].lonul = RADOF(body->dems[dc].lonul)-body->dems[dc].psize/2.;
				body->dems[dc].lonlr = body->dems[dc].lonul + (body->dems[dc].xcount) * body->dems[dc].psize;
				body->dems[dc].latul = RADOF(body->dems[dc].latul)+body->dems[dc].psize/2.;
				body->dems[dc].latlr = body->dems[dc].latul - (body->dems[dc].ycount) * body->dems[dc].psize;
				dc++;
			}
			fclose(fp1);
			break;
		}
	}
	fclose(fp);

	// Now populate demindex matrix

	for (uint16_t i=0; i<dc; i++)
	{
		irmin = (int)(200.*(DPI2+body->dems[i].latlr)/DPI);
		if (irmin < 0)
			irmin = 0;
		irmax = (int)(200.*(DPI2+body->dems[i].latul)/DPI);
		if (irmax > 199)
			irmax = 199;
		for (ir=irmin; ir<=irmax; ir++)
		{
			icmin = (int)(200.*(DPI+body->dems[i].lonul)/DPI);
			if (icmin < 0)
				icmin = 0;
			icmax = (int)(200.*(DPI+body->dems[i].lonlr)/DPI);
			if (icmax > 399)
				icmax = 399;
			for (ic=icmin; ic<=icmax; ic++)
			{
				uint16_t j;
				for (j=0; j<body->demindexc[ir][ic]; j++)
				{
					if (body->demindexi[ir][ic][j] == i)
					{
						break;
					}
				}
				if (j == body->demindexc[ir][ic] && body->demindexc[ir][ic] < MAX_DEMINDEX)
				{
					body->demindexi[ir][ic][body->demindexc[ir][ic]] = i;
					body->demindexc[ir][ic]++;
				}
				if (body->demindexc[ir][ic] > maxcount)
				{
					maxcount = body->demindexc[ir][ic];
				}
			}
		}
	}

	body->demcount = dc;
	bodies[bodynum-1] = body;
	return (body);
}

void map_dem_scale(map_dem_body *body, double vscale, double hscale)
{
	body->vscale = vscale;
	body->hscale = hscale;
	body->highest = body->radius + vscale * (body->highest - body->radius);
}

double map_dem_alt(int body, double lon, double lat, double res)
{
	dem_pixel pixel;

	pixel = map_dem_pixel(body,lon,lat,res);

	return (pixel.alt);
}

//! Height in DEM
/*! If the Lat:Lon is within one of the provided DEM's, return the
 * data for that pixel.
    \param body Integer value of planetary body. See \ref convertlib_constants for values.
	\param lon Longitude in readians.
	\param lat Latitude in radians.
	\param res Best resolution required, in radians.
	\return ::dem_pixel for that location.
*/
dem_pixel map_dem_pixel(int body, double lon, double lat, double res)
{
	dem_pixel pixel={0., {0., 0., 1.}};
	double decrow, deccol, cutc;
	int32_t erow, ecol, dci, cidx, cbdy;
	uint32_t drow, dcol;
	uint32_t i, j, dsize, m;
	map_dem_dem *sdem;
	FILE *fp;

	// running: 0 = uninitialized, 1 = ready, 2 = insufficient memory
	if (running == 2)
		return (pixel);

	if (bodies[body-1] == NULL)
	{
		if (map_dem_open(body) == NULL)
			return (pixel);
	}

	if (std::isnan(lat) || std::isnan(lon) || lat<-DPI2 || lat>DPI || lon<-DPI || lon>DPI)
		return (pixel);

	if (bodies[body-1] == NULL)
		return (pixel);

	sdem = NULL;

	// First: Find which DEM we need

	ecol = (int32_t)(400.*(lon + DPI)/D2PI);
	if (ecol < 0)
		ecol = 0;
	if (ecol > 399)
		ecol = 399;

	erow = (int32_t)(200.*(lat + DPI2)/DPI);
	if (erow < 0)
		erow = 0;
	if (erow > 199)
		erow = 199;

	dci = -1;
	// Search the appropriate demindex for a DEM that fits our needs.
	for (i=0; i<bodies[body-1]->demindexc[erow][ecol]; i++)
	{
		j = bodies[body-1]->demindexi[erow][ecol][i];
		if (lon >= bodies[body-1]->dems[j].lonul-1e-13 && lon <= bodies[body-1]->dems[j].lonlr+1e-13 && lat >=
		bodies[body-1]->dems[j].latlr-1e-13 && lat <= bodies[body-1]->dems[j].latul+1e-13)
		{
			if (dci < 0)
				dci = j;
			if (res/bodies[body-1]->dems[j].psize >= 2.)
				break;
			dci = j;
		}
	}

	// Check whether the DEM is already loaded and load if necessary
//	sem_wait(bsem);
	bsem.lock();
	sdem = &bodies[body-1]->dems[dci];
//	if (sdem->pixel == NULL)
	if (sdem->pixel.size() == 0)
	{
		dsize = sdem->ycount*(sizeof(dem_pixel *) + sdem->xcount*sizeof(dem_pixel));
		while (totalloc + dsize > maxalloc)
		{
			cbdy = cidx = -1;
			cutc = currentmjd(0.);
			for (m=0; m<MAX_DEM_BODIES; m++)
			{
				if (bodies[m])
				{
					for (i=0; i<bodies[m]->demcount; i++)
					{
//						if (bodies[m]->dems[i].pixel)
						if (bodies[m]->dems[i].pixel.size())
						{
							if (bodies[m]->dems[i].utc < cutc)
							{
								cidx = i;
								cbdy = m;
								cutc = bodies[m]->dems[i].utc;
							}
						}
					}
					if (cidx >= 0)
					{
						bodies[cbdy]->dems[cidx].pixel.clear();
						totalloc -= bodies[cbdy]->dems[cidx].ycount*(sizeof(dem_pixel *) + bodies[cbdy]->dems[cidx].xcount*sizeof(dem_pixel));
						bodies[cbdy]->dems[cidx].utc = 0.;
						if (totalloc + dsize <= maxalloc)
							break;
					}
				}
			}
		}
		std::string fname;
		int32_t iretn = get_cosmosresources(fname);
		if (iretn < 0)
		{
			errno = -iretn;
			return pixel;
		}
		fname += "/mapping/";
		fname += bodies[body-1]->name;
		fname += "/";
		fname += sdem->name;
		fp = fopen(fname.c_str(),"rb");
		sdem->pixel.resize(sdem->ycount);
		if (sdem->pixel.size() != sdem->ycount)
		{
			maxalloc = (uint32_t)(.9 * maxalloc);
			return (pixel);
		}
		for (i=0; i<sdem->ycount; i++)
		{
			sdem->pixel[i].resize(sdem->xcount);
			if (sdem->pixel[i].size() != sdem->xcount)
			{
				sdem->pixel.clear();
				maxalloc = (uint32_t)(.9 * maxalloc);
				return (pixel);
			}
			size_t count = fread(sdem->pixel[i].data(),sizeof(dem_pixel),sdem->xcount,fp);
			if (!count)
			{
				break;
			}
		}
		sdem->utc = currentmjd(0.);
		totalloc += dsize;
		if (maxalloc < totalloc + 2 * dsize)
		{
			maxalloc += 2 * dsize;

		}
		fclose(fp);
	}

	decrow = ((sdem->latul - lat) /sdem->psize) - .5;
	if (decrow < 0.)
		drow = 0;
	else
		drow = (uint32_t)decrow;
	if (drow >= sdem->ycount)
		drow = sdem->ycount - 1;
	deccol = ((lon - sdem->lonul) /sdem->psize) + .5;
	if (deccol < 0.)
		dcol = 0;
	else
		dcol = (int32_t)deccol;
	if (dcol >= sdem->xcount)
		dcol = sdem->xcount - 1;

	pixel = sdem->pixel[drow][dcol];
//	sem_post(bsem);
	bsem.unlock();
	pixel.alt = (float)(pixel.alt * bodies[body-1]->vscale);
	pixel.nmap[2] = (float)(pixel.nmap[2] * bodies[body-1]->htov);

	return (pixel);
}

int map_dem_tilt(int body, double lon, double lat, double scalekm, dem_pixel *pixel)
{
	double tiltrho;

	if (lat<-DPI2 || lat>DPI || lon<-DPI || lon>DPI || scalekm <=0. || scalekm >1000.)
		return (-1);

	tiltrho = scalekm/bodies[body-1]->radius;
	*pixel = map_dem_pixel(body,lon,lat,tiltrho);
	bodies[body-1]->htov = (bodies[body-1]->hscale/bodies[body-1]->vscale);
	return 0;
}
