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

int main(int argc, char* argv[])
{
    Agent *myagent;

    if (argc == 2) {
        myagent = new Agent("", argv[1]);
    } else {
        char hostname[60];
        gethostname(hostname, sizeof (hostname));
        myagent = new Agent("", hostname);
    }

    size_t total = 0;
    size_t count = 0;
    cosmosstruc* c = myagent->cinfo;
    c->shrinkusage();

    total += sizeof(c->timestamp);
    total += sizeof(c->jmapped);

    count = sizeof(c->jmap);
    total += sizeof(c->jmap);
    for (size_t i=0; i<c->jmap.size(); ++i)
    {
        for (size_t j=0; j<c->jmap[i].size(); ++j)
        {
            count += c->jmap[i][j].memoryusage();
            total += c->jmap[i][j].memoryusage();
        }
    }
    printf("Jmap:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->emap);
    total += sizeof(c->emap);
    for (size_t i=0; i<c->emap.size(); ++i)
    {
        for (size_t j=0; j<c->emap[i].size(); ++j)
        {
            count += c->emap[i][j].memoryusage();
            total += c->emap[i][j].memoryusage();
        }
    }
    printf("Emap:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->unit);
    total += sizeof(c->unit);
    for (size_t i=0; i<c->unit.size(); ++i)
    {
        for (size_t j=0; j<c->unit[i].size(); ++j)
        {
            count += c->unit[i][j].memoryusage();
            total += c->unit[i][j].memoryusage();
        }
    }
    printf("Units:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->equation);
    total += sizeof(c->equation);
    for (size_t i=0; i<c->equation.size(); ++i)
    {
        count += c->equation[i].memoryusage();
        total += c->equation[i].memoryusage();
    }
    printf("Equations:\t%lu\t%lu\n", count, total);

    count = sizeof(c->alias);
    total += sizeof(c->alias);
    for (size_t i=0; i<c->alias.size(); ++i)
    {
        count += c->alias[i].memoryusage();
        total += c->alias[i].memoryusage();
    }
    printf("Aliases:\t%lu\t%lu\n", count, total);

    count = c->node.memoryusage();
    total += c->node.memoryusage();
    printf("Node:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->node.phys.vertices);
    total += sizeof(c->node.phys.vertices);
    for (size_t i=0; i<c->node.phys.vertices.size(); ++i)
    {
        count += sizeof(c->node.phys.vertices[i]);
        total += sizeof(c->node.phys.vertices[i]);
    }
    printf("Vertexs:\t%lu\t%lu\n", count, total);

    count = sizeof(c->node.phys.faces);
    total += sizeof(c->node.phys.faces);
    for (size_t i=0; i<c->node.phys.faces.size(); ++i)
    {
        count += c->node.phys.faces[i].memoryusage();
        total += c->node.phys.faces[i].memoryusage();
    }
    printf("Faces:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->pieces);
    total += sizeof(c->pieces);
    for (size_t i=0; i<c->pieces.size(); ++i)
    {
        count += c->pieces[i].memoryusage();
        total += c->pieces[i].memoryusage();
    }
    printf("Pieces:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->obj);
    total += sizeof(c->obj);
    printf("Obj:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->device);
    total += sizeof(c->device);
    for (size_t i=0; i<c->device.size(); ++i)
    {
        count += sizeof(c->device[i]);
        total += sizeof(c->device[i]);
    }
    printf("Devices:\t%lu\t%lu\n", count, total);

    count = c->devspec.memoryusage();
    total += c->devspec.memoryusage();
    printf("Device Special:\t%lu\t%lu\n", count, total);

    count = sizeof(c->port);
    total += sizeof(c->port);
    for (size_t i=0; i<c->port.size(); ++i)
    {
        count += c->port[i].memoryusage();
        total += c->port[i].memoryusage();
    }
    printf("Ports:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->agent0);
    total += sizeof(c->agent0);
    printf("Agents:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->sim_states);
    total += sizeof(c->sim_states);
    for (size_t i=0; i<c->sim_states.size(); ++i)
    {
        count += sizeof(c->sim_states[i]);
        total += sizeof(c->sim_states[i]);
    }
    printf("Sim States:\t%lu\t%lu\n", count, total);

    count = sizeof(c->event);
    total += sizeof(c->event);
    for (size_t i=0; i<c->event.size(); ++i)
    {
        count += c->event[i].memoryusage();
        total += c->event[i].memoryusage();
    }
    printf("Events:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->target);
    total += sizeof(c->target);
    for (size_t i=0; i<c->target.size(); ++i)
    {
        count += c->target[i].memoryusage();
        total += c->target[i].memoryusage();
    }
    printf("Targets:\t%lu\t%lu\n", count, total);

    count = sizeof(c->user);
    total += sizeof(c->user);
    for (size_t i=0; i<c->user.size(); ++i)
    {
        count += c->user[i].memoryusage();
        total += c->user[i].memoryusage();
    }
    printf("Users:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->tle);
    total += sizeof(c->tle);
    for (size_t i=0; i<c->tle.size(); ++i)
    {
        count += sizeof(c->tle[i]);
        total += sizeof(c->tle[i]);
    }
    printf("TLEs:\t\t%lu\t%lu\n", count, total);


    count = sizeof(c->json);
    total += sizeof(c->json);
    printf("JSON:\t\t%lu\t%lu\n", count, total);

    count = sizeof(c->names) + sizeof(c->types);
    total += sizeof(c->names) + sizeof(c->types);
    for (auto &name : c->names)
    {
        count += sizeof(name) + name.first.capacity() + sizeof(name.second);
        total += sizeof(name) + name.first.capacity() + sizeof(name.second);
    }
    for (auto &type : c->types)
    {
        count += sizeof(type) + type.first.capacity() + type.second.capacity();
        total += sizeof(type) + type.first.capacity() + type.second.capacity();
    }
    printf("Names 2.0:\t%lu\t%lu\n", count, total);

    fflush(stdout);
}
