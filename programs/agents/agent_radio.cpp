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
uint16_t radioaddr;
bool radioconnected = false;
size_t channelnum = 0;

astrodev_handle astrodev;
ts2000_state ts2000;
ic9100_handle ic9100;

tcvstruc target;

int32_t lasterrorcode;
char lasterrormessage[300];

int32_t request_get_bandpass(char *request, char* response, void *);
int32_t request_get_frequency(char *request, char* response, void *);
int32_t request_get_opmode(char *request, char* response, void *);
int32_t request_get_power(char *request, char* response, void *);
int32_t request_set_bandpass(char *request, char* response, void *);
int32_t request_set_frequency(char *request, char* response, void *);
int32_t request_set_opmode(char *request, char* response, void *);
int32_t request_set_power(char *request, char* response, void *);

int32_t connect_radio();
string opmode2string(uint8_t opmode);

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
	if (!(cdata = agent_setup_server(AGENT_TYPE_UDP, nodename.c_str(), agentname.c_str(), 1.0, 0, AGENTMAXBUFFER)))
	{
		cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
		exit (AGENT_ERROR_JSON_CREATE);
	}
	else
	{
		cout<<"Starting " << agentname << " for Node: " << nodename << endl;
	}

	// Add requests
	if ((iretn=agent_add_request(cdata, (char *)"get_frequency",request_get_frequency,"", "returns the radio frequency")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"get_bandpass",request_get_bandpass,"", "returns the radio filter bandpass")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"get_opmode",request_get_opmode,"", "returns the radio mode")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"get_power",request_get_power,"", "returns the radio power")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_frequency",request_set_frequency,"", "sets the radio frequency")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_bandpass",request_set_bandpass,"", "sets the radio filter bandpass")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_opmode",request_set_opmode,"", "sets the radio operating mode")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_power",request_set_power,"", "sets the radio power")))
		exit (iretn);


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

	// Set SOH string
	char sohstring[200];
	switch (radiotype)
	{
	case DEVICE_TYPE_TXR:
		sprintf(sohstring, "{\"device_txr_freq_%03d\",\"device_txr_maxpower_%03d\",\"device_txr_power_%03d\",\"device_txr_opmode_%03d\"}", radioindex, radioindex, radioindex, radioindex);
		break;
	case DEVICE_TYPE_RXR:
		sprintf(sohstring, "{\"device_rxr_freq_%03d\",\"device_rxr_power_%03d\",\"device_rxr_band_%03d\",\"device_rxr_opmode_%03d\"}", radioindex, radioindex, radioindex, radioindex);
	case DEVICE_TYPE_TCV:
		sprintf(sohstring, "{\"device_tcv_freq_%03d\",\"device_tcv_power_%03d\",\"device_tcv_maxpower_%03d\",\"device_tcv_band_%03d\",\"device_tcv_opmode_%03d\"}", radioindex, radioindex, radioindex, radioindex, radioindex);
		break;
	}
	agent_set_sohstring(cdata, sohstring);

	radiodevice = cdata[0].port[cdata[0].device[deviceindex].all.gen.portidx].name;
	radioaddr = cdata[0].device[deviceindex].all.gen.addr;

	// Initialize values so connect_radio will work
	target = cdata[0].device[deviceindex].tcv;

	iretn = connect_radio();

	while (agent_running(cdata))
	{
		if (radioconnected)
		{
			switch (cdata[0].device[deviceindex].all.gen.model)
			{
			case DEVICE_MODEL_ASTRODEV:
				break;
			case DEVICE_MODEL_IC9100:
				iretn = ic9100_get_frequency(ic9100);
				if (iretn >= 0)
				{
					cdata[0].device[deviceindex].tcv.freq = ic9100.frequency;
					/*
					if (ic9100_freq2band(target.freq) != ic9100.freqband)
					{
						iretn = ic9100_set_channel(ic9100, IC9100_CHANNEL_SWAP);
						iretn = ic9100_get_frequency(ic9100);
					}
*/
					if (target.freq != ic9100.frequency)
					{
						iretn = ic9100_set_frequency(ic9100, target.freq);
					}
				}
				else
				{
					radioconnected = false;
				}

				iretn = ic9100_get_bandpass(ic9100);
				if (iretn >= 0)
				{
					cdata[0].device[deviceindex].tcv.band = ic9100.bandpass;
					if (target.band != ic9100.bandpass)
					{
						iretn = ic9100_set_bandpass(ic9100, target.band);
					}
				}
				else
				{
					radioconnected = false;
				}

				iretn = ic9100_get_mode(ic9100);
				if (iretn >= 0)
				{
					cdata[0].device[deviceindex].tcv.opmode = ic9100.opmode;
					if (target.opmode != ic9100.opmode)
					{
						iretn = ic9100_set_mode(ic9100, target.opmode);
					}
				}
				else
				{
					radioconnected = false;
				}

				iretn = ic9100_get_rfpower(ic9100);
				if (iretn >= 0)
				{
					cdata[0].device[deviceindex].txr.maxpower = ic9100.maxpower;
					if (target.maxpower != ic9100.maxpower)
					{
						iretn = ic9100_set_rfpower(ic9100, target.maxpower);
					}
				}
				else
				{
					radioconnected = false;
				}

				//				iretn = ic9100_get_swrmeter(ic9100);
				//				iretn = ic9100_get_alcmeter(ic9100);
				iretn = ic9100_get_smeter(ic9100);
				if (iretn >= 0)
				{
					if (!ic9100.smeter)
					{
						iretn = ic9100_get_rfmeter(ic9100);
						if (iretn >= 0)
						{
							cdata[0].device[deviceindex].tcv.power = ic9100.power;
						}
						else
						{
							radioconnected = false;
						}
					}
					else
					{
						cdata[0].device[deviceindex].tcv.power = ic9100.power;
					}
				}
				else
				{
					radioconnected = false;
				}
				break;
			case DEVICE_MODEL_TS2000:
				break;
			}

			COSMOS_SLEEP(1.);
		}
		else
		{
			iretn = connect_radio();
			if (iretn < 0)
			{
				COSMOS_SLEEP(1.);
			}
		}
	}
}

