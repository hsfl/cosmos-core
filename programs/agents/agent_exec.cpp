#include "configCosmos.h"

#include "agentlib.h"
#include "jsonlib.h"
#include "convertlib.h"
#include "datalib.h"

#include <iostream>
#include <iomanip>

/*! \file agent_exec.cpp
* \brief Executive Agent source file
*/

//! \ingroup agents
//! \defgroup agent_exec Executive Agent program
//! This Agent manages commanding within the COSMOS system.
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
#include <dirent.h>
#include <list>
#include <fstream>
#include <sstream>

cosmosstruc* cdata;
#include "agent_exec.h"

#define LOOPMSEC 100
#define MAXREQUESTENTRY 10
#define MAXQUEUEENTRY 5000
#define MAXLISTENTRY 10
#define MAXAGENTSIZE 50
#define MAXCOMMANDWORD 20

#define STATE_STANDBY 0
#define STATE_LAUNCH 1
#define STATE_DEPLOY 2
#define STATE_SAFE 3
#define STATE_NORMAL 4
#define STATE_POWERSAVE 5


string incoming_dir;
string outgoing_dir;
string temp_dir;

string nodename;
DIR *dir = NULL;
struct dirent *dir_entry = NULL;
double logdate=0.;
double newlogstride = 900. / 86400.;
double logstride = 0.;
bool queue_changed = false;

int32_t request_get_queue_size(char *request, char* response, void *cdata);
int32_t request_get_queue_entry(char *request, char* response, void *cdata);
int32_t request_del_queue_entry(char *request, char* response, void *cdata);
int32_t request_add_queue_entry(char *request, char* response, void *cdata);
int32_t request_run(char *request, char* response, void *cdata);
int32_t request_soh(char *request, char* response, void *cdata);
int32_t request_reopen(char* request, char* output, void *cdata);
int32_t request_set_logstride(char* request, char* output, void *cdata);

void collect_data_loop();
thread cdthread;
command_queue cmd_queue;

extern agent_request_structure reqs;


