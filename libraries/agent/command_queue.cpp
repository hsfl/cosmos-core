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
        namespace Command
        {
            int32_t Shell(string command_line, string outpath, string inpath, string errpath)
            {
                int32_t iretn=0;
                int devin, devout, deverr;
                int prev_stdin, prev_stdout, prev_stderr;

                if (command_line.empty())
                {
                    return GENERAL_ERROR_EMPTY;
                }

                if (outpath.empty())
                {
                    devout = dup(STDOUT_FILENO);
                }
                else
                {
                    devout = open(outpath.c_str(), O_CREAT|O_WRONLY|O_APPEND, 00666);
                }
                // Redirect.
                prev_stdout = dup(STDOUT_FILENO);
                dup2(devout, STDOUT_FILENO);
                close(devout);

                if (inpath.empty())
                {
                    devin = open("/dev/null", O_RDWR);
                }
                else
                {
                    devin = open(inpath.c_str(), O_RDONLY, 00666);
                }
                prev_stdin = dup(STDIN_FILENO);
                dup2(devin, STDIN_FILENO);
                close(devin);

                prev_stderr = dup(STDERR_FILENO);
                if (errpath.empty())
                {
                    deverr = devout;
                }
                else
                {
                    deverr = open(errpath.c_str(), O_CREAT|O_WRONLY|O_APPEND, 00666);
                }
                dup2(deverr, STDERR_FILENO);
                close(deverr);

                // Execute the command.
                iretn = system(command_line.c_str());

                // Reset standard file handles
                dup2(prev_stdin, STDIN_FILENO);
                dup2(prev_stdout, STDOUT_FILENO);
                dup2(prev_stderr, STDERR_FILENO);
                close(prev_stdin);
                close(prev_stdout);
                close(prev_stderr);

                return iretn;
            }
        }

        // *************************************************************************
        // Class: CommandQueue
        // *************************************************************************


        CommandQueue::~CommandQueue () { join_event_threads(); }

        // Before moving log files, we must join the event threads and ensure that each
        // event spawned is not currently active.
        size_t CommandQueue::join_event_threads()
        {
            //            static auto join_event = [] (thread &t) {
            //                t.join();
            //            };

            //            std::for_each(event_threads.begin(), event_threads.end(), join_event);
            //            event_threads.clear();
            size_t count = 0;
            for(uint16_t i=0; i<event_threads.size(); ++i)
            {
                if (event_threads[i].joinable())
                {
                    event_threads[i].join();
                    ++count;
                }
            }

            if (count == event_threads.size())
            {
                event_threads.clear();
                return count;
            }
            else
            {
                return event_threads.size();
            }
        }

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
            iretn = agent->process_request(request, bufferout);
            if (iretn >= 0)
            {
                FILE *fp = fopen(outpath.c_str(), "w");
                if (fp != nullptr)
                {
                    fwrite(bufferout.data(), 1, bufferout.size(), fp);
                }
                // log to event file
                log_write(node_name, "exec", logdate_exec, "event", cmd.get_event_string().c_str());
            }
            return iretn;
        }

        //! Run the given Command Event
        /*!
                Executes a command in a separate shell (system) using threads. For each
                command run, the time of execution (utcexec) is set, the flag
                EVENT_FLAG_ACTUAL is set to true, and this updated command information is
                logged to the OUTPUT directory.

                \param	cmd	Reference to event to run
                \param	nodename	Name of node
                \param	logdate_exec	Time of execution (for logging purposes)
            */
        int32_t CommandQueue::run_command(Event& cmd, string node_name, double logdate_exec)
        {
            queue_changed = true;

            // set time executed & actual flag
            cmd.set_utcexec();
            cmd.set_actual();

            string outpath = data_type_path(node_name, "temp", "exec", logdate_exec, "out");
            char command_line[100];
            strcpy(command_line, cmd.get_data().c_str());

            // We keep track of all threads spawned to join before moving log files.
            event_threads.push_back(thread([=] ()
            {
                int devn, prev_stdin, prev_stdout, prev_stderr;
                if (outpath.empty()) {
                    devn = open("/dev/null", O_RDWR);
                }
                else {
                    devn = open(outpath.c_str(), O_CREAT|O_WRONLY|O_APPEND, 00666);
                }

                prev_stdin = dup(STDIN_FILENO);
                prev_stdout = dup(STDOUT_FILENO);
                prev_stderr = dup(STDERR_FILENO);

                // Redirect all output our executed command.
                dup2(devn, STDIN_FILENO);
                dup2(devn, STDOUT_FILENO);
                dup2(devn, STDERR_FILENO);
                close(devn);

                // Execute the command.
                size_t ret = system(command_line);
                if (ret == 0)
                {
                    queue_changed = true;
                }

                dup2(prev_stdin, STDIN_FILENO);
                dup2(prev_stdout, STDOUT_FILENO);
                dup2(prev_stderr, STDERR_FILENO);
                close(prev_stdin);
                close(prev_stdout);
                close(prev_stderr);
            }));

            // log to event file
            log_write(node_name, "exec", logdate_exec, "event", cmd.get_event_string().c_str());
            return 0;
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
            for(std::list<Event>::iterator ii = commands.begin(); ii != commands.end(); ++ii)
            {
                // if command is not solo, or event_threads queue is empty
                if (event_threads.empty() || (!ii->is_solo() && !queue_blocked))
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
                                        if (ii->type == EVENT_TYPE_COMMAND)
                                        {
                                            run_command(*ii, node_name, logdate_exec);
                                        }
                                        else if (ii->type == EVENT_TYPE_REQUEST)
                                        {
                                            run_request(agent, *ii, node_name, logdate_exec);
                                        }
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
                                    if (ii->type == EVENT_TYPE_COMMAND)
                                    {
                                        run_command(*ii, node_name, logdate_exec);
                                    }
                                    else if (ii->type == EVENT_TYPE_REQUEST)
                                    {
                                        run_request(agent, *ii, node_name, logdate_exec);
                                    }
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
                            if (ii->type == EVENT_TYPE_COMMAND)
                            {
                                run_command(*ii, node_name, logdate_exec);
                            }
                            else if (ii->type == EVENT_TYPE_REQUEST)
                            {
                                run_request(agent, *ii, node_name, logdate_exec);
                            }
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
            size_t prev_sz = commands.size();
            std::list<Event>::iterator b = commands.begin();

            std::advance(b, pos);
            commands.erase(b);
            queue_changed = true;
            return static_cast<int32_t>(prev_sz - commands.size());
        }

        int32_t CommandQueue::add_command(Event& c)
        {
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
            for(std::list<Event>::iterator ii = cmdq.commands.begin(); ii != cmdq.commands.end(); ++ii)
                out << *ii << std::endl;
            return out;
        }
    } // end namespace Support
} // end namespace Cosmos

//! @}
