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
#include <stdlib.h>
#include "agentlib.h"
#include "jsonlib.h"
#include "jsonlib.h"

char request[AGENTMAXBUFFER], output[AGENTMAXBUFFER];
cosmosstruc *cdata;

int main(int argc, char *argv[])
{
int nbytes, bus=0, state=0;
beatstruc cbeat;

switch (argc)
	{
case 4:
	state = atol(argv[3]);
case 3:
	bus = atol(argv[2]);
case 2:
	json_setup_node(argv[1],cdata);
	break;
default:
	printf("Usage: power_switch node{ bus {state}}\n");
	exit (1);
	break;
	}

if ((nbytes = agent_get_server(cdata, cdata[0].node.name,(char *)"engine",8,&cbeat)) > 0)
	{
	sprintf(request,"set_bus %d %d",bus,state);
	nbytes = agent_send_request(cdata, cbeat,request,output,AGENTMAXBUFFER,5);
	printf("%s [%d]\n",output,nbytes);
	}
else
	{
	if (!nbytes)
		printf("%s not found\n",argv[1]);
	else
		printf("Error: %d\n",nbytes);
	}

}
