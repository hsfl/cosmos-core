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
#include <sys/stat.h>
//#include <sys/types.h>
#include <stdio.h>
//#ifdef _MSC_BUILD
//#include "dirent/dirent.h"
//#else
//#include <dirent.h>
//#endif

#include "agent/agentclass.h"
#include "jsonlib.h"

CosmosAgent *agent;

int main(int argc, char *argv[])
{
char direc[20] = "telemetry";
char yearname[5], dayname[4];
std::string  answer;
char toname[150], fromname[150];
int i;
DIR *cdir;
struct dirent *nextdir;
beatstruc hbeat;

if ((i = agent->get_server("", "soh", 2, &hbeat)) > 0)
	{
    i = agent->send_request(hbeat, "reopen", answer, 1.);
	COSMOS_USLEEP(2000000);
	}

cdir = opendir(direc);

while ((nextdir=readdir(cdir)) != NULL)
	{
	if (nextdir->d_name[0] == '.')
		continue;
	for (i=0; i<20; i++)
		{
		if (nextdir->d_name[i] == '.')
			break;
		}
	strncpy(yearname,&nextdir->d_name[i+1],4);
	yearname[4] = 0;
	strncpy(dayname,&nextdir->d_name[i+5],3);
	dayname[3] = 0;
	sprintf(fromname,"%s/%s",direc,nextdir->d_name);
#if defined(COSMOS_WIN_OS)
	mkdir("data");
#else
	mkdir("data",00775);
#endif
	sprintf(toname,"data/%s",yearname);
#if defined(COSMOS_WIN_OS)
	mkdir(toname);
#else
	mkdir(toname,00775);
#endif
	sprintf(toname,"data/%s/%s",yearname,dayname);
#if defined(COSMOS_WIN_OS)
	mkdir(toname);
#else
	mkdir(toname,00775);
#endif
	sprintf(toname,"data/%s/%s/%s",yearname,dayname,nextdir->d_name);
	rename(fromname,toname);
	}
}