int main(int argc, char *argv[])
{

	cout<<"Starting the executive agent...";
	int32_t iretn;
	int ntype = SOCKET_TYPE_UDP;

	// Set node name to first argument
	if (argc!=2)	{
		cout<<"Usage: agent_exec node"<<endl;
		exit(1);
	}
	nodename = argv[1];

	// Establish the command channel and heartbeat
	cdata = agent_setup_server(ntype, (char *)nodename.c_str(), (char *)"exec", 1., 0, AGENTMAXBUFFER);
	if(cdata == NULL)	{
		cout<<"agent_exec: agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
		exit (AGENT_ERROR_JSON_CREATE);
	}
	cdata->node.utc = 0.;

	cout<<"  started."<<endl;

	// Set the incoming, outgoing, and temp directories
	incoming_dir = data_base_path(nodename, "incoming", "exec") + "/";
	if (incoming_dir.empty())
	{
		cout<<"unable to create directory: <"<<(nodename+"/incoming")+"/exec"<<"> ... exiting."<<endl;
		exit(1);
	}
	outgoing_dir = data_base_path(nodename, "outgoing", "exec") + "/";
	if (outgoing_dir.empty())
	{
		cout<<"unable to create directory: <"<<(nodename+"/outgoing")+"/exec"<<"> ... exiting."<<endl;
		exit(1);
	}
	outgoing_dir = data_base_path(nodename, "outgoing", "exec") + "/";
	if (outgoing_dir.empty())
	{
		cout<<"unable to create directory: <"<<(nodename+"/outgoing")+"/exec"<<"> ... exiting."<<endl;
		exit(1);
	}
	temp_dir = data_base_path(nodename, "temp", "exec") + "/";
	if (temp_dir.empty())
	{
		cout<<"unable to create directory: <"<<(nodename+"/temp")+"/exec"<<"> ... exiting."<<endl;
		exit(1);
	}

	// Add agent request functions
	if ((iretn=agent_add_request(cdata, (char *)"get_queue_size",request_get_queue_size,"", "returns the current size of the command queue")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"del_queue_entry",request_del_queue_entry,"entry #","deletes the specified command queue entry")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"get_queue_entry",request_get_queue_entry,"[ entry # ]","returns the requested command queue entry (or all if none specified)")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"add_queue_entry",request_add_queue_entry,"{\"event_name\":\"\"}{\"event_utc\":0}{\"event_utcexec\":0}{\"event_flag\":0}{\"event_type\":0}{\"event_data\":\"\"}{\"event_condition\":\"\"}","adds the specified command queue entry")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"run",request_run,"","run the requested command")))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"this_is_a_super_long_ass_name",request_run)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"reopen",request_reopen)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"set_logstride",request_set_logstride)))
		exit (iretn);

	// Start thread to collect SOH data
	cdthread = thread(collect_data_loop);

	// Reload existing queue
	string infilepath = temp_dir + ".queue";
	ifstream infile(infilepath.c_str());
	if(!infile.is_open())
	{
		cout<<"unable to read file <"<<infilepath<<">"<<endl;
	}
	else
	{

		//file is open for reading commands
		string line;
		command cmd;

		while(getline(infile,line))
		{
			cmd.set_command(line);

			cout<<cmd;

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


	// Start performing the body of the agent
	while(agent_running(cdata))
	{
		cdata->node.utc = currentmjd(0.);
		if (newlogstride != logstride )
		{
			logstride = newlogstride;
			logdate = currentmjd(0.);
			log_move(nodename, "exec");
		}

        // exceeded the time
		if (floor(currentmjd(0.)/logstride)*logstride > logdate)
		{
			logdate = floor(currentmjd(0.)/logstride)*logstride;
			log_move(nodename, "exec");
		}

		cmd_queue.load_commands();
		cmd_queue.run_commands();
		cmd_queue.save_commands();
		COSMOS_USLEEP(100000);
	}

	agent_shutdown_server(cdata);
	cdthread.join();
}

int32_t request_set_logstride(char* request, char* output, void *cdata)
{
	sscanf(request,"set_logstride %lf",&newlogstride);
	return 0;
}

int32_t request_reopen(char* request, char* output, void *cdata)
{
	logdate = ((cosmosstruc *)cdata)->node.loc.utc;
	log_move(((cosmosstruc *)cdata)->node.name, "exec");
	return 0;
}

int32_t request_get_queue_size(char *request, char* response, void *cdata)
{
	sprintf(response,"%" PRIu32 "", cmd_queue.get_size());
	return 0;
}

int32_t request_get_queue_entry(char *request, char* response, void *cdata)
{
	ostringstream ss;

	if(cmd_queue.get_size()==0)
		ss << "the command queue is empty";
	else
	{
		int j;
		int32_t iretn = sscanf(request,"get_queue_entry %d",&j);

		// if valid index then return command
		if (iretn == 1)
			if(j >= 0 && j < (int)cmd_queue.get_size() )
				ss << cmd_queue.get_command(j);
			else
				ss << "<" << j << "> is not a valid command queue index (current range between 0 and " << cmd_queue.get_size()-1 << ")";

		// if no index given, return the entire queue
		else if (iretn ==  -1)
			for(unsigned long int i = 0; i < cmd_queue.get_size(); ++i)
				ss << cmd_queue.get_command(i) << endl;

		// if the user supplied something that couldn't be turned into an integer
		else if (iretn == 0)
			ss << "Usage:\tget_queue_entry [ index ]\t";
	}

	strcpy(response, ss.str().c_str());
	return 0;
}

// Delete Queue Entry - by date and contents
int32_t request_del_queue_entry(char *request, char* response, void *cdata)
{
	command cmd;
	string line(request);

	// remove "del_queue_entry " from request string
	line.erase(0, 16);

	cmd.set_command(line);

	//delete command
	int n = cmd_queue.del_command(cmd);

	sprintf(response,"%d commands deleted from the queue",n);

	return 0;
}

// Add Queue Entry
int32_t request_add_queue_entry(char *request, char* response, void *cdata)
{
	command cmd;
	string line(request);

	// remove "add_queue_entry " from request string
	line.erase(0, 16);

	cmd.set_command(line);

	// add command
	if(cmd.is_command())
		cmd_queue.add_command(cmd);
	
	strcpy(response, line.c_str());
	return 0;
}

int32_t request_run(char *request, char* response, void *cdata)
{
	int i;
	int32_t iretn = 0;
	FILE *pd;
	bool flag;

	// Run Program
	flag = false;

	for (i=0; i<AGENTMAXBUFFER-1; i++)
	{
		if (flag)
		{
			if (request[i] != ' ')
				break;
		}
		else
		{
			if (request[i] == ' ')
				flag = true;
		}
	}

	if (i == AGENTMAXBUFFER-1)
	{
		sprintf(response,"unmatched");
	}
	else
	{
		if ((pd=popen(&request[i],"r")) != NULL)
		{
			iretn = fread(response,1,AGENTMAXBUFFER-1,pd);
			response[iretn] = 0;

			iretn = 0;
			pclose(pd);
		}
		else
		{
			response[0] = 0;
			iretn = 0;
		}
	}

	return (iretn);
}

void collect_data_loop()
{
	int nbytes;
	string message;

	while (agent_running(cdata))
	{
		// Collect new data
		if((nbytes=agent_poll(cdata, message, AGENT_MESSAGE_BEAT, 0)))
		{
			if (json_convert_string(json_extract_namedobject(message.c_str(), "agent_node")) != cdata->node.name)
			{
				continue;
			}
			cdata[1].node = cdata[0].node;
			cdata[1].device = cdata[0].device;
			json_parse(message,&cdata[1]);
			cdata[0].node  = cdata[1].node ;
			cdata[0].device  = cdata[1].device ;
			loc_update(&cdata->node.loc);
			cdata->node.utc = currentmjd(0.);
		}
	}
	return;
}

// Prints the command information stored in local the copy of cdata->event[0].l
void print_command()
{
	jstring js = {0,0,0};
	jstring *jsp = &js;
	json_startout(jsp);
	json_out(jsp,(char*)"event_utc",cdata);
	json_out(jsp,(char*)"event_utcexec",cdata);
	json_out(jsp,(char*)"event_name",cdata);
	json_out(jsp,(char*)"event_type",cdata);
	json_out(jsp,(char*)"event_flag",cdata);
	json_out(jsp,(char*)"event_data",cdata);
	json_out(jsp,(char*)"event_condition",cdata);
	json_stopout(jsp);
	cout<<"<"<<jsp->string<<">"<<endl;

	return;
}



// *************************************************************************
// Class: command
// *************************************************************************

// Copies the current command object to the output stream using JSON format
ostream& operator<<(ostream& out, const command& cmd)
{
	out	<< setprecision(15) <<"{\"event_utc\":"<< cmd.utc
		<< "}{\"event_utcexec\":" << cmd.utcexec
		<< "}{\"event_name\":\"" << cmd.name
		<< "\"}{\"event_type\":" << cmd.type
		<< "}{\"event_flag\":" << cmd.flag
		<< "}{\"event_data\":\"" << cmd.data
		<< "\"}{\"event_condition\":\"" << cmd.condition
		<< "\"}";
	return out;
}

// Equality Operator for command objects
bool operator==(const command& cmd1, const command& cmd2)
{
	return (	cmd1.name==cmd2.name &&
				cmd1.utc==cmd2.utc &&
				cmd1.utcexec==cmd2.utcexec &&
				cmd1.type==cmd2.type &&
				cmd1.flag==cmd2.flag &&
				cmd1.data==cmd2.data &&
				cmd1.condition==cmd2.condition);
}


// Default Constructor for command objects
command::command() : utc(0), utcexec(0), name(""), type(0), flag(0), data(""), condition(""), already_ran(false)
{
}

// Copies the command information stored in the local copy
// cdata->event[0].l into the current command object
void command::set_command(string line)
{
	json_clear_cosmosstruc(JSON_GROUP_EVENT, &cdata[1]);
	json_parse(line, &cdata[1]);
	utc = cdata[1].event[0].l.utc;
	utcexec = cdata[1].event[0].l.utcexec;
	name = cdata[1].event[0].l.name;
	type = cdata[1].event[0].l.type;
	flag = cdata[1].event[0].l.flag;
	data = cdata[1].event[0].l.data;
	condition = cdata[1].event[0].l.condition;
}

string command::get_json()
{
	jstring js = {0,0,0};
	jstring *jsp = &js;
	//    char dtemp[50];
	string json;
	longeventstruc event;

	event.utc = utc;
	event.utcexec = utcexec;
	strcpy(event.name, name.c_str());
	event.type = type;
	event.flag = flag;
	strcpy(event.data, data.c_str());
	strcpy(event.condition, condition.c_str());

	json_startout(jsp);
	json_out_commandevent(jsp, event);
	json_stopout(jsp);
	json = jsp->string;
	return json;
}



// *************************************************************************
// Class: command_queue
// *************************************************************************

// Copies the current command_queue object to the output stream using JSON format
ostream& operator<<(ostream& out, command_queue& cmdq)
{
	for(list<command>::iterator ii = cmdq.commands.begin(); ii != cmdq.commands.end(); ++ii)
		out << *ii << endl;
	return out;
}


// Executes a command using fork().  For each command run, the time of
// execution (utcexec) is set, the flag EVENT_FLAG_ACTUAL is set to true,
// and this updated command information is logged to the OUTPUT directory.
void run_command(command& cmd)
{
	queue_changed = true;

	// set time executed & actual flag
	cmd.set_utcexec();
	cmd.set_actual();

	// execute command
#if defined(COSMOS_WIN_OS)
	char command_line[100];
	strcpy(command_line, cmd.get_data());

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	if (CreateProcess(NULL, command_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
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
		string_parse(cmd.get_data(),words,MAXCOMMANDWORD);
		string outpath = data_type_path(nodename, "temp", "exec", logdate, "out");
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

	// log to outfile
	//	outfile << cmd <<endl;
	//	outfile.close();
	// log to event file
	log_write(nodename, "exec", logdate, "event", cmd.get_json().c_str());
}


// Manages the logic of when to run commands in the command queue.
void command_queue::run_commands()
{
	for(list<command>::iterator ii = commands.begin(); ii != commands.end(); ++ii)
	{
		if (ii->is_ready())
		{
			if (ii->is_conditional())
			{
				if(ii->condition_true())
				{
					if(ii->is_repeat())
					{
						if(!ii->already_ran)	{
							run_command(*ii);
							ii->already_ran = true;
						}
					}
					else // non-repeatable
					{
						run_command(*ii);
						commands.erase(ii--);
					}
				} // condition is false
				else
				{
					ii->already_ran = false;
				}
			}
			else  // non-conditional
			{
				run_command(*ii);
				commands.erase(ii--);
			}
		}
	}
	return;
}

// Saves commands to .queue file located in the temp directory
// Commands are taken from the global command queue
// Command queue is sorted by utc after loading
void command_queue::save_commands()
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
		for (command cmd: commands)
		{
			fprintf(fd, "%s\n", cmd.get_json().c_str());
		}
		fclose(fd);
	}
}

// Loads new commands from *.command files located in the incoming directory
// Commands are loaded into the global command_queue object (cmd_queue),
// *.command files are removed, and the command list is sorted by utc.
void command_queue::load_commands()
{

	// open the incoming directory
	if ((dir = opendir((char *)incoming_dir.c_str())) == NULL)
	{
		cout<<"error: unable to open node's incoming directory <"<<incoming_dir<<"> not found"<<endl;
		agent_shutdown_server(cdata);
		exit(1);
	}

	// cycle through all the file names in the incoming directory
	while((dir_entry = readdir(dir)) != NULL)
	{
		string filename = dir_entry->d_name;

		if (filename.find(".command") != string::npos)
		{

			string infilepath = incoming_dir + filename;
			ifstream infile(infilepath.c_str());
			if(!infile.is_open())
			{
				cout<<"unable to read file <"<<infilepath<<">"<<endl;
				continue;
			}

			//file is open for reading commands
			string line;
			command cmd;

			while(getline(infile,line))
			{
				//clear cdata->event[0].l, parse line into cdata->event[0].l, set command object, and add to command queue
				//				json_parse("{\"event_name\":\"\"}{\"event_utc\":0}{\"event_utcexec\":0}{\"event_flag\":0}{\"event_type\":0}{\"event_data\":\"\"}{\"event_condition\":\"\"}", cdata);
				//				cout<<"<"<<line.c_str()<<">"<<endl;
				//				cout<<"Returned "<<ireturn<<" And the command is: "<<endl;

				cmd.set_command(line);

				cout<<cmd;

				if(cmd.is_command())
					cmd_queue.add_command(cmd);
				else
					cout<<"Not a command!"<<endl;
			}
			infile.close();

			//remove the .command file from incoming directory
			if(remove(infilepath.c_str()))	{
				cout<<"unable to delete file <"<<filename<<">"<<endl;
				continue;
			}

			cout<<"The size of the command queue is: "<<cmd_queue.get_size()<<endl;
		}
	}

	cmd_queue.sort();
	closedir(dir);

	return;
}

// Remove command object from the command queue, uses command == operator)
int command_queue::del_command(command& c)
{
	int n = 0;
	for(list<command>::iterator ii = commands.begin(); ii != commands.end(); ++ii)
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

void command_queue::add_command(command& c)
{
	commands.push_back(c);
	queue_changed = true;
}
