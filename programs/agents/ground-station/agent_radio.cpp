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

/*! \file
    \brief  Agent Radio for Satellite Ground Station

    This is the main agent that controls the ground station. It it the channel for the T&C and it is also responsible for the \ref agent_antenna and \ref agent_radio

*/

/*! \ingroup agents
    \defgroup agent_radio Agent Radio
    This agent controls the ground station radio (frequency, doppler shift, etc.).
    This agent is listed in agent_radio.cpp

    @code

List of available requests:

        help
                list of available requests for this agent

        shutdown
                request to shutdown this agent

        idle
                request to transition this agent to idle state

        monitor
                request to transition this agent to monitor state

        run
                request to transition this agent to run state

        status
                request the status of this agent

        getvalue {"name1","name2",...}
                get specified value(s) from agent

        setvalue {"name1":value},{"name2":value},...}
                set specified value(s) in agent

        listnames
                list the Namespace of the agent

        forward nbytes packet
                Broadcast JSON packet to the default SEND port on local network

        echo utc crc nbytes bytes
                echo array of nbytes bytes, sent at time utc, with CRC crc.

        nodejson
                return description JSON for Node

        statejson
                return description JSON for State vector

        utcstartjson
                return description JSON for UTC Start time

        piecesjson
                return description JSON for Pieces

        devgenjson
                return description JSON for General Devices

        devspecjson
                return description JSON for Specific Devices

        portsjson
                return description JSON for Ports

        targetsjson
                return description JSON for Targets

        aliasesjson
                return description JSON for Aliases

        enable enable
                Enable active control of the radio frequency

        disable disable
                Disable active control of the radio frequency

        get_state get_state
                returns the radio frequency

        get_frequency get_frequency
                returns the radio frequency

        get_bandpass get_bandpass
                returns the radio filter bandpass

        get_opmode get_opmode
                returns the radio mode

        get_powerin get_powerin
                returns the current RX radio power

        get_powerout get_powerout
                returns the current TX radio power

        set_frequency set_frequency Hz
                sets the radio frequency

        set_bandpass set_bandpass Hz
                sets the radio filter bandpass

        set_opmode set_opmode {am, amd, fm, fmd, dv, dvd, cw, cwr}
                sets the radio operating mode

        set_power set_maxpower watts
                sets the maximum TX radio power

        set_offset set_offset Hz
                sets the radio frequency offset



    @endcode
*/

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "device/general/ts2000_lib.h"
//#include "device/astrodev/astrodev_lib.h"
#include "device/general/ic9100_lib.h"

static Agent *agent;
static std::string nodename="";
static std::string radioname;
static size_t deviceindex;
static size_t radioindex = 9999;
static std::string radiodevice;
static uint16_t radioaddr;
static bool radioconnected = false;
static bool radioenabled = false;
static size_t channelnum = 0;
static float freqoffset;
static string port;

static uint16_t model;
static uint16_t radiotype = static_cast<uint16_t>(DeviceType::NONE);
static float freq;
static float band;
static uint8_t opmode = static_cast<uint8_t>(DEVICE_RADIO_MODE_UNDEF);

//astrodev_handle astrodev;
static ts2000_state ts2000;
static ic9100_handle ic9100;

static tcvstruc target;
static tcvstruc actual;
static tcvstruc initial;
static bool initialized = false;

static int32_t lasterrorcode;
static char lasterrormessage[300];

int32_t request_enable(string &request, string &response, Agent *);
int32_t request_disable(string &request, string &response, Agent *);
int32_t request_get_state(string &request, string &response, Agent *);
int32_t request_get_bandpass(string &request, string &response, Agent *);
int32_t request_get_frequency(string &request, string &response, Agent *);
int32_t request_get_opmode(string &request, string &response, Agent *);
int32_t request_get_powerin(string &request, string &response, Agent *);
int32_t request_get_powerout(string &request, string &response, Agent *);
int32_t request_set_bandpass(string &request, string &response, Agent *);
int32_t request_set_frequency(string &request, string &response, Agent *);
int32_t request_set_opmode(string &request, string &response, Agent *);
int32_t request_set_maxpower(string &request, string &response, Agent *);
int32_t request_set_offset(string &request, string &response, Agent *);

int32_t connect_radio();
int32_t disconnect_radio();
string opmode2string(uint8_t opmode);

