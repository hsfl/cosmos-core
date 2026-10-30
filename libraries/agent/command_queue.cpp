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

/*! \file agentclass.cpp
    \brief Agent support functions
*/

#include "agent/command_queue.h"

namespace Cosmos
{
    namespace Support
    {
        // *************************************************************************
        // Class: CommandQueue
        // *************************************************************************


        CommandQueue::~CommandQueue () {}

        //! Run the given Request Event
        /*!
            Submits a Request to the local agent. For each
            command run, the time of execution (utcexec) is set, the flag
            EVENT_FLAG_ACTUAL is set to true, and this updated command information is
            logged to the OUTPUT directory.

            \param	cmd	Reference to event to run
            \param	nodename	Name of node
            \param	logdate_exec	Time of execution (for logging purposes)
        */
        int32_t CommandQueue::run_request(Agent *agent, Event& cmd, string node_name, double logdate_exec)
        {
            int32_t iretn = 0;
            queue_changed = true;

            // set time executed & actual flag
            cmd.set_utcexec();
            cmd.set_actual();

            string outpath = data_type_path(node_name, "temp", "exec", logdate_exec, "out");
            string bufferout;
            string request = cmd.get_data();
            iretn = agent->process_request(request, bufferout, false);
            printf("CMD: running request [%s] in [%s] with result %d\n", request.c_str(), outpath.c_str(), iretn);
            if (iretn >= 0)
            {
                FILE *fp = fopen(outpath.c_str(), "w");
                if (fp != nullptr)
                {
                    fwrite(bufferout.data(), 1, bufferout.size(), fp);
                    fclose(fp);
                    log_move_file(outpath, string_replace(outpath, "temp", "outgoing"), true);
                    printf("CMD: moving request result to [%s]\n", string_replace(outpath, "temp", "outgoing").c_str());
                }
                else
                {
                    printf("CMD: failed to move request result to [%s] for %d\n", outpath.c_str(), errno);
                }
                // log to event file
                log_write(node_name, "exec", logdate_exec, "", "event", cmd.get_event_string().c_str(), "outgoing");
            }
            return iretn;
        }

        //! Run the given Command Event
        /*!
                Executes a command in a separate shell (system) using threads. For each
                command run, the time of execution (utcexec) is set, the flag
                EVENT_FLAG_ACTUAL is set to true, and this updated command information is
                logged to the OUTPUT directory.

                \param  agent Pointer to Agent object (for task addition)
                \param	cmd	Reference to event to run
                \param	nodename	Name of node
                \param	logdate_exec	Time of execution (for logging purposes)
            */
        int32_t CommandQueue::run_command(Agent* agent, Event& cmd, string node_name, double logdate_exec)
        {
            queue_changed = true;

            // set time executed & actual flag
            cmd.set_utcexec();
            cmd.set_actual();

            string outpath = data_type_path(node_name, "temp", "exec", logdate_exec, "out");
            printf("CMD: running command [%s] in [%s]\n", cmd.get_data().c_str(), outpath.c_str());
            // log to event file
            log_write(node_name, "exec", logdate_exec, "", "event", cmd.get_event_string().c_str(), "outgoing");
            int32_t iretn = agent->task_add(cmd.get_data(), node_name, cmd.timeout);
            if (iretn < 0)
            {
                printf("CMD: failed to run command [%s] for %d\n", cmd.get_data().c_str(), iretn);
                return iretn;
            }

            return 0;
        }


        int32_t CommandQueue::run_packet(Agent* agent, Event& cmd, string node_name, double logdate_exec)
        {
            queue_changed = true;

            PacketComm packet;
            int32_t iretn = packet.FromHexString(cmd.get_data());
            if (iretn < 0)
            {
                printf("CMD: failed to decode packet [%s] for %d\n", cmd.get_name().c_str(), iretn);
                return iretn;
            }

            printf("CMD: pushing packet [%s] type 0x%x to channel [%s]\n", cmd.get_name().c_str(), static_cast<uint16_t>(packet.header.type), packet_channel.c_str());
            iretn = agent->channel_push(packet_channel, packet);
            if (iretn < 0)
            {
                // Not marked actual or logged, so the event log only shows packets that were handed off
                printf("CMD: failed to push packet [%s] to channel [%s] for %d\n", cmd.get_name().c_str(), packet_channel.c_str(), iretn);
                return iretn;
            }

            // set time executed & actual flag
            cmd.set_utcexec();
            cmd.set_actual();
            // log to event file
            log_write(node_name, "exec", logdate_exec, "", "event", cmd.get_event_string().c_str(), "outgoing");
            return iretn;
        }

