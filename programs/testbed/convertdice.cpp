#include "physicslib.h"
#include "mathlib.h"
#include "jsonlib.h"
#include "datalib.h"
#include "jsonlib.h"
#include "timelib.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

//#define MMCORRECT -2.766301289e-10
#define MMCORRECT 0.0

cosmosstruc *cdata;
stkstruc stk;
shorteventstruc events[20];
string mainjstring;
int ecount;

int main(int argc, char *argv[])
{
	int i;
	FILE *fp1, *fp2, *fp3;
	cartpos npos;
	double lmjd=0.;
	double nmjd;
	double utc;
	rvector bearth;
	ssenstruc ssen[1];
	tsenstruc tsen[14];


	cdata->physics.mode = atol(argv[1]);
	node_init((char *)"dice",cdata);

	load_lines((char *)"tle_dice1.tle", cdata->tle);

	fp1 = fopen("dice_attitude_mag_gpsweek_1691.txt","r");
	fp2 = fopen("dice_attitude_sun_gpsweek_1691.txt","r");
	fp3 = fopen("dice_housekeeping_tmp_gpsweek_1691.txt","r");

	fscanf(fp1,"%lf %lf %lf %lf",&utc,&bearth.col[0],&bearth.col[1],&bearth.col[2]);
	lmjd = (utc * 864000.) / 864000.;
	nmjd = (int)(lmjd);

	for (utc=nmjd; utc<lmjd; utc+=10./86400.)
	{
		cdata->node.loc.utc = currentmjd(cdata->node.utcoffset);
		if (lines2eci(utc, cdata->tle, &npos) < 0)
			exit (1);
		cdata->node.loc.utc = utc;
		update_eci(*cdata, cdata->node.loc.utc, npos);
		log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,json_of_soh(mainjstring, cdata));
		//		ecount = check_events(events,20,cdata);
		for (i=0; i<ecount; i++)
		{
			cdata->event[0].s = events[i];
			strcpy(cdata->event[0].l.condition,cdata->emap[events[i].handle.hash][events[i].handle.index].text);
			log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, cdata));
		}
	}

	cdata->node.loc.utc = lmjd;

	while (1)
	{
		do
		{
			if (fscanf(fp1,"%lf %lf %lf %lf",&utc,&bearth.col[0],&bearth.col[1],&bearth.col[2]) == EOF)
				break;
		} while (utc < lmjd);
		if (feof(fp1))
			break;

		do
		{
			if (fscanf(fp2,"%lf %f %f %f %f",&utc,&ssen[0].qva,&ssen[0].qvb,&ssen[0].qvc,&ssen[0].qvd) == EOF)
				break;
		} while (utc < lmjd);
		if (feof(fp1))
			break;

		do
		{
			if (fscanf(fp3,"%lf",&utc) == EOF)
				break;
			for (i=0; i<14; i++)
			{
				if (fscanf(fp3," %f",&tsen[i].gen.temp) == EOF)
					break;
			}
		} while (utc < lmjd);
		if (feof(fp1))
			break;

		cdata->node.loc.utc = currentmjd(cdata->node.utcoffset);
		if (lines2eci(utc, cdata->tle, &npos) < 0)
			exit (1);
		cdata->node.loc.utc = utc;
		update_eci(*cdata, cdata->node.loc.utc, npos);
		bearth.col[2] = 0.;
		cdata->node.loc.bearth = bearth;
		cdata->devspec.imu[0]->mag = bearth;
		*cdata->devspec.ssen[0] = ssen[0];
		for (i=0; i<14; i++)
		{
			*cdata->devspec.tsen[i] = tsen[i];
		}
		log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,(char *)json_of_soh(mainjstring, cdata));
		lmjd += 10./86400.;
		//		ecount = check_events(events,20,cdata);
		for (i=0; i<ecount; i++)
		{
			cdata->event[0].s = events[i];
			strcpy(cdata->event[0].l.condition,cdata->emap[events[i].handle.hash][events[i].handle.index].text);
			log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, cdata));
		}
	}

	nmjd = (int)(lmjd+1);
	for (utc=lmjd; utc<nmjd; utc+=10./86400.)
	{
		cdata->node.loc.utc = currentmjd(cdata->node.utcoffset);
		if (lines2eci(utc, cdata->tle, &npos) < 0)
			exit (1);
		cdata->node.loc.utc = utc;
		update_eci(*cdata, cdata->node.loc.utc, npos);
		log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,json_of_soh(mainjstring, cdata));
		//		ecount = check_events(events,20,cdata);
		for (i=0; i<ecount; i++)
		{
			cdata->event[0].s = events[i];
			strcpy(cdata->event[0].l.condition,cdata->emap[events[i].handle.hash][events[i].handle.index].text);
			log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, cdata));
		}
	}

}
