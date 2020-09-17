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
//! Usage: agent_exec agent->getNode()

static Agent *agent;

static CommandQueue cmd_queue;

void move_and_compress_exec ();
static double logdate_exec=0.;
static double newlogstride_exec = 300. / 86400.;
static double logstride_exec = 0.;
static std::mutex exec_mutex;

int32_t get_last_offset();
int32_t get_flags();

int32_t request_get_queue_size(string &request, string &response, Agent* agent);
int32_t request_get_event(string &request, string &response, Agent* agent);
int32_t request_get_command(string &request, string &response, Agent* agent);
int32_t request_del_command(string &request, string &response, Agent* agent);
int32_t request_del_command_id(string &request, string &response, Agent* agent);
int32_t request_add_command(string &request, string &response, Agent* agent);
int32_t request_remote_command(string &request, string &response, Agent* agent);
int32_t request_reopen_exec(string &request, string &response, Agent* agent);
int32_t request_set_logstride_exec(string &request, string &response, Agent* agent);
int32_t request_get_logstride_exec(string &request, string &response, Agent* agent);

// SOH specific declarations
int32_t request_reopen_soh(string &request, string &response, Agent *agent);
int32_t request_set_logperiod(string &request, string &response, Agent *agent);
int32_t request_get_logperiod(string &request, string &response, Agent *agent);
int32_t request_set_logstride_soh(string &request, string &response, Agent *agent);
int32_t request_get_logstride_soh(string &request, string &response, Agent *agent);
int32_t request_set_logstring(string &request, string &response, Agent *agent);
int32_t request_get_logstring(string &request, string &response, Agent *agent);

static string jjstring;
static string myjstring;

void collect_data_loop() noexcept;
static thread cdthread;

void move_and_compress_soh();
static string logstring;
static vector<jsonentry*> logtable;
static double logdate_soh=0.;
static double newlogperiod = 30. / 86400.;
static double logperiod = 0;
static double newlogstride_soh = 600. / 86400.;
static double logstride_soh = 0.;
static std::mutex soh_mutex;

static std::mutex beacon_mutex;
void move_and_compress_beacon();
//void get_beacon_cpu();
int32_t get_power_mode();

static vector<shorteventstruc> eventdict;
static vector<shorteventstruc> events;

static beatstruc iscbeat;

struct boot_flags
{
    bool launched : 1;
    bool bootcheck : 1;
    bool deployed : 1;
} boot_flags;

double correcttime;
double epsilon;
double delta;

// default node name
//static string node = "neutron1";

