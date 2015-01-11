#include "jsonlib.h"
#include "jsonlib.h"

cosmosstruc *cdata;

int main(int argc, char *argv[])
{
string ojstring;

cdata = json_create();

cdata->event[0].l.utc = cdata->event[0].l.utcexec = 0.;
cdata->event[0].l.node[0] = 0;
cdata->event[0].l.data[0] = 0;
cdata->event[0].l.type = EVENT_TYPE_PHYSICAL;

// Umbra In
cdata->event[0].l.type = EVENT_TYPE_UMBRA;
strcpy(cdata->event[0].l.name,"UMB_IN");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR;
cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (6*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_earthsep");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_earthsep\\\"<0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Umbra Out
cdata->event[0].l.type = EVENT_TYPE_UMBRA;
strcpy(cdata->event[0].l.name,"UMB_OUT");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_EXIT | EVENT_FLAG_PAIR;
cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (6*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_pos_earthsep");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_earthsep\\\">0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 60 Descending
cdata->event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata->event[0].l.name,"N60D");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (0*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 30 Descending
cdata->event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata->event[0].l.name,"N30D");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (0*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 60 Descending
cdata->event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata->event[0].l.name,"S60D");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<-1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 30 Descending
cdata->event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata->event[0].l.name,"S30D");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<-0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 60 Ascending
cdata->event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata->event[0].l.name,"N60A");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// North 30 Ascending
cdata->event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata->event[0].l.name,"N30A");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 60 Ascending
cdata->event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata->event[0].l.name,"S60A");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">-1.0472)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// South 30 Ascending
cdata->event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata->event[0].l.name,"S30A");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">-0.5236)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Equator Descending
cdata->event[0].l.type = EVENT_TYPE_LATD;
strcpy(cdata->event[0].l.name,"EQD");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\"<0.0)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Equator Ascending
cdata->event[0].l.type = EVENT_TYPE_LATA;
strcpy(cdata->event[0].l.name,"EQA");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_s_lat\\\">0.0)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Maximum Ascending
cdata->event[0].l.type = EVENT_TYPE_LATMAX;
strcpy(cdata->event[0].l.name,"MaxN");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_v_lat\\\"<0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Maximum Descending
cdata->event[0].l.type = EVENT_TYPE_LATMIN;
strcpy(cdata->event[0].l.name,"MaxS");
cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
cdata->event[0].l.flag |= (2*EVENT_SCALE_PRIORITY) | (9*EVENT_SCALE_COLOR);
strcpy(cdata->event[0].l.data,"node_loc_pos_geod_s_lat");
strcpy(cdata->event[0].l.condition,"(\\\"node_loc_pos_geod_v_lat\\\">0.)");
printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

// Ground station Events
	for (int i=0; i<100; ++i)
	{
		// Target in view
		sprintf(cdata->event[0].l.name,"TIV_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (2*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_TARG;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=5)&(\\\"target_elto_%03d\\\">\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Target out of view
		sprintf(cdata->event[0].l.name,"TOV_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR | EVENT_FLAG_EXIT;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (2*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_TARG;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=5)&(\\\"target_elto_%03d\\\"<\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Target minimum range
		sprintf(cdata->event[0].l.name,"TMR_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata->event[0].l.flag |= (3*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (0*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (2*EVENT_SCALE_COLOR);
		cdata->event[0].l.flag |= EVENT_FLAG_COUNTDOWN;
		cdata->event[0].l.type = EVENT_TYPE_TARG;
		sprintf(cdata->event[0].l.data,"target_range_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=5)&(\\\"target_close_%03d\\\"<0))",i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Acquisition of Signal
		sprintf(cdata->event[0].l.name,"AOS_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_GS;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\">\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Acquisition of Signal + 5 degrees
		sprintf(cdata->event[0].l.name,"AOS+5_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_GS5;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\">(\\\"target_min_%03d\\\"+.0873)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Acquisition of Signal + 10 degrees
		sprintf(cdata->event[0].l.name,"AOS+10_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_GS10;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\">(\\\"target_min_%03d\\\"+.1745)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Loss of Signal
		sprintf(cdata->event[0].l.name,"LOS_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT | EVENT_FLAG_PAIR | EVENT_FLAG_EXIT;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_GS;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\"<\\\"target_min_%03d\\\"))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Loss of Signal + 5 degrees
		sprintf(cdata->event[0].l.name,"LOS+5_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_GS5;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\"<(\\\"target_min_%03d\\\"+.0873)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));

		// Loss of Signal + 10 degrees
		sprintf(cdata->event[0].l.name,"LOS+10_${target_name_%03d}",i);
		cdata->event[0].l.flag = EVENT_FLAG_ACTUAL | EVENT_FLAG_CONDITIONAL | EVENT_FLAG_REPEAT;
		cdata->event[0].l.flag |= (1*EVENT_SCALE_PRIORITY) | (1*EVENT_FLAG_COUNTDOWN) | (3*EVENT_SCALE_WARNING) | (0*EVENT_SCALE_ALARM) | (3*EVENT_SCALE_COLOR);
		cdata->event[0].l.type = EVENT_TYPE_GS10;
		sprintf(cdata->event[0].l.data,"target_elto_%03d",i);
		sprintf(cdata->event[0].l.condition,"((\\\"target_type_%03d\\\"=1)&(\\\"target_elto_%03d\\\"<(\\\"target_min_%03d\\\"+.1745)))",i,i,i);
		printf("%s\n",json_of_list(ojstring, (char *)"{\"event_name\",\"event_type\",\"event_flag\",\"event_condition\",\"event_data\"}",cdata));
	}
}
