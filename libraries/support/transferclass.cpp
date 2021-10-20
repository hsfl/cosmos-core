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
#include "support/transferclass.h"
#include "support/timelib.h"
#include "support/stringlib.h"

namespace Cosmos {
    namespace Support {
        Transfer::Transfer()
        {
            calstruc cal = mjd2cal(currentmjd());
            txid = 100 * (utc2unixseconds(currentmjd()) - utc2unixseconds(cal2mjd(cal.year, 1.))) - 1;
        }

        int32_t Transfer::Init(string node, string agent, uint16_t chunk_size)
        {
            this->node = node;
            this->agent = agent;
            this->chunk_size = chunk_size;
            return 0;
        }


        int32_t Transfer::Load(string filename, vector<chunk> &chunks)
        {
            int32_t iretn;
            FILE *fp = fopen(filename.c_str(), "r");
            if (filename.find(".xfr") != string::npos)
            {
                json.resize(data_size(filename));
                fgets((char *)json.data(), json.size(), fp);
                string estring;
                json11::Json jmeta = json11::Json::parse(json.data(), estring);
                name = jmeta["name"].string_value();
                size = jmeta["size"].number_value();
                node = jmeta["node"].string_value();
                agent = jmeta["agent"].string_value();
                txid = jmeta["txid"].number_value();
                chunk_size = jmeta["chunksize"].number_value();
            }
            else
            {
                ++txid;
                name = filename;
                size = data_size(name);
                if (!size)
                {
                    return (GENERAL_ERROR_BAD_SIZE);
                }
                if (!chunks.size())
                {
                    chunk tchunk;
                    tchunk.start = 0;
                    tchunk.end = ((size - 1) / chunk_size);
                    chunks.push_back(tchunk);
                }
                json = "{";
                json += to_json("txid", txid);
                json += "," + to_json("name", name);
                json += "," + to_json("size", size);
                json += "," + to_json("node", node);
                json += "," + to_json("agent", agent);
                json += "," + to_json("chunksize", chunk_size);
                json += "}";
            }
            meta = vector<uint8_t>(json.begin(), json.end());
            data.resize(1+((size-1) / chunk_size));
            for (chunk tchunk : chunks)
            {
                for (uint32_t chunkidx=tchunk.start; chunkidx <= tchunk.end; ++chunkidx)
                {
                    data[chunkidx].resize(chunk_size);
                    if (fseek(fp, chunkidx*chunk_size, SEEK_SET) == 0 && (iretn=fread(data[chunkidx].data(), 1, chunk_size, fp)) > 0)
                    {
                        if ((size_t)iretn < chunk_size)
                        {
//                            data[chunkidx].resize(iretn);
                            break;
                        }
                    }
                }
            }
            fclose(fp);
            return size;
        }
    }
}
