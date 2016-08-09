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

#include "configCosmos.h"
#include "agent/agent.h"

int main(int, char* argv[])
{
    cosmosAgent *myagent;

    myagent = new cosmosAgent();
    json_setup_node(argv[1], myagent->cinfo, false);

    size_t total = 0;
    size_t count = 0;

    total += COSMOS_SIZEOF(nodestruc);
    printf("Node: %u : %u\n", COSMOS_SIZEOF(nodestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.piece.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(piecestruc);
    }
    printf("Pieces: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(piecestruc), count*COSMOS_SIZEOF(piecestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.device.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(devicestruc);
    }
    printf("Devices: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(devicestruc), count*COSMOS_SIZEOF(devicestruc), total);

    total += COSMOS_SIZEOF(devspecstruc);
    printf("Device Special: %u : %u\n", COSMOS_SIZEOF(devspecstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.port.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(portstruc);
    }
    printf("Ports: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(portstruc), count*COSMOS_SIZEOF(portstruc), total);

    total += COSMOS_SIZEOF(physicsstruc);
    printf("Physics: %u : %u\n", COSMOS_SIZEOF(physicsstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.agent.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(agentstruc);
    }
    printf("Agents: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(agentstruc), count*COSMOS_SIZEOF(agentstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.target.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(targetstruc);
    }
    printf("Targets: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(targetstruc), count*COSMOS_SIZEOF(targetstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.user.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(userstruc);
    }
    printf("Users: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(userstruc), count*COSMOS_SIZEOF(userstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.glossary.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(glossarystruc);
    }
    printf("Glossarys: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(glossarystruc), count*COSMOS_SIZEOF(glossarystruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->pdata.tle.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(tlestruc);
    }
    printf("TLEs: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(tlestruc), count*COSMOS_SIZEOF(tlestruc), total);

    printf("Total for pdata: %u\n\n", total);

    total = 0;
    count = 0;

    total += COSMOS_SIZEOF(nodestruc);
    printf("Node: %u : %u\n", COSMOS_SIZEOF(nodestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.piece.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(piecestruc);
    }
    printf("Pieces: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(piecestruc), count*COSMOS_SIZEOF(piecestruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.device.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(devicestruc);
    }
    printf("Devices: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(devicestruc), count*COSMOS_SIZEOF(devicestruc), total);

    total += COSMOS_SIZEOF(devspecstruc);
    printf("Device Special: %u : %u\n", COSMOS_SIZEOF(devspecstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.port.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(portstruc);
    }
    printf("Ports: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(portstruc), count*COSMOS_SIZEOF(portstruc), total);

    total += COSMOS_SIZEOF(physicsstruc);
    printf("Physics: %u : %u\n", COSMOS_SIZEOF(physicsstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.agent.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(agentstruc);
    }
    printf("Agents: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(agentstruc), count*COSMOS_SIZEOF(agentstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.target.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(targetstruc);
    }
    printf("Targets: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(targetstruc), count*COSMOS_SIZEOF(targetstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.user.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(userstruc);
    }
    printf("Users: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(userstruc), count*COSMOS_SIZEOF(userstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.glossary.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(glossarystruc);
    }
    printf("Glossarys: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(glossarystruc), count*COSMOS_SIZEOF(glossarystruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->sdata.tle.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(tlestruc);
    }
    printf("TLEs: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(tlestruc), count*COSMOS_SIZEOF(tlestruc), total);

    printf("Total for sdata: %u\n\n", total);

    total = 0;
    count = 0;
    for (size_t i=0; i<myagent->cinfo->meta.jmap.size(); ++i)
    {
        for (size_t j=0; j<myagent->cinfo->meta.jmap[j].size(); ++j)
        {
            ++count;
            total += COSMOS_SIZEOF(jsonentry);
        }
    }
    printf("Jmap: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(jsonentry), count*COSMOS_SIZEOF(jsonentry), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->meta.emap.size(); ++i)
    {
        for (size_t j=0; j<myagent->cinfo->meta.emap[j].size(); ++j)
        {
            ++count;
            total += COSMOS_SIZEOF(jsonequation);
        }
    }
    printf("Emap: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(jsonequation), count*COSMOS_SIZEOF(jsonequation), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->meta.unit.size(); ++i)
    {
        for (size_t j=0; j<myagent->cinfo->meta.unit[j].size(); ++j)
        {
            ++count;
            total += COSMOS_SIZEOF(jsonequation);
        }
    }
    printf("Units: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(unitstruc), count*COSMOS_SIZEOF(unitstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->meta.alias.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(aliasstruc);
    }
    printf("Aliass: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(aliasstruc), count*COSMOS_SIZEOF(aliasstruc), total);

    count = 0;
    for (size_t i=0; i<myagent->cinfo->meta.equation.size(); ++i)
    {
        ++count;
        total += COSMOS_SIZEOF(equationstruc);
    }
    printf("Equations: %u x %u = %u : %u\n", count, COSMOS_SIZEOF(equationstruc), count*COSMOS_SIZEOF(equationstruc), total);

    printf("Total for meta: %u\n\n", total);

    fflush(stdout);
}
