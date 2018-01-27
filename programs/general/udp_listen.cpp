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

/*
 ****************************************************************************
 *** Title: Network UDP/IP Listener
 ***
 *** Purpose: for NPS Ballon sat detection
 ***
 *** Usage: -
 ****************************************************************************
 *** Version 1.0
 *** Target OS: Linux, Mac OS
 *** Update Log:
 *** -
 *** Created: 2013.08.06 Miguel Nunes
 ****************************************************************************
 ***
 *** Bugs:
 *** 1.
 ****************************************************************************
*/

#include "support/configCosmos.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "support/convertlib.h"


#define BUFSIZE 10000

bool debug_verbose = false;
char address[] = "0.0.0.0";
uint16_t port = 6868;
uint16_t bsize = 10000;

Agent *agent;



int main(int argc, char *argv[])
{
    // **** Declare Variables

    std::string input;
    std::string token;

	/*
    char gps_time[50];
    char gps_lat[50];
    char gps_lon[50];
    char gps_alt[50];
    char gps_fix[50];
    char pressure[50];
    char temp_ext[50];
    char temp_batt[50];
    char temp_arduino[50];
    char acc_x[50];
    char acc_y[50];
    char acc_z[50];
	*/
	double mjd;

	char logstring[AGENTMAXBUFFER-20];

    // Variables: Packet Reception: Data handling
    socket_channel chan; // Agent UDP Channel Sructure
    unsigned int fromlen; // UDP receive from "fromlen" parameter
    int received; // Num bytes received per UDP packet
    uint8_t buf1[BUFSIZE]; // RX Buffer

    std::string myjstring;
    std::string jjstring;


	uint32_t i, j, tlen;

    // Variables: Scratchpad
    int32_t iretn; // Scratch Return value

    // **** Initialize Variables



    // Begin Program, Initialize Socket, Blocking, Timeout set for 1 second
    if ((iretn=socket_open(&chan, NetworkType::UDP, address, port, SOCKET_LISTEN, SOCKET_BLOCKING, 1000000)) < 0)
    {
        printf("Err: Unable to open connection to [%s:%d]\n",address,port);
    } else {
        printf("Netperf Listen is now listening on port %d...\n\n",port);
    }

    agent = new Agent("nps", "udp");

	// Create default logstring
    strcpy(logstring,json_of_soh(jjstring, agent->cinfo));
	j = 0;
	tlen = strlen(logstring);
	for (i=1; i<tlen-j; ++i)
	{
		if (logstring[i+j] == ':')
		{
			logstring[i] = ',';
			do
			{
				++j;
			} while (i+j < tlen && logstring[i+j] != '{');
		}
		else
			logstring[i] = logstring[i+j];
	}
	logstring[i-1] = '}';
	logstring[i] = 0;

    // Begin Main Forever Loop
    while (1)
    {
        // UDP Receive: Check for new packet, return -1 if none
		received = recvfrom( chan.cudp, (char *)buf1, BUFSIZE, 0, (struct sockaddr*) &chan.caddr, (socklen_t*) &fromlen);

        if (received < 1)
        {
            // do nothing
            printf("\n");
        }
        else
        {

            printf(">> %s\n",buf1);
            // Parse Packet
            // Fields:
            // 1 - gpsTime (hh:mm:ss.cc),
            // 2 - gpsLat (deg x 10-5),
            // 3 - gpsLong (deg x 10-5),
            // 4 - gpsAlt (m),
            // 5 - gpsFix (1,0,-1),
            // 6 - Pressure (kPa),
            // 7 - externalTemp (K),
            // 8 - battTemp (K),
            // 9 - arduinoTemp (degC),
            // 10- acc_x,
            // 11- acc_y,
            // 12- acc_z (g)

            //std::string string_input;
            //string_input = buf1;
            //char string_input[100];
            //strcpy(input,string_input);
            //istringstream ss(string_input);

			int32_t hour, min, lat, lon, alt, press, etemp, btemp, atemp, fix, sec, hsec;
			float accx, accy, accz;

			sscanf((char *)buf1,"%2d%2d%2d%2d %d %d %d %d %d %d %d %d %f %f %f",
				&hour, &min, &sec, &hsec,
				&lat, &lon, &alt, &fix,
				&press, &etemp, &btemp, &atemp,
				&accx, &accy, &accz);
				/*
            sscanf((char *)buf1,"%s %s %s %s %s %s %s %s %s %s %s %s",
                   &gps_time,
                   &gps_lat,
                   &gps_lon,
                   &gps_alt,
                   &gps_fix,
                   &pressure,
                   &temp_ext,
                   &temp_batt,
                   &temp_arduino,
                   &acc_x,
                   &acc_y,
                   &acc_z
                   );


            printf("GPS Time: %s\n", gps_time);
            printf("GPS Lat: %s\n", gps_lat);
            printf("GPS Lon: %s\n", gps_lon);
            printf("GPS Alt: %s\n", gps_alt);
            printf("GPS Fix: %s\n", gps_fix);
            printf("Pressure: %s\n", pressure);
            printf("Temp Ext: %s\n", temp_ext);
            printf("Temp Batt: %s\n", temp_batt);
            printf("Temp Ard.: %s\n", temp_arduino);
            printf("Acc x: %s\n", acc_x);
            printf("Acc y: %s\n", acc_y);
            printf("Acc z: %s\n", acc_z);
				   */

			// IMU
            agent->cinfo->devspec.imu[0]->accel.col[0] = accx;
            agent->cinfo->node.loc.pos.geod.a.lon = (accx/REARTHM)/cos(agent->cinfo->node.loc.pos.geod.s.lat);
            agent->cinfo->devspec.imu[0]->accel.col[1] = accy;
            agent->cinfo->node.loc.pos.geod.a.lat = (accy/REARTHM);
            agent->cinfo->devspec.imu[0]->accel.col[2] = accz;
            agent->cinfo->node.loc.pos.geod.a.h = accz;

			// Pressure sensor
            agent->cinfo->devspec.psen[0]->press = press / 1000.;

			// Temperatures
            agent->cinfo->devspec.tsen[0]->temp = etemp;
            agent->cinfo->devspec.tsen[1]->temp = btemp;
            agent->cinfo->devspec.tsen[2]->temp = atemp + 273.15;

			// GPS
			if (fix == 1)
			{
			mjd = (int)currentmjd(0.) + hour / 24. + min / 1400. + sec / 86400. + hsec / 8640000.;;
            agent->cinfo->devspec.gps[0]->utc = agent->cinfo->node.loc.utc = agent->cinfo->node.loc.pos.geod.utc = mjd;
            agent->cinfo->devspec.gps[0]->geocs = agent->cinfo->node.loc.pos.geoc.s;
            agent->cinfo->node.loc.pos.geod.s.lat = RADOF(lat / 1.e5);
            agent->cinfo->node.loc.pos.geod.s.lon = RADOF(lon / 1.e5);
            agent->cinfo->node.loc.pos.geod.s.h = alt;
            agent->cinfo->devspec.gps[0]->geocv = rv_zero();

            agent->cinfo->node.loc.pos.geod.v.lat = 0.;
            agent->cinfo->node.loc.pos.geod.v.lon = 0.;
            agent->cinfo->node.loc.pos.geod.v.h = 0.;
            agent->cinfo->node.loc.att.geoc.s = q_eye();
            agent->cinfo->node.loc.att.geoc.v = rv_zero();
            agent->cinfo->node.loc.att.geoc.a = rv_zero();

            ++agent->cinfo->node.loc.pos.geod.pass;
            pos_geod(&agent->cinfo->node.loc);
			}

			// Broadcast it
            agent->post(Agent::AgentMessage::SOH, json_of_list(myjstring, logstring, agent->cinfo));
            log_write(agent->cinfo->node.name,DATA_LOG_TYPE_SOH,floor(agent->cinfo->node.loc.utc), json_of_list(jjstring,logstring, agent->cinfo));

        } // End If: packet reception / parse / idle cycle

    } // End Main Forever Loop

} // End Main
