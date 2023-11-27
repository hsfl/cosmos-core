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
#include "support/timelib.h"
#include "agent/agentclass.h"
#include "thirdparty/zlib/zlib.h"
#include <stdio.h>

Agent *agent;

int main(int argc, char* argv[])
{
    string node = "";
    string subagent = "exec";
    string location = "incoming";

    switch (argc)
    {
    case 4:
        node = argv[3];
    case 3:
        subagent = argv[2];
        location = argv[1];
        break;
    default:
        printf("Usage: latest_file location subagent [node]\n");
        exit (1);
    }

    if (!(agent = new Agent("", node)))
    {
        printf("Couldn't establish client for node %s\n", node.c_str());
        exit (-1);
    }

    string name = data_list_latest_file(agent->cinfo->node.name, location, subagent);
    printf("%s\n", name.c_str());
}
