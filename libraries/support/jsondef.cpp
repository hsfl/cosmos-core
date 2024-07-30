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

/*! \file jsonlib.cpp
    \brief JSON support source file
*/

#include "support/cosmos-errno.h"
#include "support/jsondef.h"

namespace Cosmos
{
    namespace Support
    {
        cosmosstruc::cosmosstruc ()
        {
            
        }
        #ifndef INFLIGHT_BUILD

        void cosmosstruc::add_default_names()
        {
            // the whole she-bang
            add_name("cinfo", this, "cosmosstruc");

            // double timestamp
            add_name("timestamp", &timestamp, "double");

            // uint16_t jmapped
            add_name("jmapped", &jmapped, "uint16_t");
            add_name("ujmapped", &ujmapped, "uint16_t");

            // vector<vector<unitstruc>> unit
            add_name("unit", &unit, "vector<vector<unitstruc>>");
            for(size_t i = 0; i < unit.size(); ++i) {
                string basename = "unit[" + std::to_string(i) + "]";
                add_name(basename, &unit[i], "vector<unitstruc>");
                for(size_t j = 0; j < unit[i].size(); ++j) {
                    string rebasename = basename + "[" + std::to_string(j) + "]";
                    add_name(rebasename, &unit[i][j], "unitstruc");
                    add_name(rebasename+".name", &unit[i][j].name, "string");
                    add_name(rebasename+".type", &unit[i][j].type, "uint16_t");
                    add_name(rebasename+".p0", &unit[i][j].p0, "float");
                    add_name(rebasename+".p1", &unit[i][j].p1, "float");
                    add_name(rebasename+".p2", &unit[i][j].p2, "float");
                }
            }


            // vector<equationstruc> equation
            add_name("equation", &equation, "vector<equationstruc>");
            for(size_t i = 0; i < equation.size(); ++i) {
                string basename = "equation[" + std::to_string(i) + "]";
                add_name(basename, &equation[i], "equationstruc");
                add_name(basename+".name", &equation[i].name, "string");
                add_name(basename+".value", &equation[i].value, "string");
            }

            // nodestruc node
            add_name("node", &node, "nodestruc");
            add_name("node.name", &node.name, "string");
//            add_name("agent0.name", &agent0.name, "string");
            add_name("node.lastevent", &node.lastevent, "string");
            add_name("node.lasteventutc", &node.lasteventutc, "double");
            add_name("node.type", &node.type, "uint16_t");
            add_name("node.state", &node.state, "uint16_t");
            add_name("vertex_cnt", &vertex_cnt, "uint16_t");
            add_name("normal_cnt", &normal_cnt, "uint16_t");
            add_name("face_cnt", &face_cnt, "uint16_t");
            add_name("piece_cnt", &piece_cnt, "uint16_t");
            add_name("device_cnt", &device_cnt, "uint16_t");
            add_name("port_cnt", &port_cnt, "uint16_t");
//            add_name("agent_cnt", &agent_cnt, "uint16_t");
            add_name("event_cnt", &event_cnt, "uint16_t");
            add_name("target_cnt", &target_cnt, "uint16_t");
            add_name("user_cnt", &user_cnt, "uint16_t");
            add_name("tle_cnt", &tle_cnt, "uint16_t");
            add_name("node.flags", &node.flags, "uint16_t");
            add_name("node.powmode", &node.powmode, "int16_t");
            add_name("node.downtime", &node.downtime, "uint32_t");
            add_name("node.azfrom", &node.azfrom, "float");
            add_name("node.elfrom", &node.elfrom, "float");
            add_name("node.azto", &node.azto, "float");
            add_name("node.elto", &node.elto, "float");
            add_name("node.range", &node.range, "float");
            add_name("node.utcoffset", &node.utcoffset, "double");
            add_name("node.utc", &node.utc, "double");
            add_name("node.utcstart", &node.utcstart, "double");
            add_name("node.deci", &node.deci, "uint32_t");
            add_name("node.loc", &node.loc, "locstruc");
            add_name("node.loc.utc", &node.loc.utc, "double");
            add_name("node.loc.pos", &node.loc.pos, "posstruc");
            add_name("node.loc.pos.utc", &node.loc.pos.utc, "double");
            add_name("node.loc.pos.icrf", &node.loc.pos.icrf, "cartpos");
            add_name("node.loc.pos.icrf.utc", &node.loc.pos.icrf.utc, "double");
            add_name("node.loc.pos.icrf.s", &node.loc.pos.icrf.s, "rvector");
            add_name("node.loc.pos.icrf.s.col", &node.loc.pos.icrf.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.icrf.s.col)/sizeof(node.loc.pos.icrf.s.col[0]); ++i) {
                string basename = "node.loc.pos.icrf.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.icrf.s.col[i], "double");
            }
            add_name("node.loc.pos.icrf.v", &node.loc.pos.icrf.v, "rvector");
            add_name("node.loc.pos.icrf.v.col", &node.loc.pos.icrf.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.icrf.v.col)/sizeof(node.loc.pos.icrf.v.col[0]); ++i) {
                string basename = "node.loc.pos.icrf.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.icrf.v.col[i], "double");
            }
            add_name("node.loc.pos.icrf.a", &node.loc.pos.icrf.a, "rvector");
            add_name("node.loc.pos.icrf.a.col", &node.loc.pos.icrf.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.icrf.a.col)/sizeof(node.loc.pos.icrf.a.col[0]); ++i) {
                string basename = "node.loc.pos.icrf.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.icrf.a.col[i], "double");
            }
            add_name("node.loc.pos.icrf.pass", &node.loc.pos.icrf.pass, "uint32_t");
            add_name("node.loc.pos.eci", &node.loc.pos.eci, "cartpos");
            add_name("node.loc.pos.eci.utc", &node.loc.pos.eci.utc, "double");
            add_name("node.loc.pos.eci.s", &node.loc.pos.eci.s, "rvector");
            add_name("node.loc.pos.eci.s.col", &node.loc.pos.eci.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.eci.s.col)/sizeof(node.loc.pos.eci.s.col[0]); ++i) {
                string basename = "node.loc.pos.eci.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.eci.s.col[i], "double");
            }
            add_name("node.loc.pos.eci.v", &node.loc.pos.eci.v, "rvector");
            add_name("node.loc.pos.eci.v.col", &node.loc.pos.eci.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.eci.v.col)/sizeof(node.loc.pos.eci.v.col[0]); ++i) {
                string basename = "node.loc.pos.eci.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.eci.v.col[i], "double");
            }
            add_name("node.loc.pos.eci.a", &node.loc.pos.eci.a, "rvector");
            add_name("node.loc.pos.eci.a.col", &node.loc.pos.eci.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.eci.a.col)/sizeof(node.loc.pos.eci.a.col[0]); ++i) {
                string basename = "node.loc.pos.eci.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.eci.a.col[i], "double");
            }
            add_name("node.loc.pos.eci.pass", &node.loc.pos.eci.pass, "uint32_t");
            add_name("node.loc.pos.sci", &node.loc.pos.sci, "cartpos");
            add_name("node.loc.pos.sci.utc", &node.loc.pos.sci.utc, "double");
            add_name("node.loc.pos.sci.s", &node.loc.pos.sci.s, "rvector");
            add_name("node.loc.pos.sci.s.col", &node.loc.pos.sci.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.sci.s.col)/sizeof(node.loc.pos.sci.s.col[0]); ++i) {
                string basename = "node.loc.pos.sci.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.sci.s.col[i], "double");
            }
            add_name("node.loc.pos.sci.v", &node.loc.pos.sci.v, "rvector");
            add_name("node.loc.pos.sci.v.col", &node.loc.pos.sci.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.sci.v.col)/sizeof(node.loc.pos.sci.v.col[0]); ++i) {
                string basename = "node.loc.pos.sci.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.sci.v.col[i], "double");
            }
            add_name("node.loc.pos.sci.a", &node.loc.pos.sci.a, "rvector");
            add_name("node.loc.pos.sci.a.col", &node.loc.pos.sci.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.sci.a.col)/sizeof(node.loc.pos.sci.a.col[0]); ++i) {
                string basename = "node.loc.pos.sci.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.sci.a.col[i], "double");
            }
            add_name("node.loc.pos.sci.pass", &node.loc.pos.sci.pass, "uint32_t");
            add_name("node.loc.pos.geoc", &node.loc.pos.geoc, "cartpos");
            add_name("node.loc.pos.geoc.utc", &node.loc.pos.geoc.utc, "double");
            add_name("node.loc.pos.geoc.s", &node.loc.pos.geoc.s, "rvector");
            add_name("node.loc.pos.geoc.s.col", &node.loc.pos.geoc.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.geoc.s.col)/sizeof(node.loc.pos.geoc.s.col[0]); ++i) {
                string basename = "node.loc.pos.geoc.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.geoc.s.col[i], "double");
            }
            add_name("node.loc.pos.geoc.v", &node.loc.pos.geoc.v, "rvector");
            add_name("node.loc.pos.geoc.v.col", &node.loc.pos.geoc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.geoc.v.col)/sizeof(node.loc.pos.geoc.v.col[0]); ++i) {
                string basename = "node.loc.pos.geoc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.geoc.v.col[i], "double");
            }
            add_name("node.loc.pos.geoc.a", &node.loc.pos.geoc.a, "rvector");
            add_name("node.loc.pos.geoc.a.col", &node.loc.pos.geoc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.geoc.a.col)/sizeof(node.loc.pos.geoc.a.col[0]); ++i) {
                string basename = "node.loc.pos.geoc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.geoc.a.col[i], "double");
            }
            add_name("node.loc.pos.geoc.pass", &node.loc.pos.geoc.pass, "uint32_t");
            add_name("node.loc.pos.selc", &node.loc.pos.selc, "cartpos");
            add_name("node.loc.pos.selc.utc", &node.loc.pos.selc.utc, "double");
            add_name("node.loc.pos.selc.s", &node.loc.pos.selc.s, "rvector");
            add_name("node.loc.pos.selc.s.col", &node.loc.pos.selc.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.selc.s.col)/sizeof(node.loc.pos.selc.s.col[0]); ++i) {
                string basename = "node.loc.pos.selc.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.selc.s.col[i], "double");
            }
            add_name("node.loc.pos.selc.v", &node.loc.pos.selc.v, "rvector");
            add_name("node.loc.pos.selc.v.col", &node.loc.pos.selc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.selc.v.col)/sizeof(node.loc.pos.selc.v.col[0]); ++i) {
                string basename = "node.loc.pos.selc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.selc.v.col[i], "double");
            }
            add_name("node.loc.pos.selc.a", &node.loc.pos.selc.a, "rvector");
            add_name("node.loc.pos.selc.a.col", &node.loc.pos.selc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.selc.a.col)/sizeof(node.loc.pos.selc.a.col[0]); ++i) {
                string basename = "node.loc.pos.selc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.selc.a.col[i], "double");
            }
            add_name("node.loc.pos.selc.pass", &node.loc.pos.selc.pass, "uint32_t");

            add_name("node.loc.pos.lvlh", &node.loc.pos.lvlh, "cartpos");
            add_name("node.loc.pos.lvlh.utc", &node.loc.pos.lvlh.utc, "double");
            add_name("node.loc.pos.lvlh.s", &node.loc.pos.lvlh.s, "rvector");
            add_name("node.loc.pos.lvlh.s.col", &node.loc.pos.lvlh.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.lvlh.s.col)/sizeof(node.loc.pos.lvlh.s.col[0]); ++i) {
                string basename = "node.loc.pos.lvlh.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.lvlh.s.col[i], "double");
            }
            add_name("node.loc.pos.lvlh.v", &node.loc.pos.lvlh.v, "rvector");
            add_name("node.loc.pos.lvlh.v.col", &node.loc.pos.lvlh.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.lvlh.v.col)/sizeof(node.loc.pos.lvlh.v.col[0]); ++i) {
                string basename = "node.loc.pos.lvlh.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.lvlh.v.col[i], "double");
            }
            add_name("node.loc.pos.lvlh.a", &node.loc.pos.lvlh.a, "rvector");
            add_name("node.loc.pos.lvlh.a.col", &node.loc.pos.lvlh.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.lvlh.a.col)/sizeof(node.loc.pos.lvlh.a.col[0]); ++i) {
                string basename = "node.loc.pos.lvlh.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.lvlh.a.col[i], "double");
            }
            add_name("node.loc.pos.lvlh.pass", &node.loc.pos.lvlh.pass, "uint32_t");

            add_name("node.loc.pos.geod", &node.loc.pos.geod, "geoidpos");
            add_name("node.loc.pos.geod.utc", &node.loc.pos.geod.utc, "double");
            add_name("node.loc.pos.geod.s", &node.loc.pos.geod.s, "gvector");
            add_name("node.loc.pos.geod.s.lat", &node.loc.pos.geod.s.lat, "double");
            add_name("node.loc.pos.geod.s.lon", &node.loc.pos.geod.s.lon, "double");
            add_name("node.loc.pos.geod.s.h", &node.loc.pos.geod.s.h, "double");
            add_name("node.loc.pos.geod.v", &node.loc.pos.geod.v, "gvector");
            add_name("node.loc.pos.geod.v.lat", &node.loc.pos.geod.v.lat, "double");
            add_name("node.loc.pos.geod.v.lon", &node.loc.pos.geod.v.lon, "double");
            add_name("node.loc.pos.geod.v.h", &node.loc.pos.geod.v.h, "double");
            add_name("node.loc.pos.geod.a", &node.loc.pos.geod.a, "gvector");
            add_name("node.loc.pos.geod.a.lat", &node.loc.pos.geod.a.lat, "double");
            add_name("node.loc.pos.geod.a.lon", &node.loc.pos.geod.a.lon, "double");
            add_name("node.loc.pos.geod.a.h", &node.loc.pos.geod.a.h, "double");
            add_name("node.loc.pos.geod.pass", &node.loc.pos.geod.pass, "uint32_t");
            add_name("node.loc.pos.selg", &node.loc.pos.selg, "geoidpos");
            add_name("node.loc.pos.selg.utc", &node.loc.pos.selg.utc, "double");
            add_name("node.loc.pos.selg.s", &node.loc.pos.selg.s, "gvector");
            add_name("node.loc.pos.selg.s.lat", &node.loc.pos.selg.s.lat, "double");
            add_name("node.loc.pos.selg.s.lon", &node.loc.pos.selg.s.lon, "double");
            add_name("node.loc.pos.selg.s.h", &node.loc.pos.selg.s.h, "double");
            add_name("node.loc.pos.selg.v", &node.loc.pos.selg.v, "gvector");
            add_name("node.loc.pos.selg.v.lat", &node.loc.pos.selg.v.lat, "double");
            add_name("node.loc.pos.selg.v.lon", &node.loc.pos.selg.v.lon, "double");
            add_name("node.loc.pos.selg.v.h", &node.loc.pos.selg.v.h, "double");
            add_name("node.loc.pos.selg.a", &node.loc.pos.selg.a, "gvector");
            add_name("node.loc.pos.selg.a.lat", &node.loc.pos.selg.a.lat, "double");
            add_name("node.loc.pos.selg.a.lon", &node.loc.pos.selg.a.lon, "double");
            add_name("node.loc.pos.selg.a.h", &node.loc.pos.selg.a.h, "double");
            add_name("node.loc.pos.selg.pass", &node.loc.pos.selg.pass, "uint32_t");
            add_name("node.loc.pos.geos", &node.loc.pos.geos, "spherpos");
            add_name("node.loc.pos.geos.utc", &node.loc.pos.geos.utc, "double");
            add_name("node.loc.pos.geos.s", &node.loc.pos.geos.s, "svector");
            add_name("node.loc.pos.geos.s.phi", &node.loc.pos.geos.s.phi, "double");
            add_name("node.loc.pos.geos.s.lambda", &node.loc.pos.geos.s.lambda, "double");
            add_name("node.loc.pos.geos.s.r", &node.loc.pos.geos.s.r, "double");
            add_name("node.loc.pos.geos.v", &node.loc.pos.geos.v, "svector");
            add_name("node.loc.pos.geos.v.phi", &node.loc.pos.geos.v.phi, "double");
            add_name("node.loc.pos.geos.v.lambda", &node.loc.pos.geos.v.lambda, "double");
            add_name("node.loc.pos.geos.v.r", &node.loc.pos.geos.v.r, "double");
            add_name("node.loc.pos.geos.a", &node.loc.pos.geos.a, "svector");
            add_name("node.loc.pos.geos.a.phi", &node.loc.pos.geos.a.phi, "double");
            add_name("node.loc.pos.geos.a.lambda", &node.loc.pos.geos.a.lambda, "double");
            add_name("node.loc.pos.geos.a.r", &node.loc.pos.geos.a.r, "double");
            add_name("node.loc.pos.geos.pass", &node.loc.pos.geos.pass, "uint32_t");
            add_name("node.loc.pos.extra", &node.loc.pos.extra, "extrapos");
            add_name("node.loc.pos.extra.utc", &node.loc.pos.extra.utc, "double");
            add_name("node.loc.pos.extra.tt", &node.loc.pos.extra.tt, "double");
            add_name("node.loc.pos.extra.ut", &node.loc.pos.extra.ut, "double");
            add_name("node.loc.pos.extra.tdb", &node.loc.pos.extra.tdb, "double");
            add_name("node.loc.pos.extra.j2e", &node.loc.pos.extra.j2e, "rmatrix");
            add_name("node.loc.pos.extra.j2e.row", &node.loc.pos.extra.j2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.j2e.row)/sizeof(node.loc.pos.extra.j2e.row[0]); ++i) {
                string basename = "node.loc.pos.extra.j2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.j2e.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.j2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.j2e.row[i].col)/sizeof(node.loc.pos.extra.j2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.j2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.dj2e", &node.loc.pos.extra.dj2e, "rmatrix");
            add_name("node.loc.pos.extra.dj2e.row", &node.loc.pos.extra.dj2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.dj2e.row)/sizeof(node.loc.pos.extra.dj2e.row[0]); ++i) {
                string basename = "node.loc.pos.extra.dj2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.dj2e.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.dj2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.dj2e.row[i].col)/sizeof(node.loc.pos.extra.dj2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.dj2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.ddj2e", &node.loc.pos.extra.ddj2e, "rmatrix");
            add_name("node.loc.pos.extra.ddj2e.row", &node.loc.pos.extra.ddj2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.ddj2e.row)/sizeof(node.loc.pos.extra.ddj2e.row[0]); ++i) {
                string basename = "node.loc.pos.extra.ddj2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.ddj2e.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.ddj2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.ddj2e.row[i].col)/sizeof(node.loc.pos.extra.ddj2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.ddj2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.e2j", &node.loc.pos.extra.e2j, "rmatrix");
            add_name("node.loc.pos.extra.e2j.row", &node.loc.pos.extra.e2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.e2j.row)/sizeof(node.loc.pos.extra.e2j.row[0]); ++i) {
                string basename = "node.loc.pos.extra.e2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.e2j.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.e2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.e2j.row[i].col)/sizeof(node.loc.pos.extra.e2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.e2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.de2j", &node.loc.pos.extra.de2j, "rmatrix");
            add_name("node.loc.pos.extra.de2j.row", &node.loc.pos.extra.de2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.de2j.row)/sizeof(node.loc.pos.extra.de2j.row[0]); ++i) {
                string basename = "node.loc.pos.extra.de2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.de2j.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.de2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.de2j.row[i].col)/sizeof(node.loc.pos.extra.de2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.de2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.dde2j", &node.loc.pos.extra.dde2j, "rmatrix");
            add_name("node.loc.pos.extra.dde2j.row", &node.loc.pos.extra.dde2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.dde2j.row)/sizeof(node.loc.pos.extra.dde2j.row[0]); ++i) {
                string basename = "node.loc.pos.extra.dde2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.dde2j.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.dde2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.dde2j.row[i].col)/sizeof(node.loc.pos.extra.dde2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.dde2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.j2t", &node.loc.pos.extra.j2t, "rmatrix");
            add_name("node.loc.pos.extra.j2t.row", &node.loc.pos.extra.j2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.j2t.row)/sizeof(node.loc.pos.extra.j2t.row[0]); ++i) {
                string basename = "node.loc.pos.extra.j2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.j2t.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.j2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.j2t.row[i].col)/sizeof(node.loc.pos.extra.j2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.j2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.j2s", &node.loc.pos.extra.j2s, "rmatrix");
            add_name("node.loc.pos.extra.j2s.row", &node.loc.pos.extra.j2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.j2s.row)/sizeof(node.loc.pos.extra.j2s.row[0]); ++i) {
                string basename = "node.loc.pos.extra.j2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.j2s.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.j2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.j2s.row[i].col)/sizeof(node.loc.pos.extra.j2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.j2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.t2j", &node.loc.pos.extra.t2j, "rmatrix");
            add_name("node.loc.pos.extra.t2j.row", &node.loc.pos.extra.t2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.t2j.row)/sizeof(node.loc.pos.extra.t2j.row[0]); ++i) {
                string basename = "node.loc.pos.extra.t2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.t2j.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.t2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.t2j.row[i].col)/sizeof(node.loc.pos.extra.t2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.t2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.s2j", &node.loc.pos.extra.s2j, "rmatrix");
            add_name("node.loc.pos.extra.s2j.row", &node.loc.pos.extra.s2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.s2j.row)/sizeof(node.loc.pos.extra.s2j.row[0]); ++i) {
                string basename = "node.loc.pos.extra.s2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.s2j.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.s2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.s2j.row[i].col)/sizeof(node.loc.pos.extra.s2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.s2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.s2t", &node.loc.pos.extra.s2t, "rmatrix");
            add_name("node.loc.pos.extra.s2t.row", &node.loc.pos.extra.s2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.s2t.row)/sizeof(node.loc.pos.extra.s2t.row[0]); ++i) {
                string basename = "node.loc.pos.extra.s2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.s2t.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.s2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.s2t.row[i].col)/sizeof(node.loc.pos.extra.s2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.s2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.ds2t", &node.loc.pos.extra.ds2t, "rmatrix");
            add_name("node.loc.pos.extra.ds2t.row", &node.loc.pos.extra.ds2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.ds2t.row)/sizeof(node.loc.pos.extra.ds2t.row[0]); ++i) {
                string basename = "node.loc.pos.extra.ds2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.ds2t.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.ds2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.ds2t.row[i].col)/sizeof(node.loc.pos.extra.ds2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.ds2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.t2s", &node.loc.pos.extra.t2s, "rmatrix");
            add_name("node.loc.pos.extra.t2s.row", &node.loc.pos.extra.t2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.t2s.row)/sizeof(node.loc.pos.extra.t2s.row[0]); ++i) {
                string basename = "node.loc.pos.extra.t2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.t2s.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.t2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.t2s.row[i].col)/sizeof(node.loc.pos.extra.t2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.t2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.dt2s", &node.loc.pos.extra.dt2s, "rmatrix");
            add_name("node.loc.pos.extra.dt2s.row", &node.loc.pos.extra.dt2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.dt2s.row)/sizeof(node.loc.pos.extra.dt2s.row[0]); ++i) {
                string basename = "node.loc.pos.extra.dt2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.dt2s.row[i], "rvector");
                add_name(basename+".col", &node.loc.pos.extra.dt2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.pos.extra.dt2s.row[i].col)/sizeof(node.loc.pos.extra.dt2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.pos.extra.dt2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc.pos.extra.l2e", &node.loc.pos.extra.l2e, "quaternion");
            add_name("node.loc.pos.extra.l2e.d", &node.loc.pos.extra.l2e.d, "cvector");
            add_name("node.loc.pos.extra.l2e.d.x", &node.loc.pos.extra.l2e.d.x, "double");
            add_name("node.loc.pos.extra.l2e.d.y", &node.loc.pos.extra.l2e.d.y, "double");
            add_name("node.loc.pos.extra.l2e.d.z", &node.loc.pos.extra.l2e.d.z, "double");
            add_name("node.loc.pos.extra.l2e.w", &node.loc.pos.extra.l2e.w, "double");
            add_name("node.loc.pos.extra.e2l", &node.loc.pos.extra.e2l, "quaternion");
            add_name("node.loc.pos.extra.e2l.d", &node.loc.pos.extra.e2l.d, "cvector");
            add_name("node.loc.pos.extra.e2l.d.x", &node.loc.pos.extra.e2l.d.x, "double");
            add_name("node.loc.pos.extra.e2l.d.y", &node.loc.pos.extra.e2l.d.y, "double");
            add_name("node.loc.pos.extra.e2l.d.z", &node.loc.pos.extra.e2l.d.z, "double");
            add_name("node.loc.pos.extra.e2l.w", &node.loc.pos.extra.e2l.w, "double");
            add_name("node.loc.pos.extra.sun2earth", &node.loc.pos.extra.sun2earth, "cartpos");
            add_name("node.loc.pos.extra.sun2earth.utc", &node.loc.pos.extra.sun2earth.utc, "double");
            add_name("node.loc.pos.extra.sun2earth.s", &node.loc.pos.extra.sun2earth.s, "rvector");
            add_name("node.loc.pos.extra.sun2earth.s.col", &node.loc.pos.extra.sun2earth.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.sun2earth.s.col)/sizeof(node.loc.pos.extra.sun2earth.s.col[0]); ++i) {
                string basename = "node.loc.pos.extra.sun2earth.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.sun2earth.s.col[i], "double");
            }
            add_name("node.loc.pos.extra.sun2earth.v", &node.loc.pos.extra.sun2earth.v, "rvector");
            add_name("node.loc.pos.extra.sun2earth.v.col", &node.loc.pos.extra.sun2earth.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.sun2earth.v.col)/sizeof(node.loc.pos.extra.sun2earth.v.col[0]); ++i) {
                string basename = "node.loc.pos.extra.sun2earth.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.sun2earth.v.col[i], "double");
            }
            add_name("node.loc.pos.extra.sun2earth.a", &node.loc.pos.extra.sun2earth.a, "rvector");
            add_name("node.loc.pos.extra.sun2earth.a.col", &node.loc.pos.extra.sun2earth.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.sun2earth.a.col)/sizeof(node.loc.pos.extra.sun2earth.a.col[0]); ++i) {
                string basename = "node.loc.pos.extra.sun2earth.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.sun2earth.a.col[i], "double");
            }
            add_name("node.loc.pos.extra.sun2earth.pass", &node.loc.pos.extra.sun2earth.pass, "uint32_t");
            add_name("node.loc.pos.extra.sun2moon", &node.loc.pos.extra.sun2moon, "cartpos");
            add_name("node.loc.pos.extra.sun2moon.utc", &node.loc.pos.extra.sun2moon.utc, "double");
            add_name("node.loc.pos.extra.sun2moon.s", &node.loc.pos.extra.sun2moon.s, "rvector");
            add_name("node.loc.pos.extra.sun2moon.s.col", &node.loc.pos.extra.sun2moon.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.sun2moon.s.col)/sizeof(node.loc.pos.extra.sun2moon.s.col[0]); ++i) {
                string basename = "node.loc.pos.extra.sun2moon.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.sun2moon.s.col[i], "double");
            }
            add_name("node.loc.pos.extra.sun2moon.v", &node.loc.pos.extra.sun2moon.v, "rvector");
            add_name("node.loc.pos.extra.sun2moon.v.col", &node.loc.pos.extra.sun2moon.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.sun2moon.v.col)/sizeof(node.loc.pos.extra.sun2moon.v.col[0]); ++i) {
                string basename = "node.loc.pos.extra.sun2moon.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.sun2moon.v.col[i], "double");
            }
            add_name("node.loc.pos.extra.sun2moon.a", &node.loc.pos.extra.sun2moon.a, "rvector");
            add_name("node.loc.pos.extra.sun2moon.a.col", &node.loc.pos.extra.sun2moon.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.extra.sun2moon.a.col)/sizeof(node.loc.pos.extra.sun2moon.a.col[0]); ++i) {
                string basename = "node.loc.pos.extra.sun2moon.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.extra.sun2moon.a.col[i], "double");
            }
            add_name("node.loc.pos.extra.sun2moon.pass", &node.loc.pos.extra.sun2moon.pass, "uint32_t");
            add_name("node.loc.pos.extra.closest", &node.loc.pos.extra.closest, "uint16_t");
            add_name("node.loc.pos.earthsep", &node.loc.pos.earthsep, "float");
            add_name("node.loc.pos.moonsep", &node.loc.pos.moonsep, "float");
            add_name("node.loc.pos.sunsize", &node.loc.pos.sunsize, "float");
            add_name("node.loc.pos.sunradiance", &node.loc.pos.sunradiance, "float");
            add_name("node.loc.pos.bearth", &node.loc.pos.bearth, "rvector");
            add_name("node.loc.pos.bearth.col", &node.loc.pos.bearth.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.pos.bearth.col)/sizeof(node.loc.pos.bearth.col[0]); ++i) {
                string basename = "node.loc.pos.bearth.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.pos.bearth.col[i], "double");
            }
            add_name("node.loc.pos.orbit", &node.loc.pos.orbit, "double");
            add_name("node.loc.att", &node.loc.att, "attstruc");
            add_name("node.loc.att.utc", &node.loc.att.utc, "double");
            add_name("node.loc.att.topo", &node.loc.att.topo, "qatt");
            add_name("node.loc.att.topo.utc", &node.loc.att.topo.utc, "double");
            add_name("node.loc.att.topo.s", &node.loc.att.topo.s, "quaternion");
            add_name("node.loc.att.topo.s.d", &node.loc.att.topo.s.d, "cvector");
            add_name("node.loc.att.topo.s.d.x", &node.loc.att.topo.s.d.x, "double");
            add_name("node.loc.att.topo.s.d.y", &node.loc.att.topo.s.d.y, "double");
            add_name("node.loc.att.topo.s.d.z", &node.loc.att.topo.s.d.z, "double");
            add_name("node.loc.att.topo.s.w", &node.loc.att.topo.s.w, "double");
            add_name("node.loc.att.topo.v", &node.loc.att.topo.v, "rvector");
            add_name("node.loc.att.topo.v.col", &node.loc.att.topo.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.topo.v.col)/sizeof(node.loc.att.topo.v.col[0]); ++i) {
                string basename = "node.loc.att.topo.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.topo.v.col[i], "double");
            }
            add_name("node.loc.att.topo.a", &node.loc.att.topo.a, "rvector");
            add_name("node.loc.att.topo.a.col", &node.loc.att.topo.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.topo.a.col)/sizeof(node.loc.att.topo.a.col[0]); ++i) {
                string basename = "node.loc.att.topo.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.topo.a.col[i], "double");
            }
            add_name("node.loc.att.topo.pass", &node.loc.att.topo.pass, "uint32_t");
            add_name("node.loc.att.lvlh", &node.loc.att.lvlh, "qatt");
            add_name("node.loc.att.lvlh.utc", &node.loc.att.lvlh.utc, "double");
            add_name("node.loc.att.lvlh.s", &node.loc.att.lvlh.s, "quaternion");
            add_name("node.loc.att.lvlh.s.d", &node.loc.att.lvlh.s.d, "cvector");
            add_name("node.loc.att.lvlh.s.d.x", &node.loc.att.lvlh.s.d.x, "double");
            add_name("node.loc.att.lvlh.s.d.y", &node.loc.att.lvlh.s.d.y, "double");
            add_name("node.loc.att.lvlh.s.d.z", &node.loc.att.lvlh.s.d.z, "double");
            add_name("node.loc.att.lvlh.s.w", &node.loc.att.lvlh.s.w, "double");
            add_name("node.loc.att.lvlh.v", &node.loc.att.lvlh.v, "rvector");
            add_name("node.loc.att.lvlh.v.col", &node.loc.att.lvlh.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.lvlh.v.col)/sizeof(node.loc.att.lvlh.v.col[0]); ++i) {
                string basename = "node.loc.att.lvlh.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.lvlh.v.col[i], "double");
            }
            add_name("node.loc.att.lvlh.a", &node.loc.att.lvlh.a, "rvector");
            add_name("node.loc.att.lvlh.a.col", &node.loc.att.lvlh.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.lvlh.a.col)/sizeof(node.loc.att.lvlh.a.col[0]); ++i) {
                string basename = "node.loc.att.lvlh.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.lvlh.a.col[i], "double");
            }
            add_name("node.loc.att.lvlh.pass", &node.loc.att.lvlh.pass, "uint32_t");
            add_name("node.loc.att.geoc", &node.loc.att.geoc, "qatt");
            add_name("node.loc.att.geoc.utc", &node.loc.att.geoc.utc, "double");
            add_name("node.loc.att.geoc.s", &node.loc.att.geoc.s, "quaternion");
            add_name("node.loc.att.geoc.s.d", &node.loc.att.geoc.s.d, "cvector");
            add_name("node.loc.att.geoc.s.d.x", &node.loc.att.geoc.s.d.x, "double");
            add_name("node.loc.att.geoc.s.d.y", &node.loc.att.geoc.s.d.y, "double");
            add_name("node.loc.att.geoc.s.d.z", &node.loc.att.geoc.s.d.z, "double");
            add_name("node.loc.att.geoc.s.w", &node.loc.att.geoc.s.w, "double");
            add_name("node.loc.att.geoc.v", &node.loc.att.geoc.v, "rvector");
            add_name("node.loc.att.geoc.v.col", &node.loc.att.geoc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.geoc.v.col)/sizeof(node.loc.att.geoc.v.col[0]); ++i) {
                string basename = "node.loc.att.geoc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.geoc.v.col[i], "double");
            }
            add_name("node.loc.att.geoc.a", &node.loc.att.geoc.a, "rvector");
            add_name("node.loc.att.geoc.a.col", &node.loc.att.geoc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.geoc.a.col)/sizeof(node.loc.att.geoc.a.col[0]); ++i) {
                string basename = "node.loc.att.geoc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.geoc.a.col[i], "double");
            }
            add_name("node.loc.att.geoc.pass", &node.loc.att.geoc.pass, "uint32_t");

            add_name("node.loc.att.selc", &node.loc.att.selc, "qatt");
            add_name("node.loc.att.selc.utc", &node.loc.att.selc.utc, "double");
            add_name("node.loc.att.selc.s", &node.loc.att.selc.s, "quaternion");
            add_name("node.loc.att.selc.s.d", &node.loc.att.selc.s.d, "cvector");
            add_name("node.loc.att.selc.s.d.x", &node.loc.att.selc.s.d.x, "double");
            add_name("node.loc.att.selc.s.d.y", &node.loc.att.selc.s.d.y, "double");
            add_name("node.loc.att.selc.s.d.z", &node.loc.att.selc.s.d.z, "double");
            add_name("node.loc.att.selc.s.w", &node.loc.att.selc.s.w, "double");
            add_name("node.loc.att.selc.v", &node.loc.att.selc.v, "rvector");
            add_name("node.loc.att.selc.v.col", &node.loc.att.selc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.selc.v.col)/sizeof(node.loc.att.selc.v.col[0]); ++i) {
                string basename = "node.loc.att.selc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.selc.v.col[i], "double");
            }
            add_name("node.loc.att.selc.a", &node.loc.att.selc.a, "rvector");
            add_name("node.loc.att.selc.a.col", &node.loc.att.selc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.selc.a.col)/sizeof(node.loc.att.selc.a.col[0]); ++i) {
                string basename = "node.loc.att.selc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.selc.a.col[i], "double");
            }
            add_name("node.loc.att.selc.pass", &node.loc.att.selc.pass, "uint32_t");

            add_name("node.loc.att.icrf", &node.loc.att.icrf, "qatt");
            add_name("node.loc.att.icrf.utc", &node.loc.att.icrf.utc, "double");
            add_name("node.loc.att.icrf.s", &node.loc.att.icrf.s, "quaternion");
            add_name("node.loc.att.icrf.s.d", &node.loc.att.icrf.s.d, "cvector");
            add_name("node.loc.att.icrf.s.d.x", &node.loc.att.icrf.s.d.x, "double");
            add_name("node.loc.att.icrf.s.d.y", &node.loc.att.icrf.s.d.y, "double");
            add_name("node.loc.att.icrf.s.d.z", &node.loc.att.icrf.s.d.z, "double");
            add_name("node.loc.att.icrf.s.w", &node.loc.att.icrf.s.w, "double");
            add_name("node.loc.att.icrf.v", &node.loc.att.icrf.v, "rvector");
            add_name("node.loc.att.icrf.v.col", &node.loc.att.icrf.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.icrf.v.col)/sizeof(node.loc.att.icrf.v.col[0]); ++i) {
                string basename = "node.loc.att.icrf.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.icrf.v.col[i], "double");
            }
            add_name("node.loc.att.icrf.a", &node.loc.att.icrf.a, "rvector");
            add_name("node.loc.att.icrf.a.col", &node.loc.att.icrf.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc.att.icrf.a.col)/sizeof(node.loc.att.icrf.a.col[0]); ++i) {
                string basename = "node.loc.att.icrf.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.icrf.a.col[i], "double");
            }
            add_name("node.loc.att.icrf.pass", &node.loc.att.icrf.pass, "uint32_t");
            add_name("node.loc.att.extra", &node.loc.att.extra, "extraatt");
            add_name("node.loc.att.extra.utc", &node.loc.att.extra.utc, "double");
            add_name("node.loc.att.extra.j2b", &node.loc.att.extra.j2b, "rmatrix");
            add_name("node.loc.att.extra.j2b.row", &node.loc.att.extra.j2b.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.att.extra.j2b.row)/sizeof(node.loc.att.extra.j2b.row[0]); ++i) {
                string basename = "node.loc.att.extra.j2b.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.extra.j2b.row[i], "rvector");
                add_name(basename+".col", &node.loc.att.extra.j2b.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.att.extra.j2b.row[i].col)/sizeof(node.loc.att.extra.j2b.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.att.extra.j2b.row[i].col[j], "double");
                }
            }
            add_name("node.loc.att.extra.b2j", &node.loc.att.extra.b2j, "rmatrix");
            add_name("node.loc.att.extra.b2j.row", &node.loc.att.extra.b2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc.att.extra.b2j.row)/sizeof(node.loc.att.extra.b2j.row[0]); ++i) {
                string basename = "node.loc.att.extra.b2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc.att.extra.b2j.row[i], "rvector");
                add_name(basename+".col", &node.loc.att.extra.b2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc.att.extra.b2j.row[i].col)/sizeof(node.loc.att.extra.b2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc.att.extra.b2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est", &node.loc_est, "locstruc");
            add_name("node.loc_est.utc", &node.loc_est.utc, "double");
            add_name("node.loc_est.pos", &node.loc_est.pos, "posstruc");
            add_name("node.loc_est.pos.utc", &node.loc_est.pos.utc, "double");
            add_name("node.loc_est.pos.icrf", &node.loc_est.pos.icrf, "cartpos");
            add_name("node.loc_est.pos.icrf.utc", &node.loc_est.pos.icrf.utc, "double");
            add_name("node.loc_est.pos.icrf.s", &node.loc_est.pos.icrf.s, "rvector");
            add_name("node.loc_est.pos.icrf.s.col", &node.loc_est.pos.icrf.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.icrf.s.col)/sizeof(node.loc_est.pos.icrf.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.icrf.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.icrf.s.col[i], "double");
            }
            add_name("node.loc_est.pos.icrf.v", &node.loc_est.pos.icrf.v, "rvector");
            add_name("node.loc_est.pos.icrf.v.col", &node.loc_est.pos.icrf.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.icrf.v.col)/sizeof(node.loc_est.pos.icrf.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.icrf.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.icrf.v.col[i], "double");
            }
            add_name("node.loc_est.pos.icrf.a", &node.loc_est.pos.icrf.a, "rvector");
            add_name("node.loc_est.pos.icrf.a.col", &node.loc_est.pos.icrf.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.icrf.a.col)/sizeof(node.loc_est.pos.icrf.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.icrf.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.icrf.a.col[i], "double");
            }
            add_name("node.loc_est.pos.icrf.pass", &node.loc_est.pos.icrf.pass, "uint32_t");
            add_name("node.loc_est.pos.eci", &node.loc_est.pos.eci, "cartpos");
            add_name("node.loc_est.pos.eci.utc", &node.loc_est.pos.eci.utc, "double");
            add_name("node.loc_est.pos.eci.s", &node.loc_est.pos.eci.s, "rvector");
            add_name("node.loc_est.pos.eci.s.col", &node.loc_est.pos.eci.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.eci.s.col)/sizeof(node.loc_est.pos.eci.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.eci.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.eci.s.col[i], "double");
            }
            add_name("node.loc_est.pos.eci.v", &node.loc_est.pos.eci.v, "rvector");
            add_name("node.loc_est.pos.eci.v.col", &node.loc_est.pos.eci.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.eci.v.col)/sizeof(node.loc_est.pos.eci.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.eci.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.eci.v.col[i], "double");
            }
            add_name("node.loc_est.pos.eci.a", &node.loc_est.pos.eci.a, "rvector");
            add_name("node.loc_est.pos.eci.a.col", &node.loc_est.pos.eci.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.eci.a.col)/sizeof(node.loc_est.pos.eci.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.eci.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.eci.a.col[i], "double");
            }
            add_name("node.loc_est.pos.eci.pass", &node.loc_est.pos.eci.pass, "uint32_t");
            add_name("node.loc_est.pos.sci", &node.loc_est.pos.sci, "cartpos");
            add_name("node.loc_est.pos.sci.utc", &node.loc_est.pos.sci.utc, "double");
            add_name("node.loc_est.pos.sci.s", &node.loc_est.pos.sci.s, "rvector");
            add_name("node.loc_est.pos.sci.s.col", &node.loc_est.pos.sci.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.sci.s.col)/sizeof(node.loc_est.pos.sci.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.sci.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.sci.s.col[i], "double");
            }
            add_name("node.loc_est.pos.sci.v", &node.loc_est.pos.sci.v, "rvector");
            add_name("node.loc_est.pos.sci.v.col", &node.loc_est.pos.sci.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.sci.v.col)/sizeof(node.loc_est.pos.sci.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.sci.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.sci.v.col[i], "double");
            }
            add_name("node.loc_est.pos.sci.a", &node.loc_est.pos.sci.a, "rvector");
            add_name("node.loc_est.pos.sci.a.col", &node.loc_est.pos.sci.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.sci.a.col)/sizeof(node.loc_est.pos.sci.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.sci.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.sci.a.col[i], "double");
            }
            add_name("node.loc_est.pos.sci.pass", &node.loc_est.pos.sci.pass, "uint32_t");
            add_name("node.loc_est.pos.geoc", &node.loc_est.pos.geoc, "cartpos");
            add_name("node.loc_est.pos.geoc.utc", &node.loc_est.pos.geoc.utc, "double");
            add_name("node.loc_est.pos.geoc.s", &node.loc_est.pos.geoc.s, "rvector");
            add_name("node.loc_est.pos.geoc.s.col", &node.loc_est.pos.geoc.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.geoc.s.col)/sizeof(node.loc_est.pos.geoc.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.geoc.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.geoc.s.col[i], "double");
            }
            add_name("node.loc_est.pos.geoc.v", &node.loc_est.pos.geoc.v, "rvector");
            add_name("node.loc_est.pos.geoc.v.col", &node.loc_est.pos.geoc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.geoc.v.col)/sizeof(node.loc_est.pos.geoc.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.geoc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.geoc.v.col[i], "double");
            }
            add_name("node.loc_est.pos.geoc.a", &node.loc_est.pos.geoc.a, "rvector");
            add_name("node.loc_est.pos.geoc.a.col", &node.loc_est.pos.geoc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.geoc.a.col)/sizeof(node.loc_est.pos.geoc.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.geoc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.geoc.a.col[i], "double");
            }
            add_name("node.loc_est.pos.geoc.pass", &node.loc_est.pos.geoc.pass, "uint32_t");

            add_name("node.loc_est.pos.selc", &node.loc_est.pos.selc, "cartpos");
            add_name("node.loc_est.pos.selc.utc", &node.loc_est.pos.selc.utc, "double");
            add_name("node.loc_est.pos.selc.s", &node.loc_est.pos.selc.s, "rvector");
            add_name("node.loc_est.pos.selc.s.col", &node.loc_est.pos.selc.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.selc.s.col)/sizeof(node.loc_est.pos.selc.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.selc.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.selc.s.col[i], "double");
            }
            add_name("node.loc_est.pos.selc.v", &node.loc_est.pos.selc.v, "rvector");
            add_name("node.loc_est.pos.selc.v.col", &node.loc_est.pos.selc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.selc.v.col)/sizeof(node.loc_est.pos.selc.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.selc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.selc.v.col[i], "double");
            }
            add_name("node.loc_est.pos.selc.a", &node.loc_est.pos.selc.a, "rvector");
            add_name("node.loc_est.pos.selc.a.col", &node.loc_est.pos.selc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.selc.a.col)/sizeof(node.loc_est.pos.selc.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.selc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.selc.a.col[i], "double");
            }
            add_name("node.loc_est.pos.selc.pass", &node.loc_est.pos.selc.pass, "uint32_t");

            add_name("node.loc_est.pos.lvlh", &node.loc_est.pos.lvlh, "cartpos");
            add_name("node.loc_est.pos.lvlh.utc", &node.loc_est.pos.lvlh.utc, "double");
            add_name("node.loc_est.pos.lvlh.s", &node.loc_est.pos.lvlh.s, "rvector");
            add_name("node.loc_est.pos.lvlh.s.col", &node.loc_est.pos.lvlh.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.lvlh.s.col)/sizeof(node.loc_est.pos.lvlh.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.lvlh.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.lvlh.s.col[i], "double");
            }
            add_name("node.loc_est.pos.lvlh.v", &node.loc_est.pos.lvlh.v, "rvector");
            add_name("node.loc_est.pos.lvlh.v.col", &node.loc_est.pos.lvlh.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.lvlh.v.col)/sizeof(node.loc_est.pos.lvlh.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.lvlh.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.lvlh.v.col[i], "double");
            }
            add_name("node.loc_est.pos.lvlh.a", &node.loc_est.pos.lvlh.a, "rvector");
            add_name("node.loc_est.pos.lvlh.a.col", &node.loc_est.pos.lvlh.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.lvlh.a.col)/sizeof(node.loc_est.pos.lvlh.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.lvlh.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.lvlh.a.col[i], "double");
            }
            add_name("node.loc_est.pos.lvlh.pass", &node.loc_est.pos.lvlh.pass, "uint32_t");

            add_name("node.loc_est.pos.geod", &node.loc_est.pos.geod, "geoidpos");
            add_name("node.loc_est.pos.geod.utc", &node.loc_est.pos.geod.utc, "double");
            add_name("node.loc_est.pos.geod.s", &node.loc_est.pos.geod.s, "gvector");
            add_name("node.loc_est.pos.geod.s.lat", &node.loc_est.pos.geod.s.lat, "double");
            add_name("node.loc_est.pos.geod.s.lon", &node.loc_est.pos.geod.s.lon, "double");
            add_name("node.loc_est.pos.geod.s.h", &node.loc_est.pos.geod.s.h, "double");
            add_name("node.loc_est.pos.geod.v", &node.loc_est.pos.geod.v, "gvector");
            add_name("node.loc_est.pos.geod.v.lat", &node.loc_est.pos.geod.v.lat, "double");
            add_name("node.loc_est.pos.geod.v.lon", &node.loc_est.pos.geod.v.lon, "double");
            add_name("node.loc_est.pos.geod.v.h", &node.loc_est.pos.geod.v.h, "double");
            add_name("node.loc_est.pos.geod.a", &node.loc_est.pos.geod.a, "gvector");
            add_name("node.loc_est.pos.geod.a.lat", &node.loc_est.pos.geod.a.lat, "double");
            add_name("node.loc_est.pos.geod.a.lon", &node.loc_est.pos.geod.a.lon, "double");
            add_name("node.loc_est.pos.geod.a.h", &node.loc_est.pos.geod.a.h, "double");
            add_name("node.loc_est.pos.geod.pass", &node.loc_est.pos.geod.pass, "uint32_t");
            add_name("node.loc_est.pos.selg", &node.loc_est.pos.selg, "geoidpos");
            add_name("node.loc_est.pos.selg.utc", &node.loc_est.pos.selg.utc, "double");
            add_name("node.loc_est.pos.selg.s", &node.loc_est.pos.selg.s, "gvector");
            add_name("node.loc_est.pos.selg.s.lat", &node.loc_est.pos.selg.s.lat, "double");
            add_name("node.loc_est.pos.selg.s.lon", &node.loc_est.pos.selg.s.lon, "double");
            add_name("node.loc_est.pos.selg.s.h", &node.loc_est.pos.selg.s.h, "double");
            add_name("node.loc_est.pos.selg.v", &node.loc_est.pos.selg.v, "gvector");
            add_name("node.loc_est.pos.selg.v.lat", &node.loc_est.pos.selg.v.lat, "double");
            add_name("node.loc_est.pos.selg.v.lon", &node.loc_est.pos.selg.v.lon, "double");
            add_name("node.loc_est.pos.selg.v.h", &node.loc_est.pos.selg.v.h, "double");
            add_name("node.loc_est.pos.selg.a", &node.loc_est.pos.selg.a, "gvector");
            add_name("node.loc_est.pos.selg.a.lat", &node.loc_est.pos.selg.a.lat, "double");
            add_name("node.loc_est.pos.selg.a.lon", &node.loc_est.pos.selg.a.lon, "double");
            add_name("node.loc_est.pos.selg.a.h", &node.loc_est.pos.selg.a.h, "double");
            add_name("node.loc_est.pos.selg.pass", &node.loc_est.pos.selg.pass, "uint32_t");
            add_name("node.loc_est.pos.geos", &node.loc_est.pos.geos, "spherpos");
            add_name("node.loc_est.pos.geos.utc", &node.loc_est.pos.geos.utc, "double");
            add_name("node.loc_est.pos.geos.s", &node.loc_est.pos.geos.s, "svector");
            add_name("node.loc_est.pos.geos.s.phi", &node.loc_est.pos.geos.s.phi, "double");
            add_name("node.loc_est.pos.geos.s.lambda", &node.loc_est.pos.geos.s.lambda, "double");
            add_name("node.loc_est.pos.geos.s.r", &node.loc_est.pos.geos.s.r, "double");
            add_name("node.loc_est.pos.geos.v", &node.loc_est.pos.geos.v, "svector");
            add_name("node.loc_est.pos.geos.v.phi", &node.loc_est.pos.geos.v.phi, "double");
            add_name("node.loc_est.pos.geos.v.lambda", &node.loc_est.pos.geos.v.lambda, "double");
            add_name("node.loc_est.pos.geos.v.r", &node.loc_est.pos.geos.v.r, "double");
            add_name("node.loc_est.pos.geos.a", &node.loc_est.pos.geos.a, "svector");
            add_name("node.loc_est.pos.geos.a.phi", &node.loc_est.pos.geos.a.phi, "double");
            add_name("node.loc_est.pos.geos.a.lambda", &node.loc_est.pos.geos.a.lambda, "double");
            add_name("node.loc_est.pos.geos.a.r", &node.loc_est.pos.geos.a.r, "double");
            add_name("node.loc_est.pos.geos.pass", &node.loc_est.pos.geos.pass, "uint32_t");
            add_name("node.loc_est.pos.extra", &node.loc_est.pos.extra, "extrapos");
            add_name("node.loc_est.pos.extra.utc", &node.loc_est.pos.extra.utc, "double");
            add_name("node.loc_est.pos.extra.tt", &node.loc_est.pos.extra.tt, "double");
            add_name("node.loc_est.pos.extra.ut", &node.loc_est.pos.extra.ut, "double");
            add_name("node.loc_est.pos.extra.tdb", &node.loc_est.pos.extra.tdb, "double");
            add_name("node.loc_est.pos.extra.j2e", &node.loc_est.pos.extra.j2e, "rmatrix");
            add_name("node.loc_est.pos.extra.j2e.row", &node.loc_est.pos.extra.j2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.j2e.row)/sizeof(node.loc_est.pos.extra.j2e.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.j2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.j2e.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.j2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.j2e.row[i].col)/sizeof(node.loc_est.pos.extra.j2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.j2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.dj2e", &node.loc_est.pos.extra.dj2e, "rmatrix");
            add_name("node.loc_est.pos.extra.dj2e.row", &node.loc_est.pos.extra.dj2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.dj2e.row)/sizeof(node.loc_est.pos.extra.dj2e.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.dj2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.dj2e.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.dj2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.dj2e.row[i].col)/sizeof(node.loc_est.pos.extra.dj2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.dj2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.ddj2e", &node.loc_est.pos.extra.ddj2e, "rmatrix");
            add_name("node.loc_est.pos.extra.ddj2e.row", &node.loc_est.pos.extra.ddj2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.ddj2e.row)/sizeof(node.loc_est.pos.extra.ddj2e.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.ddj2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.ddj2e.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.ddj2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.ddj2e.row[i].col)/sizeof(node.loc_est.pos.extra.ddj2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.ddj2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.e2j", &node.loc_est.pos.extra.e2j, "rmatrix");
            add_name("node.loc_est.pos.extra.e2j.row", &node.loc_est.pos.extra.e2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.e2j.row)/sizeof(node.loc_est.pos.extra.e2j.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.e2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.e2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.e2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.e2j.row[i].col)/sizeof(node.loc_est.pos.extra.e2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.e2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.de2j", &node.loc_est.pos.extra.de2j, "rmatrix");
            add_name("node.loc_est.pos.extra.de2j.row", &node.loc_est.pos.extra.de2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.de2j.row)/sizeof(node.loc_est.pos.extra.de2j.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.de2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.de2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.de2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.de2j.row[i].col)/sizeof(node.loc_est.pos.extra.de2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.de2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.dde2j", &node.loc_est.pos.extra.dde2j, "rmatrix");
            add_name("node.loc_est.pos.extra.dde2j.row", &node.loc_est.pos.extra.dde2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.dde2j.row)/sizeof(node.loc_est.pos.extra.dde2j.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.dde2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.dde2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.dde2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.dde2j.row[i].col)/sizeof(node.loc_est.pos.extra.dde2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.dde2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.j2t", &node.loc_est.pos.extra.j2t, "rmatrix");
            add_name("node.loc_est.pos.extra.j2t.row", &node.loc_est.pos.extra.j2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.j2t.row)/sizeof(node.loc_est.pos.extra.j2t.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.j2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.j2t.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.j2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.j2t.row[i].col)/sizeof(node.loc_est.pos.extra.j2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.j2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.j2s", &node.loc_est.pos.extra.j2s, "rmatrix");
            add_name("node.loc_est.pos.extra.j2s.row", &node.loc_est.pos.extra.j2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.j2s.row)/sizeof(node.loc_est.pos.extra.j2s.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.j2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.j2s.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.j2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.j2s.row[i].col)/sizeof(node.loc_est.pos.extra.j2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.j2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.t2j", &node.loc_est.pos.extra.t2j, "rmatrix");
            add_name("node.loc_est.pos.extra.t2j.row", &node.loc_est.pos.extra.t2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.t2j.row)/sizeof(node.loc_est.pos.extra.t2j.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.t2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.t2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.t2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.t2j.row[i].col)/sizeof(node.loc_est.pos.extra.t2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.t2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.s2j", &node.loc_est.pos.extra.s2j, "rmatrix");
            add_name("node.loc_est.pos.extra.s2j.row", &node.loc_est.pos.extra.s2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.s2j.row)/sizeof(node.loc_est.pos.extra.s2j.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.s2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.s2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.s2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.s2j.row[i].col)/sizeof(node.loc_est.pos.extra.s2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.s2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.s2t", &node.loc_est.pos.extra.s2t, "rmatrix");
            add_name("node.loc_est.pos.extra.s2t.row", &node.loc_est.pos.extra.s2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.s2t.row)/sizeof(node.loc_est.pos.extra.s2t.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.s2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.s2t.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.s2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.s2t.row[i].col)/sizeof(node.loc_est.pos.extra.s2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.s2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.ds2t", &node.loc_est.pos.extra.ds2t, "rmatrix");
            add_name("node.loc_est.pos.extra.ds2t.row", &node.loc_est.pos.extra.ds2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.ds2t.row)/sizeof(node.loc_est.pos.extra.ds2t.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.ds2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.ds2t.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.ds2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.ds2t.row[i].col)/sizeof(node.loc_est.pos.extra.ds2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.ds2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.t2s", &node.loc_est.pos.extra.t2s, "rmatrix");
            add_name("node.loc_est.pos.extra.t2s.row", &node.loc_est.pos.extra.t2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.t2s.row)/sizeof(node.loc_est.pos.extra.t2s.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.t2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.t2s.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.t2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.t2s.row[i].col)/sizeof(node.loc_est.pos.extra.t2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.t2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.dt2s", &node.loc_est.pos.extra.dt2s, "rmatrix");
            add_name("node.loc_est.pos.extra.dt2s.row", &node.loc_est.pos.extra.dt2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.dt2s.row)/sizeof(node.loc_est.pos.extra.dt2s.row[0]); ++i) {
                string basename = "node.loc_est.pos.extra.dt2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.dt2s.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.pos.extra.dt2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.pos.extra.dt2s.row[i].col)/sizeof(node.loc_est.pos.extra.dt2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.pos.extra.dt2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.pos.extra.l2e", &node.loc_est.pos.extra.l2e, "quaternion");
            add_name("node.loc_est.pos.extra.l2e.d", &node.loc_est.pos.extra.l2e.d, "cvector");
            add_name("node.loc_est.pos.extra.l2e.d.x", &node.loc_est.pos.extra.l2e.d.x, "double");
            add_name("node.loc_est.pos.extra.l2e.d.y", &node.loc_est.pos.extra.l2e.d.y, "double");
            add_name("node.loc_est.pos.extra.l2e.d.z", &node.loc_est.pos.extra.l2e.d.z, "double");
            add_name("node.loc_est.pos.extra.l2e.w", &node.loc_est.pos.extra.l2e.w, "double");
            add_name("node.loc_est.pos.extra.e2l", &node.loc_est.pos.extra.e2l, "quaternion");
            add_name("node.loc_est.pos.extra.e2l.d", &node.loc_est.pos.extra.e2l.d, "cvector");
            add_name("node.loc_est.pos.extra.e2l.d.x", &node.loc_est.pos.extra.e2l.d.x, "double");
            add_name("node.loc_est.pos.extra.e2l.d.y", &node.loc_est.pos.extra.e2l.d.y, "double");
            add_name("node.loc_est.pos.extra.e2l.d.z", &node.loc_est.pos.extra.e2l.d.z, "double");
            add_name("node.loc_est.pos.extra.e2l.w", &node.loc_est.pos.extra.e2l.w, "double");
            add_name("node.loc_est.pos.extra.sun2earth", &node.loc_est.pos.extra.sun2earth, "cartpos");
            add_name("node.loc_est.pos.extra.sun2earth.utc", &node.loc_est.pos.extra.sun2earth.utc, "double");
            add_name("node.loc_est.pos.extra.sun2earth.s", &node.loc_est.pos.extra.sun2earth.s, "rvector");
            add_name("node.loc_est.pos.extra.sun2earth.s.col", &node.loc_est.pos.extra.sun2earth.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.sun2earth.s.col)/sizeof(node.loc_est.pos.extra.sun2earth.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.extra.sun2earth.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.sun2earth.s.col[i], "double");
            }
            add_name("node.loc_est.pos.extra.sun2earth.v", &node.loc_est.pos.extra.sun2earth.v, "rvector");
            add_name("node.loc_est.pos.extra.sun2earth.v.col", &node.loc_est.pos.extra.sun2earth.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.sun2earth.v.col)/sizeof(node.loc_est.pos.extra.sun2earth.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.extra.sun2earth.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.sun2earth.v.col[i], "double");
            }
            add_name("node.loc_est.pos.extra.sun2earth.a", &node.loc_est.pos.extra.sun2earth.a, "rvector");
            add_name("node.loc_est.pos.extra.sun2earth.a.col", &node.loc_est.pos.extra.sun2earth.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.sun2earth.a.col)/sizeof(node.loc_est.pos.extra.sun2earth.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.extra.sun2earth.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.sun2earth.a.col[i], "double");
            }
            add_name("node.loc_est.pos.extra.sun2earth.pass", &node.loc_est.pos.extra.sun2earth.pass, "uint32_t");
            add_name("node.loc_est.pos.extra.sun2moon", &node.loc_est.pos.extra.sun2moon, "cartpos");
            add_name("node.loc_est.pos.extra.sun2moon.utc", &node.loc_est.pos.extra.sun2moon.utc, "double");
            add_name("node.loc_est.pos.extra.sun2moon.s", &node.loc_est.pos.extra.sun2moon.s, "rvector");
            add_name("node.loc_est.pos.extra.sun2moon.s.col", &node.loc_est.pos.extra.sun2moon.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.sun2moon.s.col)/sizeof(node.loc_est.pos.extra.sun2moon.s.col[0]); ++i) {
                string basename = "node.loc_est.pos.extra.sun2moon.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.sun2moon.s.col[i], "double");
            }
            add_name("node.loc_est.pos.extra.sun2moon.v", &node.loc_est.pos.extra.sun2moon.v, "rvector");
            add_name("node.loc_est.pos.extra.sun2moon.v.col", &node.loc_est.pos.extra.sun2moon.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.sun2moon.v.col)/sizeof(node.loc_est.pos.extra.sun2moon.v.col[0]); ++i) {
                string basename = "node.loc_est.pos.extra.sun2moon.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.sun2moon.v.col[i], "double");
            }
            add_name("node.loc_est.pos.extra.sun2moon.a", &node.loc_est.pos.extra.sun2moon.a, "rvector");
            add_name("node.loc_est.pos.extra.sun2moon.a.col", &node.loc_est.pos.extra.sun2moon.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.extra.sun2moon.a.col)/sizeof(node.loc_est.pos.extra.sun2moon.a.col[0]); ++i) {
                string basename = "node.loc_est.pos.extra.sun2moon.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.extra.sun2moon.a.col[i], "double");
            }
            add_name("node.loc_est.pos.extra.sun2moon.pass", &node.loc_est.pos.extra.sun2moon.pass, "uint32_t");
            add_name("node.loc_est.pos.extra.closest", &node.loc_est.pos.extra.closest, "uint16_t");
            add_name("node.loc_est.pos.earthsep", &node.loc_est.pos.earthsep, "float");
            add_name("node.loc_est.pos.moonsep", &node.loc_est.pos.moonsep, "float");
            add_name("node.loc_est.pos.sunsize", &node.loc_est.pos.sunsize, "float");
            add_name("node.loc_est.pos.sunradiance", &node.loc_est.pos.sunradiance, "float");
            add_name("node.loc_est.pos.bearth", &node.loc_est.pos.bearth, "rvector");
            add_name("node.loc_est.pos.bearth.col", &node.loc_est.pos.bearth.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.pos.bearth.col)/sizeof(node.loc_est.pos.bearth.col[0]); ++i) {
                string basename = "node.loc_est.pos.bearth.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.pos.bearth.col[i], "double");
            }
            add_name("node.loc_est.pos.orbit", &node.loc_est.pos.orbit, "double");
            add_name("node.loc_est.att", &node.loc_est.att, "attstruc");
            add_name("node.loc_est.att.utc", &node.loc_est.att.utc, "double");
            add_name("node.loc_est.att.topo", &node.loc_est.att.topo, "qatt");
            add_name("node.loc_est.att.topo.utc", &node.loc_est.att.topo.utc, "double");
            add_name("node.loc_est.att.topo.s", &node.loc_est.att.topo.s, "quaternion");
            add_name("node.loc_est.att.topo.s.d", &node.loc_est.att.topo.s.d, "cvector");
            add_name("node.loc_est.att.topo.s.d.x", &node.loc_est.att.topo.s.d.x, "double");
            add_name("node.loc_est.att.topo.s.d.y", &node.loc_est.att.topo.s.d.y, "double");
            add_name("node.loc_est.att.topo.s.d.z", &node.loc_est.att.topo.s.d.z, "double");
            add_name("node.loc_est.att.topo.s.w", &node.loc_est.att.topo.s.w, "double");
            add_name("node.loc_est.att.topo.v", &node.loc_est.att.topo.v, "rvector");
            add_name("node.loc_est.att.topo.v.col", &node.loc_est.att.topo.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.topo.v.col)/sizeof(node.loc_est.att.topo.v.col[0]); ++i) {
                string basename = "node.loc_est.att.topo.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.topo.v.col[i], "double");
            }
            add_name("node.loc_est.att.topo.a", &node.loc_est.att.topo.a, "rvector");
            add_name("node.loc_est.att.topo.a.col", &node.loc_est.att.topo.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.topo.a.col)/sizeof(node.loc_est.att.topo.a.col[0]); ++i) {
                string basename = "node.loc_est.att.topo.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.topo.a.col[i], "double");
            }
            add_name("node.loc_est.att.topo.pass", &node.loc_est.att.topo.pass, "uint32_t");
            add_name("node.loc_est.att.lvlh", &node.loc_est.att.lvlh, "qatt");
            add_name("node.loc_est.att.lvlh.utc", &node.loc_est.att.lvlh.utc, "double");
            add_name("node.loc_est.att.lvlh.s", &node.loc_est.att.lvlh.s, "quaternion");
            add_name("node.loc_est.att.lvlh.s.d", &node.loc_est.att.lvlh.s.d, "cvector");
            add_name("node.loc_est.att.lvlh.s.d.x", &node.loc_est.att.lvlh.s.d.x, "double");
            add_name("node.loc_est.att.lvlh.s.d.y", &node.loc_est.att.lvlh.s.d.y, "double");
            add_name("node.loc_est.att.lvlh.s.d.z", &node.loc_est.att.lvlh.s.d.z, "double");
            add_name("node.loc_est.att.lvlh.s.w", &node.loc_est.att.lvlh.s.w, "double");
            add_name("node.loc_est.att.lvlh.v", &node.loc_est.att.lvlh.v, "rvector");
            add_name("node.loc_est.att.lvlh.v.col", &node.loc_est.att.lvlh.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.lvlh.v.col)/sizeof(node.loc_est.att.lvlh.v.col[0]); ++i) {
                string basename = "node.loc_est.att.lvlh.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.lvlh.v.col[i], "double");
            }
            add_name("node.loc_est.att.lvlh.a", &node.loc_est.att.lvlh.a, "rvector");
            add_name("node.loc_est.att.lvlh.a.col", &node.loc_est.att.lvlh.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.lvlh.a.col)/sizeof(node.loc_est.att.lvlh.a.col[0]); ++i) {
                string basename = "node.loc_est.att.lvlh.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.lvlh.a.col[i], "double");
            }
            add_name("node.loc_est.att.lvlh.pass", &node.loc_est.att.lvlh.pass, "uint32_t");
            add_name("node.loc_est.att.geoc", &node.loc_est.att.geoc, "qatt");
            add_name("node.loc_est.att.geoc.utc", &node.loc_est.att.geoc.utc, "double");
            add_name("node.loc_est.att.geoc.s", &node.loc_est.att.geoc.s, "quaternion");
            add_name("node.loc_est.att.geoc.s.d", &node.loc_est.att.geoc.s.d, "cvector");
            add_name("node.loc_est.att.geoc.s.d.x", &node.loc_est.att.geoc.s.d.x, "double");
            add_name("node.loc_est.att.geoc.s.d.y", &node.loc_est.att.geoc.s.d.y, "double");
            add_name("node.loc_est.att.geoc.s.d.z", &node.loc_est.att.geoc.s.d.z, "double");
            add_name("node.loc_est.att.geoc.s.w", &node.loc_est.att.geoc.s.w, "double");
            add_name("node.loc_est.att.geoc.v", &node.loc_est.att.geoc.v, "rvector");
            add_name("node.loc_est.att.geoc.v.col", &node.loc_est.att.geoc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.geoc.v.col)/sizeof(node.loc_est.att.geoc.v.col[0]); ++i) {
                string basename = "node.loc_est.att.geoc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.geoc.v.col[i], "double");
            }
            add_name("node.loc_est.att.geoc.a", &node.loc_est.att.geoc.a, "rvector");
            add_name("node.loc_est.att.geoc.a.col", &node.loc_est.att.geoc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.geoc.a.col)/sizeof(node.loc_est.att.geoc.a.col[0]); ++i) {
                string basename = "node.loc_est.att.geoc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.geoc.a.col[i], "double");
            }
            add_name("node.loc_est.att.geoc.pass", &node.loc_est.att.geoc.pass, "uint32_t");
            add_name("node.loc_est.att.selc", &node.loc_est.att.selc, "qatt");
            add_name("node.loc_est.att.selc.utc", &node.loc_est.att.selc.utc, "double");
            add_name("node.loc_est.att.selc.s", &node.loc_est.att.selc.s, "quaternion");
            add_name("node.loc_est.att.selc.s.d", &node.loc_est.att.selc.s.d, "cvector");
            add_name("node.loc_est.att.selc.s.d.x", &node.loc_est.att.selc.s.d.x, "double");
            add_name("node.loc_est.att.selc.s.d.y", &node.loc_est.att.selc.s.d.y, "double");
            add_name("node.loc_est.att.selc.s.d.z", &node.loc_est.att.selc.s.d.z, "double");
            add_name("node.loc_est.att.selc.s.w", &node.loc_est.att.selc.s.w, "double");
            add_name("node.loc_est.att.selc.v", &node.loc_est.att.selc.v, "rvector");
            add_name("node.loc_est.att.selc.v.col", &node.loc_est.att.selc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.selc.v.col)/sizeof(node.loc_est.att.selc.v.col[0]); ++i) {
                string basename = "node.loc_est.att.selc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.selc.v.col[i], "double");
            }
            add_name("node.loc_est.att.selc.a", &node.loc_est.att.selc.a, "rvector");
            add_name("node.loc_est.att.selc.a.col", &node.loc_est.att.selc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.selc.a.col)/sizeof(node.loc_est.att.selc.a.col[0]); ++i) {
                string basename = "node.loc_est.att.selc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.selc.a.col[i], "double");
            }
            add_name("node.loc_est.att.selc.pass", &node.loc_est.att.selc.pass, "uint32_t");
            add_name("node.loc_est.att.icrf", &node.loc_est.att.icrf, "qatt");
            add_name("node.loc_est.att.icrf.utc", &node.loc_est.att.icrf.utc, "double");
            add_name("node.loc_est.att.icrf.s", &node.loc_est.att.icrf.s, "quaternion");
            add_name("node.loc_est.att.icrf.s.d", &node.loc_est.att.icrf.s.d, "cvector");
            add_name("node.loc_est.att.icrf.s.d.x", &node.loc_est.att.icrf.s.d.x, "double");
            add_name("node.loc_est.att.icrf.s.d.y", &node.loc_est.att.icrf.s.d.y, "double");
            add_name("node.loc_est.att.icrf.s.d.z", &node.loc_est.att.icrf.s.d.z, "double");
            add_name("node.loc_est.att.icrf.s.w", &node.loc_est.att.icrf.s.w, "double");
            add_name("node.loc_est.att.icrf.v", &node.loc_est.att.icrf.v, "rvector");
            add_name("node.loc_est.att.icrf.v.col", &node.loc_est.att.icrf.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.icrf.v.col)/sizeof(node.loc_est.att.icrf.v.col[0]); ++i) {
                string basename = "node.loc_est.att.icrf.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.icrf.v.col[i], "double");
            }
            add_name("node.loc_est.att.icrf.a", &node.loc_est.att.icrf.a, "rvector");
            add_name("node.loc_est.att.icrf.a.col", &node.loc_est.att.icrf.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.icrf.a.col)/sizeof(node.loc_est.att.icrf.a.col[0]); ++i) {
                string basename = "node.loc_est.att.icrf.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.icrf.a.col[i], "double");
            }
            add_name("node.loc_est.att.icrf.pass", &node.loc_est.att.icrf.pass, "uint32_t");
            add_name("node.loc_est.att.extra", &node.loc_est.att.extra, "extraatt");
            add_name("node.loc_est.att.extra.utc", &node.loc_est.att.extra.utc, "double");
            add_name("node.loc_est.att.extra.j2b", &node.loc_est.att.extra.j2b, "rmatrix");
            add_name("node.loc_est.att.extra.j2b.row", &node.loc_est.att.extra.j2b.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.extra.j2b.row)/sizeof(node.loc_est.att.extra.j2b.row[0]); ++i) {
                string basename = "node.loc_est.att.extra.j2b.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.extra.j2b.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.att.extra.j2b.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.att.extra.j2b.row[i].col)/sizeof(node.loc_est.att.extra.j2b.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.att.extra.j2b.row[i].col[j], "double");
                }
            }
            add_name("node.loc_est.att.extra.b2j", &node.loc_est.att.extra.b2j, "rmatrix");
            add_name("node.loc_est.att.extra.b2j.row", &node.loc_est.att.extra.b2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_est.att.extra.b2j.row)/sizeof(node.loc_est.att.extra.b2j.row[0]); ++i) {
                string basename = "node.loc_est.att.extra.b2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_est.att.extra.b2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_est.att.extra.b2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_est.att.extra.b2j.row[i].col)/sizeof(node.loc_est.att.extra.b2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_est.att.extra.b2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std", &node.loc_std, "locstruc");
            add_name("node.loc_std.utc", &node.loc_std.utc, "double");
            add_name("node.loc_std.pos", &node.loc_std.pos, "posstruc");
            add_name("node.loc_std.pos.utc", &node.loc_std.pos.utc, "double");
            add_name("node.loc_std.pos.icrf", &node.loc_std.pos.icrf, "cartpos");
            add_name("node.loc_std.pos.icrf.utc", &node.loc_std.pos.icrf.utc, "double");
            add_name("node.loc_std.pos.icrf.s", &node.loc_std.pos.icrf.s, "rvector");
            add_name("node.loc_std.pos.icrf.s.col", &node.loc_std.pos.icrf.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.icrf.s.col)/sizeof(node.loc_std.pos.icrf.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.icrf.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.icrf.s.col[i], "double");
            }
            add_name("node.loc_std.pos.icrf.v", &node.loc_std.pos.icrf.v, "rvector");
            add_name("node.loc_std.pos.icrf.v.col", &node.loc_std.pos.icrf.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.icrf.v.col)/sizeof(node.loc_std.pos.icrf.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.icrf.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.icrf.v.col[i], "double");
            }
            add_name("node.loc_std.pos.icrf.a", &node.loc_std.pos.icrf.a, "rvector");
            add_name("node.loc_std.pos.icrf.a.col", &node.loc_std.pos.icrf.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.icrf.a.col)/sizeof(node.loc_std.pos.icrf.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.icrf.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.icrf.a.col[i], "double");
            }
            add_name("node.loc_std.pos.icrf.pass", &node.loc_std.pos.icrf.pass, "uint32_t");
            add_name("node.loc_std.pos.eci", &node.loc_std.pos.eci, "cartpos");
            add_name("node.loc_std.pos.eci.utc", &node.loc_std.pos.eci.utc, "double");
            add_name("node.loc_std.pos.eci.s", &node.loc_std.pos.eci.s, "rvector");
            add_name("node.loc_std.pos.eci.s.col", &node.loc_std.pos.eci.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.eci.s.col)/sizeof(node.loc_std.pos.eci.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.eci.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.eci.s.col[i], "double");
            }
            add_name("node.loc_std.pos.eci.v", &node.loc_std.pos.eci.v, "rvector");
            add_name("node.loc_std.pos.eci.v.col", &node.loc_std.pos.eci.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.eci.v.col)/sizeof(node.loc_std.pos.eci.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.eci.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.eci.v.col[i], "double");
            }
            add_name("node.loc_std.pos.eci.a", &node.loc_std.pos.eci.a, "rvector");
            add_name("node.loc_std.pos.eci.a.col", &node.loc_std.pos.eci.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.eci.a.col)/sizeof(node.loc_std.pos.eci.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.eci.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.eci.a.col[i], "double");
            }
            add_name("node.loc_std.pos.eci.pass", &node.loc_std.pos.eci.pass, "uint32_t");
            add_name("node.loc_std.pos.sci", &node.loc_std.pos.sci, "cartpos");
            add_name("node.loc_std.pos.sci.utc", &node.loc_std.pos.sci.utc, "double");
            add_name("node.loc_std.pos.sci.s", &node.loc_std.pos.sci.s, "rvector");
            add_name("node.loc_std.pos.sci.s.col", &node.loc_std.pos.sci.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.sci.s.col)/sizeof(node.loc_std.pos.sci.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.sci.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.sci.s.col[i], "double");
            }
            add_name("node.loc_std.pos.sci.v", &node.loc_std.pos.sci.v, "rvector");
            add_name("node.loc_std.pos.sci.v.col", &node.loc_std.pos.sci.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.sci.v.col)/sizeof(node.loc_std.pos.sci.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.sci.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.sci.v.col[i], "double");
            }
            add_name("node.loc_std.pos.sci.a", &node.loc_std.pos.sci.a, "rvector");
            add_name("node.loc_std.pos.sci.a.col", &node.loc_std.pos.sci.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.sci.a.col)/sizeof(node.loc_std.pos.sci.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.sci.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.sci.a.col[i], "double");
            }
            add_name("node.loc_std.pos.sci.pass", &node.loc_std.pos.sci.pass, "uint32_t");
            add_name("node.loc_std.pos.geoc", &node.loc_std.pos.geoc, "cartpos");
            add_name("node.loc_std.pos.geoc.utc", &node.loc_std.pos.geoc.utc, "double");
            add_name("node.loc_std.pos.geoc.s", &node.loc_std.pos.geoc.s, "rvector");
            add_name("node.loc_std.pos.geoc.s.col", &node.loc_std.pos.geoc.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.geoc.s.col)/sizeof(node.loc_std.pos.geoc.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.geoc.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.geoc.s.col[i], "double");
            }
            add_name("node.loc_std.pos.geoc.v", &node.loc_std.pos.geoc.v, "rvector");
            add_name("node.loc_std.pos.geoc.v.col", &node.loc_std.pos.geoc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.geoc.v.col)/sizeof(node.loc_std.pos.geoc.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.geoc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.geoc.v.col[i], "double");
            }
            add_name("node.loc_std.pos.geoc.a", &node.loc_std.pos.geoc.a, "rvector");
            add_name("node.loc_std.pos.geoc.a.col", &node.loc_std.pos.geoc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.geoc.a.col)/sizeof(node.loc_std.pos.geoc.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.geoc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.geoc.a.col[i], "double");
            }
            add_name("node.loc_std.pos.geoc.pass", &node.loc_std.pos.geoc.pass, "uint32_t");

            add_name("node.loc_std.pos.selc", &node.loc_std.pos.selc, "cartpos");
            add_name("node.loc_std.pos.selc.utc", &node.loc_std.pos.selc.utc, "double");
            add_name("node.loc_std.pos.selc.s", &node.loc_std.pos.selc.s, "rvector");
            add_name("node.loc_std.pos.selc.s.col", &node.loc_std.pos.selc.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.selc.s.col)/sizeof(node.loc_std.pos.selc.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.selc.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.selc.s.col[i], "double");
            }
            add_name("node.loc_std.pos.selc.v", &node.loc_std.pos.selc.v, "rvector");
            add_name("node.loc_std.pos.selc.v.col", &node.loc_std.pos.selc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.selc.v.col)/sizeof(node.loc_std.pos.selc.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.selc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.selc.v.col[i], "double");
            }
            add_name("node.loc_std.pos.selc.a", &node.loc_std.pos.selc.a, "rvector");
            add_name("node.loc_std.pos.selc.a.col", &node.loc_std.pos.selc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.selc.a.col)/sizeof(node.loc_std.pos.selc.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.selc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.selc.a.col[i], "double");
            }
            add_name("node.loc_std.pos.selc.pass", &node.loc_std.pos.selc.pass, "uint32_t");

            add_name("node.loc_std.pos.lvlh", &node.loc_std.pos.lvlh, "cartpos");
            add_name("node.loc_std.pos.lvlh.utc", &node.loc_std.pos.lvlh.utc, "double");
            add_name("node.loc_std.pos.lvlh.s", &node.loc_std.pos.lvlh.s, "rvector");
            add_name("node.loc_std.pos.lvlh.s.col", &node.loc_std.pos.lvlh.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.lvlh.s.col)/sizeof(node.loc_std.pos.lvlh.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.lvlh.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.lvlh.s.col[i], "double");
            }
            add_name("node.loc_std.pos.lvlh.v", &node.loc_std.pos.lvlh.v, "rvector");
            add_name("node.loc_std.pos.lvlh.v.col", &node.loc_std.pos.lvlh.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.lvlh.v.col)/sizeof(node.loc_std.pos.lvlh.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.lvlh.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.lvlh.v.col[i], "double");
            }
            add_name("node.loc_std.pos.lvlh.a", &node.loc_std.pos.lvlh.a, "rvector");
            add_name("node.loc_std.pos.lvlh.a.col", &node.loc_std.pos.lvlh.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.lvlh.a.col)/sizeof(node.loc_std.pos.lvlh.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.lvlh.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.lvlh.a.col[i], "double");
            }
            add_name("node.loc_std.pos.lvlh.pass", &node.loc_std.pos.lvlh.pass, "uint32_t");

            add_name("node.loc_std.pos.geod", &node.loc_std.pos.geod, "geoidpos");
            add_name("node.loc_std.pos.geod.utc", &node.loc_std.pos.geod.utc, "double");
            add_name("node.loc_std.pos.geod.s", &node.loc_std.pos.geod.s, "gvector");
            add_name("node.loc_std.pos.geod.s.lat", &node.loc_std.pos.geod.s.lat, "double");
            add_name("node.loc_std.pos.geod.s.lon", &node.loc_std.pos.geod.s.lon, "double");
            add_name("node.loc_std.pos.geod.s.h", &node.loc_std.pos.geod.s.h, "double");
            add_name("node.loc_std.pos.geod.v", &node.loc_std.pos.geod.v, "gvector");
            add_name("node.loc_std.pos.geod.v.lat", &node.loc_std.pos.geod.v.lat, "double");
            add_name("node.loc_std.pos.geod.v.lon", &node.loc_std.pos.geod.v.lon, "double");
            add_name("node.loc_std.pos.geod.v.h", &node.loc_std.pos.geod.v.h, "double");
            add_name("node.loc_std.pos.geod.a", &node.loc_std.pos.geod.a, "gvector");
            add_name("node.loc_std.pos.geod.a.lat", &node.loc_std.pos.geod.a.lat, "double");
            add_name("node.loc_std.pos.geod.a.lon", &node.loc_std.pos.geod.a.lon, "double");
            add_name("node.loc_std.pos.geod.a.h", &node.loc_std.pos.geod.a.h, "double");
            add_name("node.loc_std.pos.geod.pass", &node.loc_std.pos.geod.pass, "uint32_t");
            add_name("node.loc_std.pos.selg", &node.loc_std.pos.selg, "geoidpos");
            add_name("node.loc_std.pos.selg.utc", &node.loc_std.pos.selg.utc, "double");
            add_name("node.loc_std.pos.selg.s", &node.loc_std.pos.selg.s, "gvector");
            add_name("node.loc_std.pos.selg.s.lat", &node.loc_std.pos.selg.s.lat, "double");
            add_name("node.loc_std.pos.selg.s.lon", &node.loc_std.pos.selg.s.lon, "double");
            add_name("node.loc_std.pos.selg.s.h", &node.loc_std.pos.selg.s.h, "double");
            add_name("node.loc_std.pos.selg.v", &node.loc_std.pos.selg.v, "gvector");
            add_name("node.loc_std.pos.selg.v.lat", &node.loc_std.pos.selg.v.lat, "double");
            add_name("node.loc_std.pos.selg.v.lon", &node.loc_std.pos.selg.v.lon, "double");
            add_name("node.loc_std.pos.selg.v.h", &node.loc_std.pos.selg.v.h, "double");
            add_name("node.loc_std.pos.selg.a", &node.loc_std.pos.selg.a, "gvector");
            add_name("node.loc_std.pos.selg.a.lat", &node.loc_std.pos.selg.a.lat, "double");
            add_name("node.loc_std.pos.selg.a.lon", &node.loc_std.pos.selg.a.lon, "double");
            add_name("node.loc_std.pos.selg.a.h", &node.loc_std.pos.selg.a.h, "double");
            add_name("node.loc_std.pos.selg.pass", &node.loc_std.pos.selg.pass, "uint32_t");
            add_name("node.loc_std.pos.geos", &node.loc_std.pos.geos, "spherpos");
            add_name("node.loc_std.pos.geos.utc", &node.loc_std.pos.geos.utc, "double");
            add_name("node.loc_std.pos.geos.s", &node.loc_std.pos.geos.s, "svector");
            add_name("node.loc_std.pos.geos.s.phi", &node.loc_std.pos.geos.s.phi, "double");
            add_name("node.loc_std.pos.geos.s.lambda", &node.loc_std.pos.geos.s.lambda, "double");
            add_name("node.loc_std.pos.geos.s.r", &node.loc_std.pos.geos.s.r, "double");
            add_name("node.loc_std.pos.geos.v", &node.loc_std.pos.geos.v, "svector");
            add_name("node.loc_std.pos.geos.v.phi", &node.loc_std.pos.geos.v.phi, "double");
            add_name("node.loc_std.pos.geos.v.lambda", &node.loc_std.pos.geos.v.lambda, "double");
            add_name("node.loc_std.pos.geos.v.r", &node.loc_std.pos.geos.v.r, "double");
            add_name("node.loc_std.pos.geos.a", &node.loc_std.pos.geos.a, "svector");
            add_name("node.loc_std.pos.geos.a.phi", &node.loc_std.pos.geos.a.phi, "double");
            add_name("node.loc_std.pos.geos.a.lambda", &node.loc_std.pos.geos.a.lambda, "double");
            add_name("node.loc_std.pos.geos.a.r", &node.loc_std.pos.geos.a.r, "double");
            add_name("node.loc_std.pos.geos.pass", &node.loc_std.pos.geos.pass, "uint32_t");
            add_name("node.loc_std.pos.extra", &node.loc_std.pos.extra, "extrapos");
            add_name("node.loc_std.pos.extra.utc", &node.loc_std.pos.extra.utc, "double");
            add_name("node.loc_std.pos.extra.tt", &node.loc_std.pos.extra.tt, "double");
            add_name("node.loc_std.pos.extra.ut", &node.loc_std.pos.extra.ut, "double");
            add_name("node.loc_std.pos.extra.tdb", &node.loc_std.pos.extra.tdb, "double");
            add_name("node.loc_std.pos.extra.j2e", &node.loc_std.pos.extra.j2e, "rmatrix");
            add_name("node.loc_std.pos.extra.j2e.row", &node.loc_std.pos.extra.j2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.j2e.row)/sizeof(node.loc_std.pos.extra.j2e.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.j2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.j2e.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.j2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.j2e.row[i].col)/sizeof(node.loc_std.pos.extra.j2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.j2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.dj2e", &node.loc_std.pos.extra.dj2e, "rmatrix");
            add_name("node.loc_std.pos.extra.dj2e.row", &node.loc_std.pos.extra.dj2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.dj2e.row)/sizeof(node.loc_std.pos.extra.dj2e.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.dj2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.dj2e.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.dj2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.dj2e.row[i].col)/sizeof(node.loc_std.pos.extra.dj2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.dj2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.ddj2e", &node.loc_std.pos.extra.ddj2e, "rmatrix");
            add_name("node.loc_std.pos.extra.ddj2e.row", &node.loc_std.pos.extra.ddj2e.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.ddj2e.row)/sizeof(node.loc_std.pos.extra.ddj2e.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.ddj2e.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.ddj2e.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.ddj2e.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.ddj2e.row[i].col)/sizeof(node.loc_std.pos.extra.ddj2e.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.ddj2e.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.e2j", &node.loc_std.pos.extra.e2j, "rmatrix");
            add_name("node.loc_std.pos.extra.e2j.row", &node.loc_std.pos.extra.e2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.e2j.row)/sizeof(node.loc_std.pos.extra.e2j.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.e2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.e2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.e2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.e2j.row[i].col)/sizeof(node.loc_std.pos.extra.e2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.e2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.de2j", &node.loc_std.pos.extra.de2j, "rmatrix");
            add_name("node.loc_std.pos.extra.de2j.row", &node.loc_std.pos.extra.de2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.de2j.row)/sizeof(node.loc_std.pos.extra.de2j.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.de2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.de2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.de2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.de2j.row[i].col)/sizeof(node.loc_std.pos.extra.de2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.de2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.dde2j", &node.loc_std.pos.extra.dde2j, "rmatrix");
            add_name("node.loc_std.pos.extra.dde2j.row", &node.loc_std.pos.extra.dde2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.dde2j.row)/sizeof(node.loc_std.pos.extra.dde2j.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.dde2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.dde2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.dde2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.dde2j.row[i].col)/sizeof(node.loc_std.pos.extra.dde2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.dde2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.j2t", &node.loc_std.pos.extra.j2t, "rmatrix");
            add_name("node.loc_std.pos.extra.j2t.row", &node.loc_std.pos.extra.j2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.j2t.row)/sizeof(node.loc_std.pos.extra.j2t.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.j2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.j2t.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.j2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.j2t.row[i].col)/sizeof(node.loc_std.pos.extra.j2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.j2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.j2s", &node.loc_std.pos.extra.j2s, "rmatrix");
            add_name("node.loc_std.pos.extra.j2s.row", &node.loc_std.pos.extra.j2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.j2s.row)/sizeof(node.loc_std.pos.extra.j2s.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.j2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.j2s.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.j2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.j2s.row[i].col)/sizeof(node.loc_std.pos.extra.j2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.j2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.t2j", &node.loc_std.pos.extra.t2j, "rmatrix");
            add_name("node.loc_std.pos.extra.t2j.row", &node.loc_std.pos.extra.t2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.t2j.row)/sizeof(node.loc_std.pos.extra.t2j.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.t2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.t2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.t2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.t2j.row[i].col)/sizeof(node.loc_std.pos.extra.t2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.t2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.s2j", &node.loc_std.pos.extra.s2j, "rmatrix");
            add_name("node.loc_std.pos.extra.s2j.row", &node.loc_std.pos.extra.s2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.s2j.row)/sizeof(node.loc_std.pos.extra.s2j.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.s2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.s2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.s2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.s2j.row[i].col)/sizeof(node.loc_std.pos.extra.s2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.s2j.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.s2t", &node.loc_std.pos.extra.s2t, "rmatrix");
            add_name("node.loc_std.pos.extra.s2t.row", &node.loc_std.pos.extra.s2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.s2t.row)/sizeof(node.loc_std.pos.extra.s2t.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.s2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.s2t.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.s2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.s2t.row[i].col)/sizeof(node.loc_std.pos.extra.s2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.s2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.ds2t", &node.loc_std.pos.extra.ds2t, "rmatrix");
            add_name("node.loc_std.pos.extra.ds2t.row", &node.loc_std.pos.extra.ds2t.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.ds2t.row)/sizeof(node.loc_std.pos.extra.ds2t.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.ds2t.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.ds2t.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.ds2t.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.ds2t.row[i].col)/sizeof(node.loc_std.pos.extra.ds2t.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.ds2t.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.t2s", &node.loc_std.pos.extra.t2s, "rmatrix");
            add_name("node.loc_std.pos.extra.t2s.row", &node.loc_std.pos.extra.t2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.t2s.row)/sizeof(node.loc_std.pos.extra.t2s.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.t2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.t2s.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.t2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.t2s.row[i].col)/sizeof(node.loc_std.pos.extra.t2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.t2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.dt2s", &node.loc_std.pos.extra.dt2s, "rmatrix");
            add_name("node.loc_std.pos.extra.dt2s.row", &node.loc_std.pos.extra.dt2s.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.dt2s.row)/sizeof(node.loc_std.pos.extra.dt2s.row[0]); ++i) {
                string basename = "node.loc_std.pos.extra.dt2s.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.dt2s.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.pos.extra.dt2s.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.pos.extra.dt2s.row[i].col)/sizeof(node.loc_std.pos.extra.dt2s.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.pos.extra.dt2s.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.pos.extra.l2e", &node.loc_std.pos.extra.l2e, "quaternion");
            add_name("node.loc_std.pos.extra.l2e.d", &node.loc_std.pos.extra.l2e.d, "cvector");
            add_name("node.loc_std.pos.extra.l2e.d.x", &node.loc_std.pos.extra.l2e.d.x, "double");
            add_name("node.loc_std.pos.extra.l2e.d.y", &node.loc_std.pos.extra.l2e.d.y, "double");
            add_name("node.loc_std.pos.extra.l2e.d.z", &node.loc_std.pos.extra.l2e.d.z, "double");
            add_name("node.loc_std.pos.extra.l2e.w", &node.loc_std.pos.extra.l2e.w, "double");
            add_name("node.loc_std.pos.extra.e2l", &node.loc_std.pos.extra.e2l, "quaternion");
            add_name("node.loc_std.pos.extra.e2l.d", &node.loc_std.pos.extra.e2l.d, "cvector");
            add_name("node.loc_std.pos.extra.e2l.d.x", &node.loc_std.pos.extra.e2l.d.x, "double");
            add_name("node.loc_std.pos.extra.e2l.d.y", &node.loc_std.pos.extra.e2l.d.y, "double");
            add_name("node.loc_std.pos.extra.e2l.d.z", &node.loc_std.pos.extra.e2l.d.z, "double");
            add_name("node.loc_std.pos.extra.e2l.w", &node.loc_std.pos.extra.e2l.w, "double");
            add_name("node.loc_std.pos.extra.sun2earth", &node.loc_std.pos.extra.sun2earth, "cartpos");
            add_name("node.loc_std.pos.extra.sun2earth.utc", &node.loc_std.pos.extra.sun2earth.utc, "double");
            add_name("node.loc_std.pos.extra.sun2earth.s", &node.loc_std.pos.extra.sun2earth.s, "rvector");
            add_name("node.loc_std.pos.extra.sun2earth.s.col", &node.loc_std.pos.extra.sun2earth.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.sun2earth.s.col)/sizeof(node.loc_std.pos.extra.sun2earth.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.extra.sun2earth.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.sun2earth.s.col[i], "double");
            }
            add_name("node.loc_std.pos.extra.sun2earth.v", &node.loc_std.pos.extra.sun2earth.v, "rvector");
            add_name("node.loc_std.pos.extra.sun2earth.v.col", &node.loc_std.pos.extra.sun2earth.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.sun2earth.v.col)/sizeof(node.loc_std.pos.extra.sun2earth.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.extra.sun2earth.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.sun2earth.v.col[i], "double");
            }
            add_name("node.loc_std.pos.extra.sun2earth.a", &node.loc_std.pos.extra.sun2earth.a, "rvector");
            add_name("node.loc_std.pos.extra.sun2earth.a.col", &node.loc_std.pos.extra.sun2earth.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.sun2earth.a.col)/sizeof(node.loc_std.pos.extra.sun2earth.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.extra.sun2earth.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.sun2earth.a.col[i], "double");
            }
            add_name("node.loc_std.pos.extra.sun2earth.pass", &node.loc_std.pos.extra.sun2earth.pass, "uint32_t");
            add_name("node.loc_std.pos.extra.sun2moon", &node.loc_std.pos.extra.sun2moon, "cartpos");
            add_name("node.loc_std.pos.extra.sun2moon.utc", &node.loc_std.pos.extra.sun2moon.utc, "double");
            add_name("node.loc_std.pos.extra.sun2moon.s", &node.loc_std.pos.extra.sun2moon.s, "rvector");
            add_name("node.loc_std.pos.extra.sun2moon.s.col", &node.loc_std.pos.extra.sun2moon.s.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.sun2moon.s.col)/sizeof(node.loc_std.pos.extra.sun2moon.s.col[0]); ++i) {
                string basename = "node.loc_std.pos.extra.sun2moon.s.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.sun2moon.s.col[i], "double");
            }
            add_name("node.loc_std.pos.extra.sun2moon.v", &node.loc_std.pos.extra.sun2moon.v, "rvector");
            add_name("node.loc_std.pos.extra.sun2moon.v.col", &node.loc_std.pos.extra.sun2moon.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.sun2moon.v.col)/sizeof(node.loc_std.pos.extra.sun2moon.v.col[0]); ++i) {
                string basename = "node.loc_std.pos.extra.sun2moon.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.sun2moon.v.col[i], "double");
            }
            add_name("node.loc_std.pos.extra.sun2moon.a", &node.loc_std.pos.extra.sun2moon.a, "rvector");
            add_name("node.loc_std.pos.extra.sun2moon.a.col", &node.loc_std.pos.extra.sun2moon.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.extra.sun2moon.a.col)/sizeof(node.loc_std.pos.extra.sun2moon.a.col[0]); ++i) {
                string basename = "node.loc_std.pos.extra.sun2moon.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.extra.sun2moon.a.col[i], "double");
            }
            add_name("node.loc_std.pos.extra.sun2moon.pass", &node.loc_std.pos.extra.sun2moon.pass, "uint32_t");
            add_name("node.loc_std.pos.extra.closest", &node.loc_std.pos.extra.closest, "uint16_t");
            add_name("node.loc_std.pos.earthsep", &node.loc_std.pos.earthsep, "float");
            add_name("node.loc_std.pos.moonsep", &node.loc_std.pos.moonsep, "float");
            add_name("node.loc_std.pos.sunsize", &node.loc_std.pos.sunsize, "float");
            add_name("node.loc_std.pos.sunradiance", &node.loc_std.pos.sunradiance, "float");
            add_name("node.loc_std.pos.bearth", &node.loc_std.pos.bearth, "rvector");
            add_name("node.loc_std.pos.bearth.col", &node.loc_std.pos.bearth.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.pos.bearth.col)/sizeof(node.loc_std.pos.bearth.col[0]); ++i) {
                string basename = "node.loc_std.pos.bearth.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.pos.bearth.col[i], "double");
            }
            add_name("node.loc_std.pos.orbit", &node.loc_std.pos.orbit, "double");
            add_name("node.loc_std.att", &node.loc_std.att, "attstruc");
            add_name("node.loc_std.att.utc", &node.loc_std.att.utc, "double");
            add_name("node.loc_std.att.topo", &node.loc_std.att.topo, "qatt");
            add_name("node.loc_std.att.topo.utc", &node.loc_std.att.topo.utc, "double");
            add_name("node.loc_std.att.topo.s", &node.loc_std.att.topo.s, "quaternion");
            add_name("node.loc_std.att.topo.s.d", &node.loc_std.att.topo.s.d, "cvector");
            add_name("node.loc_std.att.topo.s.d.x", &node.loc_std.att.topo.s.d.x, "double");
            add_name("node.loc_std.att.topo.s.d.y", &node.loc_std.att.topo.s.d.y, "double");
            add_name("node.loc_std.att.topo.s.d.z", &node.loc_std.att.topo.s.d.z, "double");
            add_name("node.loc_std.att.topo.s.w", &node.loc_std.att.topo.s.w, "double");
            add_name("node.loc_std.att.topo.v", &node.loc_std.att.topo.v, "rvector");
            add_name("node.loc_std.att.topo.v.col", &node.loc_std.att.topo.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.topo.v.col)/sizeof(node.loc_std.att.topo.v.col[0]); ++i) {
                string basename = "node.loc_std.att.topo.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.topo.v.col[i], "double");
            }
            add_name("node.loc_std.att.topo.a", &node.loc_std.att.topo.a, "rvector");
            add_name("node.loc_std.att.topo.a.col", &node.loc_std.att.topo.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.topo.a.col)/sizeof(node.loc_std.att.topo.a.col[0]); ++i) {
                string basename = "node.loc_std.att.topo.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.topo.a.col[i], "double");
            }
            add_name("node.loc_std.att.topo.pass", &node.loc_std.att.topo.pass, "uint32_t");
            add_name("node.loc_std.att.lvlh", &node.loc_std.att.lvlh, "qatt");
            add_name("node.loc_std.att.lvlh.utc", &node.loc_std.att.lvlh.utc, "double");
            add_name("node.loc_std.att.lvlh.s", &node.loc_std.att.lvlh.s, "quaternion");
            add_name("node.loc_std.att.lvlh.s.d", &node.loc_std.att.lvlh.s.d, "cvector");
            add_name("node.loc_std.att.lvlh.s.d.x", &node.loc_std.att.lvlh.s.d.x, "double");
            add_name("node.loc_std.att.lvlh.s.d.y", &node.loc_std.att.lvlh.s.d.y, "double");
            add_name("node.loc_std.att.lvlh.s.d.z", &node.loc_std.att.lvlh.s.d.z, "double");
            add_name("node.loc_std.att.lvlh.s.w", &node.loc_std.att.lvlh.s.w, "double");
            add_name("node.loc_std.att.lvlh.v", &node.loc_std.att.lvlh.v, "rvector");
            add_name("node.loc_std.att.lvlh.v.col", &node.loc_std.att.lvlh.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.lvlh.v.col)/sizeof(node.loc_std.att.lvlh.v.col[0]); ++i) {
                string basename = "node.loc_std.att.lvlh.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.lvlh.v.col[i], "double");
            }
            add_name("node.loc_std.att.lvlh.a", &node.loc_std.att.lvlh.a, "rvector");
            add_name("node.loc_std.att.lvlh.a.col", &node.loc_std.att.lvlh.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.lvlh.a.col)/sizeof(node.loc_std.att.lvlh.a.col[0]); ++i) {
                string basename = "node.loc_std.att.lvlh.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.lvlh.a.col[i], "double");
            }
            add_name("node.loc_std.att.lvlh.pass", &node.loc_std.att.lvlh.pass, "uint32_t");
            add_name("node.loc_std.att.geoc", &node.loc_std.att.geoc, "qatt");
            add_name("node.loc_std.att.geoc.utc", &node.loc_std.att.geoc.utc, "double");
            add_name("node.loc_std.att.geoc.s", &node.loc_std.att.geoc.s, "quaternion");
            add_name("node.loc_std.att.geoc.s.d", &node.loc_std.att.geoc.s.d, "cvector");
            add_name("node.loc_std.att.geoc.s.d.x", &node.loc_std.att.geoc.s.d.x, "double");
            add_name("node.loc_std.att.geoc.s.d.y", &node.loc_std.att.geoc.s.d.y, "double");
            add_name("node.loc_std.att.geoc.s.d.z", &node.loc_std.att.geoc.s.d.z, "double");
            add_name("node.loc_std.att.geoc.s.w", &node.loc_std.att.geoc.s.w, "double");
            add_name("node.loc_std.att.geoc.v", &node.loc_std.att.geoc.v, "rvector");
            add_name("node.loc_std.att.geoc.v.col", &node.loc_std.att.geoc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.geoc.v.col)/sizeof(node.loc_std.att.geoc.v.col[0]); ++i) {
                string basename = "node.loc_std.att.geoc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.geoc.v.col[i], "double");
            }
            add_name("node.loc_std.att.geoc.a", &node.loc_std.att.geoc.a, "rvector");
            add_name("node.loc_std.att.geoc.a.col", &node.loc_std.att.geoc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.geoc.a.col)/sizeof(node.loc_std.att.geoc.a.col[0]); ++i) {
                string basename = "node.loc_std.att.geoc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.geoc.a.col[i], "double");
            }
            add_name("node.loc_std.att.geoc.pass", &node.loc_std.att.geoc.pass, "uint32_t");
            add_name("node.loc_std.att.selc", &node.loc_std.att.selc, "qatt");
            add_name("node.loc_std.att.selc.utc", &node.loc_std.att.selc.utc, "double");
            add_name("node.loc_std.att.selc.s", &node.loc_std.att.selc.s, "quaternion");
            add_name("node.loc_std.att.selc.s.d", &node.loc_std.att.selc.s.d, "cvector");
            add_name("node.loc_std.att.selc.s.d.x", &node.loc_std.att.selc.s.d.x, "double");
            add_name("node.loc_std.att.selc.s.d.y", &node.loc_std.att.selc.s.d.y, "double");
            add_name("node.loc_std.att.selc.s.d.z", &node.loc_std.att.selc.s.d.z, "double");
            add_name("node.loc_std.att.selc.s.w", &node.loc_std.att.selc.s.w, "double");
            add_name("node.loc_std.att.selc.v", &node.loc_std.att.selc.v, "rvector");
            add_name("node.loc_std.att.selc.v.col", &node.loc_std.att.selc.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.selc.v.col)/sizeof(node.loc_std.att.selc.v.col[0]); ++i) {
                string basename = "node.loc_std.att.selc.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.selc.v.col[i], "double");
            }
            add_name("node.loc_std.att.selc.a", &node.loc_std.att.selc.a, "rvector");
            add_name("node.loc_std.att.selc.a.col", &node.loc_std.att.selc.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.selc.a.col)/sizeof(node.loc_std.att.selc.a.col[0]); ++i) {
                string basename = "node.loc_std.att.selc.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.selc.a.col[i], "double");
            }
            add_name("node.loc_std.att.selc.pass", &node.loc_std.att.selc.pass, "uint32_t");
            add_name("node.loc_std.att.icrf", &node.loc_std.att.icrf, "qatt");
            add_name("node.loc_std.att.icrf.utc", &node.loc_std.att.icrf.utc, "double");
            add_name("node.loc_std.att.icrf.s", &node.loc_std.att.icrf.s, "quaternion");
            add_name("node.loc_std.att.icrf.s.d", &node.loc_std.att.icrf.s.d, "cvector");
            add_name("node.loc_std.att.icrf.s.d.x", &node.loc_std.att.icrf.s.d.x, "double");
            add_name("node.loc_std.att.icrf.s.d.y", &node.loc_std.att.icrf.s.d.y, "double");
            add_name("node.loc_std.att.icrf.s.d.z", &node.loc_std.att.icrf.s.d.z, "double");
            add_name("node.loc_std.att.icrf.s.w", &node.loc_std.att.icrf.s.w, "double");
            add_name("node.loc_std.att.icrf.v", &node.loc_std.att.icrf.v, "rvector");
            add_name("node.loc_std.att.icrf.v.col", &node.loc_std.att.icrf.v.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.icrf.v.col)/sizeof(node.loc_std.att.icrf.v.col[0]); ++i) {
                string basename = "node.loc_std.att.icrf.v.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.icrf.v.col[i], "double");
            }
            add_name("node.loc_std.att.icrf.a", &node.loc_std.att.icrf.a, "rvector");
            add_name("node.loc_std.att.icrf.a.col", &node.loc_std.att.icrf.a.col, "double[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.icrf.a.col)/sizeof(node.loc_std.att.icrf.a.col[0]); ++i) {
                string basename = "node.loc_std.att.icrf.a.col[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.icrf.a.col[i], "double");
            }
            add_name("node.loc_std.att.icrf.pass", &node.loc_std.att.icrf.pass, "uint32_t");
            add_name("node.loc_std.att.extra", &node.loc_std.att.extra, "extraatt");
            add_name("node.loc_std.att.extra.utc", &node.loc_std.att.extra.utc, "double");
            add_name("node.loc_std.att.extra.j2b", &node.loc_std.att.extra.j2b, "rmatrix");
            add_name("node.loc_std.att.extra.j2b.row", &node.loc_std.att.extra.j2b.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.extra.j2b.row)/sizeof(node.loc_std.att.extra.j2b.row[0]); ++i) {
                string basename = "node.loc_std.att.extra.j2b.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.extra.j2b.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.att.extra.j2b.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.att.extra.j2b.row[i].col)/sizeof(node.loc_std.att.extra.j2b.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.att.extra.j2b.row[i].col[j], "double");
                }
            }
            add_name("node.loc_std.att.extra.b2j", &node.loc_std.att.extra.b2j, "rmatrix");
            add_name("node.loc_std.att.extra.b2j.row", &node.loc_std.att.extra.b2j.row, "rvector[]");
            for(size_t i = 0; i < sizeof(node.loc_std.att.extra.b2j.row)/sizeof(node.loc_std.att.extra.b2j.row[0]); ++i) {
                string basename = "node.loc_std.att.extra.b2j.row[" + std::to_string(i) + "]";
                add_name(basename, &node.loc_std.att.extra.b2j.row[i], "rvector");
                add_name(basename+".col", &node.loc_std.att.extra.b2j.row[i].col, "double[]");
                for(size_t j = 0; j < sizeof(node.loc_std.att.extra.b2j.row[i].col)/sizeof(node.loc_std.att.extra.b2j.row[i].col[0]); ++j) {
                    string rebasename = basename + ".col[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.loc_std.att.extra.b2j.row[i].col[j], "double");
                }
            }
            add_name("node.phys", &node.phys, "physicsstruc");
            add_name("node.phys.dt", &node.phys.dt, "double");
            add_name("node.phys.dtj", &node.phys.dtj, "double");
            add_name("node.phys.utc", &node.phys.utc, "double");
            add_name("node.phys.mjdaccel", &node.phys.mjdaccel, "double");
            add_name("node.phys.mjddiff", &node.phys.mjddiff, "double");
            add_name("node.phys.hcap", &node.phys.hcap, "float");
            add_name("node.phys.mass", &node.phys.mass, "float");
            add_name("node.phys.temp", &node.phys.temp, "float");
            add_name("node.phys.heat", &node.phys.heat, "float");
            add_name("node.phys.area", &node.phys.area, "float");
            add_name("node.phys.battcap", &node.phys.battcap, "float");
            add_name("node.phys.battlev", &node.phys.battlev, "float");
            add_name("node.phys.powgen", &node.phys.powgen, "float");
            add_name("node.phys.powuse", &node.phys.powuse, "float");
            add_name("node.phys.mode", &node.phys.mode, "int32_t");
            add_name("node.phys.ftorque", &node.phys.ftorque, "Vector");
            add_name("node.phys.ftorque.x", &node.phys.ftorque.x, "double");
            add_name("node.phys.ftorque.y", &node.phys.ftorque.y, "double");
            add_name("node.phys.ftorque.z", &node.phys.ftorque.z, "double");
            add_name("node.phys.ftorque.w", &node.phys.ftorque.w, "double");
            add_name("node.phys.atorque", &node.phys.atorque, "Vector");
            add_name("node.phys.atorque.x", &node.phys.atorque.x, "double");
            add_name("node.phys.atorque.y", &node.phys.atorque.y, "double");
            add_name("node.phys.atorque.z", &node.phys.atorque.z, "double");
            add_name("node.phys.atorque.w", &node.phys.atorque.w, "double");
            add_name("node.phys.rtorque", &node.phys.rtorque, "Vector");
            add_name("node.phys.rtorque.x", &node.phys.rtorque.x, "double");
            add_name("node.phys.rtorque.y", &node.phys.rtorque.y, "double");
            add_name("node.phys.rtorque.z", &node.phys.rtorque.z, "double");
            add_name("node.phys.rtorque.w", &node.phys.rtorque.w, "double");
            add_name("node.phys.gtorque", &node.phys.gtorque, "Vector");
            add_name("node.phys.gtorque.x", &node.phys.gtorque.x, "double");
            add_name("node.phys.gtorque.y", &node.phys.gtorque.y, "double");
            add_name("node.phys.gtorque.z", &node.phys.gtorque.z, "double");
            add_name("node.phys.gtorque.w", &node.phys.gtorque.w, "double");
            add_name("node.phys.htorque", &node.phys.htorque, "Vector");
            add_name("node.phys.htorque.x", &node.phys.htorque.x, "double");
            add_name("node.phys.htorque.y", &node.phys.htorque.y, "double");
            add_name("node.phys.htorque.z", &node.phys.htorque.z, "double");
            add_name("node.phys.htorque.w", &node.phys.htorque.w, "double");
            add_name("node.phys.hmomentum", &node.phys.hmomentum, "Vector");
            add_name("node.phys.hmomentum.x", &node.phys.hmomentum.x, "double");
            add_name("node.phys.hmomentum.y", &node.phys.hmomentum.y, "double");
            add_name("node.phys.hmomentum.z", &node.phys.hmomentum.z, "double");
            add_name("node.phys.hmomentum.w", &node.phys.hmomentum.w, "double");
            add_name("node.phys.ctorque", &node.phys.ctorque, "Vector");
            add_name("node.phys.ctorque.x", &node.phys.ctorque.x, "double");
            add_name("node.phys.ctorque.y", &node.phys.ctorque.y, "double");
            add_name("node.phys.ctorque.z", &node.phys.ctorque.z, "double");
            add_name("node.phys.ctorque.w", &node.phys.ctorque.w, "double");
            add_name("node.phys.fdrag", &node.phys.fdrag, "Vector");
            add_name("node.phys.fdrag.x", &node.phys.fdrag.x, "double");
            add_name("node.phys.fdrag.y", &node.phys.fdrag.y, "double");
            add_name("node.phys.fdrag.z", &node.phys.fdrag.z, "double");
            add_name("node.phys.fdrag.w", &node.phys.fdrag.w, "double");
            add_name("node.phys.adrag", &node.phys.adrag, "Vector");
            add_name("node.phys.adrag.x", &node.phys.adrag.x, "double");
            add_name("node.phys.adrag.y", &node.phys.adrag.y, "double");
            add_name("node.phys.adrag.z", &node.phys.adrag.z, "double");
            add_name("node.phys.adrag.w", &node.phys.adrag.w, "double");
            add_name("node.phys.rdrag", &node.phys.rdrag, "Vector");
            add_name("node.phys.rdrag.x", &node.phys.rdrag.x, "double");
            add_name("node.phys.rdrag.y", &node.phys.rdrag.y, "double");
            add_name("node.phys.rdrag.z", &node.phys.rdrag.z, "double");
            add_name("node.phys.rdrag.w", &node.phys.rdrag.w, "double");
            add_name("node.phys.thrust", &node.phys.thrust, "Vector");
            add_name("node.phys.thrust.x", &node.phys.thrust.x, "double");
            add_name("node.phys.thrust.y", &node.phys.thrust.y, "double");
            add_name("node.phys.thrust.z", &node.phys.thrust.z, "double");
            add_name("node.phys.thrust.w", &node.phys.thrust.w, "double");
            add_name("node.phys.moi", &node.phys.moi, "Vector");
            add_name("node.phys.moi.x", &node.phys.moi.x, "double");
            add_name("node.phys.moi.y", &node.phys.moi.y, "double");
            add_name("node.phys.moi.z", &node.phys.moi.z, "double");
            add_name("node.phys.moi.w", &node.phys.moi.w, "double");
            add_name("node.phys.com", &node.phys.com, "Vector");
            add_name("node.phys.com.x", &node.phys.com.x, "double");
            add_name("node.phys.com.y", &node.phys.com.y, "double");
            add_name("node.phys.com.z", &node.phys.com.z, "double");
            add_name("node.phys.com.w", &node.phys.com.w, "double");
            add_name("node.phys.vertices", &node.phys.vertices, "vector<Vector>");
            for(size_t i = 0; i < node.phys.vertices.size(); ++i) {
                string basename = "node.phys.vertices[" + std::to_string(i) + "]";
                add_name(basename, &node.phys.vertices[i], "Vector");
                add_name(basename+".x", &node.phys.vertices[i].x, "double");
                add_name(basename+".y", &node.phys.vertices[i].y, "double");
                add_name(basename+".z", &node.phys.vertices[i].z, "double");
                add_name(basename+".w", &node.phys.vertices[i].w, "double");
            }
            add_name("node.phys.triangles", &node.phys.triangles, "vector<trianglestruc>");
            for(size_t i = 0; i < node.phys.triangles.size(); ++i) {
                string basename = "node.phys.triangles[" + std::to_string(i) + "]";
                add_name(basename, &node.phys.triangles[i], "trianglestruc");
                add_name(basename+".external", &node.phys.triangles[i].external, "bool");
                add_name(basename+".com", &node.phys.triangles[i].com, "Vector");
                add_name(basename+".com.x", &node.phys.triangles[i].com.x, "double");
                add_name(basename+".com.y", &node.phys.triangles[i].com.y, "double");
                add_name(basename+".com.z", &node.phys.triangles[i].com.z, "double");
                add_name(basename+".com.w", &node.phys.triangles[i].com.w, "double");
                add_name(basename+".normal", &node.phys.triangles[i].normal, "Vector");
                add_name(basename+".normal.x", &node.phys.triangles[i].normal.x, "double");
                add_name(basename+".normal.y", &node.phys.triangles[i].normal.y, "double");
                add_name(basename+".normal.z", &node.phys.triangles[i].normal.z, "double");
                add_name(basename+".normal.w", &node.phys.triangles[i].normal.w, "double");
                add_name(basename+".shove", &node.phys.triangles[i].shove, "Vector");
                add_name(basename+".shove.x", &node.phys.triangles[i].shove.x, "double");
                add_name(basename+".shove.y", &node.phys.triangles[i].shove.y, "double");
                add_name(basename+".shove.z", &node.phys.triangles[i].shove.z, "double");
                add_name(basename+".shove.w", &node.phys.triangles[i].shove.w, "double");
                add_name(basename+".twist", &node.phys.triangles[i].twist, "Vector");
                add_name(basename+".twist.x", &node.phys.triangles[i].twist.x, "double");
                add_name(basename+".twist.y", &node.phys.triangles[i].twist.y, "double");
                add_name(basename+".twist.z", &node.phys.triangles[i].twist.z, "double");
                add_name(basename+".twist.w", &node.phys.triangles[i].twist.w, "double");
                add_name(basename+".pidx", &node.phys.triangles[i].pidx, "uint16_t");
                add_name(basename+".tidx", &node.phys.triangles[i].tidx, "uint16_t[]");
                for(size_t j = 0; j < sizeof(node.phys.triangles[i].tidx)/sizeof(node.phys.triangles[i].tidx[0]); ++j) {
                    string rebasename = basename + ".tidx[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.phys.triangles[i].tidx[j], "uint16_t");
                }
                add_name(basename+".heat", &node.phys.triangles[i].heat, "float");
                add_name(basename+".hcap", &node.phys.triangles[i].hcap, "float");
                add_name(basename+".emi", &node.phys.triangles[i].emi, "float");
                add_name(basename+".abs", &node.phys.triangles[i].abs, "float");
                add_name(basename+".mass", &node.phys.triangles[i].mass, "float");
                add_name(basename+".temp", &node.phys.triangles[i].temp, "float");
                add_name(basename+".area", &node.phys.triangles[i].area, "float");
                add_name(basename+".perimeter", &node.phys.triangles[i].perimeter, "float");
                add_name(basename+".sirradiation", &node.phys.triangles[i].sirradiation, "float");
                add_name(basename+".eirradiation", &node.phys.triangles[i].eirradiation, "float");
                add_name(basename+".pcell", &node.phys.triangles[i].pcell, "float");
                add_name(basename+".ecellbase", &node.phys.triangles[i].ecellbase, "float");
                add_name(basename+".ecellslope", &node.phys.triangles[i].ecellslope, "float");
                add_name(basename+".triangleindex", &node.phys.triangles[i].triangleindex, "vector<vector<uint16_t>>");
                for(size_t j = 0; j < node.phys.triangles[i].triangleindex.size(); ++j) {
                    string rebasename = basename + ".triangleindex[" + std::to_string(j) + "]";
                    add_name(rebasename, &node.phys.triangles[i].triangleindex[j], "vector<uint16_t>");
                    for(size_t k = 0; k < node.phys.triangles[i].triangleindex[j].size(); ++k) {
                        string rebasename2 = rebasename + "[" + std::to_string(k) + "]";
                        add_name(rebasename2, &node.phys.triangles[i].triangleindex[j][k], "uint16_t");
                    }
                }
            }


            // vector<vertexstruc> vertexs
            add_name("vertexs", &vertexs, "vector<vertexstruc>");
            for(size_t i = 0; i < vertexs.size(); ++i) {
                string basename = "vertexs[" + std::to_string(i) + "]";
                add_name(basename, &vertexs[i], "vertexstruc");
                add_name(basename+".x", &vertexs[i].x, "double");
                add_name(basename+".y", &vertexs[i].y, "double");
                add_name(basename+".z", &vertexs[i].z, "double");
                add_name(basename+".w", &vertexs[i].w, "double");
            }

            // vector<vertexstruc> normals
            add_name("normals", &normals, "vector<vertexstruc>");
            for(size_t i = 0; i < normals.size(); ++i) {
                string basename = "normals[" + std::to_string(i) + "]";
                add_name(basename, &normals[i], "vertexstruc");
                add_name(basename+".x", &normals[i].x, "double");
                add_name(basename+".y", &normals[i].y, "double");
                add_name(basename+".z", &normals[i].z, "double");
                add_name(basename+".w", &normals[i].w, "double");
            }

            // vector<facestruc> faces
            add_name("faces", &faces, "vector<facestruc>");
            for(size_t i = 0; i < faces.size(); ++i) {
                string basename = "faces[" + std::to_string(i) + "]";
                add_name(basename, &faces[i], "facestruc");
                add_name(basename+".vertex_cnt", &faces[i].vertex_cnt, "uint16_t");
                add_name(basename+".vertex_idx", &faces[i].vertex_idx, "vector<uint16_t>");
                for(size_t j = 0; j < faces[i].vertex_idx.size(); ++j) {
                    string rebasename = basename + ".vertex_idx[" + std::to_string(j) + "]";
                    add_name(rebasename, &faces[i].vertex_idx[j], "uint16_t");
                }
                add_name(basename+".com", &faces[i].com, "Vector");
                add_name(basename+".com.x", &faces[i].com.x, "double");
                add_name(basename+".com.y", &faces[i].com.y, "double");
                add_name(basename+".com.z", &faces[i].com.z, "double");
                add_name(basename+".com.w", &faces[i].com.w, "double");
                add_name(basename+".normal", &faces[i].normal, "Vector");
                add_name(basename+".normal.x", &faces[i].normal.x, "double");
                add_name(basename+".normal.y", &faces[i].normal.y, "double");
                add_name(basename+".normal.z", &faces[i].normal.z, "double");
                add_name(basename+".normal.w", &faces[i].normal.w, "double");
                add_name(basename+".area", &faces[i].area, "double");
            }

            // vector<piecestruc> pieces
            add_name("pieces", &pieces, "vector<piecestruc>");
            for(size_t i = 0; i < pieces.size(); ++i) {
                string basename = "pieces[" + std::to_string(i) + "]";
                add_name(basename, &pieces[i], "piecestruc");
                add_name(basename+".name", &pieces[i].name, "string");
                add_name(basename+".enabled", &pieces[i].enabled, "bool");
                add_name(basename+".cidx", &pieces[i].cidx, "uint16_t");
                add_name(basename+".density", &pieces[i].density, "float");
                add_name(basename+".mass", &pieces[i].mass, "float");
                add_name(basename+".emi", &pieces[i].emi, "float");
                add_name(basename+".abs", &pieces[i].abs, "float");
                add_name(basename+".hcap", &pieces[i].hcap, "float");
                add_name(basename+".hcon", &pieces[i].hcon, "float");
                add_name(basename+".dim", &pieces[i].dim, "float");
                add_name(basename+".area", &pieces[i].area, "float");
                add_name(basename+".volume", &pieces[i].volume, "float");
                add_name(basename+".face_cnt", &pieces[i].face_cnt, "uint16_t");
                add_name(basename+".face_idx", &pieces[i].face_idx, "vector<uint16_t>");
                for(size_t j = 0; j < pieces[i].face_idx.size(); ++j) {
                    string rebasename = basename + ".face_idx[" + std::to_string(j) + "]";
                    add_name(rebasename, &pieces[i].face_idx[j], "uint16_t");
                }
                add_name(basename+".com", &pieces[i].com, "Vector");
                add_name(basename+".com.x", &pieces[i].com.x, "double");
                add_name(basename+".com.y", &pieces[i].com.y, "double");
                add_name(basename+".com.z", &pieces[i].com.z, "double");
                add_name(basename+".com.w", &pieces[i].com.w, "double");
                add_name(basename+".shove", &pieces[i].shove, "Vector");
                add_name(basename+".shove.x", &pieces[i].shove.x, "double");
                add_name(basename+".shove.y", &pieces[i].shove.y, "double");
                add_name(basename+".shove.z", &pieces[i].shove.z, "double");
                add_name(basename+".shove.w", &pieces[i].shove.w, "double");
                add_name(basename+".twist", &pieces[i].twist, "Vector");
                add_name(basename+".twist.x", &pieces[i].twist.x, "double");
                add_name(basename+".twist.y", &pieces[i].twist.y, "double");
                add_name(basename+".twist.z", &pieces[i].twist.z, "double");
                add_name(basename+".twist.w", &pieces[i].twist.w, "double");
                add_name(basename+".heat", &pieces[i].heat, "float");
                add_name(basename+".temp", &pieces[i].temp, "float");
                add_name(basename+".insol", &pieces[i].insol, "float");
                add_name(basename+".material_density", &pieces[i].material_density, "float");
                add_name(basename+".material_ambient", &pieces[i].material_ambient, "Vector");
                add_name(basename+".material_ambient.x", &pieces[i].material_ambient.x, "double");
                add_name(basename+".material_ambient.y", &pieces[i].material_ambient.y, "double");
                add_name(basename+".material_ambient.z", &pieces[i].material_ambient.z, "double");
                add_name(basename+".material_ambient.w", &pieces[i].material_ambient.w, "double");
                add_name(basename+".material_diffuse", &pieces[i].material_diffuse, "Vector");
                add_name(basename+".material_diffuse.x", &pieces[i].material_diffuse.x, "double");
                add_name(basename+".material_diffuse.y", &pieces[i].material_diffuse.y, "double");
                add_name(basename+".material_diffuse.z", &pieces[i].material_diffuse.z, "double");
                add_name(basename+".material_diffuse.w", &pieces[i].material_diffuse.w, "double");
                add_name(basename+".material_specular", &pieces[i].material_specular, "Vector");
                add_name(basename+".material_specular.x", &pieces[i].material_specular.x, "double");
                add_name(basename+".material_specular.y", &pieces[i].material_specular.y, "double");
                add_name(basename+".material_specular.z", &pieces[i].material_specular.z, "double");
                add_name(basename+".material_specular.w", &pieces[i].material_specular.w, "double");
            }

            // wavefront obj
            add_name("obj", &obj, "wavefront");
            add_name("obj.Vg", &obj.Vg, "vector<Vector>");
            for(size_t i = 0; i < obj.Vg.size(); ++i) {
                string basename = "obj.Vg[" + std::to_string(i) + "]";
                add_name(basename, &obj.Vg[i], "Vector");
                add_name(basename+".x", &obj.Vg[i].x, "double");
                add_name(basename+".y", &obj.Vg[i].y, "double");
                add_name(basename+".z", &obj.Vg[i].z, "double");
                add_name(basename+".w", &obj.Vg[i].w, "double");
            }
            add_name("obj.Vt", &obj.Vt, "vector<Vector>");
            for(size_t i = 0; i < obj.Vt.size(); ++i) {
                string basename = "obj.Vt[" + std::to_string(i) + "]";
                add_name(basename, &obj.Vt[i], "Vector");
                add_name(basename+".x", &obj.Vt[i].x, "double");
                add_name(basename+".y", &obj.Vt[i].y, "double");
                add_name(basename+".z", &obj.Vt[i].z, "double");
                add_name(basename+".w", &obj.Vt[i].w, "double");
            }
            add_name("obj.Vn", &obj.Vn, "vector<Vector>");
            for(size_t i = 0; i < obj.Vn.size(); ++i) {
                string basename = "obj.Vn[" + std::to_string(i) + "]";
                add_name(basename, &obj.Vn[i], "Vector");
                add_name(basename+".x", &obj.Vn[i].x, "double");
                add_name(basename+".y", &obj.Vn[i].y, "double");
                add_name(basename+".z", &obj.Vn[i].z, "double");
                add_name(basename+".w", &obj.Vn[i].w, "double");
            }
            add_name("obj.Vp", &obj.Vp, "vector<Vector>");
            for(size_t i = 0; i < obj.Vp.size(); ++i) {
                string basename = "obj.Vp[" + std::to_string(i) + "]";
                add_name(basename, &obj.Vp[i], "Vector");
                add_name(basename+".x", &obj.Vp[i].x, "double");
                add_name(basename+".y", &obj.Vp[i].y, "double");
                add_name(basename+".z", &obj.Vp[i].z, "double");
                add_name(basename+".w", &obj.Vp[i].w, "double");
            }
            add_name("obj.Materials", &obj.Materials, "vector<material>");
            for(size_t i = 0; i < obj.Materials.size(); ++i) {
                string basename = "obj.Materials[" + std::to_string(i) + "]";
                add_name(basename, &obj.Materials[i], "material");
                add_name(basename+".name", &obj.Materials[i].name, "string");
                add_name(basename+".density", &obj.Materials[i].density, "float");
                add_name(basename+".ambient", &obj.Materials[i].ambient, "Vector");
                add_name(basename+".ambient.x", &obj.Materials[i].ambient.x, "double");
                add_name(basename+".ambient.y", &obj.Materials[i].ambient.y, "double");
                add_name(basename+".ambient.z", &obj.Materials[i].ambient.z, "double");
                add_name(basename+".ambient.w", &obj.Materials[i].ambient.w, "double");
                add_name(basename+".diffuse", &obj.Materials[i].diffuse, "Vector");
                add_name(basename+".diffuse.x", &obj.Materials[i].diffuse.x, "double");
                add_name(basename+".diffuse.y", &obj.Materials[i].diffuse.y, "double");
                add_name(basename+".diffuse.z", &obj.Materials[i].diffuse.z, "double");
                add_name(basename+".diffuse.w", &obj.Materials[i].diffuse.w, "double");
                add_name(basename+".specular", &obj.Materials[i].specular, "Vector");
                add_name(basename+".specular.x", &obj.Materials[i].specular.x, "double");
                add_name(basename+".specular.y", &obj.Materials[i].specular.y, "double");
                add_name(basename+".specular.z", &obj.Materials[i].specular.z, "double");
                add_name(basename+".specular.w", &obj.Materials[i].specular.w, "double");
            }
            add_name("obj.Points", &obj.Points, "vector<point>");
            for(size_t i = 0; i < obj.Points.size(); ++i) {
                string basename = "obj.Points[" + std::to_string(i) + "]";
                add_name(basename, &obj.Points[i], "point");
                add_name(basename+".groups", &obj.Points[i].groups, "vector<size_t>");
                for(size_t j = 0; j < obj.Points[i].groups.size(); ++j) {
                    string rebasename = basename + ".groups[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Points[i].groups[j], "size_t");
                }
                add_name(basename+".vertex", &obj.Points[i].vertex, "size_t");
            }
            add_name("obj.Lines", &obj.Lines, "vector<line>");
            for(size_t i = 0; i < obj.Lines.size(); ++i) {
                string basename = "obj.Lines[" + std::to_string(i) + "]";
                add_name(basename, &obj.Lines[i], "line");
                add_name(basename+".groups", &obj.Lines[i].groups, "vector<size_t>");
                for(size_t j = 0; j < obj.Lines[i].groups.size(); ++j) {
                    string rebasename = basename + ".groups[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Lines[i].groups[j], "size_t");
                }
                add_name(basename+".vertices", &obj.Lines[i].vertices, "vector<vertex>");
                for(size_t j = 0; j < obj.Lines[i].vertices.size(); ++j) {
                    string rebasename = basename + ".vertices[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Lines[i].vertices[j], "vertex");
                    add_name(rebasename+".v", &obj.Lines[i].vertices[j].v, "size_t");
                    add_name(rebasename+".vt", &obj.Lines[i].vertices[j].vt, "size_t");
                    add_name(rebasename+".vn", &obj.Lines[i].vertices[j].vn, "size_t");
                }
                add_name(basename+".centroid", &obj.Lines[i].centroid, "Vector");
                add_name(basename+".centroid.x", &obj.Lines[i].centroid.x, "double");
                add_name(basename+".centroid.y", &obj.Lines[i].centroid.y, "double");
                add_name(basename+".centroid.z", &obj.Lines[i].centroid.z, "double");
                add_name(basename+".centroid.w", &obj.Lines[i].centroid.w, "double");
                add_name(basename+".length", &obj.Lines[i].length, "double");
            }
            add_name("obj.Faces", &obj.Faces, "vector<face>");
            for(size_t i = 0; i < obj.Faces.size(); ++i) {
                string basename = "obj.Faces[" + std::to_string(i) + "]";
                add_name(basename, &obj.Faces[i], "face");
                add_name(basename+".groups", &obj.Faces[i].groups, "vector<size_t>");
                for(size_t j = 0; j < obj.Faces[i].groups.size(); ++j) {
                    string rebasename = basename + ".groups[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Faces[i].groups[j], "size_t");
                }
                add_name(basename+".vertices", &obj.Faces[i].vertices, "vector<vertex>");
                for(size_t j = 0; j < obj.Faces[i].vertices.size(); ++j) {
                    string rebasename = basename + ".vertices[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Faces[i].vertices[j], "vertex");
                    add_name(rebasename+".v", &obj.Faces[i].vertices[j].v, "size_t");
                    add_name(rebasename+".vt", &obj.Faces[i].vertices[j].vt, "size_t");
                    add_name(rebasename+".vn", &obj.Faces[i].vertices[j].vn, "size_t");
                }
                add_name(basename+".com", &obj.Faces[i].com, "Vector");
                add_name(basename+".com.x", &obj.Faces[i].com.x, "double");
                add_name(basename+".com.y", &obj.Faces[i].com.y, "double");
                add_name(basename+".com.z", &obj.Faces[i].com.z, "double");
                add_name(basename+".com.w", &obj.Faces[i].com.w, "double");
                add_name(basename+".normal", &obj.Faces[i].normal, "Vector");
                add_name(basename+".normal.x", &obj.Faces[i].normal.x, "double");
                add_name(basename+".normal.y", &obj.Faces[i].normal.y, "double");
                add_name(basename+".normal.z", &obj.Faces[i].normal.z, "double");
                add_name(basename+".normal.w", &obj.Faces[i].normal.w, "double");
                add_name(basename+".area", &obj.Faces[i].area, "double");
            }
            add_name("obj.Groups", &obj.Groups, "vector<group>");
            for(size_t i = 0; i < obj.Groups.size(); ++i) {
                string basename = "obj.Groups[" + std::to_string(i) + "]";
                add_name(basename, &obj.Groups[i], "group");
                add_name(basename+".name", &obj.Groups[i].name, "string");
                add_name(basename+".materialidx", &obj.Groups[i].materialidx, "size_t");
                add_name(basename+".pointidx", &obj.Groups[i].pointidx, "vector<size_t>");
                for(size_t j = 0; j < obj.Groups[i].pointidx.size(); ++j) {
                    string rebasename = basename + ".pointidx[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Groups[i].pointidx[j], "size_t");
                }
                add_name(basename+".lineidx", &obj.Groups[i].lineidx, "vector<size_t>");
                for(size_t j = 0; j < obj.Groups[i].lineidx.size(); ++j) {
                    string rebasename = basename + ".lineidx[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Groups[i].lineidx[j], "size_t");
                }
                add_name(basename+".faceidx", &obj.Groups[i].faceidx, "vector<size_t>");
                for(size_t j = 0; j < obj.Groups[i].faceidx.size(); ++j) {
                    string rebasename = basename + ".faceidx[" + std::to_string(j) + "]";
                    add_name(rebasename, &obj.Groups[i].faceidx[j], "size_t");
                }
                add_name(basename+".com", &obj.Groups[i].com, "Vector");
                add_name(basename+".com.x", &obj.Groups[i].com.x, "double");
                add_name(basename+".com.y", &obj.Groups[i].com.y, "double");
                add_name(basename+".com.z", &obj.Groups[i].com.z, "double");
                add_name(basename+".com.w", &obj.Groups[i].com.w, "double");
                add_name(basename+".volume", &obj.Groups[i].volume, "double");
            }

            // vector<devicestruc*> device
            add_name("device", &device, "vector<devicestruc*>");
            for(size_t i = 0; i < device.size(); ++i)
            {
                string basename = "device[" + std::to_string(i) + "]";
                add_name(basename, &device[i], "devicestruc");
                add_name(basename+".name", &device[i]->name, "string");
                add_name(basename+".state", &device[i]->state, "uint8_t");
                add_name(basename+".type", &device[i]->type, "uint16_t");
                add_name(basename+".model", &device[i]->model, "uint16_t");
                add_name(basename+".flag", &device[i]->flag, "uint32_t");
                add_name(basename+".addr", &device[i]->addr, "uint16_t");
                add_name(basename+".cidx", &device[i]->cidx, "uint16_t");
                add_name(basename+".didx", &device[i]->didx, "uint16_t");
                add_name(basename+".pidx", &device[i]->pidx, "uint16_t");
                add_name(basename+".bidx", &device[i]->bidx, "uint16_t");
                add_name(basename+".portidx", &device[i]->portidx, "uint16_t");
                add_name(basename+".namp", &device[i]->namp, "float");
                add_name(basename+".nvolt", &device[i]->nvolt, "float");
                add_name(basename+".amp", &device[i]->amp, "float");
                add_name(basename+".volt", &device[i]->volt, "float");
                add_name(basename+".power", &device[i]->power, "float");
                add_name(basename+".energy", &device[i]->energy, "float");
                add_name(basename+".drate", &device[i]->drate, "float");
                add_name(basename+".temp", &device[i]->temp, "float");
                add_name(basename+".utc", &device[i]->utc, "double");
                add_name(basename+".expiration", &device[i]->expiration, "double");

                size_t didx = device[i]->didx;
                switch (device[i]->type)
                {
                case DeviceType::ANT:
                    basename = "devspec.ant[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.ant[didx], "antstruc");
                    add_name(basename+".align", &devspec.ant[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.ant[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.ant[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.ant[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.ant[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.ant[didx].align.w, "double");
                    add_name(basename+".azim", &devspec.ant[didx].azim, "float");
                    add_name(basename+".elev", &devspec.ant[didx].elev, "float");
                    add_name(basename+".minelev", &devspec.ant[didx].minelev, "float");
                    add_name(basename+".maxelev", &devspec.ant[didx].maxelev, "float");
                    add_name(basename+".minazim", &devspec.ant[didx].minazim, "float");
                    add_name(basename+".maxazim", &devspec.ant[didx].maxazim, "float");
                    add_name(basename+".threshelev", &devspec.ant[didx].threshelev, "float");
                    break;
                case DeviceType::BATT:
                    basename = "devspec.batt[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.batt[didx], "battstruc");
                    add_name(basename+".maxvolt", &devspec.batt[didx].maxvolt, "float");
                    add_name(basename+".minvolt", &devspec.batt[didx].minvolt, "float");
                    add_name(basename+".cap", &devspec.batt[didx].capacity, "float");
                    add_name(basename+".eff", &devspec.batt[didx].efficiency, "float");
                    add_name(basename+".charge", &devspec.batt[didx].charge, "float");
                    add_name(basename+".celltemp", &devspec.batt[didx].celltemp, "float");
                    add_name(basename+".rin", &devspec.batt[didx].rin, "float");
                    add_name(basename+".rout", &devspec.batt[didx].rout, "float");
                    add_name(basename+".percentage", &devspec.batt[didx].percentage, "float");
                    add_name(basename+".time_remaining", &devspec.batt[didx].time_remaining, "float");
                    break;
                case DeviceType::BCREG:
                    basename = "devspec.bcreg[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.bcreg[didx], "bcregstruc");
                    add_name(basename+".mpptin_volt", &devspec.bcreg[didx].mpptin_volt, "float");
                    add_name(basename+".mpptin_amp", &devspec.bcreg[didx].mpptin_amp, "float");
                    add_name(basename+".mpptout_volt", &devspec.bcreg[didx].mpptout_volt, "float");
                    add_name(basename+".mpptout_amp", &devspec.bcreg[didx].mpptout_amp, "float");
                    break;
                case DeviceType::BUS:
                    basename = "devspec.bus[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.bus[didx], "busstruc");
                    add_name(basename+".wdt", &devspec.bus[didx].wdt, "float");
                    break;
                case DeviceType::CAM:
                    basename = "devspec.cam[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.cam[didx], "camstruc");
                    add_name(basename+".lstep", &devspec.cam[didx].lstep, "uint16_t");
                    add_name(basename+".pwidth", &devspec.cam[didx].pwidth, "uint16_t");
                    add_name(basename+".pheight", &devspec.cam[didx].pheight, "uint16_t");
                    add_name(basename+".width", &devspec.cam[didx].width, "float");
                    add_name(basename+".height", &devspec.cam[didx].height, "float");
                    add_name(basename+".flength", &devspec.cam[didx].flength, "float");
                    add_name(basename+".ltemp", &devspec.cam[didx].ltemp, "float");
                    add_name(basename+".ttemp", &devspec.cam[didx].ttemp, "float");
                    add_name(basename+".fov", &devspec.cam[didx].fov, "float");
                    add_name(basename+".ifov", &devspec.cam[didx].ifov, "float");
                    add_name(basename+".specmin", &devspec.cam[didx].specmin, "float");
                    add_name(basename+".specmax", &devspec.cam[didx].specmax, "float");
                    break;
                case DeviceType::CPU:
                    basename = "devspec.cpu[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.cpu[didx], "cpustruc");
                    add_name(basename+".uptime", &devspec.cpu[didx].uptime, "uint32_t");
                    add_name(basename+".load", &devspec.cpu[didx].load, "float");
                    add_name(basename+".maxload", &devspec.cpu[didx].maxload, "float");
                    add_name(basename+".maxgib", &devspec.cpu[didx].maxgib, "float");
                    add_name(basename+".gib", &devspec.cpu[didx].gib, "float");
                    add_name(basename+".boot_count", &devspec.cpu[didx].boot_count, "uint32_t");
                    add_name(basename+".storage", &devspec.cpu[didx].boot_count, "float");
                    break;
                case DeviceType::DISK:
                    basename = "devspec.disk[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.disk[didx], "diskstruc");
                    add_name(basename+".maxgib", &devspec.disk[didx].maxgib, "float");
                    add_name(basename+".gib", &devspec.disk[didx].gib, "float");
                    add_name(basename+".path", &devspec.disk[didx].path, "string");
                    break;
                case DeviceType::GPS:
                    basename = "devspec.gps[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.gps[didx], "gpsstruc");
                    add_name(basename+".dutc", &devspec.gps[didx].dutc, "double");
                    add_name(basename+".geocs", &devspec.gps[didx].geocs, "rvector");
                    add_name(basename+".geocs.col", &devspec.gps[didx].geocs.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.gps[didx].geocs.col)/sizeof(devspec.gps[didx].geocs.col[0]); ++j) {
                        string rebasename = basename + "gps.geocs.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.gps[didx].geocs.col[j], "double");
                    }
                    add_name(basename+".geocv", &devspec.gps[didx].geocv, "rvector");
                    add_name(basename+".geocv.col", &devspec.gps[didx].geocv.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.gps[didx].geocv.col)/sizeof(devspec.gps[didx].geocv.col[0]); ++j) {
                        string rebasename = basename + "gps.geocv.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.gps[didx].geocv.col[j], "double");
                    }
                    add_name(basename+".dgeocs", &devspec.gps[didx].dgeocs, "rvector");
                    add_name(basename+".dgeocs.col", &devspec.gps[didx].dgeocs.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.gps[didx].dgeocs.col)/sizeof(devspec.gps[didx].dgeocs.col[0]); ++j) {
                        string rebasename = basename + "gps.dgeocs.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.gps[didx].dgeocs.col[j], "double");
                    }
                    add_name(basename+".dgeocv", &devspec.gps[didx].dgeocv, "rvector");
                    add_name(basename+".dgeocv.col", &devspec.gps[didx].dgeocv.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.gps[didx].dgeocv.col)/sizeof(devspec.gps[didx].dgeocv.col[0]); ++j) {
                        string rebasename = basename + "gps.dgeocv.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.gps[didx].dgeocv.col[j], "double");
                    }
                    add_name(basename+".geods", &devspec.gps[didx].geods, "gvector");
                    add_name(basename+".geods.lat", &devspec.gps[didx].geods.lat, "double");
                    add_name(basename+".geods.lon", &devspec.gps[didx].geods.lon, "double");
                    add_name(basename+".geods.h", &devspec.gps[didx].geods.h, "double");
                    add_name(basename+".geodv", &devspec.gps[didx].geodv, "gvector");
                    add_name(basename+".geodv.lat", &devspec.gps[didx].geodv.lat, "double");
                    add_name(basename+".geodv.lon", &devspec.gps[didx].geodv.lon, "double");
                    add_name(basename+".geodv.h", &devspec.gps[didx].geodv.h, "double");
                    add_name(basename+".dgeods", &devspec.gps[didx].dgeods, "gvector");
                    add_name(basename+".dgeods.lat", &devspec.gps[didx].dgeods.lat, "double");
                    add_name(basename+".dgeods.lon", &devspec.gps[didx].dgeods.lon, "double");
                    add_name(basename+".dgeods.h", &devspec.gps[didx].dgeods.h, "double");
                    add_name(basename+".dgeodv", &devspec.gps[didx].dgeodv, "gvector");
                    add_name(basename+".dgeodv.lat", &devspec.gps[didx].dgeodv.lat, "double");
                    add_name(basename+".dgeodv.lon", &devspec.gps[didx].dgeodv.lon, "double");
                    add_name(basename+".dgeodv.h", &devspec.gps[didx].dgeodv.h, "double");
                    add_name(basename+".heading", &devspec.gps[didx].heading, "float");
                    add_name(basename+".sats_used", &devspec.gps[didx].sats_used, "uint16_t");
                    add_name(basename+".sats_visible", &devspec.gps[didx].sats_visible, "uint16_t");
                    add_name(basename+".time_status", &devspec.gps[didx].time_status, "uint16_t");
                    add_name(basename+".position_type", &devspec.gps[didx].position_type, "uint16_t");
                    add_name(basename+".solution_status", &devspec.gps[didx].solution_status, "uint16_t");
                    break;
                case DeviceType::GYRO:
                    basename = "devspec.gyro[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.gyro[didx], "gyrostruc");
                    add_name(basename+".align", &devspec.gyro[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.gyro[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.gyro[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.gyro[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.gyro[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.gyro[didx].align.w, "double");
                    add_name(basename+".omega", &devspec.gyro[didx].omega, "float");
                    add_name(basename+".alpha", &devspec.gyro[didx].alpha, "float");
                    break;
                case DeviceType::HTR:
                    basename = "devspec.htr[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.htr[didx], "htrstruc");
                    add_name(basename+".state", &devspec.htr[didx].state, "bool");
                    add_name(basename+".setvertex", &devspec.htr[didx].setvertex, "float");
                    break;
                case DeviceType::IMU:
                    basename = "devspec.imu[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.imu[didx], "imustruc");
                    add_name(basename+".align", &devspec.imu[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.imu[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.imu[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.imu[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.imu[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.imu[didx].align.w, "double");
                    add_name(basename+".accel", &devspec.imu[didx].accel, "rvector");
                    add_name(basename+".accel.col", &devspec.imu[didx].accel.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.imu[didx].accel.col)/sizeof(devspec.imu[didx].accel.col[0]); ++j) {
                        string rebasename = basename + "imu.accel.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.imu[didx].accel.col[j], "double");
                    }
                    add_name(basename+".theta", &devspec.imu[didx].theta, "quaternion");
                    add_name(basename+".theta.d", &devspec.imu[didx].theta.d, "cvector");
                    add_name(basename+".theta.d.x", &devspec.imu[didx].theta.d.x, "double");
                    add_name(basename+".theta.d.y", &devspec.imu[didx].theta.d.y, "double");
                    add_name(basename+".theta.d.z", &devspec.imu[didx].theta.d.z, "double");
                    add_name(basename+".theta.w", &devspec.imu[didx].theta.w, "double");
                    add_name(basename+".euler", &devspec.imu[didx].euler, "avector");
                    add_name(basename+".euler.h", &devspec.imu[didx].euler.h, "double");
                    add_name(basename+".euler.e", &devspec.imu[didx].euler.e, "double");
                    add_name(basename+".euler.b", &devspec.imu[didx].euler.b, "double");
                    add_name(basename+".omega", &devspec.imu[didx].omega, "rvector");
                    add_name(basename+".omega.col", &devspec.imu[didx].omega.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.imu[didx].omega.col)/sizeof(devspec.imu[didx].omega.col[0]); ++j) {
                        string rebasename = basename + "imu.omega.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.imu[didx].omega.col[j], "double");
                    }
                    add_name(basename+".alpha", &devspec.imu[didx].alpha, "rvector");
                    add_name(basename+".alpha.col", &devspec.imu[didx].alpha.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.imu[didx].alpha.col)/sizeof(devspec.imu[didx].alpha.col[0]); ++j) {
                        string rebasename = basename + "imu.alpha.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.imu[didx].alpha.col[j], "double");
                    }
                    add_name(basename+".mag", &devspec.imu[didx].mag, "rvector");
                    add_name(basename+".mag.col", &devspec.imu[didx].mag.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.imu[didx].mag.col)/sizeof(devspec.imu[didx].mag.col[0]); ++j) {
                        string rebasename = basename + "imu.mag.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.imu[didx].mag.col[j], "double");
                    }
                    add_name(basename+".bdot", &devspec.imu[didx].bdot, "rvector");
                    add_name(basename+".bdot.col", &devspec.imu[didx].bdot.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.imu[didx].bdot.col)/sizeof(devspec.imu[didx].bdot.col[0]); ++j) {
                        string rebasename = basename + "imu.bdot.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.imu[didx].bdot.col[j], "double");
                    }
                    break;
                case DeviceType::MAG:
                    basename = "devspec.mag[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.mag[didx], "magstruc");
                    add_name(basename+".align", &devspec.mag[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.mag[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.mag[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.mag[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.mag[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.mag[didx].align.w, "double");
//                        add_name(basename+".omega", &devspec.mag[didx].omega, "rvector");
//                        add_name(basename+".omega.col", &devspec.mag[didx].omega.col, "double[]");
//                        for(size_t j = 0; j < sizeof(devspec.mag[didx].omega.col)/sizeof(devspec.mag[didx].omega.col[0]); ++j) {
//                            string rebasename = basename + "mag.omega.col[" + std::to_string(j) + "]";
//                            add_name(rebasename, &devspec.mag[didx].omega.col[j], "double");
//                        }
//                        add_name(basename+".alpha", &devspec.mag[didx].alpha, "rvector");
//                        add_name(basename+".alpha.col", &devspec.mag[didx].alpha.col, "double[]");
//                        for(size_t j = 0; j < sizeof(devspec.mag[didx].alpha.col)/sizeof(devspec.mag[didx].alpha.col[0]); ++j) {
//                            string rebasename = basename + "mag.alpha.col[" + std::to_string(j) + "]";
//                            add_name(rebasename, &devspec.mag[didx].alpha.col[j], "double");
//                        }
                    add_name(basename+".mag", &devspec.mag[didx].mag, "rvector");
                    add_name(basename+".mag.col", &devspec.mag[didx].mag.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.mag[didx].mag.col)/sizeof(devspec.mag[didx].mag.col[0]); ++j) {
                        string rebasename = basename + "mag.mag.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.mag[didx].mag.col[j], "double");
                    }
                    add_name(basename+".bdot", &devspec.mag[didx].bdot, "rvector");
                    add_name(basename+".bdot.col", &devspec.mag[didx].bdot.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.mag[didx].bdot.col)/sizeof(devspec.mag[didx].bdot.col[0]); ++j) {
                        string rebasename = basename + "mag.bdot.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.mag[didx].bdot.col[j], "double");
                    }
                    break;
                case DeviceType::MCC:
                    basename = "devspec.mcc[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.mcc[didx], "mccstruc");
                    add_name(basename+".align", &devspec.mcc[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.mcc[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.mcc[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.mcc[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.mcc[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.mcc[didx].align.w, "double");
                    add_name(basename+".q", &devspec.mcc[didx].q, "quaternion");
                    add_name(basename+".q.d", &devspec.mcc[didx].q.d, "cvector");
                    add_name(basename+".q.d.x", &devspec.mcc[didx].q.d.x, "double");
                    add_name(basename+".q.d.y", &devspec.mcc[didx].q.d.y, "double");
                    add_name(basename+".q.d.z", &devspec.mcc[didx].q.d.z, "double");
                    add_name(basename+".q.w", &devspec.mcc[didx].q.w, "double");
                    add_name(basename+".o", &devspec.mcc[didx].o, "rvector");
                    add_name(basename+".o.col", &devspec.mcc[didx].o.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.mcc[didx].o.col)/sizeof(devspec.mcc[didx].o.col[0]); ++j) {
                        string rebasename = basename + "mcc.o.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.mcc[didx].o.col[j], "double");
                    }
                    add_name(basename+".a", &devspec.mcc[didx].a, "rvector");
                    add_name(basename+".a.col", &devspec.mcc[didx].a.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.mcc[didx].a.col)/sizeof(devspec.mcc[didx].a.col[0]); ++j) {
                        string rebasename = basename + "mcc.a.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.mcc[didx].a.col[j], "double");
                    }
                    break;
                case DeviceType::MOTR:
                    basename = "devspec.motr[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.motr[didx], "motrstruc");
                    add_name(basename+".max", &devspec.motr[didx].max, "float");
                    add_name(basename+".rat", &devspec.motr[didx].rat, "float");
                    add_name(basename+".spd", &devspec.motr[didx].spd, "float");
                    break;
                case DeviceType::MTR:
                    basename = "devspec.mtr[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.mtr[didx], "mtrstruc");
                    add_name(basename+".align", &devspec.mtr[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.mtr[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.mtr[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.mtr[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.mtr[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.mtr[didx].align.w, "double");
                    add_name(basename+".npoly", &devspec.mtr[didx].npoly, "float[]");
                    for(size_t j = 0; j < sizeof(devspec.mtr[didx].npoly)/sizeof(devspec.mtr[didx].npoly[0]); ++j) {
                        string rebasename = basename + "mtr.npoly[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.mtr[didx].npoly[j], "float");
                    }
                    add_name(basename+".ppoly", &devspec.mtr[didx].ppoly, "float[]");
                    for(size_t j = 0; j < sizeof(devspec.mtr[didx].ppoly)/sizeof(devspec.mtr[didx].ppoly[0]); ++j) {
                        string rebasename = basename + "mtr.ppoly[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.mtr[didx].ppoly[j], "float");
                    }
                    add_name(basename+".mxmom", &devspec.mtr[didx].mxmom, "float");
                    add_name(basename+".tc", &devspec.mtr[didx].tc, "float");
                    add_name(basename+".rmom", &devspec.mtr[didx].rmom, "float");
                    add_name(basename+".mom", &devspec.mtr[didx].mom, "float");
                    break;
                case DeviceType::PLOAD:
                    basename = "devspec.pload[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.pload[didx], "ploadstruc");
                    add_name(basename+".keycnt", &devspec.pload[didx].keycnt, "uint16_t");
                    add_name(basename+".keyidx", &devspec.pload[didx].keyidx, "uint16_t[]");
                    for(size_t j = 0; j < sizeof(devspec.pload[didx].keyidx)/sizeof(devspec.pload[didx].keyidx[0]); ++j) {
                        string rebasename = basename + ".keyidx[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.pload[didx].keyidx[j], "uint16_t");
                    }
                    add_name(basename+".keyval", &devspec.pload[didx].keyval, "float[]");
                    for(size_t j = 0; j < sizeof(devspec.pload[didx].keyval)/sizeof(devspec.pload[didx].keyval[0]); ++j) {
                        string rebasename = basename + ".keyval[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.pload[didx].keyval[j], "float");
                    }
                    break;
                case DeviceType::PROP:
                    basename = "devspec.prop[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.prop[didx], "propstruc");
                    add_name(basename+".cap", &devspec.prop[didx].cap, "float");
                    add_name(basename+".lev", &devspec.prop[didx].lev, "float");
                    break;
                case DeviceType::PSEN:
                    basename = "devspec.psen[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.psen[didx], "psenstruc");
                    add_name(basename+".press", &devspec.psen[didx].press, "float");
                    break;
                case DeviceType::PVSTRG:
                    basename = "devspec.pvstrg[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.pvstrg[didx], "pvstrgstruc");
                    add_name(basename+".bcidx", &devspec.pvstrg[didx].bcidx, "uint16_t");
                    add_name(basename+".effbase", &devspec.pvstrg[didx].effbase, "float");
                    add_name(basename+".effslope", &devspec.pvstrg[didx].effslope, "float");
                    add_name(basename+".maxpower", &devspec.pvstrg[didx].maxpower, "float");
                    add_name(basename+".power", &devspec.pvstrg[didx].power, "float");
                    break;
                case DeviceType::ROT:
                    basename = "devspec.rot[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.rot[didx], "rotstruc");
                    add_name(basename+".angle", &devspec.rot[didx].angle, "float");
                    break;
                case DeviceType::RW:
                    basename = "devspec.rw[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.rw[didx], "rwstruc");
                    add_name(basename+".align", &devspec.rw[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.rw[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.rw[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.rw[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.rw[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.rw[didx].align.w, "double");
                    add_name(basename+".mom", &devspec.rw[didx].mom, "rvector");
                    add_name(basename+".mom.col", &devspec.rw[didx].mom.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.rw[didx].mom.col)/sizeof(devspec.rw[didx].mom.col[0]); ++j) {
                        string rebasename = basename + ".mom.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.rw[didx].mom.col[j], "double");
                    }
                    add_name(basename+".mxomg", &devspec.rw[didx].mxomg, "float");
                    add_name(basename+".mxalp", &devspec.rw[didx].mxalp, "float");
                    add_name(basename+".tc", &devspec.rw[didx].tc, "float");
                    add_name(basename+".omg", &devspec.rw[didx].omg, "float");
                    add_name(basename+".alp", &devspec.rw[didx].alp, "float");
                    add_name(basename+".romg", &devspec.rw[didx].romg, "float");
                    add_name(basename+".ralp", &devspec.rw[didx].ralp, "float");
                    break;
                case DeviceType::RXR:
                    basename = "devspec.rxr[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.rxr[didx], "rxrstruc");
                    add_name(basename+".opmode", &devspec.rxr[didx].opmode, "uint16_t");
                    add_name(basename+".modulation", &devspec.rxr[didx].modulation, "uint16_t");
                    add_name(basename+".rssi", &devspec.rxr[didx].rssi, "uint16_t");
                    add_name(basename+".pktsize", &devspec.rxr[didx].pktsize, "uint16_t");
                    add_name(basename+".freq", &devspec.rxr[didx].freq, "double");
                    add_name(basename+".maxfreq", &devspec.rxr[didx].maxfreq, "double");
                    add_name(basename+".minfreq", &devspec.rxr[didx].minfreq, "double");
                    add_name(basename+".byte_rate", &devspec.rxr[didx].byte_rate, "float");
                    add_name(basename+".powerin", &devspec.rxr[didx].powerin, "float");
                    add_name(basename+".powerout", &devspec.rxr[didx].powerout, "float");
                    add_name(basename+".maxpower", &devspec.rxr[didx].maxpower, "float");
                    add_name(basename+".band", &devspec.rxr[didx].band, "float");
                    add_name(basename+".squelch_tone", &devspec.rxr[didx].squelch_tone, "float");
                    add_name(basename+".goodratio", &devspec.rxr[didx].goodratio, "double");
                    add_name(basename+".utcin", &devspec.rxr[didx].utcin, "double");
                    add_name(basename+".uptime", &devspec.rxr[didx].uptime, "double");
                    add_name(basename+".bytesin", &devspec.rxr[didx].bytesin, "int32_t");
                    break;
                case DeviceType::SSEN:
                    basename = "devspec.ssen[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.ssen[didx], "ssenstruc");
                    add_name(basename+".align", &devspec.ssen[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.ssen[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.ssen[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.ssen[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.ssen[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.ssen[didx].align.w, "double");
                    add_name(basename+".qva", &devspec.ssen[didx].qva, "float");
                    add_name(basename+".qvb", &devspec.ssen[didx].qvb, "float");
                    add_name(basename+".qvc", &devspec.ssen[didx].qvc, "float");
                    add_name(basename+".qvd", &devspec.ssen[didx].qvd, "float");
                    add_name(basename+".azimuth", &devspec.ssen[didx].azimuth, "float");
                    add_name(basename+".elevation", &devspec.ssen[didx].elevation, "float");
                    break;
                case DeviceType::STT:
                    basename = "devspec.stt[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.stt[didx], "sttstruc");
                    add_name(basename+".align", &devspec.stt[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.stt[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.stt[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.stt[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.stt[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.stt[didx].align.w, "double");
                    add_name(basename+".att", &devspec.stt[didx].att, "quaternion");
                    add_name(basename+".att.d", &devspec.stt[didx].att.d, "cvector");
                    add_name(basename+".att.d.x", &devspec.stt[didx].att.d.x, "double");
                    add_name(basename+".att.d.y", &devspec.stt[didx].att.d.y, "double");
                    add_name(basename+".att.d.z", &devspec.stt[didx].att.d.z, "double");
                    add_name(basename+".att.w", &devspec.stt[didx].att.w, "double");
                    add_name(basename+".omega", &devspec.stt[didx].omega, "rvector");
                    add_name(basename+".omega.col", &devspec.stt[didx].omega.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.stt[didx].omega.col)/sizeof(devspec.stt[didx].omega.col[0]); ++j) {
                        string rebasename = basename + "stt.omega.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.stt[didx].omega.col[j], "double");
                    }
                    add_name(basename+".alpha", &devspec.stt[didx].alpha, "rvector");
                    add_name(basename+".alpha.col", &devspec.stt[didx].alpha.col, "double[]");
                    for(size_t j = 0; j < sizeof(devspec.stt[didx].alpha.col)/sizeof(devspec.stt[didx].alpha.col[0]); ++j) {
                        string rebasename = basename + "stt.alpha.col[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.stt[didx].alpha.col[j], "double");
                    }
                    add_name(basename+".retcode", &devspec.stt[didx].retcode, "uint16_t");
                    add_name(basename+".status", &devspec.stt[didx].status, "uint32_t");
                    break;
                case DeviceType::SUCHI:
                    basename = "devspec.suchi[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.suchi[didx], "suchistruc");
                    add_name(basename+".align", &devspec.suchi[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.suchi[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.suchi[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.suchi[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.suchi[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.suchi[didx].align.w, "double");
                    add_name(basename+".press", &devspec.suchi[didx].press, "float");
                    add_name(basename+".temps", &devspec.suchi[didx].temps, "float[]");
                    for(size_t j = 0; j < sizeof(devspec.suchi[didx].temps)/sizeof(devspec.suchi[didx].temps[0]); ++j) {
                        string rebasename = basename + "suchi.temps[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.suchi[didx].temps[j], "float");
                    }
                    break;
                case DeviceType::SWCH:
                    basename = "devspec.swch[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.swch[didx], "swchstruc");
                    break;
                case DeviceType::TCU:
                    basename = "devspec.tcu[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.tcu[didx], "tcustruc");
                    add_name(basename+".mcnt", &devspec.tcu[didx].mcnt, "uint16_t");
                    add_name(basename+".mcidx", &devspec.tcu[didx].mcidx, "uint16_t[]");
                    for(size_t j = 0; j < sizeof(devspec.tcu[didx].mcidx)/sizeof(devspec.tcu[didx].mcidx[0]); ++j) {
                        string rebasename = basename + ".mcidx[" + std::to_string(j) + "]";
                        add_name(rebasename, &devspec.tcu[didx].mcidx[j], "uint16_t");
                    }
                    break;
                case DeviceType::TCV:
                    basename = "devspec.tcv[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.tcv[didx], "tcvstruc");
                    add_name(basename+".opmode", &devspec.tcv[didx].opmode, "uint16_t");
                    add_name(basename+".modulation", &devspec.tcv[didx].modulation, "uint16_t");
                    add_name(basename+".rssi", &devspec.tcv[didx].rssi, "uint16_t");
                    add_name(basename+".pktsize", &devspec.tcv[didx].pktsize, "uint16_t");
                    add_name(basename+".freq", &devspec.tcv[didx].freq, "double");
                    add_name(basename+".maxfreq", &devspec.tcv[didx].maxfreq, "double");
                    add_name(basename+".minfreq", &devspec.tcv[didx].minfreq, "double");
                    add_name(basename+".byte_rate", &devspec.tcv[didx].byte_rate, "float");
                    add_name(basename+".powerin", &devspec.tcv[didx].powerin, "float");
                    add_name(basename+".powerout", &devspec.tcv[didx].powerout, "float");
                    add_name(basename+".maxpower", &devspec.tcv[didx].maxpower, "float");
                    add_name(basename+".band", &devspec.tcv[didx].band, "float");
                    add_name(basename+".squelch_tone", &devspec.tcv[didx].squelch_tone, "float");
                    add_name(basename+".goodratio", &devspec.tcv[didx].goodratio, "double");
                    add_name(basename+".utcout", &devspec.tcv[didx].utcout, "double");
                    add_name(basename+".utcin", &devspec.tcv[didx].utcin, "double");
                    add_name(basename+".uptime", &devspec.tcv[didx].uptime, "double");
                    add_name(basename+".bytesout", &devspec.tcv[didx].bytesout, "int32_t");
                    break;
                case DeviceType::TELEM:
                    basename = "devspec.telem[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.telem[didx], "telemstruc");
                    add_name(basename+".vuint8", &devspec.telem[didx].vuint8, "uint8_t");
                    add_name(basename+".vint8", &devspec.telem[didx].vint8, "int8_t");
                    add_name(basename+".vuint16", &devspec.telem[didx].vuint16, "uint16_t");
                    add_name(basename+".vint16", &devspec.telem[didx].vint16, "int16_t");
                    add_name(basename+".vuint32", &devspec.telem[didx].vuint32, "uint32_t");
                    add_name(basename+".vint32", &devspec.telem[didx].vint32, "int32_t");
                    add_name(basename+".vfloat", &devspec.telem[didx].vfloat, "float");
                    add_name(basename+".vdouble", &devspec.telem[didx].vdouble, "double");
                    add_name(basename+".vstring", &devspec.telem[didx].vstring, "string");
                    break;
                case DeviceType::NTELEM:
                    basename = "devspec.ntelem[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.telem[didx], "telemstruc");
                    add_name(basename+".value", &devspec.ntelem[didx].value, "double");
                    break;
                case DeviceType::THST:
                    basename = "devspec.thst[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.thst[didx], "thststruc");
                    add_name(basename+".align", &devspec.thst[didx].align, "quaternion");
                    add_name(basename+".align.d", &devspec.thst[didx].align.d, "cvector");
                    add_name(basename+".align.d.x", &devspec.thst[didx].align.d.x, "double");
                    add_name(basename+".align.d.y", &devspec.thst[didx].align.d.y, "double");
                    add_name(basename+".align.d.z", &devspec.thst[didx].align.d.z, "double");
                    add_name(basename+".align.w", &devspec.thst[didx].align.w, "double");
                    add_name(basename+".flw", &devspec.thst[didx].flw, "float");
                    add_name(basename+".isp", &devspec.thst[didx].isp, "float");
                    add_name(basename+".maxthrust", &devspec.thst[didx].maxthrust, "float");
                    add_name(basename+".utilization", &devspec.thst[didx].utilization, "float");
                    break;
                case DeviceType::TNC:
                    basename = "devspec.tnc[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.tnc[didx], "tncstruc");
                    break;
                case DeviceType::TSEN:
                    basename = "devspec.tsen[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.tsen[didx], "tsenstruc");
                    break;
                case DeviceType::TXR:
                    basename = "devspec.txr[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.txr[didx], "txrstruc");
                    add_name(basename+".opmode", &devspec.txr[didx].opmode, "uint16_t");
                    add_name(basename+".modulation", &devspec.txr[didx].modulation, "uint16_t");
                    add_name(basename+".rssi", &devspec.txr[didx].rssi, "uint16_t");
                    add_name(basename+".pktsize", &devspec.txr[didx].pktsize, "uint16_t");
                    add_name(basename+".freq", &devspec.txr[didx].freq, "double");
                    add_name(basename+".maxfreq", &devspec.txr[didx].maxfreq, "double");
                    add_name(basename+".minfreq", &devspec.txr[didx].minfreq, "double");
                    add_name(basename+".byte_rate", &devspec.txr[didx].byte_rate, "float");
                    add_name(basename+".powerin", &devspec.txr[didx].powerin, "float");
                    add_name(basename+".powerout", &devspec.txr[didx].powerout, "float");
                    add_name(basename+".maxpower", &devspec.txr[didx].maxpower, "float");
                    add_name(basename+".band", &devspec.txr[didx].band, "float");
                    add_name(basename+".squelch_tone", &devspec.txr[didx].squelch_tone, "float");
                    add_name(basename+".goodratio", &devspec.txr[didx].goodratio, "double");
                    add_name(basename+".utcout", &devspec.txr[didx].utcout, "double");
                    add_name(basename+".uptime", &devspec.txr[didx].uptime, "double");
                    add_name(basename+".bytesout", &devspec.tcv[didx].bytesout, "int32_t");
                    add_name(basename+".bytesin", &devspec.rxr[didx].bytesin, "int32_t");
                    break;
                case DeviceType::XYZSEN:
                    basename = "devspec.xyzsen[" + std::to_string(didx) + "]";
                    add_name(basename+"", &devspec.xyzsen[didx], "xyzsenstruc");
                    add_name(basename+".direction", &devspec.xyzsen[didx].direction, "rvector");
                    break;
                }
            
                // Add inherited devicestruc names for each specific device
                add_name(basename+".name", &device[i]->name, "string");
                add_name(basename+".state", &device[i]->state, "uint8_t");
                add_name(basename+".type", &device[i]->type, "uint16_t");
                add_name(basename+".model", &device[i]->model, "uint16_t");
                add_name(basename+".flag", &device[i]->flag, "uint32_t");
                add_name(basename+".addr", &device[i]->addr, "uint16_t");
                add_name(basename+".cidx", &device[i]->cidx, "uint16_t");
                add_name(basename+".didx", &device[i]->didx, "uint16_t");
                add_name(basename+".pidx", &device[i]->pidx, "uint16_t");
                add_name(basename+".bidx", &device[i]->bidx, "uint16_t");
                add_name(basename+".portidx", &device[i]->portidx, "uint16_t");
                add_name(basename+".namp", &device[i]->namp, "float");
                add_name(basename+".nvolt", &device[i]->nvolt, "float");
                add_name(basename+".amp", &device[i]->amp, "float");
                add_name(basename+".volt", &device[i]->volt, "float");
                add_name(basename+".power", &device[i]->power, "float");
                add_name(basename+".energy", &device[i]->energy, "float");
                add_name(basename+".drate", &device[i]->drate, "float");
                add_name(basename+".temp", &device[i]->temp, "float");
                add_name(basename+".utc", &device[i]->utc, "double");
                add_name(basename+".expiration", &device[i]->expiration, "double");
            }


            // devspecstruc devspec
            add_name("devspec", &devspec, "devspecstruc");
            //                add_name("devspec.all_cnt", &devspec.all_cnt, "uint16_t");
            add_name("devspec.ant_cnt", &devspec.ant_cnt, "uint16_t");
            add_name("devspec.batt_cnt", &devspec.batt_cnt, "uint16_t");
            add_name("devspec.bus_cnt", &devspec.bus_cnt, "uint16_t");
            add_name("devspec.cam_cnt", &devspec.cam_cnt, "uint16_t");
            add_name("devspec.cpu_cnt", &devspec.cpu_cnt, "uint16_t");
            add_name("devspec.disk_cnt", &devspec.disk_cnt, "uint16_t");
            add_name("devspec.gps_cnt", &devspec.gps_cnt, "uint16_t");
            add_name("devspec.gyro_cnt", &devspec.gyro_cnt, "uint16_t");
            add_name("devspec.htr_cnt", &devspec.htr_cnt, "uint16_t");
            add_name("devspec.imu_cnt", &devspec.imu_cnt, "uint16_t");
            add_name("devspec.mag_cnt", &devspec.mag_cnt, "uint16_t");
            add_name("devspec.mcc_cnt", &devspec.mcc_cnt, "uint16_t");
            add_name("devspec.motr_cnt", &devspec.motr_cnt, "uint16_t");
            add_name("devspec.mtr_cnt", &devspec.mtr_cnt, "uint16_t");
            add_name("devspec.pload_cnt", &devspec.pload_cnt, "uint16_t");
            add_name("devspec.prop_cnt", &devspec.prop_cnt, "uint16_t");
            add_name("devspec.psen_cnt", &devspec.psen_cnt, "uint16_t");
            add_name("devspec.bcreg_cnt", &devspec.bcreg_cnt, "uint16_t");
            add_name("devspec.rot_cnt", &devspec.rot_cnt, "uint16_t");
            add_name("devspec.rw_cnt", &devspec.rw_cnt, "uint16_t");
            add_name("devspec.rxr_cnt", &devspec.rxr_cnt, "uint16_t");
            add_name("devspec.ssen_cnt", &devspec.ssen_cnt, "uint16_t");
            add_name("devspec.pvstrg_cnt", &devspec.pvstrg_cnt, "uint16_t");
            add_name("devspec.stt_cnt", &devspec.stt_cnt, "uint16_t");
            add_name("devspec.suchi_cnt", &devspec.suchi_cnt, "uint16_t");
            add_name("devspec.swch_cnt", &devspec.swch_cnt, "uint16_t");
            add_name("devspec.tcu_cnt", &devspec.tcu_cnt, "uint16_t");
            add_name("devspec.tcv_cnt", &devspec.tcv_cnt, "uint16_t");
            add_name("devspec.telem_cnt", &devspec.telem_cnt, "uint16_t");
            add_name("devspec.thst_cnt", &devspec.thst_cnt, "uint16_t");
            add_name("devspec.tsen_cnt", &devspec.tsen_cnt, "uint16_t");
            add_name("devspec.tnc_cnt", &devspec.tnc_cnt, "uint16_t");
            add_name("devspec.txr_cnt", &devspec.txr_cnt, "uint16_t");
            add_name("devspec.xyzsen_cnt", &devspec.xyzsen_cnt, "uint16_t");


            // vector<portstruc> port
            add_name("port", &port, "vector<portstruc>");
            for(size_t i = 0; i < port.size(); ++i) {
                string basename = "port[" + std::to_string(i) + "]";
                add_name(basename, &port[i], "portstruc");
                add_name(basename+".type", &port[i].type, "PORT_TYPE");
                add_name(basename+".name", &port[i].name, "string");
            }

            string basename = "agent0";
            add_name(basename, &agent0, "agentstruc");
            add_name(basename+".client", &agent0.client, "bool");
            add_name(basename+".sub", &agent0.sub, "socket_channel");
            add_name(basename+".sub.type", &agent0.sub.type, "NetworkType");
            add_name(basename+".sub.cudp", &agent0.sub.cudp, "int32_t");
            add_name(basename+".server", &agent0.server, "bool");
            add_name(basename+".ifcnt", &agent0.ifcnt, "size_t");
            add_name(basename+".pub", &agent0.pub, "socket_channel[]");
            for(size_t j = 0; j < sizeof(agent0.pub)/sizeof(agent0.pub[0]); ++j) {
                string rebasename = basename + ".pub[" + std::to_string(j) + "]";
                add_name(rebasename, &agent0.pub[j], "socket_channel");
                add_name(rebasename+".type", &agent0.pub[j].type, "NetworkType");
                add_name(rebasename+".cudp", &agent0.pub[j].cudp, "int32_t");
            }
            add_name(basename+".req", &agent0.req, "socket_channel");
            add_name(basename+".req.type", &agent0.req.type, "NetworkType");
            add_name(basename+".req.cudp", &agent0.req.cudp, "int32_t");
            add_name(basename+".pid", &agent0.pid, "int32_t");
            add_name(basename+".aprd", &agent0.aprd, "double");
            add_name(basename+".stateflag", &agent0.stateflag, "uint16_t");
            add_name(basename+".reqs", &agent0.reqs, "vector<agent_request_entry>");
            for(size_t j = 0; j < agent0.reqs.size(); ++j) {
                string rebasename = basename + ".reqs[" + std::to_string(j) + "]";
                add_name(rebasename, &agent0.reqs[j], "agent_request_entry");
                add_name(rebasename+".token", &agent0.reqs[j].token, "string");
                add_name(rebasename+".function", &agent0.reqs[j].function, "agent_request_function");
                add_name(rebasename+".synopsis", &agent0.reqs[j].synopsis, "string");
                add_name(rebasename+".description", &agent0.reqs[j].description, "string");
            }
            add_name(basename+".beat", &agent0.beat, "beatstruc");
            add_name(basename+".beat.utc", &agent0.beat.utc, "double");
            add_name(basename+".beat.realm", &agent0.beat.realm, "string");
            add_name(basename+".beat.node", &agent0.beat.node, "string");
            add_name(basename+".beat.proc", &agent0.beat.proc, "string");
            add_name(basename+".beat.ntype", &agent0.beat.ntype, "NetworkType");
            add_name(basename+".beat.addr", &agent0.beat.addr, "char[]");
            add_name(basename+".beat.port", &agent0.beat.port, "uint16_t");
            add_name(basename+".beat.bsz", &agent0.beat.bsz, "uint32_t");
            add_name(basename+".beat.bprd", &agent0.beat.bprd, "double");
            add_name(basename+".beat.user", &agent0.beat.user, "char[]");
            add_name(basename+".beat.cpu", &agent0.beat.cpu, "float");
            add_name(basename+".beat.memory", &agent0.beat.memory, "float");
            add_name(basename+".beat.jitter", &agent0.beat.jitter, "double");
            add_name(basename+".beat.dcycle", &agent0.beat.dcycle, "double");
            add_name(basename+".beat.exists", &agent0.beat.exists, "bool");

            // vector<sim_state> sim_states
            add_name("sim_states", &sim_states, "vector<sim_state>");
            for(size_t i = 0; i < sim_states.size(); ++i) {
                string basename = "sim_states[" + std::to_string(i) + "]";
                add_name(basename, &sim_states[i], "sim_state");
                add_name(basename+".node_name", &sim_states[i].node_name, "string");
                add_name(basename+".agent_name", &sim_states[i].agent_name, "string");

                add_name(basename+".t_pos", &sim_states[i].t_pos, "double");
                add_name(basename+".x_pos", &sim_states[i].x_pos, "double");
                add_name(basename+".y_pos", &sim_states[i].y_pos, "double");
                add_name(basename+".z_pos", &sim_states[i].z_pos, "double");

                add_name(basename+".t_vel", &sim_states[i].t_vel, "double");
                add_name(basename+".x_vel", &sim_states[i].x_vel, "double");
                add_name(basename+".y_vel", &sim_states[i].y_vel, "double");
                add_name(basename+".z_vel", &sim_states[i].z_vel, "double");

                add_name(basename+".t_acc", &sim_states[i].t_acc, "double");
                add_name(basename+".x_acc", &sim_states[i].x_acc, "double");
                add_name(basename+".y_acc", &sim_states[i].y_acc, "double");
                add_name(basename+".z_acc", &sim_states[i].z_acc, "double");

                add_name(basename+".t_att", &sim_states[i].t_att, "double");
                add_name(basename+".a_att", &sim_states[i].a_att, "double");
                add_name(basename+".b_att", &sim_states[i].b_att, "double");
                add_name(basename+".c_att", &sim_states[i].c_att, "double");
                add_name(basename+".d_att", &sim_states[i].d_att, "double");

                add_name(basename+".t_att_target", &sim_states[i].t_att_target, "double");
                add_name(basename+".a_att_target", &sim_states[i].a_att_target, "double");
                add_name(basename+".b_att_target", &sim_states[i].b_att_target, "double");
                add_name(basename+".c_att_target", &sim_states[i].c_att_target, "double");
                add_name(basename+".d_att_target", &sim_states[i].d_att_target, "double");

                add_name(basename+".t_omega", &sim_states[i].t_omega, "double");
                add_name(basename+".x_omega", &sim_states[i].x_omega, "double");
                add_name(basename+".y_omega", &sim_states[i].y_omega, "double");
                add_name(basename+".z_omega", &sim_states[i].z_omega, "double");

                add_name(basename+".t_alpha", &sim_states[i].t_alpha, "double");
                add_name(basename+".x_alpha", &sim_states[i].x_alpha, "double");
                add_name(basename+".y_alpha", &sim_states[i].y_alpha, "double");
                add_name(basename+".z_alpha", &sim_states[i].z_alpha, "double");

                add_name(basename+".t_thrust", &sim_states[i].t_thrust, "double");
                add_name(basename+".x_thrust", &sim_states[i].x_thrust, "double");
                add_name(basename+".y_thrust", &sim_states[i].y_thrust, "double");
                add_name(basename+".z_thrust", &sim_states[i].z_thrust, "double");

                add_name(basename+".t_torque", &sim_states[i].t_torque, "double");
                add_name(basename+".x_torque", &sim_states[i].x_torque, "double");
                add_name(basename+".y_torque", &sim_states[i].y_torque, "double");
                add_name(basename+".z_torque", &sim_states[i].z_torque, "double");

                add_name(basename+".t_acc_diff", &sim_states[i].t_acc_diff, "double");
                add_name(basename+".x_acc_diff", &sim_states[i].x_acc_diff, "double");
                add_name(basename+".y_acc_diff", &sim_states[i].y_acc_diff, "double");
                add_name(basename+".z_acc_diff", &sim_states[i].z_acc_diff, "double");

                add_name(basename+".target_latitude", &sim_states[i].target_latitude, "double");
                add_name(basename+".target_longitude", &sim_states[i].target_longitude, "double");
                add_name(basename+".target_altitude", &sim_states[i].target_altitude, "double");
            }

            // vector<eventstruc> event
            add_name("event", &event, "vector<eventstruc>");
            for(size_t i = 0; i < event.size(); ++i) {
                string basename = "event[" + std::to_string(i) + "]";
                add_name(basename, &event[i], "eventstruc");
                add_name(basename+".utc", &event[i].utc, "double");
                add_name(basename+".utcexec", &event[i].utcexec, "double");
                add_name(basename+".node", &event[i].node, "string");
                add_name(basename+".name", &event[i].name, "string");
                add_name(basename+".user", &event[i].user, "string");
                add_name(basename+".flag", &event[i].flag, "uint32_t");
                add_name(basename+".type", &event[i].type, "uint32_t");
                add_name(basename+".value", &event[i].value, "double");
                add_name(basename+".dtime", &event[i].dtime, "double");
                add_name(basename+".ctime", &event[i].ctime, "double");
                add_name(basename+".denergy", &event[i].denergy, "float");
                add_name(basename+".cenergy", &event[i].cenergy, "float");
                add_name(basename+".dmass", &event[i].dmass, "float");
                add_name(basename+".cmass", &event[i].cmass, "float");
                add_name(basename+".dbytes", &event[i].dbytes, "float");
                add_name(basename+".cbytes", &event[i].cbytes, "float");
                add_name(basename+".el", &event[i].el, "float");
                add_name(basename+".az", &event[i].az, "float");
                add_name(basename+".handle", &event[i].handle, "jsonhandle");
                add_name(basename+".handle.hash", &event[i].handle.hash, "uint16_t");
                add_name(basename+".handle.index", &event[i].handle.index, "uint16_t");
                add_name(basename+".data", &event[i].data, "string");
                add_name(basename+".condition", &event[i].condition, "string");
            }


            // vector<targetstruc> target
            add_name("target", &target, "vector<targetstruc>");
            for(size_t i = 0; i < target.size(); ++i)
            {
                string basename = "target[" + std::to_string(i) + "]";
                add_name(basename, &target[i], "targetstruc");
                add_name(basename+".utc", &target[i].utc, "double");
                add_name(basename+".name", &target[i].name, "string");
                add_name(basename+".type", &target[i].type, "uint16_t");
                add_name(basename+".azfrom", &target[i].azfrom, "float");
                add_name(basename+".elfrom", &target[i].elfrom, "float");
                add_name(basename+".azto", &target[i].azto, "float");
                add_name(basename+".elto", &target[i].elto, "float");
                add_name(basename+".maxelto", &target[i].maxelto, "float");
                add_name(basename+".range", &target[i].range, "double");
                add_name(basename+".close", &target[i].close, "double");
                add_name(basename+".min", &target[i].min, "float");
                add_name(basename+".bearing", &target[i].bearing, "float");
                add_name(basename+".distance", &target[i].distance, "float");
                add_name(basename+".loc", &target[i].loc, "locstruc");
                add_name(basename+".loc.utc", &target[i].loc.utc, "double");
                add_name(basename+".loc.pos", &target[i].loc.pos, "posstruc");
                add_name(basename+".loc.pos.utc", &target[i].loc.pos.utc, "double");
                add_name(basename+".loc.pos.icrf", &target[i].loc.pos.icrf, "cartpos");
                add_name(basename+".loc.pos.icrf.utc", &target[i].loc.pos.icrf.utc, "double");
                add_name(basename+".loc.pos.icrf.s", &target[i].loc.pos.icrf.s, "rvector");
                add_name(basename+".loc.pos.icrf.s.col", &target[i].loc.pos.icrf.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.icrf.s.col)/sizeof(target[i].loc.pos.icrf.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.icrf.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.icrf.s.col[j], "double");
                }
                add_name(basename+".loc.pos.icrf.v", &target[i].loc.pos.icrf.v, "rvector");
                add_name(basename+".loc.pos.icrf.v.col", &target[i].loc.pos.icrf.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.icrf.v.col)/sizeof(target[i].loc.pos.icrf.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.icrf.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.icrf.v.col[j], "double");
                }
                add_name(basename+".loc.pos.icrf.a", &target[i].loc.pos.icrf.a, "rvector");
                add_name(basename+".loc.pos.icrf.a.col", &target[i].loc.pos.icrf.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.icrf.a.col)/sizeof(target[i].loc.pos.icrf.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.icrf.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.icrf.a.col[j], "double");
                }
                add_name(basename+".loc.pos.icrf.pass", &target[i].loc.pos.icrf.pass, "uint32_t");
                add_name(basename+".loc.pos.eci", &target[i].loc.pos.eci, "cartpos");
                add_name(basename+".loc.pos.eci.utc", &target[i].loc.pos.eci.utc, "double");
                add_name(basename+".loc.pos.eci.s", &target[i].loc.pos.eci.s, "rvector");
                add_name(basename+".loc.pos.eci.s.col", &target[i].loc.pos.eci.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.eci.s.col)/sizeof(target[i].loc.pos.eci.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.eci.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.eci.s.col[j], "double");
                }
                add_name(basename+".loc.pos.eci.v", &target[i].loc.pos.eci.v, "rvector");
                add_name(basename+".loc.pos.eci.v.col", &target[i].loc.pos.eci.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.eci.v.col)/sizeof(target[i].loc.pos.eci.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.eci.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.eci.v.col[j], "double");
                }
                add_name(basename+".loc.pos.eci.a", &target[i].loc.pos.eci.a, "rvector");
                add_name(basename+".loc.pos.eci.a.col", &target[i].loc.pos.eci.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.eci.a.col)/sizeof(target[i].loc.pos.eci.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.eci.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.eci.a.col[j], "double");
                }
                add_name(basename+".loc.pos.eci.pass", &target[i].loc.pos.eci.pass, "uint32_t");
                add_name(basename+".loc.pos.sci", &target[i].loc.pos.sci, "cartpos");
                add_name(basename+".loc.pos.sci.utc", &target[i].loc.pos.sci.utc, "double");
                add_name(basename+".loc.pos.sci.s", &target[i].loc.pos.sci.s, "rvector");
                add_name(basename+".loc.pos.sci.s.col", &target[i].loc.pos.sci.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.sci.s.col)/sizeof(target[i].loc.pos.sci.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.sci.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.sci.s.col[j], "double");
                }
                add_name(basename+".loc.pos.sci.v", &target[i].loc.pos.sci.v, "rvector");
                add_name(basename+".loc.pos.sci.v.col", &target[i].loc.pos.sci.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.sci.v.col)/sizeof(target[i].loc.pos.sci.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.sci.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.sci.v.col[j], "double");
                }
                add_name(basename+".loc.pos.sci.a", &target[i].loc.pos.sci.a, "rvector");
                add_name(basename+".loc.pos.sci.a.col", &target[i].loc.pos.sci.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.sci.a.col)/sizeof(target[i].loc.pos.sci.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.sci.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.sci.a.col[j], "double");
                }
                add_name(basename+".loc.pos.sci.pass", &target[i].loc.pos.sci.pass, "uint32_t");
                add_name(basename+".loc.pos.geoc", &target[i].loc.pos.geoc, "cartpos");
                add_name(basename+".loc.pos.geoc.utc", &target[i].loc.pos.geoc.utc, "double");
                add_name(basename+".loc.pos.geoc.s", &target[i].loc.pos.geoc.s, "rvector");
                add_name(basename+".loc.pos.geoc.s.col", &target[i].loc.pos.geoc.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.geoc.s.col)/sizeof(target[i].loc.pos.geoc.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.geoc.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.geoc.s.col[j], "double");
                }
                add_name(basename+".loc.pos.geoc.v", &target[i].loc.pos.geoc.v, "rvector");
                add_name(basename+".loc.pos.geoc.v.col", &target[i].loc.pos.geoc.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.geoc.v.col)/sizeof(target[i].loc.pos.geoc.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.geoc.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.geoc.v.col[j], "double");
                }
                add_name(basename+".loc.pos.geoc.a", &target[i].loc.pos.geoc.a, "rvector");
                add_name(basename+".loc.pos.geoc.a.col", &target[i].loc.pos.geoc.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.geoc.a.col)/sizeof(target[i].loc.pos.geoc.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.geoc.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.geoc.a.col[j], "double");
                }
                add_name(basename+".loc.pos.geoc.pass", &target[i].loc.pos.geoc.pass, "uint32_t");

                add_name(basename+".loc.pos.selc", &target[i].loc.pos.selc, "cartpos");
                add_name(basename+".loc.pos.selc.utc", &target[i].loc.pos.selc.utc, "double");
                add_name(basename+".loc.pos.selc.s", &target[i].loc.pos.selc.s, "rvector");
                add_name(basename+".loc.pos.selc.s.col", &target[i].loc.pos.selc.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.selc.s.col)/sizeof(target[i].loc.pos.selc.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.selc.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.selc.s.col[j], "double");
                }
                add_name(basename+".loc.pos.selc.v", &target[i].loc.pos.selc.v, "rvector");
                add_name(basename+".loc.pos.selc.v.col", &target[i].loc.pos.selc.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.selc.v.col)/sizeof(target[i].loc.pos.selc.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.selc.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.selc.v.col[j], "double");
                }
                add_name(basename+".loc.pos.selc.a", &target[i].loc.pos.selc.a, "rvector");
                add_name(basename+".loc.pos.selc.a.col", &target[i].loc.pos.selc.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.selc.a.col)/sizeof(target[i].loc.pos.selc.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.selc.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.selc.a.col[j], "double");
                }
                add_name(basename+".loc.pos.selc.pass", &target[i].loc.pos.selc.pass, "uint32_t");

                add_name(basename+".loc.pos.lvlh", &target[i].loc.pos.lvlh, "cartpos");
                add_name(basename+".loc.pos.lvlh.utc", &target[i].loc.pos.lvlh.utc, "double");
                add_name(basename+".loc.pos.lvlh.s", &target[i].loc.pos.lvlh.s, "rvector");
                add_name(basename+".loc.pos.lvlh.s.col", &target[i].loc.pos.lvlh.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.lvlh.s.col)/sizeof(target[i].loc.pos.lvlh.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.lvlh.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.lvlh.s.col[j], "double");
                }
                add_name(basename+".loc.pos.lvlh.v", &target[i].loc.pos.lvlh.v, "rvector");
                add_name(basename+".loc.pos.lvlh.v.col", &target[i].loc.pos.lvlh.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.lvlh.v.col)/sizeof(target[i].loc.pos.lvlh.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.lvlh.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.lvlh.v.col[j], "double");
                }
                add_name(basename+".loc.pos.lvlh.a", &target[i].loc.pos.lvlh.a, "rvector");
                add_name(basename+".loc.pos.lvlh.a.col", &target[i].loc.pos.lvlh.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.lvlh.a.col)/sizeof(target[i].loc.pos.lvlh.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.lvlh.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.lvlh.a.col[j], "double");
                }
                add_name(basename+".loc.pos.lvlh.pass", &target[i].loc.pos.lvlh.pass, "uint32_t");

                add_name(basename+".loc.pos.geod", &target[i].loc.pos.geod, "geoidpos");
                add_name(basename+".loc.pos.geod.utc", &target[i].loc.pos.geod.utc, "double");
                add_name(basename+".loc.pos.geod.s", &target[i].loc.pos.geod.s, "gvector");
                add_name(basename+".loc.pos.geod.s.lat", &target[i].loc.pos.geod.s.lat, "double");
                add_name(basename+".loc.pos.geod.s.lon", &target[i].loc.pos.geod.s.lon, "double");
                add_name(basename+".loc.pos.geod.s.h", &target[i].loc.pos.geod.s.h, "double");
                add_name(basename+".loc.pos.geod.v", &target[i].loc.pos.geod.v, "gvector");
                add_name(basename+".loc.pos.geod.v.lat", &target[i].loc.pos.geod.v.lat, "double");
                add_name(basename+".loc.pos.geod.v.lon", &target[i].loc.pos.geod.v.lon, "double");
                add_name(basename+".loc.pos.geod.v.h", &target[i].loc.pos.geod.v.h, "double");
                add_name(basename+".loc.pos.geod.a", &target[i].loc.pos.geod.a, "gvector");
                add_name(basename+".loc.pos.geod.a.lat", &target[i].loc.pos.geod.a.lat, "double");
                add_name(basename+".loc.pos.geod.a.lon", &target[i].loc.pos.geod.a.lon, "double");
                add_name(basename+".loc.pos.geod.a.h", &target[i].loc.pos.geod.a.h, "double");
                add_name(basename+".loc.pos.geod.pass", &target[i].loc.pos.geod.pass, "uint32_t");
                add_name(basename+".loc.pos.selg", &target[i].loc.pos.selg, "geoidpos");
                add_name(basename+".loc.pos.selg.utc", &target[i].loc.pos.selg.utc, "double");
                add_name(basename+".loc.pos.selg.s", &target[i].loc.pos.selg.s, "gvector");
                add_name(basename+".loc.pos.selg.s.lat", &target[i].loc.pos.selg.s.lat, "double");
                add_name(basename+".loc.pos.selg.s.lon", &target[i].loc.pos.selg.s.lon, "double");
                add_name(basename+".loc.pos.selg.s.h", &target[i].loc.pos.selg.s.h, "double");
                add_name(basename+".loc.pos.selg.v", &target[i].loc.pos.selg.v, "gvector");
                add_name(basename+".loc.pos.selg.v.lat", &target[i].loc.pos.selg.v.lat, "double");
                add_name(basename+".loc.pos.selg.v.lon", &target[i].loc.pos.selg.v.lon, "double");
                add_name(basename+".loc.pos.selg.v.h", &target[i].loc.pos.selg.v.h, "double");
                add_name(basename+".loc.pos.selg.a", &target[i].loc.pos.selg.a, "gvector");
                add_name(basename+".loc.pos.selg.a.lat", &target[i].loc.pos.selg.a.lat, "double");
                add_name(basename+".loc.pos.selg.a.lon", &target[i].loc.pos.selg.a.lon, "double");
                add_name(basename+".loc.pos.selg.a.h", &target[i].loc.pos.selg.a.h, "double");
                add_name(basename+".loc.pos.selg.pass", &target[i].loc.pos.selg.pass, "uint32_t");
                add_name(basename+".loc.pos.geos", &target[i].loc.pos.geos, "spherpos");
                add_name(basename+".loc.pos.geos.utc", &target[i].loc.pos.geos.utc, "double");
                add_name(basename+".loc.pos.geos.s", &target[i].loc.pos.geos.s, "svector");
                add_name(basename+".loc.pos.geos.s.phi", &target[i].loc.pos.geos.s.phi, "double");
                add_name(basename+".loc.pos.geos.s.lambda", &target[i].loc.pos.geos.s.lambda, "double");
                add_name(basename+".loc.pos.geos.s.r", &target[i].loc.pos.geos.s.r, "double");
                add_name(basename+".loc.pos.geos.v", &target[i].loc.pos.geos.v, "svector");
                add_name(basename+".loc.pos.geos.v.phi", &target[i].loc.pos.geos.v.phi, "double");
                add_name(basename+".loc.pos.geos.v.lambda", &target[i].loc.pos.geos.v.lambda, "double");
                add_name(basename+".loc.pos.geos.v.r", &target[i].loc.pos.geos.v.r, "double");
                add_name(basename+".loc.pos.geos.a", &target[i].loc.pos.geos.a, "svector");
                add_name(basename+".loc.pos.geos.a.phi", &target[i].loc.pos.geos.a.phi, "double");
                add_name(basename+".loc.pos.geos.a.lambda", &target[i].loc.pos.geos.a.lambda, "double");
                add_name(basename+".loc.pos.geos.a.r", &target[i].loc.pos.geos.a.r, "double");
                add_name(basename+".loc.pos.geos.pass", &target[i].loc.pos.geos.pass, "uint32_t");
                add_name(basename+".loc.pos.extra", &target[i].loc.pos.extra, "extrapos");
                add_name(basename+".loc.pos.extra.utc", &target[i].loc.pos.extra.utc, "double");
                add_name(basename+".loc.pos.extra.tt", &target[i].loc.pos.extra.tt, "double");
                add_name(basename+".loc.pos.extra.ut", &target[i].loc.pos.extra.ut, "double");
                add_name(basename+".loc.pos.extra.tdb", &target[i].loc.pos.extra.tdb, "double");
                add_name(basename+".loc.pos.extra.j2e", &target[i].loc.pos.extra.j2e, "rmatrix");
                add_name(basename+".loc.pos.extra.j2e.row", &target[i].loc.pos.extra.j2e.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.j2e.row)/sizeof(target[i].loc.pos.extra.j2e.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.j2e.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.j2e.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.j2e.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.j2e.row[j].col)/sizeof(target[i].loc.pos.extra.j2e.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.j2e.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.dj2e", &target[i].loc.pos.extra.dj2e, "rmatrix");
                add_name(basename+".loc.pos.extra.dj2e.row", &target[i].loc.pos.extra.dj2e.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.dj2e.row)/sizeof(target[i].loc.pos.extra.dj2e.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.dj2e.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.dj2e.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.dj2e.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.dj2e.row[j].col)/sizeof(target[i].loc.pos.extra.dj2e.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.dj2e.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.ddj2e", &target[i].loc.pos.extra.ddj2e, "rmatrix");
                add_name(basename+".loc.pos.extra.ddj2e.row", &target[i].loc.pos.extra.ddj2e.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.ddj2e.row)/sizeof(target[i].loc.pos.extra.ddj2e.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.ddj2e.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.ddj2e.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.ddj2e.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.ddj2e.row[j].col)/sizeof(target[i].loc.pos.extra.ddj2e.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.ddj2e.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.e2j", &target[i].loc.pos.extra.e2j, "rmatrix");
                add_name(basename+".loc.pos.extra.e2j.row", &target[i].loc.pos.extra.e2j.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.e2j.row)/sizeof(target[i].loc.pos.extra.e2j.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.e2j.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.e2j.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.e2j.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.e2j.row[j].col)/sizeof(target[i].loc.pos.extra.e2j.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.e2j.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.de2j", &target[i].loc.pos.extra.de2j, "rmatrix");
                add_name(basename+".loc.pos.extra.de2j.row", &target[i].loc.pos.extra.de2j.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.de2j.row)/sizeof(target[i].loc.pos.extra.de2j.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.de2j.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.de2j.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.de2j.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.de2j.row[j].col)/sizeof(target[i].loc.pos.extra.de2j.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.de2j.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.dde2j", &target[i].loc.pos.extra.dde2j, "rmatrix");
                add_name(basename+".loc.pos.extra.dde2j.row", &target[i].loc.pos.extra.dde2j.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.dde2j.row)/sizeof(target[i].loc.pos.extra.dde2j.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.dde2j.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.dde2j.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.dde2j.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.dde2j.row[j].col)/sizeof(target[i].loc.pos.extra.dde2j.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.dde2j.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.j2t", &target[i].loc.pos.extra.j2t, "rmatrix");
                add_name(basename+".loc.pos.extra.j2t.row", &target[i].loc.pos.extra.j2t.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.j2t.row)/sizeof(target[i].loc.pos.extra.j2t.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.j2t.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.j2t.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.j2t.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.j2t.row[j].col)/sizeof(target[i].loc.pos.extra.j2t.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.j2t.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.j2s", &target[i].loc.pos.extra.j2s, "rmatrix");
                add_name(basename+".loc.pos.extra.j2s.row", &target[i].loc.pos.extra.j2s.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.j2s.row)/sizeof(target[i].loc.pos.extra.j2s.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.j2s.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.j2s.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.j2s.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.j2s.row[j].col)/sizeof(target[i].loc.pos.extra.j2s.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.j2s.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.t2j", &target[i].loc.pos.extra.t2j, "rmatrix");
                add_name(basename+".loc.pos.extra.t2j.row", &target[i].loc.pos.extra.t2j.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.t2j.row)/sizeof(target[i].loc.pos.extra.t2j.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.t2j.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.t2j.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.t2j.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.t2j.row[j].col)/sizeof(target[i].loc.pos.extra.t2j.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.t2j.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.s2j", &target[i].loc.pos.extra.s2j, "rmatrix");
                add_name(basename+".loc.pos.extra.s2j.row", &target[i].loc.pos.extra.s2j.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.s2j.row)/sizeof(target[i].loc.pos.extra.s2j.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.s2j.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.s2j.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.s2j.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.s2j.row[j].col)/sizeof(target[i].loc.pos.extra.s2j.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.s2j.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.s2t", &target[i].loc.pos.extra.s2t, "rmatrix");
                add_name(basename+".loc.pos.extra.s2t.row", &target[i].loc.pos.extra.s2t.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.s2t.row)/sizeof(target[i].loc.pos.extra.s2t.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.s2t.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.s2t.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.s2t.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.s2t.row[j].col)/sizeof(target[i].loc.pos.extra.s2t.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.s2t.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.ds2t", &target[i].loc.pos.extra.ds2t, "rmatrix");
                add_name(basename+".loc.pos.extra.ds2t.row", &target[i].loc.pos.extra.ds2t.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.ds2t.row)/sizeof(target[i].loc.pos.extra.ds2t.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.ds2t.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.ds2t.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.ds2t.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.ds2t.row[j].col)/sizeof(target[i].loc.pos.extra.ds2t.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.ds2t.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.t2s", &target[i].loc.pos.extra.t2s, "rmatrix");
                add_name(basename+".loc.pos.extra.t2s.row", &target[i].loc.pos.extra.t2s.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.t2s.row)/sizeof(target[i].loc.pos.extra.t2s.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.t2s.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.t2s.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.t2s.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.t2s.row[j].col)/sizeof(target[i].loc.pos.extra.t2s.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.t2s.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.dt2s", &target[i].loc.pos.extra.dt2s, "rmatrix");
                add_name(basename+".loc.pos.extra.dt2s.row", &target[i].loc.pos.extra.dt2s.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.dt2s.row)/sizeof(target[i].loc.pos.extra.dt2s.row[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.dt2s.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.dt2s.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.pos.extra.dt2s.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.pos.extra.dt2s.row[j].col)/sizeof(target[i].loc.pos.extra.dt2s.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.pos.extra.dt2s.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.pos.extra.l2e", &target[i].loc.pos.extra.l2e, "quaternion");
                add_name(basename+".loc.pos.extra.l2e.d", &target[i].loc.pos.extra.l2e.d, "cvector");
                add_name(basename+".loc.pos.extra.l2e.d.x", &target[i].loc.pos.extra.l2e.d.x, "double");
                add_name(basename+".loc.pos.extra.l2e.d.y", &target[i].loc.pos.extra.l2e.d.y, "double");
                add_name(basename+".loc.pos.extra.l2e.d.z", &target[i].loc.pos.extra.l2e.d.z, "double");
                add_name(basename+".loc.pos.extra.l2e.w", &target[i].loc.pos.extra.l2e.w, "double");
                add_name(basename+".loc.pos.extra.e2l", &target[i].loc.pos.extra.e2l, "quaternion");
                add_name(basename+".loc.pos.extra.e2l.d", &target[i].loc.pos.extra.e2l.d, "cvector");
                add_name(basename+".loc.pos.extra.e2l.d.x", &target[i].loc.pos.extra.e2l.d.x, "double");
                add_name(basename+".loc.pos.extra.e2l.d.y", &target[i].loc.pos.extra.e2l.d.y, "double");
                add_name(basename+".loc.pos.extra.e2l.d.z", &target[i].loc.pos.extra.e2l.d.z, "double");
                add_name(basename+".loc.pos.extra.e2l.w", &target[i].loc.pos.extra.e2l.w, "double");
                add_name(basename+".loc.pos.extra.sun2earth", &target[i].loc.pos.extra.sun2earth, "cartpos");
                add_name(basename+".loc.pos.extra.sun2earth.utc", &target[i].loc.pos.extra.sun2earth.utc, "double");
                add_name(basename+".loc.pos.extra.sun2earth.s", &target[i].loc.pos.extra.sun2earth.s, "rvector");
                add_name(basename+".loc.pos.extra.sun2earth.s.col", &target[i].loc.pos.extra.sun2earth.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.sun2earth.s.col)/sizeof(target[i].loc.pos.extra.sun2earth.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.sun2earth.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.sun2earth.s.col[j], "double");
                }
                add_name(basename+".loc.pos.extra.sun2earth.v", &target[i].loc.pos.extra.sun2earth.v, "rvector");
                add_name(basename+".loc.pos.extra.sun2earth.v.col", &target[i].loc.pos.extra.sun2earth.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.sun2earth.v.col)/sizeof(target[i].loc.pos.extra.sun2earth.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.sun2earth.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.sun2earth.v.col[j], "double");
                }
                add_name(basename+".loc.pos.extra.sun2earth.a", &target[i].loc.pos.extra.sun2earth.a, "rvector");
                add_name(basename+".loc.pos.extra.sun2earth.a.col", &target[i].loc.pos.extra.sun2earth.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.sun2earth.a.col)/sizeof(target[i].loc.pos.extra.sun2earth.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.sun2earth.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.sun2earth.a.col[j], "double");
                }
                add_name(basename+".loc.pos.extra.sun2earth.pass", &target[i].loc.pos.extra.sun2earth.pass, "uint32_t");
                add_name(basename+".loc.pos.extra.sun2moon", &target[i].loc.pos.extra.sun2moon, "cartpos");
                add_name(basename+".loc.pos.extra.sun2moon.utc", &target[i].loc.pos.extra.sun2moon.utc, "double");
                add_name(basename+".loc.pos.extra.sun2moon.s", &target[i].loc.pos.extra.sun2moon.s, "rvector");
                add_name(basename+".loc.pos.extra.sun2moon.s.col", &target[i].loc.pos.extra.sun2moon.s.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.sun2moon.s.col)/sizeof(target[i].loc.pos.extra.sun2moon.s.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.sun2moon.s.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.sun2moon.s.col[j], "double");
                }
                add_name(basename+".loc.pos.extra.sun2moon.v", &target[i].loc.pos.extra.sun2moon.v, "rvector");
                add_name(basename+".loc.pos.extra.sun2moon.v.col", &target[i].loc.pos.extra.sun2moon.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.sun2moon.v.col)/sizeof(target[i].loc.pos.extra.sun2moon.v.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.sun2moon.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.sun2moon.v.col[j], "double");
                }
                add_name(basename+".loc.pos.extra.sun2moon.a", &target[i].loc.pos.extra.sun2moon.a, "rvector");
                add_name(basename+".loc.pos.extra.sun2moon.a.col", &target[i].loc.pos.extra.sun2moon.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.extra.sun2moon.a.col)/sizeof(target[i].loc.pos.extra.sun2moon.a.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.extra.sun2moon.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.extra.sun2moon.a.col[j], "double");
                }
                add_name(basename+".loc.pos.extra.sun2moon.pass", &target[i].loc.pos.extra.sun2moon.pass, "uint32_t");
                add_name(basename+".loc.pos.extra.closest", &target[i].loc.pos.extra.closest, "uint16_t");
                add_name(basename+".loc.pos.earthsep", &target[i].loc.pos.earthsep, "float");
                add_name(basename+".loc.pos.moonsep", &target[i].loc.pos.moonsep, "float");
                add_name(basename+".loc.pos.sunsize", &target[i].loc.pos.sunsize, "float");
                add_name(basename+".loc.pos.sunradiance", &target[i].loc.pos.sunradiance, "float");
                add_name(basename+".loc.pos.bearth", &target[i].loc.pos.bearth, "rvector");
                add_name(basename+".loc.pos.bearth.col", &target[i].loc.pos.bearth.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.pos.bearth.col)/sizeof(target[i].loc.pos.bearth.col[0]); ++j) {
                    string rebasename = basename + "loc.pos.bearth.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.pos.bearth.col[j], "double");
                }
                add_name(basename+".loc.pos.orbit", &target[i].loc.pos.orbit, "double");
                add_name(basename+".loc.att", &target[i].loc.att, "attstruc");
                add_name(basename+".loc.att.utc", &target[i].loc.att.utc, "double");
                add_name(basename+".loc.att.topo", &target[i].loc.att.topo, "qatt");
                add_name(basename+".loc.att.topo.utc", &target[i].loc.att.topo.utc, "double");
                add_name(basename+".loc.att.topo.s", &target[i].loc.att.topo.s, "quaternion");
                add_name(basename+".loc.att.topo.s.d", &target[i].loc.att.topo.s.d, "cvector");
                add_name(basename+".loc.att.topo.s.d.x", &target[i].loc.att.topo.s.d.x, "double");
                add_name(basename+".loc.att.topo.s.d.y", &target[i].loc.att.topo.s.d.y, "double");
                add_name(basename+".loc.att.topo.s.d.z", &target[i].loc.att.topo.s.d.z, "double");
                add_name(basename+".loc.att.topo.s.w", &target[i].loc.att.topo.s.w, "double");
                add_name(basename+".loc.att.topo.v", &target[i].loc.att.topo.v, "rvector");
                add_name(basename+".loc.att.topo.v.col", &target[i].loc.att.topo.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.topo.v.col)/sizeof(target[i].loc.att.topo.v.col[0]); ++j) {
                    string rebasename = basename + "loc.att.topo.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.topo.v.col[j], "double");
                }
                add_name(basename+".loc.att.topo.a", &target[i].loc.att.topo.a, "rvector");
                add_name(basename+".loc.att.topo.a.col", &target[i].loc.att.topo.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.topo.a.col)/sizeof(target[i].loc.att.topo.a.col[0]); ++j) {
                    string rebasename = basename + "loc.att.topo.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.topo.a.col[j], "double");
                }
                add_name(basename+".loc.att.topo.pass", &target[i].loc.att.topo.pass, "uint32_t");
                add_name(basename+".loc.att.lvlh", &target[i].loc.att.lvlh, "qatt");
                add_name(basename+".loc.att.lvlh.utc", &target[i].loc.att.lvlh.utc, "double");
                add_name(basename+".loc.att.lvlh.s", &target[i].loc.att.lvlh.s, "quaternion");
                add_name(basename+".loc.att.lvlh.s.d", &target[i].loc.att.lvlh.s.d, "cvector");
                add_name(basename+".loc.att.lvlh.s.d.x", &target[i].loc.att.lvlh.s.d.x, "double");
                add_name(basename+".loc.att.lvlh.s.d.y", &target[i].loc.att.lvlh.s.d.y, "double");
                add_name(basename+".loc.att.lvlh.s.d.z", &target[i].loc.att.lvlh.s.d.z, "double");
                add_name(basename+".loc.att.lvlh.s.w", &target[i].loc.att.lvlh.s.w, "double");
                add_name(basename+".loc.att.lvlh.v", &target[i].loc.att.lvlh.v, "rvector");
                add_name(basename+".loc.att.lvlh.v.col", &target[i].loc.att.lvlh.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.lvlh.v.col)/sizeof(target[i].loc.att.lvlh.v.col[0]); ++j) {
                    string rebasename = basename + "loc.att.lvlh.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.lvlh.v.col[j], "double");
                }
                add_name(basename+".loc.att.lvlh.a", &target[i].loc.att.lvlh.a, "rvector");
                add_name(basename+".loc.att.lvlh.a.col", &target[i].loc.att.lvlh.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.lvlh.a.col)/sizeof(target[i].loc.att.lvlh.a.col[0]); ++j) {
                    string rebasename = basename + "loc.att.lvlh.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.lvlh.a.col[j], "double");
                }
                add_name(basename+".loc.att.lvlh.pass", &target[i].loc.att.lvlh.pass, "uint32_t");
                add_name(basename+".loc.att.geoc", &target[i].loc.att.geoc, "qatt");
                add_name(basename+".loc.att.geoc.utc", &target[i].loc.att.geoc.utc, "double");
                add_name(basename+".loc.att.geoc.s", &target[i].loc.att.geoc.s, "quaternion");
                add_name(basename+".loc.att.geoc.s.d", &target[i].loc.att.geoc.s.d, "cvector");
                add_name(basename+".loc.att.geoc.s.d.x", &target[i].loc.att.geoc.s.d.x, "double");
                add_name(basename+".loc.att.geoc.s.d.y", &target[i].loc.att.geoc.s.d.y, "double");
                add_name(basename+".loc.att.geoc.s.d.z", &target[i].loc.att.geoc.s.d.z, "double");
                add_name(basename+".loc.att.geoc.s.w", &target[i].loc.att.geoc.s.w, "double");
                add_name(basename+".loc.att.geoc.v", &target[i].loc.att.geoc.v, "rvector");
                add_name(basename+".loc.att.geoc.v.col", &target[i].loc.att.geoc.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.geoc.v.col)/sizeof(target[i].loc.att.geoc.v.col[0]); ++j) {
                    string rebasename = basename + "loc.att.geoc.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.geoc.v.col[j], "double");
                }
                add_name(basename+".loc.att.geoc.a", &target[i].loc.att.geoc.a, "rvector");
                add_name(basename+".loc.att.geoc.a.col", &target[i].loc.att.geoc.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.geoc.a.col)/sizeof(target[i].loc.att.geoc.a.col[0]); ++j) {
                    string rebasename = basename + "loc.att.geoc.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.geoc.a.col[j], "double");
                }
                add_name(basename+".loc.att.geoc.pass", &target[i].loc.att.geoc.pass, "uint32_t");
                add_name(basename+".loc.att.selc", &target[i].loc.att.selc, "qatt");
                add_name(basename+".loc.att.selc.utc", &target[i].loc.att.selc.utc, "double");
                add_name(basename+".loc.att.selc.s", &target[i].loc.att.selc.s, "quaternion");
                add_name(basename+".loc.att.selc.s.d", &target[i].loc.att.selc.s.d, "cvector");
                add_name(basename+".loc.att.selc.s.d.x", &target[i].loc.att.selc.s.d.x, "double");
                add_name(basename+".loc.att.selc.s.d.y", &target[i].loc.att.selc.s.d.y, "double");
                add_name(basename+".loc.att.selc.s.d.z", &target[i].loc.att.selc.s.d.z, "double");
                add_name(basename+".loc.att.selc.s.w", &target[i].loc.att.selc.s.w, "double");
                add_name(basename+".loc.att.selc.v", &target[i].loc.att.selc.v, "rvector");
                add_name(basename+".loc.att.selc.v.col", &target[i].loc.att.selc.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.selc.v.col)/sizeof(target[i].loc.att.selc.v.col[0]); ++j) {
                    string rebasename = basename + "loc.att.selc.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.selc.v.col[j], "double");
                }
                add_name(basename+".loc.att.selc.a", &target[i].loc.att.selc.a, "rvector");
                add_name(basename+".loc.att.selc.a.col", &target[i].loc.att.selc.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.selc.a.col)/sizeof(target[i].loc.att.selc.a.col[0]); ++j) {
                    string rebasename = basename + "loc.att.selc.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.selc.a.col[j], "double");
                }
                add_name(basename+".loc.att.selc.pass", &target[i].loc.att.selc.pass, "uint32_t");
                add_name(basename+".loc.att.icrf", &target[i].loc.att.icrf, "qatt");
                add_name(basename+".loc.att.icrf.utc", &target[i].loc.att.icrf.utc, "double");
                add_name(basename+".loc.att.icrf.s", &target[i].loc.att.icrf.s, "quaternion");
                add_name(basename+".loc.att.icrf.s.d", &target[i].loc.att.icrf.s.d, "cvector");
                add_name(basename+".loc.att.icrf.s.d.x", &target[i].loc.att.icrf.s.d.x, "double");
                add_name(basename+".loc.att.icrf.s.d.y", &target[i].loc.att.icrf.s.d.y, "double");
                add_name(basename+".loc.att.icrf.s.d.z", &target[i].loc.att.icrf.s.d.z, "double");
                add_name(basename+".loc.att.icrf.s.w", &target[i].loc.att.icrf.s.w, "double");
                add_name(basename+".loc.att.icrf.v", &target[i].loc.att.icrf.v, "rvector");
                add_name(basename+".loc.att.icrf.v.col", &target[i].loc.att.icrf.v.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.icrf.v.col)/sizeof(target[i].loc.att.icrf.v.col[0]); ++j) {
                    string rebasename = basename + "loc.att.icrf.v.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.icrf.v.col[j], "double");
                }
                add_name(basename+".loc.att.icrf.a", &target[i].loc.att.icrf.a, "rvector");
                add_name(basename+".loc.att.icrf.a.col", &target[i].loc.att.icrf.a.col, "double[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.icrf.a.col)/sizeof(target[i].loc.att.icrf.a.col[0]); ++j) {
                    string rebasename = basename + "loc.att.icrf.a.col[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.icrf.a.col[j], "double");
                }
                add_name(basename+".loc.att.icrf.pass", &target[i].loc.att.icrf.pass, "uint32_t");
                add_name(basename+".loc.att.extra", &target[i].loc.att.extra, "extraatt");
                add_name(basename+".loc.att.extra.utc", &target[i].loc.att.extra.utc, "double");
                add_name(basename+".loc.att.extra.j2b", &target[i].loc.att.extra.j2b, "rmatrix");
                add_name(basename+".loc.att.extra.j2b.row", &target[i].loc.att.extra.j2b.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.extra.j2b.row)/sizeof(target[i].loc.att.extra.j2b.row[0]); ++j) {
                    string rebasename = basename + "loc.att.extra.j2b.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.extra.j2b.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.att.extra.j2b.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.att.extra.j2b.row[j].col)/sizeof(target[i].loc.att.extra.j2b.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.att.extra.j2b.row[j].col[k], "double");
                    }
                }
                add_name(basename+".loc.att.extra.b2j", &target[i].loc.att.extra.b2j, "rmatrix");
                add_name(basename+".loc.att.extra.b2j.row", &target[i].loc.att.extra.b2j.row, "rvector[]");
                for(size_t j = 0; j < sizeof(target[i].loc.att.extra.b2j.row)/sizeof(target[i].loc.att.extra.b2j.row[0]); ++j) {
                    string rebasename = basename + "loc.att.extra.b2j.row[" + std::to_string(j) + "]";
                    add_name(rebasename, &target[i].loc.att.extra.b2j.row[j], "rvector");
                    add_name(rebasename+".col", &target[i].loc.att.extra.b2j.row[j].col, "double[]");
                    for(size_t k = 0; k < sizeof(target[i].loc.att.extra.b2j.row[j].col)/sizeof(target[i].loc.att.extra.b2j.row[j].col[0]); ++k) {
                        string rebasename2 = rebasename + ".col[" + std::to_string(k) + "]";
                        add_name(rebasename2, &target[i].loc.att.extra.b2j.row[j].col[k], "double");
                    }
                }
                add_name(basename+".size", &target[i].size, "gvector");
                add_name(basename+".area", &target[i].area, "float");
                add_name(basename+".resolution", &target[i].resolution, "float");
            }


            // vector<userstruc> user
            add_name("user", &user, "vector<userstruc>");
            for(size_t i = 0; i < user.size(); ++i) {
                string basename = "user[" + std::to_string(i) + "]";
                add_name(basename, &user[i], "userstruc");
                add_name(basename+".name", &user[i].name, "string");
                add_name(basename+".node", &user[i].node, "string");
                add_name(basename+".tool", &user[i].tool, "string");
                add_name(basename+".cpu", &user[i].cpu, "string");
            }

            // vector<Convert::tlestruc> tle
            add_name("tle", &tle, "vector<Convert::tlestruc>");
            for(size_t i = 0; i < tle.size(); ++i) {
                string basename = "tle[" + std::to_string(i) + "]";
                add_name(basename, &tle[i], "tlestruc");
                add_name(basename+".utc", &tle[i].utc, "double");
                add_name(basename+".name", &tle[i].name, "char[]");
                add_name(basename+".snumber", &tle[i].snumber, "uint32_t");
                add_name(basename+".id", &tle[i].id, "char[]");
                add_name(basename+".bstar", &tle[i].bstar, "double");
                add_name(basename+".i", &tle[i].i, "double");
                add_name(basename+".raan", &tle[i].raan, "double");
                add_name(basename+".e", &tle[i].e, "double");
                add_name(basename+".ap", &tle[i].ap, "double");
                add_name(basename+".ma", &tle[i].ma, "double");
                add_name(basename+".mm", &tle[i].mm, "double");
                add_name(basename+".dmm", &tle[i].dmm, "double");
                add_name(basename+".orbit", &tle[i].orbit, "uint32_t");
            }

            // jsonnode json
            add_name("json", &json, "jsonnode");
            add_name("json.name", &json.name, "string");
            add_name("json.node", &json.node, "string");
            add_name("json.state", &json.state, "string");
            add_name("json.vertexs", &json.vertexs, "string");
            add_name("json.faces", &json.faces, "string");
            add_name("json.pieces", &json.pieces, "string");
            add_name("json.devgen", &json.devgen, "string");
            add_name("json.devspec", &json.devspec, "string");
            add_name("json.ports", &json.ports, "string");
            add_name("json.targets", &json.targets, "string");
        }

        string cosmos2table::insert_value(cosmosstruc& C, const string& name)	{
            string insert;
            if(C.get_type(name) == "string")	{
                insert+="\'" + C.get_value<string>(name) + "\'";
            } else if (C.get_type(name) == "double")	{
                insert += std::to_string(C.get_value<double>(name));
            } else if (C.get_type(name) == "float")	{
                insert += std::to_string(C.get_value<float>(name));
            } else if (C.get_type(name) == "uint32_t")	{
                insert += std::to_string(C.get_value<uint32_t>(name));
            } else if (C.get_type(name) == "uint16_t")	{
                insert += std::to_string(C.get_value<uint16_t>(name));
            } else if (C.get_type(name) == "uint8_t")	{
                insert += std::to_string(C.get_value<uint8_t>(name));
            } else if (C.get_type(name) == "int32_t")	{
                insert += std::to_string(C.get_value<int32_t>(name));
            } else if (C.get_type(name) == "int16_t")	{
                insert += std::to_string(C.get_value<int16_t>(name));
            } else if (C.get_type(name) == "int8_t")	{
                insert += std::to_string(C.get_value<int8_t>(name));
            } else if (C.get_type(name) == "bool")	{
                insert += std::to_string(C.get_value<bool>(name));
            }
            return insert;
        }

        string cosmos2table::insert_statement(cosmosstruc& C)	{
            // namespace_names must match number of column_names, or be a multiple of it (for handling multiple rows in one instance of this class)
            bool rows_are_even = (namespace_names.size() % column_names.size()) == 0;
            if(column_names.size() == 0 || namespace_names.size() == 0 || !rows_are_even)	return "";

            size_t num_rows = namespace_names.size()/column_names.size();
            string insert = insert_type + " into " + schema_name + "." + table_name + " (" + column_names[0];
            for(size_t i = 1; i < column_names.size(); ++i)	{ insert += ", " + column_names[i]; }
            insert += ") values ";
            for (size_t row = 0; row < num_rows; ++row)
            {
                insert += "(";
                size_t start_offset = row*column_names.size();
                for(size_t i = start_offset; i < start_offset+column_names.size(); ++i)	{ insert += insert_value(C, namespace_names[i]) + ","; }
                insert.back() = ')';
                insert += ",";
            }
            insert.back() = ';';

            return insert;
        }
        
        void DBSchema::init(cosmosstruc* db_cinfo)
        {
            cinfo = db_cinfo;
            string schema_name = "cosmos";
            vector<string> column_names, namespace_names;

            // node table
            string table_name = "node";
            column_names.push_back("node_name");
            column_names.push_back("agent_name");
            column_names.push_back("utc");
            column_names.push_back("utcstart");
            // for (uint16_t i=0; i<cinfo->agent.size(); ++i)
            // {
                //cinfo->agent[i].beat.node;proc
                namespace_names.push_back("node.name");
                namespace_names.push_back("agent0.name");
                namespace_names.push_back("node.utc");
                namespace_names.push_back("node.utcstart");
            // }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "replace"));
            column_names.clear();
            namespace_names.clear();

            // device table
            table_name = "device";
            column_names.push_back("node_name");
            column_names.push_back("type");
            column_names.push_back("cidx");
            column_names.push_back("didx");
            column_names.push_back("name");
            for (uint16_t i=0; i<cinfo->device.size(); ++i)
            {
                string cidx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("device["+cidx+"].type");
                namespace_names.push_back("device["+cidx+"].cidx");
                namespace_names.push_back("device["+cidx+"].didx");
                namespace_names.push_back("device["+cidx+"].name");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "replace"));
            column_names.clear();
            namespace_names.clear();

            // battstruc table
            table_name = "battstruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("volt");
            column_names.push_back("amp");
            column_names.push_back("power");
            column_names.push_back("temp");
            column_names.push_back("percentage");
            for (uint16_t i=0; i<cinfo->devspec.batt.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.batt["+didx+"].didx");
                namespace_names.push_back("devspec.batt["+didx+"].utc");
                namespace_names.push_back("devspec.batt["+didx+"].volt");
                namespace_names.push_back("devspec.batt["+didx+"].amp");
                namespace_names.push_back("devspec.batt["+didx+"].power");
                namespace_names.push_back("devspec.batt["+didx+"].temp");
                namespace_names.push_back("devspec.batt["+didx+"].percentage");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // bcregstruc table
            table_name = "bcregstruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("volt");
            column_names.push_back("amp");
            column_names.push_back("power");
            column_names.push_back("temp");
            column_names.push_back("mpptin_amp");
            column_names.push_back("mpptin_volt");
            column_names.push_back("mpptout_amp");
            column_names.push_back("mpptout_volt");
            for (uint16_t i=0; i<cinfo->devspec.bcreg.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.bcreg["+didx+"].didx");
                namespace_names.push_back("devspec.bcreg["+didx+"].utc");
                namespace_names.push_back("devspec.bcreg["+didx+"].volt");
                namespace_names.push_back("devspec.bcreg["+didx+"].amp");
                namespace_names.push_back("devspec.bcreg["+didx+"].power");
                namespace_names.push_back("devspec.bcreg["+didx+"].temp");
                namespace_names.push_back("devspec.bcreg["+didx+"].mpptin_amp");
                namespace_names.push_back("devspec.bcreg["+didx+"].mpptin_volt");
                namespace_names.push_back("devspec.bcreg["+didx+"].mpptout_amp");
                namespace_names.push_back("devspec.bcreg["+didx+"].mpptout_volt");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // cpustruc table
            table_name = "cpustruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("temp");
            column_names.push_back("uptime");
            column_names.push_back("cpu_load");
            column_names.push_back("gib");
            column_names.push_back("boot_count");
            column_names.push_back("storage");
            for (uint16_t i=0; i<cinfo->devspec.cpu.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.cpu["+didx+"].didx");
                namespace_names.push_back("devspec.cpu["+didx+"].utc");
                namespace_names.push_back("devspec.cpu["+didx+"].temp");
                namespace_names.push_back("devspec.cpu["+didx+"].uptime");
                namespace_names.push_back("devspec.cpu["+didx+"].load");
                namespace_names.push_back("devspec.cpu["+didx+"].gib");
                namespace_names.push_back("devspec.cpu["+didx+"].boot_count");
                namespace_names.push_back("devspec.cpu["+didx+"].storage");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // gyrostruc table
            table_name = "gyrostruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("omega");
            for (uint16_t i=0; i<cinfo->devspec.gyro.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.gyro["+didx+"].didx");
                namespace_names.push_back("devspec.gyro["+didx+"].utc");
                namespace_names.push_back("devspec.gyro["+didx+"].omega");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // magstruc table
            table_name = "magstruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("mag_x");
            column_names.push_back("mag_y");
            column_names.push_back("mag_z");
            for (uint16_t i=0; i<cinfo->devspec.mag.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.mag["+didx+"].didx");
                namespace_names.push_back("devspec.mag["+didx+"].utc");
                namespace_names.push_back("devspec.mag["+didx+"].col[0]");
                namespace_names.push_back("devspec.mag["+didx+"].col[1]");
                namespace_names.push_back("devspec.mag["+didx+"].col[2]");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // mtrstruc table
            table_name = "mtrstruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("mom");
            column_names.push_back("align_w");
            column_names.push_back("align_x");
            column_names.push_back("align_y");
            column_names.push_back("align_z");
            for (uint16_t i=0; i<cinfo->devspec.mtr.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.mtr["+didx+"].didx");
                namespace_names.push_back("devspec.mtr["+didx+"].utc");
                namespace_names.push_back("devspec.mtr["+didx+"].mom");
                namespace_names.push_back("devspec.mtr["+didx+"].align.w");
                namespace_names.push_back("devspec.mtr["+didx+"].align.d.x");
                namespace_names.push_back("devspec.mtr["+didx+"].align.d.y");
                namespace_names.push_back("devspec.mtr["+didx+"].align.d.z");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // rwstruc table
            table_name = "rwstruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("amp");
            column_names.push_back("omg");
            column_names.push_back("romg");
            for (uint16_t i=0; i<cinfo->devspec.rw.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.rw["+didx+"].didx");
                namespace_names.push_back("devspec.rw["+didx+"].utc");
                namespace_names.push_back("devspec.rw["+didx+"].amp");
                namespace_names.push_back("devspec.rw["+didx+"].omg");
                namespace_names.push_back("devspec.rw["+didx+"].romg");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // swchstruc table
            table_name = "swchstruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("volt");
            column_names.push_back("amp");
            column_names.push_back("power");
            column_names.push_back("temp");
            for (uint16_t i=0; i<cinfo->devspec.swch.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.swch["+didx+"].didx");
                namespace_names.push_back("devspec.swch["+didx+"].utc");
                namespace_names.push_back("devspec.swch["+didx+"].volt");
                namespace_names.push_back("devspec.swch["+didx+"].amp");
                namespace_names.push_back("devspec.swch["+didx+"].power");
                namespace_names.push_back("devspec.swch["+didx+"].temp");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // tsenstruc table
            table_name = "tsenstruc";
            column_names.push_back("node_name");
            column_names.push_back("didx");
            column_names.push_back("utc");
            column_names.push_back("volt");
            column_names.push_back("amp");
            column_names.push_back("power");
            column_names.push_back("temp");
            for (uint16_t i=0; i<cinfo->devspec.tsen.size(); ++i)
            {
                string didx = std::to_string(i);
                namespace_names.push_back("node.name");
                namespace_names.push_back("devspec.tsen["+didx+"].didx");
                namespace_names.push_back("devspec.tsen["+didx+"].utc");
                namespace_names.push_back("devspec.tsen["+didx+"].volt");
                namespace_names.push_back("devspec.tsen["+didx+"].amp");
                namespace_names.push_back("devspec.tsen["+didx+"].power");
                namespace_names.push_back("devspec.tsen["+didx+"].temp");
            }
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names, "insert ignore"));
            column_names.clear();
            namespace_names.clear();

            // locstruc_eci table
            table_name = "locstruc_eci";
            column_names.push_back("node_name");
            column_names.push_back("utc");
            column_names.push_back("s_x");
            column_names.push_back("s_y");
            column_names.push_back("s_z");
            column_names.push_back("v_x");
            column_names.push_back("v_y");
            column_names.push_back("v_z");
            column_names.push_back("a_x");
            column_names.push_back("a_y");
            column_names.push_back("a_z");
            namespace_names.push_back("node.name");
            namespace_names.push_back("node.loc.pos.eci.utc");
            namespace_names.push_back("node.loc.pos.eci.s.col[0]");
            namespace_names.push_back("node.loc.pos.eci.s.col[1]");
            namespace_names.push_back("node.loc.pos.eci.s.col[2]");
            namespace_names.push_back("node.loc.pos.eci.v.col[0]");
            namespace_names.push_back("node.loc.pos.eci.v.col[1]");
            namespace_names.push_back("node.loc.pos.eci.v.col[2]");
            namespace_names.push_back("node.loc.pos.eci.a.col[0]");
            namespace_names.push_back("node.loc.pos.eci.a.col[1]");
            namespace_names.push_back("node.loc.pos.eci.a.col[2]");
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names));
            column_names.clear();
            namespace_names.clear();

            // attstruc_icrf table
            table_name = "attstruc_icrf";
            column_names.push_back("node_name");
            column_names.push_back("utc");
            column_names.push_back("s_x");
            column_names.push_back("s_y");
            column_names.push_back("s_z");
            column_names.push_back("s_w");
            column_names.push_back("omega_x");
            column_names.push_back("omega_y");
            column_names.push_back("omega_z");
            column_names.push_back("alpha_x");
            column_names.push_back("alpha_y");
            column_names.push_back("alpha_z");
            namespace_names.push_back("node.name");
            namespace_names.push_back("node.loc.att.icrf.utc");
            namespace_names.push_back("node.loc.att.icrf.s.d.x");
            namespace_names.push_back("node.loc.att.icrf.s.d.y");
            namespace_names.push_back("node.loc.att.icrf.s.d.z");
            namespace_names.push_back("node.loc.att.icrf.s.w");
            namespace_names.push_back("node.loc.att.icrf.v.col[0]");
            namespace_names.push_back("node.loc.att.icrf.v.col[1]");
            namespace_names.push_back("node.loc.att.icrf.v.col[2]");
            namespace_names.push_back("node.loc.att.icrf.a.col[0]");
            namespace_names.push_back("node.loc.att.icrf.a.col[1]");
            namespace_names.push_back("node.loc.att.icrf.a.col[2]");
            tables.push_back(cosmos2table(schema_name, table_name, column_names, namespace_names));
            column_names.clear();
            namespace_names.clear();
        }

        string DBSchema::init_database()	{
            string init;

            init += "drop database if exists " + schema_name + ";\n";
            init += "create database " + schema_name + ";\n";
            init += "use " + schema_name + ";\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".node (\n";
            init += "node_name VARCHAR(40) NOT NULL UNIQUE, #nodestruc\n";
            init += "agent_name VARCHAR(40) NOT NULL, #nodestruc\n";
            init += "utc DOUBLE, #nodestruc\n";
            init += "utcstart DOUBLE, #nodestruc\n";
            init += "PRIMARY KEY (node_name)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".battstruc (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "didx TINYINT UNSIGNED NOT NULL, #devicestruc\n";
            init += "utc DOUBLE NOT NULL, #devicestruc\n";
            init += "volt DECIMAL(5,2), #devicestruc\n";
            init += "amp DECIMAL(5,2), #devicestruc\n";
            init += "power DECIMAL(5,2), #devicestruc\n";
            init += "temp DECIMAL(5,2), #devicestruc\n";
            init += "percentage DECIMAL(5,2), #battstruc\n";
            init += "PRIMARY KEY (node_name, didx, utc)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".bcregstruc (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "didx TINYINT UNSIGNED NOT NULL, #devicestruc\n";
            init += "utc DOUBLE NOT NULL, #devicestruc\n";
            init += "volt DECIMAL(5,2), #devicestruc\n";
            init += "amp DECIMAL(5,2), #devicestruc\n";
            init += "power DECIMAL(5,2), #devicestruc\n";
            init += "temp DECIMAL(5,2), #devicestruc\n";
            init += "mpptin_amp DECIMAL(5,2),    #bcregstruc\n";
            init += "mpptin_volt DECIMAL(5,2),   #bcregstruc\n";
            init += "mpptout_amp DECIMAL(5,2),   #bcregstruc\n";
            init += "mpptout_volt DECIMAL(5,2),  #bcregstruc\n";
            init += "PRIMARY KEY (node_name, didx, utc)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".cpustruc (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "didx TINYINT UNSIGNED NOT NULL, #devicestruc\n";
            init += "utc DOUBLE NOT NULL, #devicestruc\n";
            init += "temp DECIMAL(5,2), #devicestruc\n";
            init += "uptime INT UNSIGNED,    #cpustruc\n";
            init += "cpu_load DECIMAL(5,2),  #cpustruc\n";
            init += "gib DECIMAL(5,2),   #cpustruc\n";
            init += "boot_count INT UNSIGNED,    #cpustruc\n";
            init += "storage DECIMAL(5,2),   #cpustruc\n";
            init += "PRIMARY KEY (node_name, didx, utc)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".magstruc (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "didx TINYINT UNSIGNED NOT NULL, #devicestruc\n";
            init += "utc DOUBLE NOT NULL, #devicestruc\n";
            init += "mag_x DECIMAL(5,2),\n";
            init += "mag_y DECIMAL(5,2),\n";
            init += "mag_z DECIMAL(5,2),\n";
            init += "PRIMARY KEY (node_name, didx, utc)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".swchstruc (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "didx TINYINT UNSIGNED NOT NULL,\n";
            init += "utc DOUBLE NOT NULL, #devicestruc\n";
            init += "volt DECIMAL(5,2), #devicestruc\n";
            init += "amp DECIMAL(5,2), #devicestruc\n";
            init += "power DECIMAL(5,2), #devicestruc\n";
            init += "temp DECIMAL(5,2), #devicestruc\n";
            init += "PRIMARY KEY (node_name, didx, utc)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".tsenstruc (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "didx TINYINT UNSIGNED NOT NULL,\n";
            init += "utc DOUBLE NOT NULL, #devicestruc\n";
            init += "temp DECIMAL(5,2), #devicestruc\n";
            init += "PRIMARY KEY (node_name, didx, utc)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".locstruc_eci (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "utc DOUBLE NOT NULL,\n";
            init += "s_x DOUBLE,\n";
            init += "s_y DOUBLE,\n";
            init += "s_z DOUBLE,\n";
            init += "v_x DOUBLE,\n";
            init += "v_y DOUBLE,\n";
            init += "v_z DOUBLE,\n";
            init += "a_x DOUBLE,\n";
            init += "a_y DOUBLE,\n";
            init += "a_z DOUBLE,\n";
            init += "PRIMARY KEY (node_name, utc)\n";
            init += ");\n";

            init += "CREATE TABLE IF NOT EXISTS " + schema_name + ".attstruc_icrf (\n";
            init += "node_name VARCHAR(40) NOT NULL,\n";
            init += "utc DOUBLE NOT NULL,\n";
            init += "s_x DOUBLE,\n";
            init += "s_y DOUBLE,\n";
            init += "s_z DOUBLE,\n";
            init += "s_w DOUBLE,\n";
            init += "omega_x DOUBLE,\n";
            init += "omega_y DOUBLE,\n";
            init += "omega_z DOUBLE,\n";
            init += "alpha_x DOUBLE,\n";
            init += "alpha_y DOUBLE,\n";
            init += "alpha_z DOUBLE,\n";
            init += "PRIMARY KEY (node_name, utc)\n";
            init += ");\n";

            return init;
        }

        int32_t DBSchema::find_table(const string& name)
        {
            for (size_t i=0; i < tables.size(); ++i)
            {
                if (tables[i].table_name == name)
                {
                    return i;
                }
            }
            return COSMOS_GENERAL_ERROR_NAME;
        }

        string DBSchema::table_insert_statement(const string& name)
        {
            int32_t iretn = find_table(name);
            if (iretn < 0)
            {
                return "";
            }
            return tables[iretn].insert_statement(*cinfo);
        }

        vector<string> DBSchema::multi_table_insert_statement(const vector<string>& names)
        {
            int32_t iretn = 0;
            vector<string> statements;
            for (const string& name : names)
            {
                iretn = find_table(name);
                if (iretn < 0)
                {
                    continue;
                }
                string statement = tables[iretn].insert_statement(*cinfo);
                if (statement.size())
                {
                    statements.push_back(statement);
                }
            }
            
            return statements;
        }
        #endif
    } // End namespace Support
} // End namespace Cosmos
