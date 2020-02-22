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

#include "support/configCosmos.h"

#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "support/convertlib.h"
#include "support/datalib.h"
#include "support/command_queue.h"

#include <iostream>
#include <iomanip>
#include <list>
#include <fstream>
#include <sstream>
#include <mutex>

using std::string;
using std::vector;
using std::cout;
using std::endl;

/*! \file agent_exec.cpp
* \brief Executive Agent source file
*/

//! \ingroup agents
//! \defgroup agent_exec Executive Agent program
//! This Agent manages the execution of commands within the COSMOS system.
//! A single command queue is kept containing both time, and time and condition driven
//! commands. Commands can be added or removed from this queue, either through direct requests
//! or through command files.
//!
//! Commands are represented as a ::eventstruc. If EVENT_FLAG_CONDITIONAL is set, the condition
//! part of the ::eventstruc is evaluated as a COSMOS equation to determine whether the command
//! should be executed. Either way, commands are only executed if their time has passed. Once a
//! command has executed, it is either remove from the queue, or if EVENT_FLAG_REPEAT is set,
//! it is disabled from executing until such time as the condition goes false again, after which
//! it can once again execute.
//!
//! Any execution of a command is reflected in two log files, one of which tracks the results of the
//! command, and the other of which logs the actual ::eventstruc for the command, with the utcexec field
//! set to the actual time of execution.
//!
//! Usage: agent_exec node_name

static Agent *agent;
static string node_name;

static CommandQueue cmd_queue;

void move_and_compress_exec ();
static double logdate_exec=0.;
static double newlogstride_exec = 900. / 86400.;
static double logstride_exec = 0.;
static std::mutex exec_mutex;

int32_t request_get_queue_size(char* request, char* response, Agent* agent);
int32_t request_get_event(char* request, char* response, Agent* agent);
int32_t request_del_event(char* request, char* response, Agent* agent);
int32_t request_del_event_id(char* request, char* response, Agent* agent);
int32_t request_add_event(char* request, char* response, Agent* agent);
int32_t request_remote_command(char *request, char* response, Agent* agent);
int32_t request_soh(char *request, char* response, Agent* agent);
int32_t request_reopen_exec(char* request, char* response, Agent* agent);
int32_t request_set_logstride_exec(char* request, char* response, Agent* agent);
int32_t request_get_logstride_exec(char* request, char* response, Agent* agent);

// SOH specific declarations
int32_t request_reopen_soh(char* request, char* response, Agent *agent);
int32_t request_set_logperiod(char* request, char* response, Agent *agent);
int32_t request_get_logperiod(char* request, char* response, Agent *agent);
int32_t request_set_logstride_soh(char* request, char* response, Agent *agent);
int32_t request_get_logstride_soh(char* request, char* response, Agent *agent);
int32_t request_set_logstring(char* request, char* response, Agent *agent);
int32_t request_get_logstring(char* request, char* response, Agent *agent);

static string jjstring;
static string myjstring;

void collect_data_loop();
static thread cdthread;

void move_and_compress_soh();
static string logstring;
static vector<jsonentry*> logtable;
static double logdate_soh=0.;
static int32_t newlogperiod = 10, logperiod = 0;
static double newlogstride_soh = 900. / 86400.;
static double logstride_soh = 0.;
static std::mutex soh_mutex;

static std::mutex beacon_mutex;
void move_and_compress_beacon();
void get_beacon_cpu();

static vector<shorteventstruc> eventdict;
static vector<shorteventstruc> events;

static beatstruc iscbeat;

// default node name
//static string node = "neutron1";

