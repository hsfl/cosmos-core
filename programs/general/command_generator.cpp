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
#include "support/datalib.h"
#include "support/jsonlib.h"
#include "support/timelib.h"
#include "support/command.h"
#include "agent/scheduler.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    longeventstruc com;

    com.type = EVENT_TYPE_COMMAND;
    com.flag = 0;
    com.data[0] = 0;
    com.condition[0] = 0;
    com.utc = 0;
    com.utcexec = 0.;

    string node = "";

    switch (argc)
    {
    case 7: // set repeat flag
    {
        com.flag |= EVENT_FLAG_REPEAT;
    }
    case 6: // set conditions
    {
        strcpy(com.condition, argv[5]);
        com.flag |= EVENT_FLAG_CONDITIONAL;
    }
    case 5: // add command to the scheduler
    {
        node = string(argv[4]);
    }
    case 4: // set time utc in mjd
    {
        switch (argv[3][0])
        {
        // add a few seconds to current time
        case '+':
        {
            double seconds = atof(&argv[3][1]);
            com.utc = currentmjd() + seconds / 86400.;
            break;
        }
        default:
            // use set time
        {
            com.utc = atof(argv[3]);
            break;
        }
        }
    }
    case 3: // set command string
    {
        //std::string command_data = argv[2];
        strcpy(com.data, argv[2]);
    }
    case 2: // set command name
    {
        //std::string command_name = argv[1];
        strcpy(com.name, argv[1]);
        break;
    }
    default:
    {
        cout << "The command generator produces the command string to be fed into the command queue\n"
                "which is managed by the agent_exec_soh. Commands are a subset of events in COSMOS." << endl << endl;

        cout << "Usage" << endl << endl;
        cout << "  command_generator [options]" << endl;
        cout << "  command_generator name command [time | +sec] [node] [condition] [repeat_flag]" << endl << endl;


        cout << "Example" << endl << endl;
        cout << "  $ command_generator myCmd1 \"agent kauaicc_sim execsoh get_queue_size\" +10" << endl;
        cout << "   (this will run the command \"agent kauaicc_sim execsoh get_queue_size\" \n"
                "    with name 'myCmd' within 10 seconds from now)" << endl << endl;

        cout << "Options" << endl << endl;

        cout << "  -name   \t = name of the command, can be a string or combination of \n "
                "          \t   alphanumeric characters (ex: myCmd1)" << endl;
        cout << "  -command\t = the actual command to be executed, if more than one word \n"
                "          \t   enclose the command string in quotes \n"
                "          \t   (ex: \"agent kauaicc_sim execsoh get_queue_size\"" << endl;
        cout << "  -time   \t = optional argument to enter the desired modified julian date (mjd)\n"
                "          \t   if not entered it will use the current time. \n"
                "          \t   If '+' is used instead then the number of seconds can be inserted.\n"
                "          \t   For 10 seconds in the future use +10" << endl;
        cout << "  -node   \t = optional argument to add the generated command to the command queue\n "
                "          \t   on the specified node (ex: kauaicc_sim) " << endl;
        return 0;
    }
    }

    Command command;
    cout << "Command string:" << endl;
    cout << command.generator(com) << endl << endl;

    if (!node.empty()) {
        cout << "Adding command to node " << node << endl;
        Scheduler scheduler(node);

        // Examples on how to use the scheduler class:
        scheduler.addCommand(com);
        sleep(1);
        //scheduler.deleteCommand(com.name, com.data, com.utc, com.condition, com.flag);
        //scheduler.getQueueSize();
        scheduler.getQueueList();
    }
}