        int32_t CommandQueue::dispatch(Agent *agent, Event &cmd, string node_name, double &logdate_exec)
        {
            int32_t iretn = 0;
            switch (cmd.type)
            {
            case EVENT_TYPE_COMMAND:
                iretn = run_command(agent, cmd, node_name, logdate_exec);
                break;
            case EVENT_TYPE_REQUEST:
                iretn = run_request(agent, cmd, node_name, logdate_exec);
                break;
            case EVENT_TYPE_PACKET:
                iretn = run_packet(agent, cmd, node_name, logdate_exec);
                break;
            default:
                return 0;
            }
            logdate_exec += 1./864000.;
            return iretn;
        }

        //! Parse an Event JSON string and add it to the queue
        /*!
            \param line Event as a JSON string
            \param response Description of the result
            \return 0 on success, negative on error
        */
        int32_t CommandQueue::add_command_string(const string &line, string &response, bool packets_only)
        {
            Event cmd;
            cmd.set_command(line);

            if (!cmd.is_command())
            {
                response =  "Not a valid command: " + line;
                return GENERAL_ERROR_ARGS;
            }

            if (cmd.type == EVENT_TYPE_PACKET)
            {
                // Checked again by add_command(), but here we can say why
                PacketComm packet;
                int32_t iretn = check_packet_event(cmd, packet);
                if (iretn == GENERAL_ERROR_INPUT)
                {
                    response = "Packet type 0x" + to_hex(static_cast<uint16_t>(packet.header.type)) + " may not be queued: " + line;
                    return iretn;
                }
                else if (iretn < 0)
                {
                    response =  "Not a valid packet: " + line;
                    return iretn;
                }
            }
            else if (packets_only)
            {
                response = "Only packet events (type " + std::to_string(EVENT_TYPE_PACKET) + ") may be queued this way: " + line;
                return GENERAL_ERROR_INPUT;
            }

            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            int32_t iretn = add_command(cmd);
            if (iretn < 0)
            {
                response =  "Unable to queue: " + line;
                return iretn;
            }
            sort();
            response =  "Command added to queue: " + line;
            return 0;
        }

        int32_t CommandQueue::check_packet_event(Event &cmd, PacketComm &packet)
        {
            int32_t iretn = packet.FromHexString(cmd.get_data());
            if (iretn < 0)
            {
                return iretn;
            }
            if (packet_filter && !packet_filter(packet))
            {
                return GENERAL_ERROR_INPUT;
            }
            return 0;
        }

        int32_t CommandQueue::add_packet(PacketComm &packet, double utc, string name, uint32_t flag, string condition)
        {
            Event cmd;
            cmd.mjd = utc;
            cmd.name = name;
            cmd.type = EVENT_TYPE_PACKET;
            cmd.flag = flag;
            cmd.data = packet.ToHexString();
            cmd.condition = condition;

            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            int32_t iretn = add_command(cmd);
            sort();
            return iretn;
        }

