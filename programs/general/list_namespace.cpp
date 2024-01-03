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
#include "support/jsondef.h"
#include "agent/agentclass.h"

Agent* myagent;
struct info
{
    uint16_t pidx = 65535;
    uint16_t cidx = 65535;
    uint16_t didx = 65535;
    string initial;
    string remainder;
    string name;
};

map<string, info> user;
map<uint16_t, map<string, info>> pieces;
map<string, info> physics;
map<string, info> node;
map<string, map<string, info>> nodeloc;
map<string, info> event;
map<string, map<uint16_t, map<string, info>>> devices;
map<string, info> agent;

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        myagent = new Agent("", argv[1]);
    }
    else
    {
        myagent = new Agent();
    }

    for (auto entry : myagent->cinfo->ujmap)
    {
        vector<string> parts = string_split(entry.first, "_");
        info tinfo;
        tinfo.name = entry.first;
        tinfo.initial = parts[1];

        if (parts[0] == "user")
        {
            user[tinfo.initial] = tinfo;
        }
        else if (parts[0] == "piece")
        {
            if (parts.size() > 3)
            {
                tinfo.pidx = atoi(parts.back().c_str());
                tinfo.cidx = myagent->cinfo->pieces[tinfo.pidx].cidx;
            }
            pieces[tinfo.pidx][tinfo.initial] = tinfo;
        }
        else if (parts[0] == "physics")
        {
            physics[tinfo.initial] = tinfo;
        }
        else if (parts[0] == "node")
        {
            if (parts[1] == "loc")
            {
                if (parts.size() > 2 && (parts[2] == "est" || parts[2] == "std"))
                {
                    tinfo.initial += "_" + parts[2];
                    if (parts.size() > 3 && (parts[3] == "pos" || parts[3] == "att"))
                    {
                        tinfo.initial += "_" + parts[3];
                        for (uint16_t i=4; i<parts.size(); ++i)
                        {
                            tinfo.remainder += "." + parts[i];
                        }
                    }
                    else
                    {
                        for (uint16_t i=3; i<parts.size(); ++i)
                        {
                            tinfo.remainder += "." + parts[i];
                        }
                    }
                }
                else
                {
                    if (parts.size() > 2 && (parts[2] == "pos" || parts[2] == "att"))
                    {
                        tinfo.initial += "_" + parts[2];
                        for (uint16_t i=3; i<parts.size(); ++i)
                        {
                            tinfo.remainder += "." + parts[i];
                        }
                    }
                    else
                    {
                        for (uint16_t i=2; i<parts.size(); ++i)
                        {
                            tinfo.remainder += "." + parts[i];
                        }
                    }
                }
                nodeloc[tinfo.initial][tinfo.remainder] = tinfo;
            }
            else
            {
                node[tinfo.initial] = tinfo;
            }
        }
        else if (parts[0] == "event")
        {
            event[tinfo.initial] = tinfo;
        }
        else if (parts[0] == "device")
        {
            tinfo.name = entry.first;
            if (parts.size() == 3)
            {
                devices[tinfo.initial][tinfo.cidx][parts[2]] = tinfo;
            }
            else if (parts.size() > 3)
            {
                if (parts[1] == "all")
                {
                    tinfo.cidx = atoi(parts.back().c_str());
                    tinfo.didx = myagent->cinfo->device[tinfo.cidx]->didx;
                    tinfo.pidx = myagent->cinfo->device[tinfo.cidx]->pidx;
                }
                else
                {
                    if (parts[2] == "npoly" || parts[2] == "ppoly")
                    {
                        tinfo.didx = atoi(parts[3].c_str());
                        tinfo.cidx = device_component_index(myagent->cinfo, parts[1], tinfo.didx);
                        tinfo.pidx = myagent->cinfo->device[tinfo.cidx]->pidx;
                    }
                    else
                    {
                        tinfo.didx = atoi(parts.back().c_str());
                        tinfo.cidx = device_component_index(myagent->cinfo, parts[1], tinfo.didx);
                        tinfo.pidx = myagent->cinfo->device[tinfo.cidx]->pidx;
                    }
                }
                for (uint16_t i=2; i<parts.size()-1; ++i)
                {
                    tinfo.remainder += "." + parts[i];
                }
                devices[tinfo.initial][tinfo.didx][tinfo.remainder] = tinfo;
            }
        }
        else if (parts[0] == "agent")
        {
            agent[tinfo.initial] = tinfo;
        }
    }
    printf("Group\tInitial\tRemainder\tName\tPieceIdx\tCompIdx\tDeviceIdx\tValue\tType\n");

    for (auto info : user)
    {
        jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
        string value = json_get_string(*entry, myagent->cinfo);
        printf("user\t%s\t%s\t%s\t\t\t\t%s\t%u\n", info.first.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), value.c_str(), entry->type);
    }
    for (auto index : pieces)
    {
        for (auto info : index.second)
        {
            jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
            string value = json_get_string(*entry, myagent->cinfo);
            printf("piece\t%s\t%s\t%s\t%03u\t%03u\t%03u\t%s\t%u\n", info.first.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), info.second.pidx, info.second.cidx, info.second.didx, value.c_str(), entry->type);
        }
    }
    for (auto info : physics)
    {
        jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
        string value = json_get_string(*entry, myagent->cinfo);
        printf("physics\t%s\t%s\t%s\t\t\t\t%s\t%u\n", info.first.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), value.c_str(), entry->type);
    }
    for (auto info : node)
    {
        jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
        string value = json_get_string(*entry, myagent->cinfo);
        printf("node\t%s\t%s\t%s\t\t\t\t%s\t%u\n", info.first.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), value.c_str(), entry->type);
    }
    for (auto first : nodeloc)
    {
        for (auto info : first.second)
        {
            jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
            string value = json_get_string(*entry, myagent->cinfo);
            printf("nodeloc\t%s\t%s\t%s\t\t\t\t%s\t%u\n", info.second.initial.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), value.c_str(), entry->type);
        }
    }
    for (auto info : event)
    {
        jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
        string value = json_get_string(*entry, myagent->cinfo);
        printf("event\t%s\t%s\t%s\t\t\t\t%s\t%u\n", info.first.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), value.c_str(), entry->type);
    }
    for (auto type : devices)
    {
        for (auto index : type.second)
        {
            for (auto info : index.second)
            {
                jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
                string value = json_get_string(*entry, myagent->cinfo);
                printf("device\t%s\t%s\t%s\t%03u\t%03u\t%03u\t%s\t%u\n", info.second.initial.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), info.second.pidx, info.second.cidx, info.second.didx, value.c_str(), entry->type);
            }
        }
    }
    for (auto info : agent)
    {
        jsonentry* entry = json_entry_of(info.second.name, myagent->cinfo);
        string value = json_get_string(*entry, myagent->cinfo);
        printf("agent\t%s\t%s\t%s\t\t\t\t%s\t%u\n", info.first.c_str(), info.second.remainder.c_str(), info.second.name.c_str(), value.c_str(), entry->type);
    }
}
