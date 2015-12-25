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

//#include <stdlib.h>
//#include <stdio.h>
//#include <sys/types.h>
//#include <cstring>

#include "configCosmos.h"
#include "agentlib.h"
#include "jsonlib.h"
#include "stringlib.h"
#include "timelib.h"
#include "physicslib.h"
#include "jsonlib.h"
#include "datalib.h"
#include "zlib/zlib.h"
#include "devicecpu.h"
#include "devicedisk.h"
#include <sys/stat.h>
//#include "beacon.h"

#include <iostream>

int32_t request_reopen(char* request, char* output, void *cdata);
int32_t request_set_logperiod(char* request, char* output, void *cdata);
int32_t request_set_logstring(char* request, char* output, void *cdata);
int32_t request_get_logstring(char* request, char* output, void *cdata);
int32_t request_set_logstride(char* request, char* output, void *cdata);

std::string jjstring;
std::string myjstring;

NetworkType ntype = NetworkType::UDP;
int waitsec = 5;

void collect_data_loop();
std::thread cdthread;

int myagent();

std::string logstring;
std::vector<jsonentry*> logtable;
double logdate=0.;

std::vector<shorteventstruc> eventdict;
std::vector<shorteventstruc> events;

// variables to map to JSON
int32_t newlogperiod = 10, logperiod = 0;
double newlogstride = 900. / 86400.;
double logstride = 0.;
int pid;
int state = 0;
double cmjd;
//timestruc systime;
cosmosstruc *cdata;

beatstruc iscbeat;
std::string node = "hiakasat";
char response[300];

int main(int argc, char *argv[])
{
    int32_t iretn;

    if (argc != 2)
    {
        printf("Usage: agent_soh nodename\n");
        exit (1);
    }

    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_server(ntype,argv[1],(char *)"soh",1.,0,AGENTMAXBUFFER)))
    {
        std::cout<<"agent_soh: agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<std::endl;
        exit (AGENT_ERROR_JSON_CREATE);
    }

    cdata[0].node.utc = 0.;
    json_clone (cdata) ;
    cdata[0].agent[0].aprd = .5;


    if ((iretn=agent_add_request(cdata, (char *)"reopen",request_reopen)))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"set_logperiod",request_set_logperiod)))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"set_logstring",request_set_logstring)))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"get_logstring",request_get_logstring)))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"set_logstride",request_set_logstride)))
        exit (iretn);

    // Create default logstring
    logstring = json_list_of_soh(cdata);
    printf("logstring: %s\n", logstring.c_str());
    json_table_of_list(logtable, logstring.c_str(), cdata);
    //	agent_set_sohstring(cdata, logstring.c_str());

    load_dictionary(eventdict, cdata, (char *)"events.dict");

    // Start SOH collection thread
    cdthread = std::thread(collect_data_loop);
    //	printf("Started data collection thread\n");



    iretn = myagent();

}