        //!	Traverse the entire queue of Events, and run those which qualify.
        /*!

        An %Event only qualifies to run if the current time is greater than or equal to
        the execution time of the %Event.  Further, if the %Event is conditional, then the
        %Event condition must be true.

            \param	agent	Pointer to Agent object (for call to condition_true(..))
            \param	nodename	Name of the node
            \param	logdate_exec	Time of execution (for logging purposes)
        */
        int32_t CommandQueue::run_commands(Agent *agent, string node_name, double logdate_exec)
        {
            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            for(std::list<Event>::iterator ii = commands.begin(); ii != commands.end(); ++ii)
            {
                // if command is not solo
                if (!ii->is_solo() && !queue_blocked)
                {
                    if (ii->is_solo())
                    {
                        queue_blocked = true;
                    }
                    else
                    {
                        queue_blocked = false;
                    }
                    // if command is ready
                    if (ii->is_ready())
                    {
                        // if command is conditional
                        if (ii->is_conditional())
                        {
                            // if command condition is true
                            if(ii->condition_true(agent->cinfo))
                            {
                                // if command is repeatable
                                if(ii->is_repeat())
                                {
                                    // if command has not already run
                                    //                                    if(!ii->already_ran)
                                    if (!ii->is_alreadyrun())
                                    {
                                        //                                        strncpy(agent->cinfo->node.lastevent, ii->name.c_str(), COSMOS_MAX_NAME);
                                        agent->cinfo->node.lastevent = ii->name;
                                        agent->cinfo->node.lasteventutc = currentmjd();
                                        dispatch(agent, *ii, node_name, logdate_exec);
                                        ii->set_alreadyrun(true);
                                        events.push_back(*ii);
                                        if (events.size() > 10)
                                        {
                                            events.pop_front();
                                        }
                                        //                                        ii->already_ran = true;
                                        break;
                                    }
                                }
                                // else command is non-repeatable
                                else
                                {
                                    dispatch(agent, *ii, node_name, logdate_exec);
                                    events.push_back(*ii);
                                    if (events.size() > 10)
                                    {
                                        events.pop_front();
                                    }
                                    commands.erase(ii--);
                                    break;
                                }
                            }
                            // else command condition is false
                            else
                            {
                                ii->set_alreadyrun(false);
                            }
                        }
                        // else command is non-conditional
                        else
                        {
                            dispatch(agent, *ii, node_name, logdate_exec);
                            events.push_back(*ii);
                            if (events.size() > 10)
                            {
                                events.pop_front();
                            }
                            commands.erase(ii--);
                        }
                    }
                }
                else
                {
                    ;//cout<<"This command is *NOT* ready to run! ";
                }
            }
            return static_cast<int32_t>(commands.size());
        }

        //!	Save the queue of Events to a file
        /*!
            Commands are taken from the global command queue
            Command queue is sorted by utc after loading

            \param	temp_dir	Directory where the .queue file will be written
            \param	name	File where the .queue will be written
        */
        int32_t CommandQueue::save_commands(string temp_dir, string name)
        {
            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            if (!queue_changed)
            {
                return 0;
            }
            queue_changed = false;

            // Save previous queue
            rename((temp_dir+name).c_str(), (temp_dir+name+'.'+(utc2unixdate(currentmjd()))).c_str());

            // Open the outgoing file
            FILE *fd = fopen((temp_dir+name).c_str(), "w");
            if (fd != nullptr)
            {
                for (Event cmd: commands)
                {
                    fprintf(fd, "%s\n", cmd.get_event_string().c_str());
                }
                fclose(fd);
            }
            return static_cast<int32_t>(commands.size());
        }

        //!	Restore the queue of Events from a file
        /*!
            Commands are taken from the global command queue
            Command queue is sorted by utc after loading

            \param	temp_dir	Directory where the .queue file will be read from
            \param	name	File where the .queue will be read from
        */
        int32_t CommandQueue::restore_commands(string temp_dir, string name)
        {
            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            queue_changed = false;

            // Reload existing queue
            string infilepath = temp_dir + name;
            std::ifstream infile(infilepath.c_str());
            if(infile.is_open())
            {
                //file is open for reading commands
                string line;
                Event cmd;

                while(std::getline(infile,line))
                {
                    //cmd.set_command(line, agent);
                    cmd.set_command(line);

                    if(cmd.is_command())
                    {
                        add_command(cmd);
                    }
                }
                infile.close();
            }
            return static_cast<int32_t>(commands.size());
        }