int main(int argc, char *argv[])
{
    vector<shorteventstruc> events, eventdict;
    string incoming_dir, outgoing_dir, temp_dir, immediate_dir;
    string jjstring, myjstring;
    double llogmjd, dlogmjd, clogmjd;
    int32_t iretn;

    // Set node name to first argument
    if (argc == 2)
    {
        agent = new Agent(argv[1], "exec", 0.);
    }
    else
    {
        agent = new Agent("", "exec", 0.);
    }

    if ((iretn = agent->wait()) < 0)
    {
        fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        fprintf(agent->get_debug_fd(), "%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
    }

    // Fix time
    iretn = agent->get_agent_time(correcttime, epsilon, delta, "gps");
    if (iretn >= 0)
    {
        printf("Initialized time from time GPS: Delta %f %f Epsilon %f\n", 86400. * (correcttime - currentmjd()), 86400.*delta, 86400.*epsilon);
        set_local_clock(correcttime);
    }
    else
    {
        FILE *fp = fopen(("/cosmos/nodes/" + agent->nodeName + "/last_date").c_str(), "r");
        if (fp != nullptr)
        {
            calstruc date;
            fscanf(fp, "%02d%02d%02d%02d%04d%*c%02d\n", &date.month, &date.dom, &date.hour, &date.minute, &date.year, &date.second);
            fclose(fp);
            int32_t offset = get_last_offset();
            date.second += offset;
            double delta = cal2mjd(date) -  currentmjd();
            if (delta > 3.5e-4)
            {
                delta = set_local_clock(cal2mjd(date));
                printf("Initialized time from file: Delta %f\n", delta);
            }
        }
    }

    // Check for launched
    get_flags();

    agent->cinfo->node.utc = 0.;
    agent->cinfo->agent[0].aprd = 1.;
    cout<<"  started."<<endl;

    // Establish Executive functions

    // Set the immediate, incoming, outgoing, and temp directories
    immediate_dir = data_base_path(agent->getNode(), "immediate", "exec") + "/";
    if (immediate_dir.empty())
    {
        cout<<"unable to create directory: <"<<(agent->getNode()+"/immediate")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    incoming_dir = data_base_path(agent->getNode(), "incoming", "exec") + "/";
    if (incoming_dir.empty())
    {
        cout<<"unable to create directory: <"<<(agent->getNode()+"/incoming")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    outgoing_dir = data_base_path(agent->getNode(), "outgoing", "exec") + "/";
    if (outgoing_dir.empty())
    {
        cout<<"unable to create directory: <"<<(agent->getNode()+"/outgoing")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    temp_dir = data_base_path(agent->getNode(), "temp", "exec") + "/";
    if (temp_dir.empty())
    {
        cout<<"unable to create directory: <"<<(agent->getNode()+"/temp")+"/exec"<<"> ... exiting."<<endl;
        exit(1);
    }

    // Add agent request functions
    if ((iretn=agent->add_request("getqueuesize", request_get_queue_size, "", "returns the current size of the command queue")))
        exit (iretn);
    if ((iretn=agent->add_request("delcommand", request_del_command, "entry-string", "deletes the specified command event from the queue according to its JSON string")))
        exit (iretn);
    if ((iretn=agent->add_request("delcommandid", request_del_command_id, "entry #", "deletes the specified command event from the queue according to its position")))
        exit (iretn);
    if ((iretn=agent->add_request("getcommand", request_get_command, "[ entry # ]", "returns the requested command queue entry (or all if none specified)")))
        exit (iretn);
    if ((iretn=agent->add_request("addcommand", request_add_command, "{\"event_name\":\"\"}{\"event_utc\":0}{\"event_utcexec\":0}{\"event_flag\":0}{\"event_type\":0}{\"event_data\":\"\"}{\"event_condition\":\"\"}", "adds the specified command event to the queue")))
        exit (iretn);
    if ((iretn=agent->add_request("runremotecommand", request_remote_command, "command-to-run", "run a command local to the agent and return its output")))
        exit (iretn);
    if ((iretn=agent->add_request("getevent", request_get_event, "[ entry # ]", "returns the requested event queue entry (or all if none specified)")))
        exit (iretn);
    if ((iretn=agent->add_request("reopenexec", request_reopen_exec, "", "flushes exec log files out of temp directory")))
        exit (iretn);
    if ((iretn=agent->add_request("setlogstrideexec", request_set_logstride_exec, "logstride", "modify how frequently we flush exec log files out of temp directory")))
        exit (iretn);
    if ((iretn=agent->add_request("getlogstrideexec", request_get_logstride_exec, "", "return how frequently we flush exec log files out of temp directory")))
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

    load_dictionary(eventdict, agent->cinfo, "events.dict");

    // Start thread to collect SOH data
    bool log_data_flag = true;
    vector <beatstruc> servers = agent->find_servers(1.);
    for (beatstruc &i : servers)
    {
        if (strcmp(i.node, agent->nodeName.c_str()) && !strcmp(i.proc, "exec"))
        {
            log_data_flag = false;
            break;
        }
    }

    if (log_data_flag)
    {
        printf("Primary: SOH logger\n");
    }
    else
    {
        printf("Secondary: Not logging SOH\n");
    }

    cdthread = thread([=] { collect_data_loop(); });

    // Create default logstring
    logstring = json_list_of_soh(agent->cinfo);
    printf("===\nlogstring: %s\n===\n", logstring.c_str()); fflush(stdout);
    json_table_of_list(logtable, logstring.c_str(), agent->cinfo);

    agent->set_sohstring(logstring);

    // Start performing the body of the agent
    agent->post(Agent::AgentMessage::REQUEST, "postsoh");
    COSMOS_SLEEP(10.);
    llogmjd = currentmjd();
    clogmjd =  currentmjd();
    logdate_exec = 0.;
    logstride_exec = 0.;
    agent->start_active_loop();
    agent->debug_level = 0;
    ElapsedTime postet;
    ElapsedTime savet;
    while(agent->running())
    {
        int32_t next_power_mode = get_power_mode();
        agent->cinfo->node.powmode = next_power_mode;

        // Fix time
//        iretn = agent->get_agent_time(correcttime, epsilon, delta, "gps");
//        if (iretn >= 0 && fabs(delta*86400.) > 30.)
//        {
//            printf("Initialized time from time GPS: Delta %f %f Epsilon %f\n", 86400. * (correcttime - currentmjd()), 86400.*delta, 86400.*epsilon);
//            set_local_clock(correcttime);
//            llogmjd = currentmjd();
//            clogmjd =  currentmjd();
//            logdate_exec = 0.;
//            logstride_exec = 0.;
//        }
//        else
//        {
//            iretn = agent->get_agent_time(correcttime, epsilon, delta, "duplex");
//            if (iretn >= 0 && fabs(delta*86400.) > 30.)
//            {
//                printf("Initialized time from time DUPLEX: Delta %f %f Epsilon %f\n", 86400. * (correcttime - currentmjd()), 86400.*delta, 86400.*epsilon);
//                set_local_clock(correcttime);
//                llogmjd = currentmjd();
//                clogmjd =  currentmjd();
//                logdate_exec = 0.;
//                logstride_exec = 0.;
//            }
//        }

        dlogmjd = (clogmjd-llogmjd)*86400.;
        agent->cinfo->node.utc = clogmjd = currentmjd();
        agent->cinfo->node.downtime = get_last_offset();

        // Check if exec logstride has changed
        if (fabs(newlogstride_exec - logstride_exec) > std::numeric_limits<double>::epsilon())
        {
            logstride_exec = newlogstride_exec;
            logdate_exec = currentmjd(0.);
            move_and_compress_exec();
        }

        // Check if exec logstride has expired
        if (logstride_exec != 0. && floor(clogmjd/logstride_exec)*logstride_exec > logdate_exec)
        {
            logdate_exec = floor(clogmjd/logstride_exec)*logstride_exec;
            move_and_compress_exec();
        }

        // Check if the SOH logperiod has changed
        if (newlogperiod != logperiod )
        {
            logperiod = newlogperiod;
            logdate_soh = agent->cinfo->node.utc;

            if (log_data_flag)
            {
                move_and_compress_soh();
            }
        }

        if (fabs(newlogstride_soh - logstride_soh) > std::numeric_limits<double>::epsilon())
        {
            logstride_soh = newlogstride_soh;
            logdate_soh = currentmjd(0.);


            if (log_data_flag)
            {
                move_and_compress_soh();
                move_and_compress_beacon();
            }
        }

        if (logstride_soh != 0. && floor(clogmjd/logstride_soh)*logstride_soh > logdate_soh)
        {
            logdate_soh = floor(clogmjd/logstride_soh)*logstride_soh;
            if (log_data_flag)
            {
                move_and_compress_soh();
                move_and_compress_beacon();
            }
        }

        // Check if SOH logperiod has expired
        if (dlogmjd-logperiod > -logperiod/20.)
        {
            llogmjd = clogmjd;
            if (log_data_flag && agent->cinfo->node.utc != 0. && logstring.size())
            {
                log_write(agent->cinfo->node.name, DATA_LOG_TYPE_SOH, logdate_soh, json_of_table(jjstring, logtable, agent->cinfo));
//                log_write(agent->cinfo->node.name, DATA_LOG_TYPE_SOH, logdate_soh, json_of_table(jjstring, logtable, agent->cinfo), static_cast <string>("immediate"));
            }
        }

        // Perform SOH specific functions
        if (log_data_flag && logstride_soh > 0. && agent->cinfo->node.utc != 0. && postet.split() >= 43200. * logperiod)
        {
            postet.reset();
            agent->post(Agent::AgentMessage::REQUEST, "postsoh");

            loc_update(&agent->cinfo->node.loc);
            update_target(agent->cinfo);
            //            agent->post(Agent::AgentMessage::SOH, json_of_table(myjstring, logtable, agent->cinfo));
            calc_events(eventdict, agent->cinfo, events);
            for (uint32_t k=0; k<events.size(); ++k)
            {
                memcpy(&agent->cinfo->event[0].s,&events[k],sizeof(shorteventstruc));
                strcpy(agent->cinfo->event[0].l.condition,agent->cinfo->emap[events[k].handle.hash][events[k].handle.index].text);
                log_write(agent->cinfo->node.name,DATA_LOG_TYPE_EVENT,logdate_soh, json_of_event(jjstring, agent->cinfo));
            }
        }

        // Perform Executive specific functions
        cmd_queue.load_commands(immediate_dir);
        cmd_queue.load_commands(incoming_dir);
        cmd_queue.join_event_threads();
        cmd_queue.run_commands(agent, agent->getNode(), logdate_exec);

        if (savet.split() > 60.)
        {
            FILE *fp = fopen(("/cosmos/nodes/" + agent->nodeName + "/last_date").c_str(), "w");
            if (fp)
            {
                savet.reset();
                calstruc date = mjd2cal(currentmjd());
                fprintf(fp, "%02d%02d%02d%02d%04d.59\n", date.month, date.dom, date.hour, date.minute, date.year);
                fclose(fp);
            }
        }
        agent->finish_active_loop();
    }

    agent->shutdown();
    if (log_data_flag)
    {
        cdthread.join();
    }
}

int32_t get_last_offset()
{
    int32_t offset = 0;
    FILE *fp = fopen(("/cosmos/nodes/" + agent->nodeName + "/last_offset").c_str(), "r");
    if (fp != nullptr)
    {
        fscanf(fp, "%d", &offset);
        fclose(fp);
    }
    return offset;
}

int32_t get_flags()
{
    int8_t launched = 0;
    int8_t bootcheck = 0;
    int8_t deployed = 0;
    FILE *fp = fopen(("/cosmos/nodes/" + agent->nodeName + "/boot_flags").c_str(), "r");
    if (fp != nullptr)
    {
        fscanf(fp, "%hhd %hhd %hhd", &launched, &deployed, &bootcheck);
        if (launched)
        {
            boot_flags.launched = true;
            agent->cinfo->node.flags |= NODE_FLAG_LAUNCHED;
        }
        else
        {
            boot_flags.launched = false;
            agent->cinfo->node.flags &= ~NODE_FLAG_LAUNCHED;
        }
        if (bootcheck)
        {
            boot_flags.bootcheck = true;
            agent->cinfo->node.flags |= NODE_FLAG_BOOTCHECK;
        }
        else
        {
            boot_flags.bootcheck = false;
            agent->cinfo->node.flags &= ~NODE_FLAG_BOOTCHECK;
        }
        if (deployed)
        {
            boot_flags.deployed = true;
            agent->cinfo->node.flags |= NODE_FLAG_DEPLOYED;
        }
        else
        {
            boot_flags.deployed = false;
            agent->cinfo->node.flags &= ~NODE_FLAG_DEPLOYED;
        }
        fclose(fp);
        return 1;
    }
    else
    {
        return 0;
    }
}

// Executive specific requests
int32_t request_set_logstride_exec(string &request, string &, Agent *)
{
    sscanf(request.c_str(),"%*s %lf",&newlogstride_exec);
    return 0;
}
int32_t request_get_logstride_exec(string &, string &response, Agent *)
{
    response =  std::to_string(logstride_exec);
    return 0;
}


int32_t request_reopen_exec(string &, string &, Agent *)
{
    // Do not wait for log stride, move data from temp to incoming immediately.
    logdate_exec = agent->cinfo->node.loc.utc;
    move_and_compress_exec();
    return 0;
}

int32_t request_get_queue_size(string &, string &response, Agent *)
{
    response = std::to_string(cmd_queue.get_command_size());
    return 0;
}

int32_t request_get_event(string &request, string &response, Agent *)
{
    std::ostringstream ss;

    if(cmd_queue.get_event_size()==0)	{
        ss << "[]";
    }
    else {
        int j;
        int32_t iretn = sscanf(request.c_str(),"%*s %d",&j);

        // if valid index then return event
        if (iretn == 1) {
            if(j >= 0 && j < static_cast<int>(cmd_queue.get_event_size()))
                ss << cmd_queue.get_event(j);
            else
                ss << "<" << j << "> is not a valid event queue index (current range between 0 and "
                   << cmd_queue.get_event_size()-1 << ")";
        }

        // if no index given, return the entire queue
        else if (iretn ==  -1) {
            for(int i = 0; i < static_cast<int>(cmd_queue.get_event_size()); ++i) {
                Event cmd = cmd_queue.get_event(i);
                ss << "[" << i << "]" << "[" << mjd2iso8601(cmd.getUtc()) << "]" << cmd << endl;
            }
        }
        // if the user supplied something that couldn't be turned into an integer
        else if (iretn == 0) { ss << "Usage:\tget_event [ index ]\t"; }
    }

    response = ss.str();
    return 0;
}

int32_t request_get_command(string &request, string &response, Agent *)
{
    std::ostringstream ss;

    if(cmd_queue.get_command_size()==0)	{
        ss << "the command queue is empty";
    }
    else {
        int j;
        int32_t iretn = sscanf(request.c_str(),"%*s %d",&j);

        // if valid index then return command
        if (iretn == 1) {
            if(j >= 0 && j < static_cast<int>(cmd_queue.get_command_size()))
                ss << cmd_queue.get_command(j);
            else
                ss << "<" << j << "> is not a valid command queue index (current range between 0 and "
                   << cmd_queue.get_command_size()-1 << ")";
        }

        // if no index given, return the entire queue
        else if (iretn ==  -1) {
            for(int i = 0; i < static_cast<int>(cmd_queue.get_command_size()); ++i) {
                Event cmd = cmd_queue.get_command(i);
                ss << "[" << i << "]" << "[" << mjd2iso8601(cmd.getUtc()) << "]" << cmd << endl;
            }
        }
        // if the user supplied something that couldn't be turned into an integer
        else if (iretn == 0) { ss << "Usage:\tget_command [ index ]\t"; }
    }

    response = ss.str();
    return 0;
}

// Delete Queue Entry - by #
int32_t request_del_command_id(string &request, string &response, Agent *)
{
    Event cmd;
    std::ostringstream ss;

    if(cmd_queue.get_command_size()==0)	{
        ss << "the command queue is empty";
    }
    else {
        int j;
        int32_t iretn = sscanf(request.c_str(),"%*s %d",&j);

        // if valid index then return command
        if (iretn == 1) {
            if(j >= 0 && j < static_cast<int>(cmd_queue.get_command_size())) {
                response = std::to_string(cmd_queue.del_command(j)) + " commands deleted from the queue";
            } else {
                ss << "<" << j << "> is not a valid command queue index (current range between 0 and " << cmd_queue.get_command_size()-1 << ")";
            }
        }
        // if the user supplied something that couldn't be turned into an integer
        else if (iretn == 0)	{
            ss << "Usage:\tdel_command_id [ index ]\t";
        }
    }

    response = ss.str();
    return 0;
}

// Delete Queue Entry - by date and contents
int32_t request_del_command(string &request, string &response, Agent *)
{
    Event cmd;
    string line(request);

    // remove "del_command " from request string
    line.erase(0, 10);
    cmd.set_command(line);

    //delete command
    int n = cmd_queue.del_command(cmd);

    if(!cmd.is_command()) {
        response =  "Not a valid command: " + line;
    }
    else {
        response = std::to_string(n) + " commands deleted from the queue";
    }

    return 0;
}

// Add Queue Entry
int32_t request_add_command(string &request, string &response, Agent *)
{
    Event cmd;
    string line(request);

    // remove "add_command " from request string
    line.erase(0, 10);
    cmd.set_command(line);

    // add command
    if(cmd.is_command()) {
        cmd_queue.add_command(cmd);
        response =  "Command added to queue: " + line;
    }
    else {
        response =  "Not a valid command: " + line;
    }

    // sort the queue
    cmd_queue.sort();
    return 0;
}

// Run the command and return the output in the response.
int32_t request_remote_command(string &request, string &response, Agent *)
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
        response = "Unable to find an appropriate command.";
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
            response.resize(AGENTMAXBUFFER);
            iretn = fread(&response[0], 1, AGENTMAXBUFFER-1, pd);
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
int32_t request_reopen_soh(string &, string &, Agent *agent)
{
    // Do not wait for logstride, push soh log files out of temp and into incoming.
    logdate_soh = agent->cinfo->node.loc.utc;
    move_and_compress_soh();
    return 0;
}

int32_t request_set_logperiod(string &request, string &, Agent *)
{
    sscanf(request.c_str(),"%*s %lf",&newlogperiod);
    newlogperiod /= 86400.;
    return 0;
}
int32_t request_get_logperiod(string &, string &response, Agent *)
{
    response =  std::to_string(86400. * logperiod);
    return 0;
}

int32_t request_set_logstring(string &request, string &, Agent *agent)
{
    logstring = &request[strlen("set_logstring")+1];
    logtable.clear();
    json_table_of_list(logtable, logstring.c_str(), agent->cinfo);
    return 0;
}

int32_t request_get_logstring(string &, string &response, Agent *)
{
    response = logstring;
    return 0;
}

int32_t request_set_logstride_soh(string &request, string &, Agent *)
{
    sscanf(request.c_str(),"%*s %lf",&newlogstride_soh);
    newlogstride_soh /= 86400.;
    return 0;
}
int32_t request_get_logstride_soh(string &, string &response, Agent *)
{
    response =  std::to_string(86400. * logstride_soh);
    return 0;
}

void collect_data_loop() noexcept
{
    int32_t iretn;
    while (agent->running())
    {
        // Collect new data
        Agent::messstruc mess;
        iretn = agent->readring(mess, Agent::AgentMessage::ALL, 5., Agent::Where::TAIL, "", agent->cinfo->node.name);
        if (iretn >= 0)
        {
            if (mess.meta.type < Agent::AgentMessage::BINARY)
            {
                json_parse(mess.adata, agent->cinfo);
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
    }
    return;
}

// Moving exec and soh logs cannot occur concurrently.
void move_and_compress_exec () {
    exec_mutex.lock();
    cmd_queue.join_event_threads();
    log_move(agent->getNode(), "exec");
    exec_mutex.unlock();
}
void move_and_compress_soh () {
    soh_mutex.lock();
    log_move(agent->getNode(), "soh");
    soh_mutex.unlock();
}

void move_and_compress_beacon () {
    string beacon_string;
    beacon_mutex.lock();
    log_write(agent->cinfo->node.name, DATA_LOG_TYPE_BEACON, logdate_soh, json_of_beacon(beacon_string, agent->cinfo));
    log_move(agent->getNode(), "beacon");
    beacon_mutex.unlock();
}


int32_t get_power_mode()
{
    int32_t powermode = -1;
    FILE *fp = popen("/cosmos/scripts/power_mode_get", "r");
    if (fp == nullptr)
    {
        return -errno;
    }
    char tdata[100];
    uint16_t tindex;
    if ((fgets(tdata, 100, fp)) == tdata)
    {
        if (sscanf(tdata, "%u\n", &tindex) == 1)
        {
            if (tindex > 0)
            {
                powermode = tindex;
            }
        }
    }
    pclose(fp);
    return powermode;
}

//void get_beacon_cpu() {
//    static DeviceCpu deviceCpu;
//    static const double GiB = 1024. * 1024. * 1024.;
//    int32_t iretn;
//    iretn = json_createpiece(agent->cinfo, "main_cpu", DeviceType::CPU);
//    if (iretn < 0)
//    {
//        fprintf(agent->get_debug_fd(), "Failed to add CPU %s\n", cosmos_error_string(iretn).c_str());
//        agent->shutdown();
//        exit(1);
//    }

//    uint16_t cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
//    agent->cinfo->device[cidx].cpu.load = static_cast <float>(deviceCpu.getLoad());
//    agent->cinfo->device[cidx].cpu.gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/GiB);
//    agent->cinfo->device[cidx].cpu.maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/GiB);
//    agent->cinfo->device[cidx].cpu.maxload = deviceCpu.getCpuCount();
//}
