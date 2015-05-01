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

#include "jsonlib.h"
#include "jsonlib.h"

cosmosstruc *cdata;

int main(int argc, char *argv[])
{
string ojstring;

cdata = json_create();

cdata[0].event[0].l.utc = cdata[0].event[0].l.utcexec = 0.;
cdata[0].event[0].l.node[0] = 0;
cdata[0].event[0].l.data[0] = 0;
cdata[0].event[0].l.type = EVENT_TYPE_PHYSICAL;

// Umbra In
cdata[0].event[0].l.type = EVENT_TYPE_UMBRA;
strcpy(cdata[0].event[0].l.name,"UMB_IN");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR;
cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (6*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_earthsep");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_earthsep\\\"<0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Umbra Out
cdata[0].event[0].l.type = EVENT_TYPE_UMBRA;
strcpy(cdata[0].event[0].l.name,"UMB_OUT");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_EXIT | EVENT_FLAG_PAIR;
cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (6*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_pos_earthsep");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_earthsep\\\">0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 60 Descending
cdata[0].event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata[0].event[0].l.name,"N60D");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (0*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 30 Descending
cdata[0].event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata[0].event[0].l.name,"N30D");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (0*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 60 Descending
cdata[0].event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata[0].event[0].l.name,"S60D");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<-1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 30 Descending
cdata[0].event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata[0].event[0].l.name,"S30D");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<-0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 60 Ascending
cdata[0].event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata[0].event[0].l.name,"N60A");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 30 Ascending
cdata[0].event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata[0].event[0].l.name,"N30A");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 60 Ascending
cdata[0].event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata[0].event[0].l.name,"S60A");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">-1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 30 Ascending
cdata[0].event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata[0].event[0].l.name,"S30A");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">-0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Equator Descending
cdata[0].event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata[0].event[0].l.name,"EQD");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<0.0)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Equator Ascending
cdata[0].event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata[0].event[0].l.name,"EQA");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">0.0)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Maximum Ascending
cdata[0].event[0].l.type = EVENT_TYPE_LATMAX;
strcpy(cdata[0].event[0].l.name,"MaxN");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_v_lat\\\"<0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Maximum Descending
cdata[0].event[0].l.type = EVENT_TYPE_LATMIN;
strcpy(cdata[0].event[0].l.name,"MaxS");
cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata[0].event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata[0].event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata[0].event[0].l.condition,"(\\\"node_loc_pos_geod_v_lat\\\">0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Ground station Events
	for (int i=0; i<100; ++i)
	{
		// Target in view
		sprintf(cdata[0].event[0].l.name,"TIV_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (2*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_TARG;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=5)&(\\\"target_elto_%03d\\\">\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Target out of view
		sprintf(cdata[0].event[0].l.name,"TOV_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR | EVENT_FLAG_EXIT;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (2*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_TARG;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=5)&(\\\"target_elto_%03d\\\"<\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Target minimum range
		sprintf(cdata[0].event[0].l.name,"TMR_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata[0].event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (2*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.flag |= EVENT_FLAG_COUNTDOWN;
		cdata[0].event[0].l.type = EVENT_TYPE_TARG;
		sprintf(cdata[0].event[0].l.data,"target_range_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=5)&(\\\"target_close_%03d\\\"<0))",i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Acquisition of Signal
		sprintf(cdata[0].event[0].l.name,"AOS_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_GS;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\">\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Acquisition of Signal + 5 degrees
		sprintf(cdata[0].event[0].l.name,"AOS+5_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_GS5;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\">(\\\"target_min_%03d\\\"+.0873)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Acquisition of Signal + 10 degrees
		sprintf(cdata[0].event[0].l.name,"AOS+10_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_GS10;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\">(\\\"target_min_%03d\\\"+.1745)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Loss of Signal
		sprintf(cdata[0].event[0].l.name,"LOS_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR | EVENT_FLAG_EXIT;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_GS;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\"<\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Loss of Signal + 5 degrees
		sprintf(cdata[0].event[0].l.name,"LOS+5_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_GS5;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\"<(\\\"target_min_%03d\\\"+.0873)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Loss of Signal + 10 degrees
		sprintf(cdata[0].event[0].l.name,"LOS+10_${target_name_%03d}",i);
		cdata[0].event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata[0].event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata[0].event[0].l.type = EVENT_TYPE_GS10;
		sprintf(cdata[0].event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata[0].event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\"<(\\\"target_min_%03d\\\"+.1745)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));
	}
}