        //!	Loads new commands from *.command files located in the incoming directory
        /*!
        // Commands are loaded into the global CommandQueue object (cmd_queue),
        // *.command files are removed, and the command list is sorted by utc.
            \param	incoming_dir	Directory where the .command files will be read from
        */
        int32_t CommandQueue::load_commands(string incoming_dir)
        {
            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            DIR *dir = nullptr;
            struct dirent *dir_entry = nullptr;

            // open the incoming directory
            if ((dir = opendir((char *)incoming_dir.c_str())) == nullptr)
            {
                return GENERAL_ERROR_OPEN;
            }

            // cycle through all the file names in the incoming directory
            while((dir_entry = readdir(dir)) != nullptr)
            {
                string filename = dir_entry->d_name;

                if (filename.find(".command") != string::npos)
                {

                    string infilepath = incoming_dir + filename;
                    std::ifstream infile(infilepath.c_str());
                    if(!infile.is_open())
                    {
                        //                        std::cout<<"unable to read file <"<<infilepath<<">"<<std::endl;
                        continue;
                    }

                    //file is open for reading commands
                    string line;
                    Event cmd;

                    while(getline(infile,line))
                    {
                        cmd.set_command(line);

                        if(cmd.is_command())
                        {
                            //                            std::cout << "Command added: " << cmd;
                            add_command(cmd);
                        }
                        //                        else
                        //                            std::cout<<"Not a command!"<<std::endl;
                    }
                    infile.close();

                    //remove the .command file from incoming directory
                    if(remove(infilepath.c_str()))	{
                        //                        std::cout<<"unable to delete file <"<<filename<<">"<<std::endl;
                        continue;
                    }

                    //                    std::cout<<"\nThe size of the command queue is: "<< get_command_size()<<std::endl;
                }
            }

            sort();

            closedir(dir);

            return 0;
        }
        ///	Remove **all** matching Event from the queue
        /**
            \param	c	Event to remove
            \return	The number of Events removed

            This function only removes events from the queue if the are exactly equal to the given Event.
        */
        int32_t CommandQueue::del_command(Event& c)
        {
            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            size_t prev_sz = commands.size();
            for (std::list<Event>::iterator ii = commands.begin();
                 ii != commands.end();
                 ++ii) {
                if (c == *ii) {
                    commands.erase(ii--);
                }
            }

            queue_changed = true;
            return static_cast<int32_t>(prev_sz - commands.size());

        }

        //! Remove Event from the queue based on position
        /*!
             \param  pos  Position of event to remove
             \return The number of Events removed

             This function removes events based on their queue position (0-indexed).
        */
        int32_t CommandQueue::del_command(int pos)
        {
            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            size_t prev_sz = commands.size();
            std::list<Event>::iterator b = commands.begin();

            std::advance(b, pos);
            commands.erase(b);
            queue_changed = true;
            return static_cast<int32_t>(prev_sz - commands.size());
        }

        int32_t CommandQueue::add_command(Event& c)
        {
            // Refuse packets now rather than at release time
            if (c.type == EVENT_TYPE_PACKET)
            {
                PacketComm packet;
                int32_t iretn = check_packet_event(c, packet);
                if (iretn < 0)
                {
                    printf("CMD: refused packet event [%s] for %d\n", c.get_name().c_str(), iretn);
                    return iretn;
                }
            }

            std::lock_guard<std::recursive_mutex> lock(queue_mutex);
            // Replace if it matches an existing command, otherwise add to queue
            for (std::list<Event>::iterator ii = commands.begin(); ii != commands.end(); ++ii) {
                if (c.get_name() == ii->get_name()) {
                    *ii = c;
                    queue_changed = true;
                    return 0;
                }
            }

            commands.push_back(c);
            queue_changed = true;
            return 1;
        }

        //!	Extraction operator
        /*!
            \param	out	Reference to ostream
            \param	cmdq	Reference to CommandQueue (JIMNOTE: should be const, ya?)
            \return	Reference to modified ostream

            Writes the given CommandQueue to the given output stream (in JSON format) and returns a reference to the modified ostream.

        */
        ::std::ostream& operator<<(::std::ostream& out, CommandQueue& cmdq)
        {
            std::lock_guard<std::recursive_mutex> lock(cmdq.queue_mutex);
            for(std::list<Event>::iterator ii = cmdq.commands.begin(); ii != cmdq.commands.end(); ++ii)
                out << *ii << std::endl;
            return out;
        }
    } // end namespace Support
} // end namespace Cosmos

//! @}