int32_t request_get_frequency(char *request, char* response, void *)
{
	sprintf(response,"%f", cdata[0].device[deviceindex].tcv.freq);
	return 0;
}

int32_t request_set_frequency(char *request, char* response, void *)
{
	//	int32_t iretn;

	sscanf(request, "set_frequency %f", &target.freq);
	return 0;
}

int32_t request_get_bandpass(char *request, char* response, void *)
{
	sprintf(response,"%f", cdata[0].device[deviceindex].tcv.band);
	return 0;
}

int32_t request_set_bandpass(char *request, char* response, void *)
{
	int32_t iretn = 0;

	sscanf(request, "set_bandpass %f", &target.band);
	return iretn;
}

int32_t request_get_power(char *request, char* response, void *)
{
	sprintf(response,"%f", cdata[0].device[deviceindex].tcv.power);
	return 0;
}

int32_t request_set_power(char *request, char* response, void *)
{
	sscanf(request, "set_power %f", &target.power);
	return 0;
}

int32_t request_get_opmode(char *request, char* response, void *)
{
	strcpy(response, opmode2string(cdata[0].device[deviceindex].tcv.opmode).c_str());
	return 0;
}

int32_t request_set_opmode(char *request, char* response, void *)
{
	switch (request[strlen(request)])
	{
	case 'r':
	case 'R':
		switch(request[9])
		{
		case 'c':
		case 'C':
			target.opmode = DEVICE_RADIO_MODE_CWR;
			break;
		case 'r':
		case 'R':
			target.opmode = DEVICE_RADIO_MODE_RTTYR;
			break;
		}
		break;
	case 'd':
	case 'D':
		switch(request[9])
		{
		case 'a':
		case 'A':
			target.opmode = DEVICE_RADIO_MODE_AMD;
			break;
		case 'd':
		case 'D':
			target.opmode = DEVICE_RADIO_MODE_DVD;
			break;
		case 'f':
		case 'F':
			target.opmode = DEVICE_RADIO_MODE_FMD;
			break;
		case 'l':
		case 'L':
			target.opmode = DEVICE_RADIO_MODE_LSBD;
			break;
		case 'u':
		case 'U':
			target.opmode = DEVICE_RADIO_MODE_USBD;
			break;
		}
		break;
	default:
		switch(request[9])
		{
		case 'a':
		case 'A':
			target.opmode = DEVICE_RADIO_MODE_AM;
			break;
		case 'c':
		case 'C':
			target.opmode = DEVICE_RADIO_MODE_CW;
			break;
		case 'd':
		case 'D':
			target.opmode = DEVICE_RADIO_MODE_DV;
			break;
		case 'f':
		case 'F':
			target.opmode = DEVICE_RADIO_MODE_FM;
			break;
		case 'l':
		case 'L':
			target.opmode = DEVICE_RADIO_MODE_LSB;
			break;
		case 'r':
		case 'R':
			target.opmode = DEVICE_RADIO_MODE_RTTY;
			break;
		case 'u':
		case 'U':
			target.opmode = DEVICE_RADIO_MODE_USB;
			break;
		}
		break;
	}

	return 0;
}

