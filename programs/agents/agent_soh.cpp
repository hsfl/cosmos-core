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

int32_t request_reopen(char* request, char* output, void *cinfo);
int32_t request_set_logperiod(char* request, char* output, void *cinfo);
int32_t request_set_logstring(char* request, char* output, void *cinfo);
int32_t request_get_logstring(char* request, char* output, void *cinfo);
int32_t request_set_logstride(char* request, char* output, void *cinfo);

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
cosmosstruc *cinfo;

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
    if (!(cinfo = agent_setup_server(ntype,argv[1],(char *)"soh",1.,0,AGENTMAXBUFFER)))
    {
        std::cout<<"agent_soh: agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<std::endl;
        exit (AGENT_ERROR_JSON_CREATE);
    }

    cinfo->pdata.node.utc = 0.;
    json_clone (cinfo) ;
    cinfo->pdata.agent[0].aprd = .5;


    if ((iretn=agent_add_request(cinfo, (char *)"reopen",request_reopen)))
        exit (iretn);
    if ((iretn=agent_add_request(cinfo, (char *)"set_logperiod",request_set_logperiod)))
        exit (iretn);
    if ((iretn=agent_add_request(cinfo, (char *)"set_logstring",request_set_logstring)))
        exit (iretn);
    if ((iretn=agent_add_request(cinfo, (char *)"get_logstring",request_get_logstring)))
        exit (iretn);
    if ((iretn=agent_add_request(cinfo, (char *)"set_logstride",request_set_logstride)))
        exit (iretn);

    // Create default logstring
    logstring = json_list_of_soh(cinfo->pdata);
    printf("logstring: %s\n", logstring.c_str());
    json_table_of_list(logtable, logstring.c_str(), cinfo->meta);
    //	agent_set_sohstring(cinfo, logstring.c_str());

    load_dictionary(eventdict, cinfo->meta, cinfo->pdata, (char *)"events.dict");

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

//#if !defined(COSMOS_WIN_OS)
//    FILE *fp;
//    struct statfs fsbuf;
//#endif

    state = 1;
    //	fmjd = 0.;

    printf("Starting main thread\n");
    nextmjd = currentmjd();
    lmjd = currentmjd();
    DeviceCpu cpu;
    DeviceDisk disk;

    do
    {
        nextmjd += cinfo->pdata.agent[0].aprd/86400.;
        dmjd = (cmjd-lmjd)*86400.;

        if (newlogperiod != logperiod )
        {
            logperiod = newlogperiod;
            logdate = cinfo->pdata.node.utc;
            log_move(cinfo->pdata.node.name, "soh");
        }

        cmjd = currentmjd();
        if (cinfo->pdata.node.utc != 0.)
        {
            if (cinfo->pdata.devspec.cpu_cnt > 0)
            {
                // get load average
                cinfo->pdata.devspec.cpu[0]->load = cpu.getLoad();
                // get memory
                cinfo->pdata.devspec.cpu[0]->gib = cpu.getVirtualMemoryTotal();
            }

            if (cinfo->pdata.devspec.disk_cnt > 0)
            {
                for (size_t i=0; i<cinfo->pdata.devspec.disk_cnt; ++i)
                {
                    // get disk usage information
                    disk.getAll(cinfo->pdata.port[cinfo->pdata.devspec.disk[i]->gen.portidx].name);
                    cinfo->pdata.devspec.disk[i]->gib = disk.Used;
                    cinfo->pdata.devspec.disk[i]->maxgib = disk.Size;
                }
            }

            // get number of cpu reboots
            FILE *fp = fopen("/flight_software/cosmosroot/nodes/hiakasat/boot.count","r");
            if (fp != nullptr)
            {
                fscanf(fp,"%u",&cinfo->pdata.devspec.cpu[0]->boot_count);
                fclose(fp);
            }

            if (newlogstride != logstride )
            {
                logstride = newlogstride;
                logdate = currentmjd(0.);
                log_move(cinfo->pdata.node.name, "soh");
            }

            if ((dtemp=floor(cmjd/logstride)*logstride) > logdate)
            {
                logdate = dtemp;
                log_move(cinfo->pdata.node.name, "soh");
            }

            loc_update(&cinfo->pdata.node.loc);
            update_target(cinfo->pdata);
            agent_post(cinfo, AGENT_MESSAGE_SOH, json_of_table(myjstring, logtable, cinfo->meta, cinfo->pdata));
            calc_events(eventdict, cinfo->meta, cinfo->pdata, events);
            for (uint32_t k=0; k<events.size(); ++k)
            {
                memcpy(&cinfo->pdata.event[0].s,&events[k],sizeof(shorteventstruc));
                strcpy(cinfo->pdata.event[0].l.condition,cinfo->meta.emap[events[k].handle.hash][events[k].handle.index].text);
                log_write(cinfo->pdata.node.name,DATA_LOG_TYPE_EVENT,logdate, json_of_event(jjstring, cinfo->meta, cinfo->pdata));
            }
        }
        if (dmjd-logperiod > -logperiod/20.)
        {
            lmjd = cmjd;
            if (cinfo->pdata.node.utc != 0. && logstring.size())
            {
                log_write(cinfo->pdata.node.name,DATA_LOG_TYPE_SOH, logdate, json_of_table(jjstring, logtable, cinfo->meta, cinfo->pdata));
            }
        }

        //        // send beacon every 10 second
        //        if ((currentmjd() - mjd_start)*86400 > 10){
        //            beacon.createBeacon(cinfo);
        //            std::cout << beacon.message << "*" << beacon.message.size() << std::endl;


        //            // Shutdown radio link, if on
        //            //if (radio_state)
        //            //{
        //                if (iscbeat.utc == 0)
        //                {
        //                    iscbeat = agent_find_server(cinfo, node, "isc", 5.);
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
    } while (agent_running(cinfo));

    //	sprintf(tname,"%s/outgoing/%s",get_nodedir(cinfo->pdata.node.name),data_name(cinfo->pdata.node.name,fmjd,(char *)"telemetry"));
    //	rename(data_base_path(cinfo->pdata.node.name,fmjd,(char *)"telemetry"),tname);
    cdthread.join();
    agent_shutdown_server(cinfo);

    return 0;
}

