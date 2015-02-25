#include "jsonlib.h"
#include "physicslib.h"
#include "mathlib.h"
#include "jsonlib.h"
#include "agentlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/types.h>
#include <sys/stat.h>
#ifndef COSMOS_WIN_BUILD_MSVC
// #include <unistd.h>
#endif
#include "dirent/dirent.h"

svector azel;
vector<nodestruc> track;
cosmosstruc *cdata;
stkstruc stk;
char buf[3000], fname[200];
typedef struct
	{
	char code[10];
	char flags[7];
	} alert_type;

alert_type alerts[22] = {
	{"AOS","G11002"},
	{"AOS+5","G41000"},
	{"AOS+10","G31000"},
	{"MAX","G21000"},
	{"LOS+10","G31000"},
	{"LOS+5","G41320"},
	{"LOS","G11320"},
	{"TIV","T11216"},
	{"TOV","T21006"},
	{"NPOLE","O10000"},
	{"SPOLE","O10000"},
	{"N60D","O12000"},
	{"N30D","O12000"},
	{"S30D","O12000"},
	{"S60D","O12000"},
	{"S30A","O12000"},
	{"N30A","O12000"},
	{"N60A","O12000"},
	{"EQA","O12000"},
	{"EQD","O10000"},
	{"UMB_IN","U11321"},
	{"UMB_OUT","U11321"}};

string output;
char date[30];
FILE *eout, *fout, *tfd;

void alertfor(char *string1, char *string2);