int myagent()
{
    int sleept;
    double lmjd, dmjd, dtemp;
    double nextmjd;

    //Beacon beacon;
    //std::string beaconMessage;

    //double mjd_start = currentmjd();

#if !defined(COSMOS_WIN_OS)
    FILE *fp;
    struct statfs fsbuf;
#endif

    state = 1;
    //	fmjd = 0.;

    printf("Starting main thread\n");
    nextmjd = currentmjd();
    lmjd = currentmjd();
    DeviceCpu cpu;
    DeviceDisk disk;

    do
    {
        nextmjd += cdata[0].agent[0].aprd/86400.;
        dmjd = (cmjd-lmjd)*86400.;

        if (newlogperiod != logperiod )
        {
            logperiod = newlogperiod;
            logdate = cdata[0].node.utc;
            log_move(cdata[0].node.name, "soh");
        }

        cmjd = currentmjd();
        if (cdata[0].node.utc != 0.)
        {
            if (cdata[0].devspec.cpu_cnt > 0)
            {
                // get load average
                cdata[0].devspec.cpu[0]->load = cpu.getLoad1minAverage();
                // get memory
                cdata[0].devspec.cpu[0]->mem = cpu.getVirtualMemory();
            }

            if (cdata[0].devspec.disk_cnt > 0)
            {
                for (size_t i=0; i<cdata[0].devspec.disk_cnt; ++i)
                {
                    // get disk usage information
                    disk.getAll(cdata->port[cdata->devspec.disk[i]->gen.portidx].name);
                    cdata[0].devspec.disk[i]->disk = disk.Used;
                    cdata[0].devspec.disk[i]->maxdisk = disk.Size;
                }
            }

            // get number of cpu reboots
            FILE *fp = fopen("/flight_software/cosmosroot/nodes/hiakasat/boot.count","r");
            if (fp != nullptr)
            {
                fscanf(fp,"%u",&cdata[0].devspec.cpu[0]->boot_count);
                fclose(fp);
            }

            if (newlogstride != logstride )
            {
                logstride = newlogstride;
                logdate = currentmjd(0.);
                log_move(cdata[0].node.name, "soh");
            }

            if ((dtemp=floor(cmjd/logstride)*logstride) > logdate)
            {
                logdate = dtemp;
                log_move(cdata[0].node.name, "soh");
            }

            loc_update(&cdata[0].node.loc);
            update_target(cdata);
            agent_post(cdata, AGENT_MESSAGE_SOH, json_of_table(myjstring, logtable, cdata));
            calc_events(eventdict, cdata, events);
            for (uint32_t k=0; k<events.size(); ++k)
            {
                memcpy(&cdata[0].event[0].s,&events[k],sizeof(shorteventstruc));
                strcpy(cdata[0].event[0].l.condition,cdata[0].emap[events[k].handle.hash][events[k].handle.index].text);
                log_write(cdata[0].node.name,DATA_LOG_TYPE_EVENT,logdate, json_of_event(jjstring,cdata));
            }
        }
        if (dmjd-logperiod > -logperiod/20.)
        {
            lmjd = cmjd;
            if (cdata[0].node.utc != 0. && logstring.size())
            {
                log_write(cdata[0].node.name,DATA_LOG_TYPE_SOH, logdate, json_of_table(jjstring, logtable, cdata));
            }
        }

        //        // send beacon every 10 second
        //        if ((currentmjd() - mjd_start)*86400 > 10){
        //            beacon.createBeacon(cdata);
        //            std::cout << beacon.message << "*" << beacon.message.size() << std::endl;


        //            // Shutdown radio link, if on
        //            //if (radio_state)
        //            //{
        //                if (iscbeat.utc == 0)
        //                {
        //                    iscbeat = agent_find_server(cdata, node, "isc", 5.);
        //                }
        //                if (iscbeat.utc != 0)
        //                {
        //                    std::string requestString = "beacon_data_update ";
        //                    requestString += beacon.message;
        //                    agent_send_request(iscbeat, requestString.c_str(), response, 300, 2.);
        //                }

        //                // Take down tunnel interface
        //             //   if_takedown("sband");
        //            //}


        //            mjd_start = currentmjd();
        //        }

        sleept = (int)((nextmjd-currentmjd())*86400000000.);
        if (sleept < 0) sleept = 0;
        COSMOS_USLEEP(sleept);
    } while (agent_running(cdata));

    //	sprintf(tname,"%s/outgoing/%s",get_nodedir(cdata[0].node.name),data_name(cdata[0].node.name,fmjd,(char *)"telemetry"));
    //	rename(data_base_path(cdata[0].node.name,fmjd,(char *)"telemetry"),tname);
    cdthread.join();
    agent_shutdown_server(cdata);

    return 0;
}

int32_t request_reopen(char* request, char* output, void *cdata)
{
    logdate = ((cosmosstruc *)cdata)->node.loc.utc;
    log_move(((cosmosstruc *)cdata)->node.name, "soh");
    return 0;
}

int32_t request_set_logperiod(char* request, char* output, void *cdata)
{
    sscanf(request,"set_logperiod %d",&newlogperiod);
    return 0;
}

int32_t request_set_logstring(char* request, char* output, void *cdata)
{
    logstring = &request[strlen("set_logstring")+1];
    json_table_of_list(logtable, logstring.c_str(), (cosmosstruc*)cdata);
    return 0;
}

int32_t request_get_logstring(char* request, char* output, void *cdata)
{
    strcpy(output, logstring.c_str());
    return 0;
}

int32_t request_set_logstride(char* request, char* output, void *cdata)
{
    sscanf(request,"set_logstride %lf",&newlogstride);
    return 0;
}

void collect_data_loop()
{
    int nbytes;
    std::string message;
    pollstruc meta;

    while (agent_running(cdata))
    {
        // Collect new data
        if((nbytes=agent_poll(cdata, meta, message, AGENT_MESSAGE_BEAT,0)))
        {
            std::string tstring;
            if ((tstring=json_convert_string(json_extract_namedobject(message.c_str(), "agent_node"))) != cdata[0].node.name)
            {
                continue;
            }
            cdata[1].node = cdata[0].node;
            cdata[1].device = cdata[0].device;
            json_parse(message,&cdata[1]);
            cdata[0].node  = cdata[1].node ;
            cdata[0].device  = cdata[1].device ;
            loc_update(&cdata[0].node.loc);
            if (cdata[0].node.loc.utc > cdata[0].node.utc)
            {
                cdata[0].node.utc = cdata[0].node.loc.utc;
            }
            for (devicestruc device: cdata[0].device)
            {
                if (device.all.gen.utc > cdata[0].node.utc)
                {
                    cdata[0].node.utc = device.all.gen.utc;
                }
            }
            cdata[0].node.utc = currentmjd();
            //			update_target(cdata);
        }
    }
    return;
}