int32_t request_reopen(char* request, char* output, void *cinfo)
{
    logdate = ((cosmosstruc *)cinfo)->pdata.node.loc.utc;
    log_move(((cosmosstruc *)cinfo)->pdata.node.name, "soh");
    return 0;
}

int32_t request_set_logperiod(char* request, char* output, void *cinfo)
{
    sscanf(request,"set_logperiod %d",&newlogperiod);
    return 0;
}

int32_t request_set_logstring(char* request, char* output, void *cinfo)
{
    logstring = &request[strlen("set_logstring")+1];
    json_table_of_list(logtable, logstring.c_str(), ((cosmosstruc*)cinfo)->meta);
    return 0;
}

int32_t request_get_logstring(char* request, char* output, void *cinfo)
{
    strcpy(output, logstring.c_str());
    return 0;
}

int32_t request_set_logstride(char* request, char* output, void *cinfo)
{
    sscanf(request,"set_logstride %lf",&newlogstride);
    return 0;
}

void collect_data_loop()
{
    int nbytes;
    std::string message;
    pollstruc meta;

    while (agent_running(cinfo))
    {
        // Collect new data
        if((nbytes=agent_poll(cinfo, meta, message, AGENT_MESSAGE_BEAT,0)))
        {
            std::string tstring;
            if ((tstring=json_convert_string(json_extract_namedobject(message.c_str(), "agent_node"))) != cinfo->pdata.node.name)
            {
                continue;
            }
            cinfo->sdata.node = cinfo->pdata.node;
            cinfo->sdata.device = cinfo->pdata.device;
            json_parse(message, cinfo->meta, cinfo->sdata);
            cinfo->pdata.node  = cinfo->sdata.node ;
            cinfo->pdata.device  = cinfo->sdata.device ;
            loc_update(&cinfo->pdata.node.loc);
            if (cinfo->pdata.node.loc.utc > cinfo->pdata.node.utc)
            {
                cinfo->pdata.node.utc = cinfo->pdata.node.loc.utc;
            }
            for (devicestruc device: cinfo->pdata.device)
            {
                if (device.all.gen.utc > cinfo->pdata.node.utc)
                {
                    cinfo->pdata.node.utc = device.all.gen.utc;
                }
            }
            cinfo->pdata.node.utc = currentmjd();
            //			update_target(cinfo->pdata);
        }
    }
    return;
}