int main(int argc, char *argv[])
{
char ts[20], dtemp[256], tstring[5000], etemp[256];
alert_type talert;
double mjdnow, lastlat, lastsunradiance, fd, lastgsel[MAXTRACK];
int i, j, iretn, gsup[MAXTRACK];
int year, month, day, hour, minute, second;
DIR *ddp, *ydp, *jdp;
struct dirent *td, *yd, *jd;

cdata = json_create();
node_init(argv[1],cdata);
json_clone(cdata);



if ((eout=fopen("flags.ini","r")) != NULL)
	{
	while (fgets(buf,50,eout) != NULL)
		{
		sscanf(buf,"%s %s",talert.code,talert.flags);
		for (i=0; i<22; i++)
			{
			if (!strcmp(talert.code,alerts[i].code))
				alerts[i] = talert;
			}
		}
	fclose(eout);
	}


if ((ddp = opendir("data")) == NULL)
	{
	printf("No data directory\n");
	exit (1);
	}

if ((yd=readdir(ddp))==NULL)
	{
	printf("No year files\n");
	closedir(ddp);
	exit (1);
	}


lastsunradiance = lastlat = -999.;
for (j=0; j<cdata[0].node.target_cnt; j++)
	{
	azel = groundstation(&cdata[0].node.loc,&track[j].loc);
	lastgsel[j] = azel.phi;
	gsup[j] = 0;
	}
//fout = fopen("ephemeris","w");
do
	{
	if (strlen(yd->d_name) == 4)
		{
		sprintf(dtemp,"data/%s",yd->d_name);
		if ((ydp=opendir(dtemp))!=NULL)
			{
			while ((jd=readdir(ydp))!=NULL)
				{
				if (strlen(jd->d_name) == 3)
					{
					sprintf(dtemp,"data/%s/%s",yd->d_name,jd->d_name);
					if ((jdp=opendir(dtemp))!=NULL)
						{
						sprintf(etemp,"%s/orbitalevents",dtemp);
						eout = fopen(etemp,"w");
						while ((td=readdir(jdp))!=NULL)
							{
							if (td->d_name[0] != '.')
								{
								sprintf(dtemp,"data/%s/%s/%s",yd->d_name,jd->d_name,td->d_name);
								tfd = fopen(dtemp,"r");
								printf("%s\n",dtemp);
								while (fgets(tstring,5000,tfd)!=NULL)
									{
									json_parse(tstring,cdata);
									if (lastlat == -999.)
										{
										lastlat = cdata[0].node.loc.pos.geod.s.lat;
										lastsunradiance = cdata[0].node.loc.pos.sunradiance;
										}
									mjdnow = cdata[0].node.loc.utc;
//	output = json_of_ephemeris(jstring, cdata);
//	fprintf(fout,"%s\n",output);
//	fflush(fout);
	mjd2cal(mjdnow,&year,&month,&day,&fd,&iretn);
	hour = (int)(24. * fd);
	minute = (int)(1440. * fd - hour * 60.);
	second = (int)(86400. * fd - hour * 3600. - minute * 60.);
	sprintf(date,"%4d-%02d-%02d,%02d:%02d:%02d",year,month,day,hour,minute,second);
	if (lastlat <= RADOF(-60.) && cdata[0].node.loc.pos.geod.s.lat >= RADOF(-60.))
		alertfor((char *)"S60A",(char *)"");
	if (lastlat <= RADOF(-30.) && cdata[0].node.loc.pos.geod.s.lat >= RADOF(-30.))
		alertfor((char *)"S30A",(char *)"");
	if (lastlat <= 0. && cdata[0].node.loc.pos.geod.s.lat >= 0.)
		alertfor((char *)"EQA",(char *)"");
	if (lastlat <= RADOF(30.) && cdata[0].node.loc.pos.geod.s.lat >= RADOF(30.))
		alertfor((char *)"N30A",(char *)"");
	if (lastlat <= RADOF(60.) && cdata[0].node.loc.pos.geod.s.lat >= RADOF(60.))
		alertfor((char *)"N60A",(char *)"");
	if (lastlat >= RADOF(-60.) && cdata[0].node.loc.pos.geod.s.lat <= RADOF(-60.))
		alertfor((char *)"S60D",(char *)"");
	if (lastlat >= RADOF(-30.) && cdata[0].node.loc.pos.geod.s.lat <= RADOF(-30.))
		alertfor((char *)"S30D",(char *)"");
	if (lastlat >= 0. && cdata[0].node.loc.pos.geod.s.lat <= 0.)
		alertfor((char *)"EQD",(char *)"");
	if (lastlat >= RADOF(30.) && cdata[0].node.loc.pos.geod.s.lat <= RADOF(30.))
		alertfor((char *)"N30D",(char *)"");
	if (lastlat >= RADOF(60.) && cdata[0].node.loc.pos.geod.s.lat <= RADOF(60.))
		alertfor((char *)"N60D",(char *)"");

	if (lastsunradiance == 0. && cdata[0].node.loc.pos.sunradiance > 0.)
		{
		alertfor((char *)"UMB_OUT",(char *)"");
		}
	if (lastsunradiance > 0. && cdata[0].node.loc.pos.sunradiance == 0.)
		{
		alertfor((char *)"UMB_IN",(char *)"");
		}
	lastlat = cdata[0].node.loc.pos.geod.s.lat;
	lastsunradiance = cdata[0].node.loc.pos.sunradiance;
	for (j=0; j<cdata[0].node.target_cnt; j++)
		{
		if (azel.phi <= lastgsel[j] && azel.phi > 0.)
			{
			if (gsup[j] > 0.)
				{
				sprintf(ts,"_%3s_%03.0f",track[j].name,DEGOF(azel.phi));
				alertfor((char *)"MAX",ts);
				}
			gsup[j] = -1;
			}
		else
			gsup[j] = 1;
		sprintf(ts,"_%3s",track[j].name);
		if (azel.phi < RADOF(10.) && lastgsel[j] >= RADOF(10.))
			{
			alertfor((char *)"LOS+10",ts);
			}
		if (azel.phi < RADOF(5.) && lastgsel[j] >= RADOF(5.))
			{
			alertfor((char *)"LOS+5",ts);
			}
		if (azel.phi < 0. && lastgsel[j] >= 0.)
			{
			alertfor((char *)"LOS",ts);
			}
		if (azel.phi >= 0. && lastgsel[j] < 0.)
			{
			alertfor((char *)"AOS",ts);
			}
		if (azel.phi >= RADOF(5.) && lastgsel[j] < RADOF(5.))
			{
			alertfor((char *)"AOS+5",ts);
			}
		if (azel.phi >= RADOF(10.) && lastgsel[j] < RADOF(10.))
			{
			alertfor((char *)"AOS+10",ts);
			}
		lastgsel[j] = azel.phi;
		}
									}
								fclose(tfd);
								}
							}
						closedir(jdp);
						fclose(eout);
						}
					}
				}
			closedir(ydp);
			}
		}
	} while ((yd=readdir(ddp))!=NULL);
closedir(ddp);

//fclose(fout);
}

void alertfor(char *string1, char *string2)
{
int i;

for (i=0; i<22; i++)
	{
	if (!strcmp(string1,alerts[i].code))
		break;
	}

if (i < 22)
	{
	fprintf(eout,"%s,%s,%s%s\n",date,alerts[i].flags,string1,string2);
	fflush(eout);
	}
}