int main(int argc, char *argv[])
{
    int32_t iretn;

    switch (argc)
    {
    case 9:
        if (static_cast<string>("ic9100") == argv[3])
        {
            model = static_cast<uint16_t>(DEVICE_MODEL_IC9100);
            port = argv[8];
            radioaddr = stoi(port.substr(port.find(':')+1));
            port = port.substr(0, port.find(':'));
        }
        else if (static_cast<string>("astrodev") == argv[3])
        {
            model = static_cast<uint16_t>(DEVICE_MODEL_ASTRODEV);
            port = argv[8];
        }
        if (static_cast<string>("txr") == argv[4])
        {
            radiotype = static_cast<uint16_t>(DeviceType::TXR);
        }
        else if (static_cast<string>("rxr") == argv[4])
        {
            radiotype = static_cast<uint16_t>(DeviceType::RXR);
        }
        else if (static_cast<string>("tcv") == argv[4])
        {
            radiotype = static_cast<uint16_t>(DeviceType::TCV);
        }
        freq = atof(argv[5]);
        band = atof(argv[6]);
        if (static_cast<string>("am") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_AM);
        }
        else if (static_cast<string>("amd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_AMD);
        }
        else if (static_cast<string>("fm") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_FM);
        }
        else if (static_cast<string>("fmd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_FMD);
        }
        else if (static_cast<string>("lsb") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_LSB);
        }
        else if (static_cast<string>("lsbd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_LSBD);
        }
        else if (static_cast<string>("usb") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_USB);
        }
        else if (static_cast<string>("usbd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_USBD);
        }
        else if (static_cast<string>("dv") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_DV);
        }
        else if (static_cast<string>("dvd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_DVD);
        }
        else if (static_cast<string>("cw") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_CW);
        }
        else if (static_cast<string>("cwr") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_CWR);
        }
        else if (static_cast<string>("rtty") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_RTTY);
        }
        else if (static_cast<string>("rttyr") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_RTTYR);
        }
    case 3:
        nodename = argv[2];
    case 2:
        radioname = argv[1];
        break;
    default:
        printf("Usage: agent->radio radioname [nodename  [ic9100/astrodev txr/rxr/tcv frequency bandwidth mode device:addr]]");
        exit (1);
        break;
    }

    // Establish the command channel and heartbeat
    if (!(agent = new Agent(nodename, radioname)))
    {
        std::cout << radioname << ": agent->setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<std::endl;
        exit (AGENT_ERROR_JSON_CREATE);
    }
    else
    {
        std::cout<<"Starting " << radioname << " for Node: " << nodename << std::endl;
    }

    if (argc > 3)
    {
        switch (radiotype)
        {
        case static_cast<uint16_t>(DeviceType::TXR):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::TXR);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex].txr.didx;
            break;
        case static_cast<uint16_t>(DeviceType::RXR):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::RXR);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex].rxr.didx;
            break;
        case static_cast<uint16_t>(DeviceType::TCV):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::TCV);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex].tcv.didx;
            break;
        }
        agent->cinfo->device[deviceindex].all.model = model;
        if (model == static_cast<uint16_t>(DEVICE_MODEL_IC9100))
        {
            agent->cinfo->device[deviceindex].all.addr = radioaddr;
        }
        agent->cinfo->device[deviceindex].all.type = radiotype;
        agent->cinfo->device[deviceindex].tcv.freq = freq;
        agent->cinfo->device[deviceindex].tcv.band = band;
        agent->cinfo->device[deviceindex].tcv.opmode = opmode;
        iretn = json_createport(agent->cinfo, port, PORT_TYPE_RS232);
        if (iretn >= 0)
        {
            agent->cinfo->device[deviceindex].all.portidx = iretn;
        }
        iretn = json_dump_node(agent->cinfo);
    }

    // Add requests
    if ((iretn=agent->add_request("enable",request_enable,"enable", "Enable active control of the radio frequency")))
        exit (iretn);
    if ((iretn=agent->add_request("disable",request_disable,"disable", "Disable active control of the radio frequency")))
        exit (iretn);
    if ((iretn=agent->add_request("get_state",request_get_state,"get_state", "returns the radio frequency")))
        exit (iretn);
    if ((iretn=agent->add_request("get_frequency",request_get_frequency,"get_frequency", "returns the radio frequency")))
        exit (iretn);
    if ((iretn=agent->add_request("get_bandpass",request_get_bandpass,"get_bandpass", "returns the radio filter bandpass")))
        exit (iretn);
    if ((iretn=agent->add_request("get_opmode",request_get_opmode,"get_opmode", "returns the radio mode")))
        exit (iretn);
    if ((iretn=agent->add_request("get_powerin",request_get_powerin,"get_powerin", "returns the current RX radio power")))
        exit (iretn);
    if ((iretn=agent->add_request("get_powerout",request_get_powerout,"get_powerout", "returns the current TX radio power")))
        exit (iretn);
    if ((iretn=agent->add_request("set_frequency",request_set_frequency,"set_frequency Hz", "sets the radio frequency")))
        exit (iretn);
    if ((iretn=agent->add_request("set_bandpass",request_set_bandpass,"set_bandpass Hz", "sets the radio filter bandpass")))
        exit (iretn);
    if ((iretn=agent->add_request("set_opmode",request_set_opmode,"set_opmode {am, amd, fm, fmd, dv, dvd, cw, cwr}", "sets the radio operating mode")))
        exit (iretn);
    if ((iretn=agent->add_request("set_power",request_set_maxpower,"set_maxpower watts", "sets the maximum TX radio power")))
        exit (iretn);
    if ((iretn=agent->add_request("set_offset",request_set_offset,"set_offset Hz", "sets the radio frequency offset")))
        exit (iretn);


    // Look for named radio so we can use the right one
    for (size_t i=0; i<agent->cinfo->devspec.rxr_cnt; ++i)
    {
        if (!strcmp(argv[2], agent->cinfo->pieces[agent->cinfo->device[agent->cinfo->devspec.rxr[i]].all.pidx].name))
        {
            deviceindex = agent->cinfo->devspec.rxr[i];
            radioindex = i;
            radiotype = static_cast<uint16_t>(DeviceType::RXR);
            break;
        }
    }

    if (radiotype == 9999)
    {
        for (size_t i=0; i<agent->cinfo->devspec.txr_cnt; ++i)
        {
            if (!strcmp(radioname.c_str(), agent->cinfo->pieces[agent->cinfo->device[agent->cinfo->devspec.txr[i]].all.pidx].name))
            {
                deviceindex = agent->cinfo->devspec.txr[i];
                radioindex = i;
                radiotype = static_cast<uint16_t>(DeviceType::TXR);
                break;
            }
        }
    }

    if (radiotype == 9999)
    {
        for (size_t i=0; i<agent->cinfo->devspec.tcv_cnt; ++i)
        {
            if (!strcmp(radioname.c_str(), agent->cinfo->pieces[agent->cinfo->device[agent->cinfo->devspec.tcv[i]].all.pidx].name))
            {
                deviceindex = agent->cinfo->devspec.tcv[i];
                radioindex = i;
                radiotype = static_cast<uint16_t>(DeviceType::TCV);
                break;
            }
        }
    }

    if (radiotype == 9999)
    {
        std::cout<<"Exiting " << radioname << " for Node: " << nodename << " no radio found." << std::endl;
        agent->shutdown();
        exit (1);
    }

    // Set SOH string
    char sohstring[200];
    switch (radiotype)
    {
    case static_cast<uint16_t>(DeviceType::TXR):
        sprintf(sohstring, "{\"device_txr_freq_%03lu\",\"device_txr_maxpower_%03lu\",\"device_txr_power_%03lu\",\"device_txr_opmode_%03lu\"}", radioindex, radioindex, radioindex, radioindex);
        break;
    case static_cast<uint16_t>(DeviceType::RXR):
        sprintf(sohstring, "{\"device_rxr_freq_%03lu\",\"device_rxr_power_%03lu\",\"device_rxr_band_%03lu\",\"device_rxr_opmode_%03lu\"}", radioindex, radioindex, radioindex, radioindex);
        break;
    case static_cast<uint16_t>(DeviceType::TCV):
        sprintf(sohstring, "{\"device_tcv_freq_%03lu\",\"device_tcv_powerin_%03lu\",\"device_tcv_powerout_%03lu\",\"device_tcv_maxpower_%03lu\",\"device_tcv_band_%03lu\",\"device_tcv_opmode_%03lu\"}", radioindex, radioindex, radioindex, radioindex, radioindex, radioindex);
        break;
    }
    agent->set_sohstring(sohstring);

    radiodevice = agent->cinfo->port[agent->cinfo->device[deviceindex].all.portidx].name;
    radioaddr = agent->cinfo->device[deviceindex].all.addr;

    // Initialize values so connect_radio will work
    target = agent->cinfo->device[deviceindex].tcv;
    actual = agent->cinfo->device[deviceindex].tcv;

    iretn = connect_radio();

    while (agent->running())
    {
        if (radioconnected)
        {
            switch (agent->cinfo->device[deviceindex].all.model)
            {
            case DEVICE_MODEL_LOOPBACK:
                {
                    agent->cinfo->device[deviceindex].tcv.freq = actual.freq - freqoffset;
                    if (radioenabled && target.freq != actual.freq)
                    {
                        actual.freq = target.freq;
                    }
                    agent->cinfo->device[deviceindex].tcv.band = actual.band;
                    if (radioenabled && target.band != actual.band)
                    {
                        actual.band = target.band;
                    }
                    actual.band = target.band;
                    agent->cinfo->device[deviceindex].tcv.opmode = actual.opmode;
                    if (radioenabled && target.opmode != actual.opmode)
                    {
                        actual.opmode = target.opmode;
                    }
                }
                break;
            case DEVICE_MODEL_ASTRODEV:
                break;
            case DEVICE_MODEL_IC9100:
                iretn = ic9100_get_frequency(ic9100);
                if (iretn >= 0)
                {
                    ic9100.frequency -= freqoffset;
                    agent->cinfo->device[deviceindex].tcv.freq = ic9100.frequency;
                    /*
                    if (ic9100_freq2band(target.freq) != ic9100.freqband)
                    {
                        iretn = ic9100_set_channel(ic9100, IC9100_CHANNEL_SWAP);
                        iretn = ic9100_get_frequency(ic9100);
                    }
*/
                    if (radioenabled && target.freq != ic9100.frequency)
                    {
                        iretn = ic9100_set_frequency(ic9100, target.freq + freqoffset);
                    }
                }
                else
                {
                    radioconnected = false;
                }

                iretn = ic9100_get_bandpass(ic9100);
                if (iretn >= 0)
                {
                    agent->cinfo->device[deviceindex].tcv.band = ic9100.bandpass;
                    if (radioenabled && target.band != ic9100.bandpass)
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
                    agent->cinfo->device[deviceindex].tcv.opmode = ic9100.opmode;
                    if (radioenabled && target.opmode != ic9100.opmode)
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
                    agent->cinfo->device[deviceindex].txr.maxpower = ic9100.maxpower;
                    //					if (radioenabled && target.maxpower != ic9100.maxpower)
                    //					{
                    //						iretn = ic9100_set_rfpower(ic9100, target.maxpower);
                    //					}
                }
                else
                {
                    radioconnected = false;
                }

                iretn = ic9100_get_smeter(ic9100);
                if (iretn >= 0)
                {
                    agent->cinfo->device[deviceindex].tcv.powerin = ic9100.powerin;
                }
                else
                {
                    radioconnected = false;
                }

                iretn = ic9100_get_rfmeter(ic9100);
                if (iretn >= 0)
                {
                    agent->cinfo->device[deviceindex].tcv.powerout = ic9100.powerout;
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

    disconnect_radio();

    agent->shutdown();
}

int32_t request_enable(string &req, string &response, Agent *)
{
    radioenabled = true;
    return 0;
}

int32_t request_disable(string &req, string &response, Agent *)
{
    radioenabled = false;
    return 0;
}

int32_t request_get_state(string &req, string &response, Agent *)
{
    response = ("[%.6f] Cx: %u En: %u Mode: %s TFreq: %.0f AFreq: %.0f Offset: %.0f PowerIn: %.2f PowerOut: %.2f MaxPower: %.2f",
            currentmjd(),
            radioconnected,
            radioenabled,
            opmode2string(agent->cinfo->device[deviceindex].tcv.opmode).c_str(),
            target.freq, agent->cinfo->device[deviceindex].tcv.freq,
            freqoffset,
            agent->cinfo->device[deviceindex].tcv.powerin,
            agent->cinfo->device[deviceindex].tcv.powerout,
            agent->cinfo->device[deviceindex].tcv.maxpower);
    return (0);
}

int32_t request_get_frequency(string &request, string &response, Agent *)
{
    response = ("%f", agent->cinfo->device[deviceindex].tcv.freq);
    return 0;
}

int32_t request_set_frequency(string &request, string &response, Agent *)
{
    //	int32_t iretn;

    sscanf(request.c_str(), "set_frequency %f", &target.freq);
    return 0;
}

int32_t request_get_bandpass(string &request, string &response, Agent *)
{
    response = ("%f", agent->cinfo->device[deviceindex].tcv.band);
    return 0;
}

int32_t request_set_bandpass(string &request, string &response, Agent *)
{
    int32_t iretn = 0;

    sscanf(request.c_str(), "set_bandpass %f", &target.band);
    return iretn;
}

int32_t request_get_powerin(string &request, string &response, Agent *)
{
    response = ("%f", agent->cinfo->device[deviceindex].tcv.powerin);
    return 0;
}

int32_t request_get_powerout(string &request, string &response, Agent *)
{
    response = ("%f", agent->cinfo->device[deviceindex].tcv.powerout);
    return 0;
}

int32_t request_set_maxpower(string &request, string &response, Agent *)
{
    sscanf(request.c_str(), "set_power %f", &target.maxpower);
    return 0;
}

int32_t request_set_offset(string &request, string &response, Agent *)
{
    sscanf(request.c_str(), "set_offset %f", &freqoffset);
    return 0;
}

int32_t request_get_opmode(string &request, string &response, Agent *)
{
    response = ( opmode2string(agent->cinfo->device[deviceindex].tcv.opmode).c_str());
    return 0;
}

int32_t request_set_opmode(string &request, string &response, Agent *)
{
    char mode[20];
    sscanf(request.c_str(), "set_opmode %s", mode);
    switch (mode[strlen(mode)-1])
    {
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
        target.opmode = atol(mode);
        break;
    case 'r':
    case 'R':
        switch(mode[0])
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
        switch(mode[0])
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
        switch(mode[0])
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

    switch (agent->cinfo->device[deviceindex].all.model)
    {
    case DEVICE_MODEL_LOOPBACK:
        {
            if (!initialized)
            {
                initialized = true;
                initial = actual;
            }
            radioconnected = true;
        }
        break;
    case DEVICE_MODEL_ASTRODEV:
////        iretn=astrodev_connect(radiodevice, astrodev);
//        if (iretn < 0)
//        {
//            sprintf(lasterrormessage, "Unable to connect to Astrodev: %d", iretn);
//            lasterrorcode = iretn;
//            return iretn;
//        }

//        if (!initialized)
//        {
//            initialized = true;
//            iretn = ic9100_get_frequency(ic9100);
//            if (iretn >= 0)
//            {
//                initial.freq = ic9100.frequency;
//            }
//            iretn = ic9100_get_mode(ic9100);
//            if (iretn >= 0)
//            {
//                initial.opmode = ic9100.opmode;
//            }
//            iretn = ic9100_get_bandpass(ic9100);
//            if (iretn >= 0)
//            {
//                initial.band = ic9100.bandpass;
//            }
//            actual = initial;
//            target = actual;
//        }
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

        if (!initialized)
        {
            initialized = true;
            iretn = ic9100_get_frequency(ic9100);
            if (iretn >= 0)
            {
                initial.freq = ic9100.frequency;
            }
            iretn = ic9100_get_mode(ic9100);
            if (iretn >= 0)
            {
                initial.opmode = ic9100.opmode;
            }
            iretn = ic9100_get_bandpass(ic9100);
            if (iretn >= 0)
            {
                initial.band = ic9100.bandpass;
            }
            actual = initial;
            target = actual;
        }
        break;
    case DEVICE_MODEL_TS2000:
        break;
    default:
        sprintf(lasterrormessage, "Unknow radio model: %d", agent->cinfo->device[deviceindex].all.model);
        lasterrorcode = GENERAL_ERROR_UNDEFINED;
        return GENERAL_ERROR_UNDEFINED;
        break;
    }

    radioconnected = true;
    return 0;
}

int32_t disconnect_radio()
{
    int32_t iretn = 0;

    if (initialized)
    {
        switch (agent->cinfo->device[deviceindex].all.model)
        {
        case DEVICE_MODEL_LOOPBACK:
            {
                actual = initial;
            }
            break;
        case DEVICE_MODEL_ASTRODEV:
            break;
        case DEVICE_MODEL_IC9100:
            iretn = ic9100_set_frequency(ic9100, initial.freq);
            if (iretn < 0)
            {
                sprintf(lasterrormessage, "Unable to set IC9100 frequency to %f: %d", target.freq, iretn);
                lasterrorcode = iretn;
            }
            iretn = ic9100_set_mode(ic9100, initial.opmode);
            if (iretn < 0)
            {
                sprintf(lasterrormessage, "Unable to set IC9100 to %s: %d", opmode2string(target.opmode).c_str(), iretn);
                lasterrorcode = iretn;
            }
            iretn = ic9100_set_bandpass(ic9100, initial.band);
            if (iretn < 0)
            {
                sprintf(lasterrormessage, "Unable to set IC9100 bandpass to %f: %d", target.band, iretn);
                lasterrorcode = iretn;
            }
            break;
        case DEVICE_MODEL_TS2000:
            break;
        default:
            sprintf(lasterrormessage, "Unknow radio model: %d", agent->cinfo->device[deviceindex].all.model);
            lasterrorcode = GENERAL_ERROR_UNDEFINED;
            iretn = GENERAL_ERROR_UNDEFINED;
            break;
        }
    }

    return iretn;
}

std::string opmode2string(uint8_t opmode)
{
    std::string result;
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
