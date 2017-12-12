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

int main(int, char* argv[])
{
    Agent *myagent;

    myagent = new Agent();
    json_setup_node(argv[1], myagent->cinfo);

    size_t total = 0;
    size_t count = 0;

    total += COSMOS_SIZEOF(nodestruc);
    printf("Node: %lu : %lu\n", COSMOS_SIZEOF(nodestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pieces.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(piecestruc);
    }
    printf("Pieces: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(piecestruc), count*COSMOS_SIZEOF(piecestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->device.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(devicestruc);
    }
    printf("Devices: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(devicestruc), count*COSMOS_SIZEOF(devicestruc), total);

    total += COSMOS_SIZEOF(devspecstruc);
    printf("Device Special: %ld : %lu\n", COSMOS_SIZEOF(devspecstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->port.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(portstruc);
    }
    printf("Ports: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(portstruc), count*COSMOS_SIZEOF(portstruc), total);

    total += COSMOS_SIZEOF(physicsstruc);
    printf("Physics: %ld : %lu\n", COSMOS_SIZEOF(physicsstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->agent.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(agentstruc);
    }
    printf("Agents: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(agentstruc), count*COSMOS_SIZEOF(agentstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->target.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(targetstruc);
    }
    printf("Targets: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(targetstruc), count*COSMOS_SIZEOF(targetstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->user.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(userstruc);
    }
    printf("Users: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(userstruc), count*COSMOS_SIZEOF(userstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->glossary.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(glossarystruc);
    }
    printf("Glossarys: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(glossarystruc), count*COSMOS_SIZEOF(glossarystruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->tle.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(tlestruc);
    }
    printf("TLEs: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(tlestruc), count*COSMOS_SIZEOF(tlestruc), total);

    printf("Total for pdata: %lu\n\n", total);

    total = 0;
    count = 0;

    total += COSMOS_SIZEOF(nodestruc);
    printf("Node: %ld : %lu\n", COSMOS_SIZEOF(nodestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pieces.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(piecestruc);
    }
    printf("Pieces: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(piecestruc), count*COSMOS_SIZEOF(piecestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->device.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(devicestruc);
    }
    printf("Devices: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(devicestruc), count*COSMOS_SIZEOF(devicestruc), total);

    total += COSMOS_SIZEOF(devspecstruc);
    printf("Device Special: %ld : %lu\n", COSMOS_SIZEOF(devspecstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->port.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(portstruc);
    }
    printf("Ports: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(portstruc), count*COSMOS_SIZEOF(portstruc), total);

    total += COSMOS_SIZEOF(physicsstruc);
    printf("Physics: %ld : %lu\n", COSMOS_SIZEOF(physicsstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->agent.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(agentstruc);
    }
    printf("Agents: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(agentstruc), count*COSMOS_SIZEOF(agentstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->target.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(targetstruc);
    }
    printf("Targets: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(targetstruc), count*COSMOS_SIZEOF(targetstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->user.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(userstruc);
    }
    printf("Users: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(userstruc), count*COSMOS_SIZEOF(userstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->glossary.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(glossarystruc);
    }
    printf("Glossarys: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(glossarystruc), count*COSMOS_SIZEOF(glossarystruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->tle.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(tlestruc);
    }
    printf("TLEs: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(tlestruc), count*COSMOS_SIZEOF(tlestruc), total);

    printf("Total for sdata: %lu\n\n", total);

    total = 0;
    count = 0;
    for (size_t i=0; i<myagent->cinfo->jmap.size(); ++i)
    {
        for (size_t j=0; j<myagent->cinfo->jmap[j].size(); ++j)
        {
            ++count;
            total += COSMOS_SIZEOF(jsonentry);
        }
    }
    printf("Jmap: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(jsonentry), count*COSMOS_SIZEOF(jsonentry), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->emap.size(); ++i)
    {
        for (size_t j=0; j<myagent->cinfo->emap[j].size(); ++j)
        {
            ++count;
            total += COSMOS_SIZEOF(jsonequation);
        }
    }
    printf("Emap: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(jsonequation), count*COSMOS_SIZEOF(jsonequation), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->unit.size(); ++i)
    {
        for (size_t j=0; j<myagent->cinfo->unit[j].size(); ++j)
        {
            ++count;
            total += COSMOS_SIZEOF(jsonequation);
        }
    }
    printf("Units: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(unitstruc), count*COSMOS_SIZEOF(unitstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->alias.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(aliasstruc);
    }
    printf("Aliass: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(aliasstruc), count*COSMOS_SIZEOF(aliasstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->equation.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(equationstruc);
    }
    printf("Equations: %lu x %ld = %lu : %lu\n", count, COSMOS_SIZEOF(equationstruc), count*COSMOS_SIZEOF(equationstruc), total);

    printf("Total for meta: %lu\n\n", total);

    fflush(stdout);
}
