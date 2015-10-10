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
#include "agentlib.h"
#include "ts2000_lib.h"
#include "astrodev_lib.h"
#include "ic9100_lib.h"

cosmosstruc *cdata;
string nodename;
string agentname;
size_t deviceindex;
size_t radioindex = 9999;
uint16_t radiotype = 9999;
string radiodevice;
uint16_t radioflag;

astrodev_handle astrodev;
ts2000_state ts2000;
ic9100_handle ic9100;

int32_t request_get_filter(char *request, char* response, void *);
int32_t request_get_frequency(char *request, char* response, void *);
int32_t request_set_filter(char *request, char* response, void *);
int32_t request_set_frequency(char *request, char* response, void *);


int main(int argc, char *argv[])
{
	int32_t iretn;

	switch (argc)
	{
	case 3:
		nodename = argv[1];
		agentname = (string)"radio_" + argv[2];
		break;
	default:
		printf("Usage: agent_radio node radio");
		exit (1);
		break;
	}

	// Establish the command channel and heartbeat
	if (!(cdata = agent_setup_server(AGENT_TYPE_UDP, nodename.c_str(), agentname.c_str(), 5.0, 0, AGENTMAXBUFFER)))
	{
			cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
			exit (AGENT_ERROR_JSON_CREATE);
	}
	else
	{
			cout<<"Starting " << agentname << " for Node: " << nodename << endl;
	}

	// Look for named radio so we can use the right one
	for (size_t i=0; i<cdata[0].devspec.rxr_cnt; ++i)
	{
		if (!strcmp(argv[2], cdata[0].piece[cdata[0].devspec.rxr[i]->gen.pidx].name))
		{
			deviceindex = cdata[0].devspec.rxr[i]->gen.cidx;
			radioindex = i;
			radiotype = DEVICE_TYPE_RXR;
			break;
		}
	}

	if (radiotype == 9999)
	{
		for (size_t i=0; i<cdata[0].devspec.txr_cnt; ++i)
		{
			if (!strcmp(argv[2], cdata[0].piece[cdata[0].devspec.txr[i]->gen.pidx].name))
			{
				deviceindex = cdata[0].devspec.txr[i]->gen.cidx;
				radioindex = i;
				radiotype = DEVICE_TYPE_TXR;
				break;
			}
		}
	}

	if (radiotype == 9999)
	{
		for (size_t i=0; i<cdata[0].devspec.tcv_cnt; ++i)
		{
			if (!strcmp(argv[2], cdata[0].piece[cdata[0].devspec.tcv[i]->gen.pidx].name))
			{
				deviceindex = cdata[0].devspec.tcv[i]->gen.cidx;
				radioindex = i;
				radiotype = DEVICE_TYPE_TCV;
				break;
			}
		}
	}

	if (radiotype == 9999)
	{
			cout<<"Exiting " << agentname << " for Node: " << nodename << " no radio found." << endl;
			agent_shutdown_server(cdata);
			exit (1);
	}

	radiodevice = cdata[0].port[cdata[0].device[deviceindex].all.gen.portidx].name;
	radioflag = cdata[0].device[deviceindex].all.gen.flag;

	switch (cdata[0].device[deviceindex].all.gen.model)
	{
	case DEVICE_MODEL_ASTRODEV:
		break;
	case DEVICE_MODEL_IC9100:
		iretn = ic9100_connect(radiodevice, radioflag, ic9100);
		if (iretn < 0)
		{
			cout<<"Exiting " << agentname << " for Node: " << nodename << " error connecting to IC9100 " << iretn << endl;
			agent_shutdown_server(cdata);
		}
		break;
	case DEVICE_MODEL_TS2000:
		break;
	default:
		cout<<"Exiting " << agentname << " for Node: " << nodename << " unknown model " << cdata[0].device[deviceindex].all.gen.model << endl;
		agent_shutdown_server(cdata);
		break;
	}

	while (agent_running(cdata))
	{
		switch (cdata[0].device[deviceindex].all.gen.model)
		{
		case DEVICE_MODEL_ASTRODEV:
			break;
		case DEVICE_MODEL_IC9100:
			iretn = ic9100_get_frequency(ic9100);
			break;
		case DEVICE_MODEL_TS2000:
			break;
		}
	}
}

int32_t request_get_frequency(char *request, char* response, void *)
{
	switch (radiotype)
	{
	case DEVICE_TYPE_RXR:
		sprintf(response,"%f", cdata[0].device[deviceindex].rxr.freqin);
		break;
	case DEVICE_TYPE_TXR:
		sprintf(response,"%f", cdata[0].device[deviceindex].txr.freqout);
		break;
	case DEVICE_TYPE_TCV:
		sprintf(response,"%f %f", cdata[0].device[deviceindex].tcv.freqin, cdata[0].device[deviceindex].txr.freqout);
		break;
	default:
		sprintf(response,"0.");
	}
	return 0;
}

int32_t request_get_filter(char *request, char* response, void *)
{
	switch (radiotype)
	{
	case DEVICE_TYPE_RXR:
		sprintf(response,"%f", cdata[0].device[deviceindex].rxr.filt);
		break;
	case DEVICE_TYPE_TCV:
		sprintf(response,"%f", cdata[0].device[deviceindex].tcv.filt);
		break;
	default:
		sprintf(response,"0.");
	}
	return 0;
}

int32_t request_get_rfpower(char *request, char* response, void *)
{
	switch (radiotype)
	{
	case DEVICE_TYPE_RXR:
		sprintf(response,"%f", cdata[0].device[deviceindex].rxr.filt);
		break;
	case DEVICE_TYPE_TCV:
		sprintf(response,"%f", cdata[0].device[deviceindex].tcv.filt);
		break;
	default:
		sprintf(response,"0.");
	}
	return 0;
}