int main(int argc, char *argv[])
{
    vector<shorteventstruc> events, eventdict;
    std::string incoming_dir, outgoing_dir, temp_dir, immediate_dir;
    std::string jjstring, myjstring;
    double lmjd, dmjd, cmjd, nextmjd;
    int32_t iretn;
    int sleept;

    // Set node name to first argument
    if (argc!=2)
    {
        cout<<"Usage: agent_exec node"<<endl;
        exit(1);
    }
    node_name = argv[1];
    cout<<"Starting the executive/soh agent->..";

    // Establish the command channel and heartbeat
    agent = new Agent(node_name, "exec", 5.);
    if ((iretn = agent->wait()) < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to start Agent %s on Node %s : %s\n", agent->getAgent().c_str(), agent->getNode().c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }

    agent->cinfo->node.utc = 0.;
    agent->cinfo->agent[0].aprd = .5;
    cout<<"  started."<<endl;

    // Establish Executive functions

    // Set the immediate, incoming, outgoing, and temp directories
    immediate_dir = data_base_path(node_name, "immediate", "exec") + "/";
    if (immediate_dir.empty())
    {
        cout<<"unable to create directory: <"<<(node_name+"/immediate")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    incoming_dir = data_base_path(node_name, "incoming", "exec") + "/";
    if (incoming_dir.empty())
    {
        cout<<"unable to create directory: <"<<(node_name+"/incoming")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    outgoing_dir = data_base_path(node_name, "outgoing", "exec") + "/";
    if (outgoing_dir.empty())
    {
        cout<<"unable to create directory: <"<<(node_name+"/outgoing")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    temp_dir = data_base_path(node_name, "temp", "exec") + "/";
    if (temp_dir.empty())
    {
        cout<<"unable to create directory: <"<<(node_name+"/temp")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    // Add agent request functions
    if ((iretn=agent->add_request("get_queue_size", request_get_queue_size, "", "returns the current size of the command queue")))
        exit (iretn);
    if ((iretn=agent->add_request("del_event", request_del_event, "entry-string", "deletes the specified command event from the queue according to its JSON string")))
        exit (iretn);
    if ((iretn=agent->add_request("del_event_id", request_del_event_id, "entry #", "deletes the specified command event from the queue according to its position")))
        exit (iretn);
    if ((iretn=agent->add_request("get_event", request_get_event, "[ entry # ]", "returns the requested command queue entry (or all if none specified)")))
        exit (iretn);
    if ((iretn=agent->add_request("add_event", request_add_event, "{\"event_name\":\"\"}{\"event_utc\":0}{\"event_utcexec\":0}{\"event_flag\":0}{\"event_type\":0}{\"event_data\":\"\"}{\"event_condition\":\"\"}", "adds the specified command event to the queue")))
        exit (iretn);
    if ((iretn=agent->add_request("run_remote_command", request_remote_command, "command-to-run", "run a command local to the agent and return its output")))
        exit (iretn);
    if ((iretn=agent->add_request("reopen_exec", request_reopen_exec, "", "flushes exec log files out of temp directory")))
        exit (iretn);
    if ((iretn=agent->add_request("set_logstride_exec", request_set_logstride_exec, "logstride", "modify how frequently we flush exec log files out of temp directory")))
        exit (iretn);
    if ((iretn=agent->add_request("get_logstride_exec", request_get_logstride_exec, "", "return how frequently we flush exec log files out of temp directory")))
        exit (iretn);

    // Reload existing queue
    string infilepath = temp_dir + ".queue";
    std::ifstream infile(infilepath.c_str());
    if(!infile.is_open())
    {
        cout<<"unable to read file <"<<infilepath<<">"<<endl;
    }
    else
    {
        //file is open for reading commands
        string line;
        Event cmd;
        std::cout << "===\n";

        while(std::getline(infile,line))
        {
            //cmd.set_command(line, agent);
            cmd.set_command(line);

            if(cmd.is_command())
            {
                cmd_queue.add_command(cmd);
                printf("Loaded command: %s\n", line.c_str());
            }
            else
            {
                cout<<"Not a command!"<<endl;
            }
        }
        infile.close();
    }

    // Establish SOH functions

    if ((iretn=agent->add_request("reopen_soh", request_reopen_soh, "", "flushes soh log files out of temp directory")))
        exit (iretn);
    if ((iretn=agent->add_request("set_logperiod" ,request_set_logperiod, "logperiod", "set how often we log our SOH data")))
        exit (iretn);
    if ((iretn=agent->add_request("get_logperiod" ,request_get_logperiod, "", "return how often we log our SOH data")))
        exit (iretn);
    if ((iretn=agent->add_request("set_logstring", request_set_logstring, "logstring", "set what parts of our SOH that we log.")))
        exit (iretn);
    if ((iretn=agent->add_request("get_logstring", request_get_logstring, "", "return what portions of our SOH that we log")))
        exit (iretn);
    if ((iretn=agent->add_request("set_logstride_soh", request_set_logstride_soh, "logstride", "modify how frequently we flush our soh log files out of temp directory")))
        exit (iretn);
    if ((iretn=agent->add_request("get_logstride_soh", request_get_logstride_soh, "", "return how frequently we flush our soh log files out of temp directory")))
        exit (iretn);

    // Create default logstring
    logstring = json_list_of_soh(agent->cinfo);
    printf("===\nlogstring: %s\n===\n", logstring.c_str()); fflush(stdout);
    json_table_of_list(logtable, logstring.c_str(), agent->cinfo);

    //	agent_set_sohstring(agent->cinfo, logstring.c_str());

    load_dictionary(eventdict, agent->cinfo, "events.dict");

    // Start thread to collect SOH data
    thread cdthread = thread(collect_data_loop);

    get_beacon_cpu();
    // Start performing the body of the agent
    lmjd = cmjd = nextmjd = currentmjd();
    while(agent->running())
    {
        nextmjd += agent->cinfo->agent[0].aprd/86400.;
        dmjd = (cmjd-lmjd)*86400.;
        agent->cinfo->node.utc = cmjd = currentmjd();

        // Check if the SOH logperiod has changed
        if (newlogperiod != logperiod )
        {
            logperiod = newlogperiod;
            logdate_soh = agent->cinfo->node.utc;

            move_and_compress_soh();
        }

        // Check if either of the logstride have changed
        if (fabs(newlogstride_exec - logstride_exec) > std::numeric_limits<double>::epsilon()) {
            logstride_exec = newlogstride_exec;
            logdate_exec = currentmjd(0.);
            move_and_compress_exec();
        }

        if (fabs(newlogstride_soh - logstride_soh) > std::numeric_limits<double>::epsilon()) {
            logstride_soh = newlogstride_soh;
            logdate_soh = currentmjd(0.);


            move_and_compress_soh();
            move_and_compress_beacon();
        }

        // Check if either of the logstride have expired
        if (floor(cmjd/logstride_exec)*logstride_exec > logdate_exec)
        {
            logdate_exec = floor(cmjd/logstride_exec)*logstride_exec;
            move_and_compress_exec();
        }

        if (floor(cmjd/logstride_soh)*logstride_soh > logdate_soh)
        {
            logdate_soh = floor(cmjd/logstride_soh)*logstride_soh;
            move_and_compress_soh();
            move_and_compress_beacon();
        }

        // Perform SOH specific functions
        if (agent->cinfo->node.utc != 0.)
        {
            loc_update(&agent->cinfo->node.loc);
            update_target(agent->cinfo);
            agent->post(Agent::AgentMessage::SOH, json_of_table(myjstring, logtable, agent->cinfo));
            calc_events(eventdict, agent->cinfo, events);
            for (uint32_t k=0; k<events.size(); ++k)
            {
                memcpy(&agent->cinfo->event[0].s,&events[k],sizeof(shorteventstruc));
                strcpy(agent->cinfo->event[0].l.condition,agent->cinfo->emap[events[k].handle.hash][events[k].handle.index].text);
                log_write(agent->cinfo->node.name,DATA_LOG_TYPE_EVENT,logdate_soh, json_of_event(jjstring, agent->cinfo));
            }
        }

        // Check if SOH logperiod has expired
        if (dmjd-logperiod > -logperiod/20.)
        {
            lmjd = cmjd;
            if (agent->cinfo->node.utc != 0. && logstring.size())
            {
                log_write(agent->cinfo->node.name, DATA_LOG_TYPE_SOH, logdate_soh, json_of_table(jjstring, logtable, agent->cinfo));
                log_write(agent->cinfo->node.name, DATA_LOG_TYPE_SOH, logdate_soh, json_of_table(jjstring, logtable, agent->cinfo), static_cast <string>("immediate"));
            }
        }

        // Perform Executive specific functions
        cmd_queue.load_commands(immediate_dir);
        cmd_queue.load_commands(incoming_dir);
        cmd_queue.run_commands(agent, node_name, logdate_exec);
        cmd_queue.save_commands(temp_dir);

        sleept = static_cast<int>((nextmjd-currentmjd())*86400000000.);
        if (sleept < 0) sleept = 0;
        COSMOS_USLEEP(sleept);
    }

    agent->shutdown();
    cdthread.join();
}

// Executive specific requests
int32_t request_set_logstride_exec(char* request, char*, Agent *)
{
    sscanf(request,"set_logstride_exec %lf",&newlogstride_exec);
    return 0;
}
int32_t request_get_logstride_exec(char*, char* response, Agent *)
{
    sprintf(response, "%lf", logstride_exec);
    return 0;
}


int32_t request_reopen_exec(char*, char*, Agent *)
{
    // Do not wait for log stride, move data from temp to incoming immediately.
    logdate_exec = agent->cinfo->node.loc.utc;
    move_and_compress_exec();
    return 0;
}

int32_t request_get_queue_size(char *, char* response, Agent *)
{
    sprintf(response,"%" PRIu32 "", cmd_queue.get_size());
    return 0;
}

int32_t request_get_event(char *request, char* response, Agent *)
{
    std::ostringstream ss;

    if(cmd_queue.get_size()==0)	{
        ss << "the command queue is empty";
    }
    else {
        int j;
        int32_t iretn = sscanf(request,"get_event %d",&j);

        // if valid index then return command
        if (iretn == 1) {
            if(j >= 0 && j < static_cast<int>(cmd_queue.get_size()))
                ss << cmd_queue.get_command(j);
            else
                ss << "<" << j << "> is not a valid command queue index (current range between 0 and "
                   << cmd_queue.get_size()-1 << ")";
        }

        // if no index given, return the entire queue
        else if (iretn ==  -1) {
            for(int i = 0; i < static_cast<int>(cmd_queue.get_size()); ++i) {
                Event cmd = cmd_queue.get_command(i);
                ss << "[" << i << "]" << "[" << mjd2iso8601(cmd.getUtc()) << "]" << cmd << endl;
            }
        }
        // if the user supplied something that couldn't be turned into an integer
        else if (iretn == 0) { ss << "Usage:\tget_event [ index ]\t"; }
    }

    strcpy(response, ss.str().c_str());
    return 0;
}

// Delete Queue Entry - by #
int32_t request_del_event_id(char *request, char* response, Agent *)
{
    Event cmd;
    std::ostringstream ss;

    if(cmd_queue.get_size()==0)	{
        ss << "the command queue is empty";
    }
    else {
        int j;
        int32_t iretn = sscanf(request,"del_event_id %d",&j);

        // if valid index then return command
        if (iretn == 1) {
            if(j >= 0 && j < static_cast<int>(cmd_queue.get_size())) {
                sprintf(response,"%d commands deleted from the queue",
                    cmd_queue.del_command(j)
                );
            } else {
                ss << "<" << j << "> is not a valid command queue index (current range between 0 and " << cmd_queue.get_size()-1 << ")";
            }
        }
        // if the user supplied something that couldn't be turned into an integer
        else if (iretn == 0)	{
            ss << "Usage:\tdel_event_id [ index ]\t";
		}
    }

    strcpy(response, ss.str().c_str());
    return 0;
}

// Delete Queue Entry - by date and contents
int32_t request_del_event(char *request, char* response, Agent *)
{
    Event cmd;
    string line(request);

    // remove "del_event " from request string
    line.erase(0, 10);
    cmd.set_command(line);

    //delete command
    int n = cmd_queue.del_command(cmd);

    if(!cmd.is_command()) {
        sprintf(response, "Not a valid command: %s", line.c_str());
    }
    else {
        sprintf(response,"%d commands deleted from the queue",n);
    }

    return 0;
}

// Add Queue Entry
int32_t request_add_event(char *request, char* response, Agent *)
{
    Event cmd;
    string line(request);

    // remove "add_event " from request string
    line.erase(0, 10);
    cmd.set_command(line);

    // add command
    if(cmd.is_command()) {
        cmd_queue.add_command(cmd);
        sprintf(response, "Command added to queue: %s", line.c_str());
    }
    else {
        sprintf(response, "Not a valid command: %s", line.c_str());
    }

	// sort the queue
	cmd_queue.sort();
    return 0;
}

// Run the command and return the output in the response.
int32_t request_remote_command(char *request, char* response, Agent *)
{
    char request_re[AGENTMAXBUFFER + 5];
    int32_t iretn = 0;
    FILE *pd;
    int i;

    // Locate where our command starts.
    bool flag = false;
    for (i=0; i<AGENTMAXBUFFER-1; i++) {
        if (flag) {
            if (request[i] != ' ')
                break;
        }
        else if (request[i] == ' ') {
            flag = true;
        }
    }

    if (i == AGENTMAXBUFFER-1) {
        sprintf(response,"Unable to find an appropriate command.");
    }
    else {
        // Redirect error into buffer as well.
        strcpy(request_re, &request[i]);
        strcat(request_re, " 2>&1");

        // Run the process and create a pipe.
#ifdef COSMOS_WIN_BUILD_MSVC
        if ((pd=_popen(request_re, "r")) != NULL)
#else
        if ((pd=popen(request_re, "r")) != nullptr)
#endif
        {
            iretn = fread(response,1,AGENTMAXBUFFER-1,pd);
            response[iretn] = '\0';
            iretn = 0;
#ifdef COSMOS_WIN_BUILD_MSVC
            _pclose(pd);
#else
            pclose(pd);
#endif
        }
        else {
            response[0] = '\0';
            iretn = 0;
        }
    }

    return iretn;
}

// SOH specific requests
int32_t request_reopen_soh(char*, char*, Agent *agent)
{
    // Do not wait for logstride, push soh log files out of temp and into incoming.
    logdate_soh = agent->cinfo->node.loc.utc;
    move_and_compress_soh();
    return 0;
}

int32_t request_set_logperiod(char* request, char*, Agent *)
{
    sscanf(request,"set_logperiod %d",&newlogperiod);
    return 0;
}
int32_t request_get_logperiod(char*, char* response, Agent *)
{
    sprintf(response, "%d", logperiod);
    return 0;
}

int32_t request_set_logstring(char* request, char*, Agent *agent)
{
    logstring = &request[strlen("set_logstring")+1];
    logtable.clear();
    json_table_of_list(logtable, logstring.c_str(), agent->cinfo);
    return 0;
}

int32_t request_get_logstring(char*, char* response, Agent *)
{
    strcpy(response, logstring.c_str());
    return 0;
}

int32_t request_set_logstride_soh(char* request, char*, Agent *)
{
    sscanf(request,"set_logstride_soh %lf",&newlogstride_soh);
    return 0;
}
int32_t request_get_logstride_soh(char*, char*response, Agent *)
{
    sprintf(response, "%lf", logstride_soh);
    return 0;
}

void collect_data_loop()
{
    int my_position = -1;
    while (agent->running())
    {
        // Collect new data
        while (my_position != static_cast<int>(agent->message_head))
        {
            ++my_position;
            if (my_position >= static_cast<int>(agent->message_ring.size()))
            {
                my_position = 0;
            }
            if (agent->cinfo->node.name == agent->message_ring[my_position].meta.beat.node && agent->message_ring[my_position].meta.type < Agent::AgentMessage::BINARY)
            {
                json_parse(agent->message_ring[my_position].adata, agent->cinfo);
                agent->cinfo->node.utc = currentmjd(0.);

                for (devicestruc device: agent->cinfo->device)
                {
                    if (device.all.utc > agent->cinfo->node.utc)
                    {
                        agent->cinfo->node.utc = device.all.utc;
                    }
                }
            }
        }
        COSMOS_SLEEP(.1);
    }
    return;
}

// Moving exec and soh logs cannot occur concurrently.
void move_and_compress_exec () {
    exec_mutex.lock();
    cmd_queue.join_events();
    log_move(node_name, "exec");
    exec_mutex.unlock();
}
void move_and_compress_soh () {
    soh_mutex.lock();
    log_move(node_name, "soh");
    soh_mutex.unlock();
}

void move_and_compress_beacon () {
    std::string beacon_string;
    beacon_mutex.lock();
    log_move(node_name, "soh");
    log_write(agent->cinfo->node.name, DATA_LOG_TYPE_BEACON, logdate_soh, json_of_beacon(beacon_string, agent->cinfo));
    log_move(node_name, "beacon");
    beacon_mutex.unlock();
}

void get_beacon_cpu() {
    static DeviceCpu deviceCpu;
    static const double GiB = 1024. * 1024. * 1024.;
    int32_t iretn;
    iretn = json_createpiece(agent->cinfo, "main_cpu", DeviceType::CPU);
    if (iretn < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to add CPU %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(1);
    }

    uint16_t cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
    agent->cinfo->device[cidx].cpu.load = static_cast <float>(deviceCpu.getLoad());
    agent->cinfo->device[cidx].cpu.gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/GiB);
    agent->cinfo->device[cidx].cpu.maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/GiB);
    agent->cinfo->device[cidx].cpu.maxload = deviceCpu.getCount();
}
// Not being used... remove?
///// Prints the command information stored in local the copy of agent->cinfo->event[0].l
//void print_command()
//{
//    string jsp;
//
//    json_out(jsp,(char*)"event_utc", agent->cinfo);
//    json_out(jsp,(char*)"event_utcexec", agent->cinfo);
//    json_out(jsp,(char*)"event_name", agent->cinfo);
//    json_out(jsp,(char*)"event_type", agent->cinfo);
//    json_out(jsp,(char*)"event_flag", agent->cinfo);
//    json_out(jsp,(char*)"event_data", agent->cinfo);
//    json_out(jsp,(char*)"event_condition", agent->cinfo);
//    cout<<"<"<<jsp<<">"<<endl;
//
//    return;
//}
