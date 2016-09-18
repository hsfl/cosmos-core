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

#include "physics/physicslib.h"
#include "math/mathlib.h"
#include "jsonlib.h"
#include "agent/agent.h"
#include "jsonlib.h"
#include "datalib.h"
#include "socketlib.h"
//#include <semaphore.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
#include <sys/stat.h>
//#include <sys/types.h>

char *output;

gj_handle gjh;
CosmosAgent *agent;
std::string reqjstring;
std::string mainjstring;
double logperiod=30, newlogperiod=30;

char logstring[AGENTMAXBUFFER-20];
int logcount;
char node[200]="";

int main(int argc, char *argv[])
{
	uint32_t order;
	double dt, mjdnow, mjdaccel, lmjd, cmjd;
	int rflag, iretn, mode;
	FILE *fdes;
	char fname[200];
	char *ibuf;
	struct stat fstat;
	tlestruc tline;

	dt = .1;
	mjdaccel = 1.;
	mode = 0;
	switch (argc)
	{
	case 5:
		mjdaccel = atof(argv[4]);
	case 4:
		dt = atof(argv[3]);
	case 3:
		mode = atol(argv[2]);
	case 2:
		strcpy(node,argv[1]);
		break;
	default:
		printf("Usage: agent_physics node [attitude_mode [dt [mjdaccel]]]\n");
		exit(-1);
	}

	rflag = 1;

	// Initialization stuff

    if (!(agent = new CosmosAgent(NetworkType::UDP, node, "physics", 2.)))
	{
		printf("Failed to setup server: %d\n",AGENT_ERROR_JSON_CREATE);
		exit (AGENT_ERROR_JSON_CREATE);
	}

	order = 8;

	sprintf(fname,"%s/state.ini",get_nodedir(node).c_str());

	if ((iretn=stat(fname,&fstat)) == 0 && (fdes=fopen(fname,"r")) != NULL)
	{
		ibuf = (char *)calloc(1,fstat.st_size+1);
		fgets(ibuf,fstat.st_size,fdes);
        json_parse(ibuf, agent->cinfo->meta, agent->cinfo->pdata);
		free(ibuf);
        mjdnow = agent->cinfo->pdata.node.loc.pos.eci.utc;
	}
	else
	{
		sprintf(fname,"%s/tle.ini",get_nodedir(node).c_str());
        if ((iretn=stat(fname,&fstat)) == 0 && (iretn=load_lines(fname, agent->cinfo->pdata.tle)) > 0)
		{
            tline = get_line(0, agent->cinfo->pdata.tle);
			mjdnow = currentmjd(0.);
            lines2eci(mjdnow, agent->cinfo->pdata.tle, agent->cinfo->pdata.node.loc.pos.eci);
		}
		else
		{
			mjdnow = 55593.416667827405;
            agent->cinfo->pdata.node.loc.pos.eci.s = agent->cinfo->pdata.node.loc.pos.eci.v = agent->cinfo->pdata.node.loc.pos.eci.a = rv_zero();
            agent->cinfo->pdata.node.loc.pos.eci.s.col[0] = -1354152.3069408732;
            agent->cinfo->pdata.node.loc.pos.eci.s.col[1] = 6794509.033329579;
            agent->cinfo->pdata.node.loc.pos.eci.s.col[2] = 905.69207709436682;
            agent->cinfo->pdata.node.loc.pos.eci.v.col[0] = 983.48186257069437;
            agent->cinfo->pdata.node.loc.pos.eci.v.col[1] = 195.00552418716413;
            agent->cinfo->pdata.node.loc.pos.eci.v.col[2] = 7518.5307318052819;
            agent->cinfo->pdata.node.loc.pos.eci.a.col[0] = 1.625564;
            agent->cinfo->pdata.node.loc.pos.eci.a.col[1] = -8.155423;
            agent->cinfo->pdata.node.loc.pos.eci.a.col[2] = -0.000244;
		}
	}

    agent->cinfo->pdata.node.loc.utc = mjdnow;
    ++agent->cinfo->pdata.node.loc.pos.eci.pass;
    agent->cinfo->pdata.physics.moi.col[0] = 1.;
    agent->cinfo->pdata.physics.moi.col[1] = 3.;
    agent->cinfo->pdata.physics.moi.col[2] = 5.;
    agent->cinfo->pdata.node.loc.att.icrf.v = rv_smult(.017453293,rv_unity());
    agent->cinfo->pdata.node.loc.att.icrf.s = q_eye();
    agent->cinfo->pdata.node.loc.att.icrf.a = rv_zero();
    pos_eci(&agent->cinfo->pdata.node.loc);

/*
    agent->cinfo->pdata.node.loc.att.lvlh.v = rv_zero();
    agent->cinfo->pdata.node.loc.att.lvlh.s = q_eye();
    agent->cinfo->pdata.node.loc.att.lvlh.a = rv_zero();
    ++agent->cinfo->pdata.node.loc.att.lvlh.pass;
    att_lvlh(&agent->cinfo->pdata.node.loc);
    loc_update(&agent->cinfo->pdata.node.loc);
*/

    hardware_init_eci(agent->cinfo->pdata.devspec, agent->cinfo->pdata.node.loc);
    gauss_jackson_init_eci(gjh, order, mode, dt, mjdnow, agent->cinfo->pdata.node.loc.pos.eci, agent->cinfo->pdata.node.loc.att.icrf, agent->cinfo->pdata.physics, agent->cinfo->pdata.node.loc);
    simulate_hardware(agent->cinfo->pdata, agent->cinfo->pdata.node.loc);
    mjdnow = agent->cinfo->pdata.node.loc.utc;

    agent->cinfo->pdata.node.utcoffset = mjdnow - currentmjd(0);
	lmjd = currentmjd(0);
	mjdaccel -= 1.;

	do
	{
		mjdnow = currentmjd(0.);
        gauss_jackson_propagate(gjh, agent->cinfo->pdata.physics, agent->cinfo->pdata.node.loc, mjdnow + agent->cinfo->pdata.node.utcoffset);
        simulate_hardware(agent->cinfo->pdata, agent->cinfo->pdata.node.loc);

		/*
        if (agent->cinfo->pdata.node.state == 1)
		{
			ipos.s.col[0] = -6605464.484;
			ipos.s.col[1] = -2233537.982;
			ipos.s.col[2] = 270957.434;
			ipos.v.col[0] = -603.693;
			ipos.v.col[1] = 877.7741;
			ipos.v.col[2] = -7482.349;
            orbit_init_eci(mode,mjdnow,1./dt,ipos,cinfo);
		}
		*/
        if (agent->cinfo->pdata.node.state == 2)
			rflag = 0;

		COSMOS_USLEEP(1);
		cmjd = currentmjd(0);
        agent->cinfo->pdata.node.utcoffset += mjdaccel * (cmjd-lmjd);
		lmjd = cmjd;
    } while (agent->running() && rflag);
}

