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

#include "support/command_queue.h"

namespace Cosmos {

// *************************************************************************
// Class: CommandQueue
// *************************************************************************

// Executes a command using fork().  For each command run, the time of
// execution (utcexec) is set, the flag EVENT_FLAG_ACTUAL is set to true,
// and this updated command information is logged to the OUTPUT directory.
void CommandQueue::run_command(Event& cmd, string nodename, double logdate_exec)
{
	queue_changed = true;

	// set time executed & actual flag
	cmd.set_utcexec();
	cmd.set_actual();

	// execute command
#if defined(COSMOS_WIN_OS)
	char command_line[100];
    strcpy(command_line, cmd.get_data().c_str());

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	if (CreateProcessA(NULL, (LPSTR) command_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		//		int32_t pid = pi.dwProcessId;
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
#else
	signal(SIGCHLD, SIG_IGN);
	int32_t pid = fork();
	switch(pid)
	{
	case -1:
		break;
	case 0:
		char *words[MAXCOMMANDWORD];
		int devn;
        string_parse((char *)cmd.get_data().c_str(),words,MAXCOMMANDWORD);
		string outpath = data_type_path(nodename, "temp", "exec", logdate_exec, "out");
		if (outpath.empty())
		{
			devn = open("/dev/null",O_RDWR);
		}
		else
		{
			devn = open(outpath.c_str(), O_CREAT|O_WRONLY|O_APPEND, 00666);
		}
		dup2(devn, STDIN_FILENO);
		dup2(devn, STDOUT_FILENO);
		dup2(devn, STDERR_FILENO);
		close(devn);
		execvp(words[0],&(words[0]));
		fflush(stdout);
		exit (0);
		break;
	}
#endif

	// log to event file
    log_write(nodename, "exec", logdate_exec, "event", cmd.get_event_string().c_str());
}


// Manages the logic of when to run commands in the command queue.
void CommandQueue::run_commands(Agent *agent, string nodename, double logdate_exec) // TODO: remove dependency to pointer to agent
{
    for(std::list<Event>::iterator ii = commands.begin(); ii != commands.end(); ++ii) {
		// if command is ready
		if (ii->is_ready()) {
			// if command is conditional
			if (ii->is_conditional()) {
				// if command condition is true
				if(ii->condition_true(agent->cinfo)) {
					// if command is repeatable
					if(ii->is_repeat()) {
						// if command has not already run
						if(!ii->already_ran)	{
							run_command(*ii, nodename, logdate_exec);
							ii->already_ran = true;
						}
					// else command is non-repeatable
					} else {
						run_command(*ii, nodename, logdate_exec);
						commands.erase(ii--);
					}
				// else command condition is false
				} else {
					ii->already_ran = false;
				}
			// else command is non-conditional
			} else {
				run_command(*ii, nodename, logdate_exec);
				commands.erase(ii--);
			}
		} else {
			;//cout<<"This command is *NOT* ready to run! ";
		}
	}
	return;
}

// Saves commands to .queue file located in the temp directory
// Commands are taken from the global command queue
// Command queue is sorted by utc after loading
void CommandQueue::save_commands(string temp_dir)
{
	if (!queue_changed)
	{
		return;
	}
	queue_changed = false;

	// Open the outgoing file
	FILE *fd = fopen((temp_dir+".queue").c_str(), "w");
	if (fd != NULL)
	{
        for (Event cmd: commands)
		{
            fprintf(fd, "%s\n", cmd.get_event_string().c_str());
		}
		fclose(fd);
	}
}

// Loads new commands from *.command files located in the incoming directory
// Commands are loaded into the global CommandQueue object (cmd_queue),
// *.command files are removed, and the command list is sorted by utc.
//void CommandQueue::load_commands(string incoming_dir, Agent *agent) // TODO: change arguments so that we don't need to pass the separate directories
void CommandQueue::load_commands(string incoming_dir) 
{
	DIR *dir = NULL;
	struct dirent *dir_entry = NULL;

	// open the incoming directory
	if ((dir = opendir((char *)incoming_dir.c_str())) == NULL)
	{
		std::cout<<"error: unable to open node's incoming directory <"<<incoming_dir<<"> not found"<<std::endl;
		return;
	}

	// cycle through all the file names in the incoming directory
	while((dir_entry = readdir(dir)) != NULL)
	{
		string filename = dir_entry->d_name;

		if (filename.find(".command") != string::npos)
		{

			string infilepath = incoming_dir + filename;
			std::ifstream infile(infilepath.c_str());
			if(!infile.is_open())
			{
				std::cout<<"unable to read file <"<<infilepath<<">"<<std::endl;
				continue;
			}

			//file is open for reading commands
			string line;
            Event cmd;

			while(getline(infile,line))
			{
				//cmd.set_command(line, agent); // TODO: is it really necessary to pass *agent?
				cmd.set_command(line); // TODO: is it really necessary to pass *agent?  NOPE!
				std::cout<<cmd;

				if(cmd.is_command())
					add_command(cmd);
				else
					std::cout<<"Not a command!"<<std::endl;
			}
			infile.close();

			//remove the .command file from incoming directory
			if(remove(infilepath.c_str()))	{
				std::cout<<"unable to delete file <"<<filename<<">"<<std::endl;
				continue;
			}

			std::cout<<"The size of the command queue is: "<< get_size()<<std::endl;
		}
	}

	sort();

	closedir(dir);

	return;
}

// Remove command object from the command queue, uses command == operator)
int CommandQueue::del_command(Event& c)
{
	int n = 0;
    for(std::list<Event>::iterator ii = commands.begin(); ii != commands.end(); ++ii)
	{
		if(c==*ii)
		{
			commands.erase(ii--);
			n++;
		}
	}
	queue_changed = true;
	return n;
}

void CommandQueue::add_command(Event& c)
{
	commands.push_back(c);
	queue_changed = true;
}

//// Predicate function for comparing command objects, used by CommandQueue.sort()
//bool CommandQueue::compare_command_times(Event command1, Event command2)
//{
//	return command1.getUtc()<command2.getUtc();
//}

// Copies the current CommandQueue object to the output stream using JSON format
std::ostream& operator<<(std::ostream& out, CommandQueue& cmdq)
{
    for(std::list<Event>::iterator ii = cmdq.commands.begin(); ii != cmdq.commands.end(); ++ii)
		out << *ii << std::endl;
	return out;
}

} // end namespace Cosmos

//! @}