int32_t connect_radio()
{
	int32_t iretn;
	radioconnected = false;

	switch (cdata[0].device[deviceindex].all.gen.model)
	{
	case DEVICE_MODEL_ASTRODEV:
		break;
	case DEVICE_MODEL_IC9100:
		iretn = ic9100_connect(radiodevice, radioaddr, ic9100);
		if (iretn < 0)
		{
			sprintf(lasterrormessage, "Unable to connect to IC9100: %d", iretn);
			lasterrorcode = iretn;
			return iretn;
		}
		iretn = ic9100_set_channel(ic9100, 0);
		if (iretn < 0)
		{
			sprintf(lasterrormessage, "Unable to set IC9100 to Main: %d", iretn);
			lasterrorcode = iretn;
			return iretn;
		}

		//		iretn = ic9100_set_freqband(ic9100, 13);

		iretn = ic9100_set_mode(ic9100, target.opmode);
		if (iretn < 0)
		{
			sprintf(lasterrormessage, "Unable to set IC9100 to %s: %d", opmode2string(target.opmode).c_str(), iretn);
			lasterrorcode = iretn;
			return iretn;
		}
		iretn = ic9100_set_bandpass(ic9100, target.band);
		if (iretn < 0)
		{
			sprintf(lasterrormessage, "Unable to set IC9100 bandpass to %f: %d", target.band, iretn);
			lasterrorcode = iretn;
			return iretn;
		}
		//		iretn = ic9100_set_datamode(ic9100, IC9100_DATAMODE_ON);
		//		if (iretn < 0)
		//		{
		//			sprintf(lasterrormessage, "Unable to set IC9100 Data mode On: %d", iretn);
		//			lasterrorcode = iretn;
		//			return iretn;
		//		}
		//		iretn = ic9100_set_bps9600mode(ic9100, IC9100_9600MODE_ON);
		//		if (iretn < 0)
		//		{
		//			sprintf(lasterrormessage, "Unable to set IC9100 to 9600 bps mode: %d", iretn);
		//			lasterrorcode = iretn;
		//			return iretn;
		//		}
		break;
	case DEVICE_MODEL_TS2000:
		break;
	default:
		sprintf(lasterrormessage, "Unknow radio model: %d", cdata[0].device[deviceindex].all.gen.model);
		lasterrorcode = GENERAL_ERROR_UNDEFINED;
		return GENERAL_ERROR_UNDEFINED;
		break;
	}

	radioconnected = true;
	return 0;
}

string opmode2string(uint8_t opmode)
{
	string result;
	switch (opmode)
	{
	case DEVICE_RADIO_MODE_AM:
		result = "AM";
		break;
	case DEVICE_RADIO_MODE_AMD:
		result = "AM Data";
		break;
	case DEVICE_RADIO_MODE_FM:
		result = "FM";
		break;
	case DEVICE_RADIO_MODE_FMD:
		result = "FM Data";
		break;
	case DEVICE_RADIO_MODE_LSB:
		result = "LSB";
		break;
	case DEVICE_RADIO_MODE_LSBD:
		result = "LSB Data";
		break;
	case DEVICE_RADIO_MODE_USB:
		result = "USB";
		break;
	case DEVICE_RADIO_MODE_USBD:
		result = "USB Data";
		break;
	case DEVICE_RADIO_MODE_RTTY:
		result = "RTTY";
		break;
	case DEVICE_RADIO_MODE_RTTYR:
		result = "RTTYR";
		break;
	case DEVICE_RADIO_MODE_DV:
		result = "DV";
		break;
	case DEVICE_RADIO_MODE_DVD:
		result = "DV Data";
		break;
	case DEVICE_RADIO_MODE_CWR:
		result = "CWR";
		break;
	case DEVICE_RADIO_MODE_CW:
		result = "CW";
		break;
	}
	return result;
}