//sem_wait(&data_sem);

char* request_utc(char* request, char* output)
{
    strcpy(output,json_of_utc(reqjstring, agent->cinfo->meta, agent->cinfo->pdata));
	return (output);
}

char* request_soh(char* request, char* output)
{
    strcpy(output,json_of_soh(reqjstring, agent->cinfo->meta, agent->cinfo->pdata));
    //strcpy(output,json_of_beacon(reqjstring, agent->cinfo->meta, agent->cinfo->pdata));
	return (output);
}

char* request_beacon(char* request, char* output)
{
    strcpy(output,json_of_beacon(reqjstring, agent->cinfo->meta, agent->cinfo->pdata));
	return (output);
}

char* request_statevec(char* request, char* output)
{
    strcpy(output,json_of_state_eci(reqjstring, agent->cinfo->meta, agent->cinfo->pdata));
	return (output);
}

char* request_set_bus(char* request, char* output)
{
	int j, k;

	sscanf(request,"set_bus %d %d",&j, &k);
	if (k)
	{
        agent->cinfo->pdata.devspec.bus[j]->gen.flag |= DEVICE_FLAG_ON;
	}
	else
	{
        agent->cinfo->pdata.devspec.bus[j]->gen.flag &= ~DEVICE_FLAG_ON;
	}
	return (output);
}   

char* request_imu(char* request, char* output)
{
	int j;

	sscanf(request,"imu %d",&j);
    strcpy(output,json_of_imu(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}   

char* request_get_rw(char* request, char* output)
{
	int j;

	sscanf(request,"get_rw %d",&j);
    strcpy(output,json_of_rw(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}   

char* request_set_rw_moment(char* request, char* output)
{
	int j;
	double value[3];

	sscanf(request,"set_rw_moment %d %lf %lf %lf",&j,&value[0],&value[1],&value[2]);

    agent->cinfo->pdata.devspec.rw[j]->mom.col[0] = value[0];
    agent->cinfo->pdata.devspec.rw[j]->mom.col[1] = value[1];
    agent->cinfo->pdata.devspec.rw[j]->mom.col[2] = value[2];
    strcpy(output,json_of_rw(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_rw_omega(char* request, char* output)
{
	int j;
	double value;

	sscanf(request,"%*s %d %lf",&j,&value);
    agent->cinfo->pdata.devspec.rw[j]->omg = value;
    strcpy(output,json_of_rw(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_rw_alpha(char* request, char* output)
{   
	int j;
	double value;

	sscanf(request,"%*s %d %lf",&j,&value);
    agent->cinfo->pdata.devspec.rw[j]->alp = value;
    strcpy(output,json_of_rw(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}

char* request_get_mtr(char* request, char* output)
{   
	int j;

	sscanf(request,"get_mtr %d",&j);
    strcpy(output,json_of_mtr(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_mtr_moment(char* request, char* output)
{
	int j;
	double value[3];

	sscanf(request,"set_mtr_moment %d %lf %lf %lf",&j,&value[0],&value[1],&value[2]);

    //agent->cinfo->pdata.devspec.mtr[j]->mom.col[0] = value[0];
    //agent->cinfo->pdata.devspec.mtr[j]->mom.col[1] = value[1];
    //agent->cinfo->pdata.devspec.mtr[j]->mom.col[2] = value[2];
    strcpy(output,json_of_mtr(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_mtr_current(char* request, char* output)
{
	int j;
	double value;

	sscanf(request,"set_mtr_current %d %lf",&j,&value);

    agent->cinfo->pdata.device[agent->cinfo->pdata.devspec.mtr[j]->gen.cidx].all.gen.amp = value;
    strcpy(output,json_of_mtr(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	printf("%s\n",output);
	return (output);
}

char* request_set_mtr_field(char* request, char* output)
{
	int j;
	double value;

	sscanf(request,"set_mtr_field %d %lf",&j,&value);

    agent->cinfo->pdata.device[agent->cinfo->pdata.devspec.mtr[j]->gen.cidx].all.gen.amp = value*(4.838e-3+value*(-3.958e-5+value*3.053e-6));
    strcpy(output,json_of_rw(reqjstring, j, agent->cinfo->meta, agent->cinfo->pdata));
	return (output);
}

