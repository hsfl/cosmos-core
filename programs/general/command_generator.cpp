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
#include "support/event.h"
#include "agent/scheduler.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    longeventstruc ev;

    ev.type = EVENT_TYPE_COMMAND;
    ev.flag = 0;
    ev.data[0] = 0;
    ev.condition[0] = 0;
    ev.utc = 0;
    ev.utcexec = 0.;

    string node = "";

    switch (argc)
    {
    case 7: // set repeat flag
    {
        ev.flag |= EVENT_FLAG_REPEAT;
    }
    case 6: // set conditions
    {
        strcpy(ev.condition, argv[5]);
        ev.flag |= EVENT_FLAG_CONDITIONAL;
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
            ev.utc = currentmjd() + seconds / 86400.;
            break;
        }
        default:
            // use set time
        {
            ev.utc = atof(argv[3]);
            break;
        }
        }
    }
    case 3: // set command string
    {
        //std::string command_data = argv[2];
        strcpy(ev.data, argv[2]);
    }
    case 2: // set command name
    {
        //std::string command_name = argv[1];
        strcpy(ev.name, argv[1]);
        break;
    }
    default:
    {
        cout << "The command generator produces the command string to be fed into the command\n"
                "queue which is managed by the agent_exec_soh. Commands are a subset of events\n"
                "in COSMOS." << endl << endl;

        cout << "Usage" << endl << endl;
        cout << "  command_generator [options]" << endl;
        cout << "  command_generator name command [time | +sec] [node] [condition] [repeat_flag]" << endl << endl;


        cout << "Example" << endl << endl;
        cout << "  $ command_generator myCmd1 \"agent kauaicc_sim execsoh get_queue_size\" +10" << endl << endl;
        cout << "  This will run the command \"agent kauaicc_sim execsoh get_queue_size\" \n"
                "  with name 'myCmd' within 10 seconds from now." << endl << endl;

        cout << "Options" << endl << endl;

        cout << "  -name   \t = name of the command, can be a string or combination of \n "
                "          \t   alphanumeric characters (ex: myCmd1)" << endl;
        cout << "  -command\t = the actual command to be executed, if more than one word \n"
                "          \t   enclose the command string in quotes \n"
                "          \t   (ex: \"agent kauaicc_sim execsoh get_queue_size\"" << endl;
        cout << "  -time   \t = optional argument to enter the desired modified julian date\n"
                "          \t   (mjd). If not entered it will use the current time. \n"
                "          \t   If '+' is used instead then the number of seconds can be\n"
                "          \t   inserted. For 10 seconds in the future use +10" << endl;
        cout << "  -node   \t = optional argument to add the generated command to the command\n"
                "          \t   queue on the specified node (ex: kauaicc_sim) " << endl;
        return 0;
    }
    }

    Event event;
    cout << "Command string:" << endl;
    cout << event.generator(ev) << endl << endl;

    if (!node.empty()) {
        cout << "Adding command/event to node " << node << endl;
        Scheduler scheduler(node);

        // Examples on how to use the scheduler class:
        //        event.name = "Track;FS701;20160930.145000;20160930.145500";
        //        event.data = "agent_tracker FS701 20160930.145000 20160930.145500";
        //        event.mjd  = cal2mjd(2016, 9, 30, 14, 50, 0, 0);
        //        DateTime time( 2016, 10, 12, 14, 50, 1 );
        //        event.mjd = time.mjd;

        //        cout << event.getName() << endl;
        //        cout << event.getData() << endl;
        //        cout << event.getTime() << endl;

        scheduler.addEvent(event);
        COSMOS_SLEEP(0.1);
        //        scheduler.deleteEvent(event);
        //        scheduler.getEventQueueSize();
        scheduler.getEventQueue();
    }
}
