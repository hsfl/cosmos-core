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
#include "datalib.h"
#include "timelib.h"
#include "agentlib.h"
#include "zlib/zlib.h"
#include <stdio.h>

cosmosstruc* cdata;

int main(int argc, char* argv[])
{
    std::string node = "hiakasat";
    std::string agent = "soh";
    std::string source = "incoming";

    switch (argc)
    {
    case 4:
        source = argv[3];
    case 3:
        agent = argv[2];
    case 2:
        node = argv[1];
        break;
    default:
        printf("Usage: archive node [agent] [\"incoming\"|\"outgoing\"]\n");
        exit (1);
    }

    if (!(cdata=agent_setup_client(NetworkType::UDP, node.c_str(), 1000)))
    {
        printf("Couldn't establish client for node %s\n", node.c_str());
        exit (-1);
    }

    char buffer[8192];
    std::vector<filestruc> srcfiles;
    data_list_files(cdata[0].node.name, source.c_str(), agent.c_str(), srcfiles);

    for (filestruc srcfile: srcfiles)
    {
        if (srcfile.type != "directory")
        {
            FILE* fin;
            if ((fin = data_open(srcfile.path, (char *)"r")) != NULL)
            {
                gzFile gzin = gzdopen(fileno(fin), "r");
                uint32_t year, jday, isecond;
                sscanf(srcfile.name.c_str(), "%*[A-Z,a-z,0-9]_%4u%3u%5u", &year, &jday, &isecond);
                double utc = cal2mjd(year, 1, isecond/86400.) + jday;

                // Check for gzip
                uint16_t namelen = srcfile.name.size();
                if (namelen >= 3 && srcfile.name.substr(namelen-3, 3) == ".gz")
                {
                    srcfile.name = srcfile.name.substr(0, namelen-3);
                }

                std::string newpath = data_name_path(node, "data", agent, utc, srcfile.name);
                if (!newpath.empty() && !data_exists(newpath))
                {
                    FILE* fout;
                    if ((fout = data_open(newpath, (char *)"w")) != NULL)
                    {
                        size_t tbytes = 0;
                        int nbytes;
                        do
                        {
                            nbytes = gzread(gzin, buffer, 8192);
                            if (nbytes > 0)
                            {
                                tbytes += nbytes;
                                fwrite(buffer, 1, nbytes, fout);
                            }
                        } while (nbytes > 0 && !gzeof(gzin));
                        fclose(fout);
                        gzclose_r(gzin);

                        if (tbytes)
                        {
                            printf("Success: %s: %" PRIu32 "\n", newpath.c_str(), tbytes);
                        }
                        else
                        {
                            remove(newpath.c_str());
                            printf("Failure: %s\n", newpath.c_str());
                        }
                    }
                }
            }
        }
    }
}
